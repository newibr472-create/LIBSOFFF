// Ghidra Headless Script: Independent Anti-Cheat Function Discovery
// Purpose: Find NEW hookable functions in libanogs.so WITHOUT using any known offsets
// Strategy: Multi-layered analysis - strings, cross-references, call patterns, syscalls

import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.*;
import ghidra.program.model.address.*;
import ghidra.program.model.symbol.*;
import ghidra.program.model.data.*;
import ghidra.app.decompiler.*;
import java.util.*;
import java.io.*;

public class NewHookHunter extends GhidraScript {
    
    StringBuilder report = new StringBuilder();
    Set<String> knownOffsets = new HashSet<>(Arrays.asList(
        "31DCB0", "369658"  // Previously analyzed - EXCLUDED
    ));
    
    @Override
    public void run() throws Exception {
        report.append("=== NEW HOOK HUNTER - Independent Analysis ===\n");
        report.append("Binary: ").append(currentProgram.getName()).append("\n");
        report.append("Base Image: ").append(currentProgram.getImageBase()).append("\n\n");
        
        // PHASE 1: Find ALL strings related to anti-cheat detection
        report.append("========== PHASE 1: ANTI-CHEAT STRING DISCOVERY ==========\n\n");
        Map<String, List<Address>> stringRefs = findDetectionStrings();
        
        // PHASE 2: Find functions referencing those strings
        report.append("========== PHASE 2: FUNCTION CROSS-REFERENCE MAP ==========\n\n");
        Map<Address, List<String>> funcToStrings = findReferencingFunctions(stringRefs);
        
        // PHASE 3: Score and rank functions by importance
        report.append("========== PHASE 3: FUNCTION SCORING & RANKING ==========\n\n");
        List<ScoredFunction> scored = scoreFunctions(funcToStrings);
        
        // PHASE 4: Decompile top candidates
        report.append("========== PHASE 4: TOP CANDIDATE DECOMPILATION ==========\n\n");
        decompileTopCandidates(scored, 8);
        
        // PHASE 5: Deep analysis - find syscall wrappers and thread functions
        report.append("========== PHASE 5: SYSCALL & THREAD ANALYSIS ==========\n\n");
        analyzeSyscallsAndThreads();
        
        // Write report
        String outputPath = "/home/z/my-project/download/new_hook_report.txt";
        writeReport(outputPath);
        println("Report saved to: " + outputPath);
    }
    
    // Detection string categories
    String[][] detectionPatterns = {
        // Memory scanning
        {"ms_scan", "mem_scan", "memory_scan", "scan_memory"},
        // Hook detection  
        {"inline_hook", "hook_detect", "hook_check", "dismatch"},
        // Process/memory access
        {"/proc/self", "/proc/", "maps", "status", "cmdline"},
        // Debugger detection
        {"ptrace", "TracerPid", "debugger", "anti_debug"},
        // Root detection
        {"su_binary", "superuser", "root", "magisk", "supersu"},
        // Signature/Integrity
        {"signature", "integrity", "cert", "verify"},
        // Reporting/Network
        {"report", "upload", "send_data", "violation", "detect"},
        // AnoSDK specific
        {"AnoSDK", "anosdk", "anogs"},
        // Thread/Timer patterns
        {"thread", "timer", "interval", "periodic"},
        // VM/Emulator
        {"emulator", "simulator", "qemu", "vbox"},
    };
    
    Map<String, List<Address>> findDetectionStrings() {
        Map<String, List<Address>> results = new HashMap<>();
        Memory memory = currentProgram.getMemory();
        
        // Get all defined data strings
        Listing listing = currentProgram.getListing();
        DataIterator dataIter = listing.getDefinedData(true);
        
        while (dataIter.hasNext() && !monitor.isCancelled()) {
            Data data = dataIter.next();
            if (data == null || data.getDataType() == null) continue;
            
            String dtName = data.getDataType().getName();
            if (!dtName.contains("string") && !dtName.contains("char")) continue;
            
            try {
                String strValue = data.getValue().toString();
                if (strValue == null || strValue.length() < 3) continue;
                
                for (String[] category : detectionPatterns) {
                    for (String pattern : category) {
                        if (strValue.toLowerCase().contains(pattern.toLowerCase())) {
                            Address addr = data.getAddress();
                            String key = "[" + category[0].toUpperCase() + "] " + strValue;
                            results.computeIfAbsent(key, k -> new ArrayList<>()).add(addr);
                            break;
                        }
                    }
                }
            } catch (Exception e) {
                // Skip problematic strings
            }
        }
        
        // Also search raw memory for ASCII strings
        report.append("Found ").append(results.size()).append(" detection-related strings:\n");
        for (Map.Entry<String, List<Address>> entry : results.entrySet()) {
            for (Address addr : entry.getValue()) {
                report.append("  0x").append(Long.toHexString(addr.getOffset()))
                      .append(" : ").append(entry.getKey()).append("\n");
            }
        }
        report.append("\n");
        
        return results;
    }
    
