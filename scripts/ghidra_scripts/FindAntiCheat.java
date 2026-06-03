import ghidra.program.model.symbol.Reference;
import ghidra.program.model.listing.*;
import ghidra.program.model.address.*;
import ghidra.program.model.mem.Memory;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import java.io.*;

public class FindAntiCheat extends ghidra.app.script.GhidraScript {
    @Override
    public void run() throws Exception {
        String outPath = "/home/z/my-project/download/ghidra_decompile.txt";
        PrintWriter pw = new PrintWriter(new BufferedWriter(new FileWriter(outPath)));
        
        FunctionManager fm = currentProgram.getFunctionManager();
        Memory memory = currentProgram.getMemory();
        AddressFactory af = currentProgram.getAddressFactory();
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        
        pw.println("=== GHIDRA ANTI-CHEAT DISCOVERY ===");
        
        String[] keywords = {"ms_scan_start", "ms_set_inlie_hook", "inline_hook_opcode_dismatch",
            "debugger=", "mt2_no_root", "CertMD5"};
        
        pw.println("\n--- STRING LOCATIONS ---");
        java.util.ArrayList<Long> stringAddrs = new java.util.ArrayList<>();
        
        Address min = memory.getMinAddress();
        Address max = memory.getMaxAddress();
        
        for (String kw : keywords) {
            byte[] searchBytes = kw.getBytes("UTF-8");
            byte[] mask = new byte[searchBytes.length];
            java.util.Arrays.fill(mask, (byte)0xFF);
            
            Address addr = min;
            while (addr != null && addr.compareTo(max) <= 0) {
                addr = memory.findBytes(addr, max, searchBytes, mask, true, monitor);
                if (addr != null) {
                    stringAddrs.add(addr.getOffset());
                    pw.println("  Found '" + kw + "' at 0x" + Long.toHexString(addr.getOffset()).toUpperCase());
                    addr = addr.add(1);
                }
            }
        }
        
        pw.println("\nTotal string hits: " + stringAddrs.size());
        
        pw.println("\n--- FINDING REFERENCING FUNCTIONS ---");
        java.util.LinkedHashSet<Long> funcAddrs = new java.util.LinkedHashSet<>();
        
        for (Long sAddr : stringAddrs) {
            Address strAddr = af.getDefaultAddressSpace().getAddress(sAddr);
            try {
                Reference[] refs = getReferencesTo(strAddr);
                if (refs != null) {
                    for (Reference ref : refs) {
                        Address fromAddr = ref.getFromAddress();
                        Function containingFunc = fm.getFunctionContaining(fromAddr);
                        if (containingFunc != null) {
                            long funcEntry = containingFunc.getEntryPoint().getOffset();
                            if (!funcAddrs.contains(funcEntry)) {
                                funcAddrs.add(funcEntry);
                                pw.println("  String 0x" + Long.toHexString(sAddr).toUpperCase() + 
                                    " <- 0x" + Long.toHexString(funcEntry).toUpperCase() + 
                                    " " + containingFunc.getName() + " (size=" + containingFunc.getBody().getNumAddresses() + ")");
                            }
                        }
                    }
                }
            } catch (Exception e) {}
        }
        
        pw.println("\nFound " + funcAddrs.size() + " functions");
        
        pw.println("\n--- DECOMPILATIONS ---");
        int dcount = 0;
        for (Long funcEntry : funcAddrs) {
            if (dcount >= 12 || monitor.isCancelled()) break;
            Function func = fm.getFunctionContaining(af.getDefaultAddressSpace().getAddress(funcEntry));
            if (func == null) continue;
            
            pw.println("\n============================================================");
            pw.println("  #" + (dcount+1) + " " + func.getName() + " @ 0x" + Long.toHexString(funcEntry).toUpperCase() + " (size=" + func.getBody().getNumAddresses() + ")");
            pw.println("============================================================");
            
            try {
                DecompileResults res = decomp.decompileFunction(func, 180, monitor);
                if (res != null && res.decompileCompleted()) {
                    String c = res.getDecompiledFunction().getC();
                    if (c != null) {
                        for (String line : c.split("\n")) pw.println("    " + line);
                    }
                } else {
                    pw.println("    FAIL: " + (res != null ? res.getErrorMessage() : "null"));
                }
            } catch (Exception e) {
                pw.println("    Error: " + e.getMessage());
            }
            dcount++;
        }
        
        pw.println("\n=== DONE: " + dcount + " functions decompiled ===");
        pw.close();
        decomp.dispose();
        println("Saved: " + outPath);
    }
}
