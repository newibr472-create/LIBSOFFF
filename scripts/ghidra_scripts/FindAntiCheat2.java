import ghidra.program.model.listing.*;
import ghidra.program.model.address.*;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.program.model.symbol.Reference;
import java.io.*;

public class FindAntiCheat2 extends ghidra.app.script.GhidraScript {
    @Override
    public void run() throws Exception {
        String outPath = "/home/z/my-project/download/ghidra_decompile2.txt";
        PrintWriter pw = new PrintWriter(new BufferedWriter(new FileWriter(outPath)));
        
        FunctionManager fm = currentProgram.getFunctionManager();
        Memory memory = currentProgram.getMemory();
        AddressFactory af = currentProgram.getAddressFactory();
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        
        pw.println("=== GHIDRA SMART DISCOVERY ===");
        
        // Strings found at these offsets (from previous run)
        long[] strOffsets = {0x19F246L, 0x1A478DL, 0x1A014BL, 0x19FD05L, 0x1A0FC6L, 0x19F45FL};
        String[] strNames = {"ms_scan_start", "ms_set_inlie_hook", "inline_hook_opcode_dismatch", "debugger=", "mt2_no_root", "CertMD5"};
        
        // For each string, scan the .text section for ADRP+ADD instructions that reference it
        // In ARM64, strings are referenced via ADRP+ADD pattern
        pw.println("\n--- MANUAL XREF SEARCH (ADRP+ADD pattern) ---");
        
        MemoryBlock textBlock = memory.getBlock(af.getDefaultAddressSpace().getAddress(0x10000));
        if (textBlock == null) {
            // Find executable block
            for (MemoryBlock block : memory.getBlocks()) {
                if (block.isExecute() && block.getSize() > 100000) {
                    textBlock = block;
                    pw.println("  Using block: " + block.getName() + " 0x" + Long.toHexString(block.getStart().getOffset()) + " size=" + block.getSize());
                    break;
                }
            }
        }
        
        if (textBlock == null) {
            pw.println("  ERROR: No executable block found");
            pw.close();
            return;
        }
        
        byte[] textBytes = new byte[(int)textBlock.getSize()];
        memory.getBytes(textBlock.getStart(), textBytes);
        long textBase = textBlock.getStart().getOffset();
        
        // For each string offset, find ADRP+ADD that points to it
        java.util.LinkedHashSet<Long> foundFuncs = new java.util.LinkedHashSet<>();
        
        for (int si = 0; si < strOffsets.length; si++) {
            long strOff = strOffsets[si];
            pw.println("\n  Searching xrefs to '" + strNames[si] + "' @ 0x" + Long.toHexString(strOff).toUpperCase());
            
            // Scan for ADRP instructions where ADRP_page + ADD_offset = string_page
            long strPage = strOff & ~0xFFFL;
            long strPageOff = strOff & 0xFFFL;
            
            int xrefCount = 0;
            for (int i = 0; i < textBytes.length - 8; i += 4) {
                int insn = java.nio.ByteBuffer.wrap(textBytes, i, 4).order(java.nio.ByteOrder.LITTLE_ENDIAN).getInt();
                
                // Check ADRP: (insn & 0x9F000000) == 0x90000000
                if ((insn & 0x9F000000) == 0x90000000) {
                    long immlo = (insn >>> 29) & 3;
                    long immhi = (insn >>> 5) & 0x7FFFFL;
                    long imm = (immhi << 2) | immlo;
                    if ((imm & 0x100000L) != 0) imm |= 0xFFE00000L;
                    
                    long adrpPage = (textBase + i) & ~0xFFFL;
                    long targetPage = adrpPage + (imm << 12);
                    
                    if (targetPage == strPage && i + 4 < textBytes.length) {
                        // Check next instruction for ADD x?, x?, #offset
                        int nextInsn = java.nio.ByteBuffer.wrap(textBytes, i + 4, 4).order(java.nio.ByteOrder.LITTLE_ENDIAN).getInt();
                        // ADD immediate: (insn & 0x7F800000) == 0x11000000
                        if ((nextInsn & 0x7F800000) == 0x11000000) {
                            int imm12 = (nextInsn >>> 10) & 0xFFF;
                            int shift = ((nextInsn >>> 22) & 3) * 12;
                            long addOffset = imm12 << shift;
                            
                            if (addOffset == strPageOff) {
                                long xrefAddr = textBase + i;
                                Function func = fm.getFunctionContaining(af.getDefaultAddressSpace().getAddress(xrefAddr));
                                if (func != null) {
                                    long funcEntry = func.getEntryPoint().getOffset();
                                    if (!foundFuncs.contains(funcEntry)) {
                                        foundFuncs.add(funcEntry);
                                        pw.println("    -> ADRP+ADD at 0x" + Long.toHexString(xrefAddr).toUpperCase() + 
                                            " in " + func.getName() + " @ 0x" + Long.toHexString(funcEntry).toUpperCase() +
                                            " (size=" + func.getBody().getNumAddresses() + ")");
                                    }
                                    xrefCount++;
                                } else {
                                    xrefCount++;
                                    if (xrefCount <= 3) pw.println("    -> ADRP+ADD at 0x" + Long.toHexString(xrefAddr).toUpperCase() + " (no function)");
                                }
                            }
                        }
                    }
                }
            }
            pw.println("    Total xrefs: " + xrefCount);
        }
        
        pw.println("\n\n=== UNIQUE FUNCTIONS FOUND: " + foundFuncs.size() + " ===");
        
        // Decompile all found functions
        pw.println("\n--- DECOMPILATIONS ---");
        int dcount = 0;
        for (Long funcEntry : foundFuncs) {
            if (dcount >= 10 || monitor.isCancelled()) break;
            Function func = fm.getFunctionContaining(af.getDefaultAddressSpace().getAddress(funcEntry));
            if (func == null) continue;
            
            pw.println("\n============================================================");
            pw.println("  FUNCTION #" + (dcount+1) + ": " + func.getName() + " @ 0x" + Long.toHexString(funcEntry).toUpperCase());
            pw.println("  Size: " + func.getBody().getNumAddresses() + " bytes");
            pw.println("============================================================");
            
            try {
                DecompileResults res = decomp.decompileFunction(func, 180, monitor);
                if (res != null && res.decompileCompleted()) {
                    String c = res.getDecompiledFunction().getC();
                    if (c != null) {
                        for (String line : c.split("\n")) pw.println("    " + line);
                    }
                } else {
                    pw.println("    Decompilation failed: " + (res != null ? res.getErrorMessage() : "null"));
                }
            } catch (Exception e) {
                pw.println("    Error: " + e.getMessage());
            }
            dcount++;
        }
        
        pw.println("\n=== DONE: " + dcount + " functions decompiled ===");
        pw.close();
        decomp.dispose();
        println("Output: " + outPath);
    }
}