    Map<Address, List<String>> findReferencingFunctions(Map<String, List<Address>> stringRefs) {
        Map<Address, List<String>> funcMap = new HashMap<>();
        
        for (Map.Entry<String, List<Address>> entry : stringRefs.entrySet()) {
            String label = entry.getKey();
            for (Address strAddr : entry.getValue()) {
                // Find cross-references TO this string
                Reference[] refs = getReferencesTo(strAddr);
                for (Reference ref : refs) {
                    Address fromAddr = ref.getFromAddress();
                    // Get the function containing this reference
                    Function func = getFunctionAt(fromAddr);
                    if (func != null) {
                        funcMap.computeIfAbsent(func.getEntryPoint(), k -> new ArrayList<>())
                              .add(label + " (ref@0x" + Long.toHexString(fromAddr.getOffset()) + ")");
                    }
                }
            }
        }
        
        report.append("Found ").append(funcMap.size()).append(" functions referencing detection strings:\n");
        // Sort by number of references (more refs = more important)
        List<Map.Entry<Address, List<String>>> sorted = new ArrayList<>(funcMap.entrySet());
        sorted.sort((a, b) -> b.getValue().size() - a.getValue().size());
        
        for (Map.Entry<Address, List<String>> entry : sorted) {
            Address addr = entry.getKey();
            String hexAddr = "0x" + Long.toHexString(addr.getOffset());
            Function func = getFunctionAt(addr);
            String funcName = (func != null) ? func.getName() : "unknown";
            int size = (func != null) ? func.getBody().getNumAddresses() : 0;
            
            report.append("\n  Function: ").append(funcName)
                  .append(" @ ").append(hexAddr)
                  .append(" (size: ").append(size).append(" bytes)\n");
            
            for (String ref : entry.getValue()) {
                report.append("    -> ").append(ref).append("\n");
            }
        }
        report.append("\n");
        
        return funcMap;
    }
    
    List<ScoredFunction> scoreFunctions(Map<Address, List<String>> funcToStrings) {
        List<ScoredFunction> scored = new ArrayList<>();
        DecompilerInterface decompiler = getDecompiler();
        
        for (Map.Entry<Address, List<String>> entry : funcToStrings.entrySet()) {
            Address addr = entry.getKey();
            String hex = Long.toHexString(addr.getOffset());
            
            // Skip known offsets
            if (knownOffsets.contains(hex.toUpperCase())) {
                report.append("SKIP (known): 0x").append(hex).append("\n");
                continue;
            }
            
            ScoredFunction sf = new ScoredFunction();
            sf.address = addr;
            sf.func = getFunctionAt(addr);
            
            if (sf.func == null) continue;
            
            sf.numStringRefs = entry.getValue().size();
            
            // Score factors:
            // 1. Number of detection string references (0-30)
            sf.score += Math.min(sf.numStringRefs * 3, 30);
            
            // 2. Function size - too small = trivial, too large = complex
            int size = sf.func.getBody().getNumAddresses();
            if (size > 100 && size < 2000) sf.score += 10;  // Sweet spot
            else if (size >= 2000 && size < 5000) sf.score += 5;
            
            // 3. Number of called functions (more calls = more complex logic)
            Function[] called = sf.func.getCalledFunctions(monitor);
            sf.numCalls = called.length;
            if (called.length > 5 && called.length < 30) sf.score += 15;
            else if (called.length >= 3) sf.score += 8;
            
            // 4. Number of calling functions (more callers = more important)
            Function[] callers = sf.func.getCallingFunctions(monitor);
            sf.numCallers = callers.length;
            if (callers.length > 0 && callers.length < 10) sf.score += 10;
            else if (callers.length == 1) sf.score += 5;  // Single caller = direct dispatch
            
            // 5. String category diversity bonus
            Set<String> categories = new HashSet<>();
            for (String ref : entry.getValue()) {
                categories.add(ref.split("\\[")[1].split("\\]")[0]);
            }
            sf.categoryDiversity = categories.size();
            sf.score += categories.size() * 5;
            
            // 6. Check for loop patterns (scanning functions usually loop)
            sf.hasLoops = false;
            
            sf.stringLabels = entry.getValue();
            sf.calledFuncs = called;
            sf.callerFuncs = callers;
            
            scored.add(sf);
        }
        
        scored.sort((a, b) -> b.score - a.score);
        
        report.append("Top scored candidates (excluding known offsets):\n");
        for (int i = 0; i < Math.min(scored.size(), 15); i++) {
            ScoredFunction sf = scored.get(i);
            report.append(String.format("  #%d Score=%d | 0x%s | %s | refs=%d calls=%d callers=%d cats=%d size=%d\n",
                i+1, sf.score, Long.toHexString(sf.address.getOffset()),
                sf.func.getName(), sf.numStringRefs, sf.numCalls, sf.numCallers,
                sf.categoryDiversity, sf.func.getBody().getNumAddresses()));
        }
        report.append("\n");
        
        return scored;
    }
    
