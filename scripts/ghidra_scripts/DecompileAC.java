import ghidra.program.model.listing.*;
import ghidra.program.model.address.*;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import java.io.*;

public class DecompileAC extends ghidra.app.script.GhidraScript {
    @Override
    public void run() throws Exception {
        String outPath = "/home/z/my-project/download/ghidra_final.txt";
        PrintWriter pw = new PrintWriter(new BufferedWriter(new FileWriter(outPath)));
        
        FunctionManager fm = currentProgram.getFunctionManager();
        AddressFactory af = currentProgram.getAddressFactory();
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        
        pw.println("=== GHIDRA FINAL DECOMPILATION ===");
        
        long[] targets = {0x25979CL, 0x2BF850L, 0x378FF4L};
        String[] names = {"ROOT_CHECK", "DEBUGGER_DETECT", "SCAN_HOOK_DISPATCHER"};
        
        for (int t = 0; t < targets.length; t++) {
            long entry = targets[t];
            Address addr = af.getDefaultAddressSpace().getAddress(entry);
            
            pw.println("\n============================================================");
            pw.println("  FUNCTION: " + names[t] + " @ 0x" + Long.toHexString(entry).toUpperCase());
            pw.println("============================================================");
            
            Function func = fm.getFunctionAt(addr);
            if (func == null) {
                func = fm.getFunctionContaining(addr);
            }
            if (func == null) {
                pw.println("  Creating function at 0x" + Long.toHexString(entry).toUpperCase());
                createFunction(addr, null);
                func = fm.getFunctionAt(addr);
            }
            if (func == null) {
                pw.println("  FAILED to create function");
                continue;
            }
            
            pw.println("  Name: " + func.getName());
            pw.println("  Entry: 0x" + Long.toHexString(entry).toUpperCase());
            pw.println("  Size: " + func.getBody().getNumAddresses() + " bytes");
            
            pw.println("\n  --- DECOMPILED C ---");
            try {
                DecompileResults res = decomp.decompileFunction(func, 300, monitor);
                if (res != null && res.decompileCompleted()) {
                    String c = res.getDecompiledFunction().getC();
                    if (c != null) {
                        for (String line : c.split("\n")) pw.println("  " + line);
                    }
                } else {
                    pw.println("  FAIL: " + (res != null ? res.getErrorMessage() : "null"));
                }
            } catch (Exception e) {
                pw.println("  Error: " + e.getMessage());
            }
        }
        
        pw.println("\n=== DONE ===");
        pw.close();
        decomp.dispose();
        println("Saved: " + outPath);
    }
}