    void decompileTopCandidates(List<ScoredFunction> scored, int count) {
        DecompilerInterface decomp = getDecompiler();
        
        for (int i = 0; i < Math.min(scored.size(), count); i++) {
            ScoredFunction sf = scored.get(i);
            String hex = "0x" + Long.toHexString(sf.address.getOffset());
            
            report.append("------ CANDIDATE #").append(i+1).append(" ------\n");
            report.append("Address: ").append(hex).append("\n");
            report.append("Name: ").append(sf.func.getName()).append("\n");
            report.append("Score: ").append(sf.score).append("\n");
            report.append("String refs: ").append(sf.numStringRefs).append("\n");
            report.append("Calls out: ").append(sf.numCalls).append("\n");
            report.append("Called by: ").append(sf.numCallers).append("\n");
            
            if (sf.callerFuncs.length > 0) {
                report.append("Callers:\n");
                for (Function caller : sf.callerFuncs) {
                    report.append("  - ").append(caller.getName())
                          .append(" @ 0x").append(Long.toHexString(caller.getEntryPoint().getOffset())).append("\n");
                }
            }
            
            if (sf.calledFuncs.length > 0) {
                report.append("Calls:\n");
                for (Function called : sf.calledFuncs) {
                    report.append("  - ").append(called.getName())
                          .append(" @ 0x").append(Long.toHexString(called.getEntryPoint().getOffset())).append("\n");
                }
            }
            
            // Try to decompile
            try {
                DecompileResults results = decomp.decompileFunction(sf.func, monitor, 60);
                if (results != null && results.decompileCompleted() && results.getDecompiledFunction() != null) {
                    String cCode = results.getDecompiledFunction().getC();
                    // Truncate if too long
                    if (cCode.length() > 3000) {
                        cCode = cCode.substring(0, 3000) + "\n... [TRUNCATED]";
                    }
                    report.append("Decompiled C:\n").append(cCode).append("\n");
                } else {
                    report.append("Decompilation failed or timed out\n");
                }
            } catch (Exception e) {
                report.append("Decompilation error: ").append(e.getMessage()).append("\n");
            }
            report.append("\n");
        }
    }
    
    void analyzeSyscallsAndThreads() {
        // Find SVC instructions (ARM64 syscall)
        report.append("Searching for SVC #0 instructions (ARM64 syscalls)...\n");
        
        Listing listing = currentProgram.getListing();
        InstructionIterator iter = listing.getInstructions(true);
        int svcCount = 0;
        
        while (iter.hasNext() && !monitor.isCancelled()) {
            Instruction inst = iter.next();
            String mnemonic = inst.getMnemonicString();
            if ("SVC".equals(mnemonic)) {
                Address addr = inst.getAddress();
                Function func = getFunctionAt(addr);
                String funcName = (func != null) ? func.getName() : "NO_FUNC";
                String hex = "0x" + Long.toHexString(addr.getOffset());
                svcCount++;
                
                if (svcCount <= 50) { // Limit output
                    report.append("  SVC @ ").append(hex)
                          .append(" in ").append(funcName)
                          .append(" (func@0x").append(func != null ? Long.toHexString(func.getEntryPoint().getOffset()) : "?")
                          .append(")\n");
                }
            }
        }
        report.append("Total SVC instructions found: ").append(svcCount).append("\n\n");
        
        // Find pthread_create calls
        report.append("Searching for pthread_create references...\n");
        SymbolTable symTable = currentProgram.getSymbolTable();
        SymbolIterator symIter = symTable.getExternalEntryPointIterator();
        
        while (symIter.hasNext()) {
            Symbol sym = symIter.next();
            String name = sym.getName().toLowerCase();
            if (name.contains("pthread_create") || name.contains("clone") || 
                name.contains("fork") || name.contains("pthread") ||
                name.contains("syscall") || name.contains("mmap") ||
                name.contains("mprotect") || name.contains("openat") ||
                name.contains("readlink") || name.contains("access") ||
                name.contains("stat") || name.contains("fopen") ||
                name.contains("fgets") || name.contains("strstr")) {
                
                Address entry = sym.getAddress();
                Reference[] refs = getReferencesTo(entry);
                report.append("  External: ").append(sym.getName())
                      .append(" @ 0x").append(Long.toHexString(entry.getOffset()))
                      .append(" (").append(refs.length).append(" refs)\n");
                
                for (Reference ref : refs) {
                    Function refFunc = getFunctionAt(ref.getFromAddress());
                    if (refFunc != null) {
                        String hex = Long.toHexString(refFunc.getEntryPoint().getOffset());
                        if (!knownOffsets.contains(hex.toUpperCase())) {
                            report.append("    -> Called from: ").append(refFunc.getName())
                                  .append(" @ 0x").append(hex).append("\n");
                        }
                    }
                }
            }
        }
        report.append("\n");
        
        // Find BL instructions to specific interesting external functions
        report.append("Searching for interesting PLT calls in all functions...\n");
        iter = listing.getInstructions(true);
        Map<String, Integer> pltCallCount = new HashMap<>();
        
        while (iter.hasNext() && !monitor.isCancelled()) {
            Instruction inst = iter.next();
            if (inst.getMnemonicString() == null) continue;
            
            String mnem = inst.getMnemonicString().toUpperCase();
            if (mnem.equals("BL") || mnem.equals("B")) {
                try {
                    Address target = inst.getFlowDestination(0);
                    Function targetFunc = getFunctionAt(target);
                    if (targetFunc != null) {
                        String tname = targetFunc.getName();
                        if (tname.startsWith("FUN_") || tname.contains("EXT_") || 
                            tname.contains("plt") || tname.contains("JNI") ||
                            tname.contains("strcmp") || tname.contains("strstr") ||
                            tname.contains("memcmp") || tname.contains("open") ||
                            tname.contains("read") || tname.contains("write") ||
                            tname.contains("mmap") || tname.contains("mprotect") ||
                            tname.contains("signal") || tname.contains("sigaction")) {
                            String key = tname + "@0x" + Long.toHexString(target.getOffset());
                            pltCallCount.merge(key, 1, Integer::sum);
                        }
                    }
                } catch (Exception e) {}
            }
        }
        
        List<Map.Entry<String, Integer>> sortedPlt = new ArrayList<>(pltCallCount.entrySet());
        sortedPlt.sort((a, b) -> b.getValue() - a.getValue());
        
        report.append("Most-called external/interesting functions:\n");
        for (int i = 0; i < Math.min(30, sortedPlt.size()); i++) {
            report.append("  ").append(sortedPlt.get(i).getValue())
                  .append("x -> ").append(sortedPlt.get(i).getKey()).append("\n");
        }
    }
    
    void writeReport(String path) throws IOException {
        PrintWriter pw = new PrintWriter(new FileWriter(path));
        pw.print(report.toString());
        pw.close();
    }
    
    static class ScoredFunction {
        Address address;
        Function func;
        int score = 0;
        int numStringRefs = 0;
        int numCalls = 0;
        int numCallers = 0;
        int categoryDiversity = 0;
        boolean hasLoops = false;
        List<String> stringLabels;
        Function[] calledFuncs;
        Function[] callerFuncs;
    }
}
