import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompResults;
import ghidra.program.model.listing.*;
import ghidra.program.model.address.*;
import ghidra.program.model.mem.*;
import ghidra.program.model.symbol.*;
import java.io.*;
import java.util.*;

public class FullAnalysis extends GhidraScript {
    
    @Override
    public void run() throws Exception {
        String outPath = "/home/z/my-project/download/ghidra_output.txt";
        PrintWriter pw = new PrintWriter(new BufferedWriter(new FileWriter(outPath)));
        
        long[] TARGETS = {
            0x2F4F08L, 0x330498L, 0x335D20L, 0x369658L, 0x3A5650L,
            0x436950L, 0x4D4C98L, 0x4DCBE0L
        };
        
        FunctionManager fm = currentProgram.getFunctionManager();
        Listing listing = currentProgram.getListing();
        Memory memory = currentProgram.getMemory();
        
        DecompInterface decompiler = new DecompInterface();
        decompiler.openProgram(currentProgram);
        
        for (long off : TARGETS) {
            Address addr = currentProgram.getAddressFactory().getDefaultAddressSpace().getAddress(off);
            
            pw.println("======================================================================");
            pw.println("  ANALYZING OFFSET: 0x" + Long.toHexString(off).toUpperCase());
            pw.println("======================================================================");
            
            Function func = fm.getFunctionContaining(addr);
            if (func == null) {
                func = fm.getFunctionAt(addr);
            }
            
            if (func == null) {
                pw.println("  STATUS: No function found - creating one");
                createFunction(addr, null);
                func = fm.getFunctionAt(addr);
            }
            
            if (func == null) {
                pw.println("  STATUS: FAILED to create function");
                continue;
            }
            
            String funcName = func.getName();
            long entry = func.getEntryPoint().getOffset();
            long bodySize = func.getBody().getNumAddresses();
            
            pw.println("  Function:     " + funcName);
            pw.println("  Entry:        0x" + Long.toHexString(entry).toUpperCase());
            pw.println("  Size:         " + bodySize + " bytes");
            pw.println("  Parameters:   " + func.getParameterCount());
            pw.println("  Return Type:  " + func.getReturnType().getName());
            
            pw.println("");
            pw.println("  --- PARAMETERS ---");
            Parameter[] params = func.getParameters();
            for (Parameter p : params) {
                pw.println("    " + p.getName() + ": " + p.getDataType().getName() + " (" + p.getStorage() + ")");
            }
            
            pw.println("");
            pw.println("  --- CALLED FUNCTIONS (XREFS OUT) ---");
            List<Function> called = func.getCalledFunctions(monitor);
            int cnt = 0;
            for (Function calledFunc : called) {
                pw.println("    -> 0x" + Long.toHexString(calledFunc.getEntryPoint().getOffset()).toUpperCase() + " " + calledFunc.getName());
                cnt++;
                if (cnt > 25) { pw.println("    ... truncated"); break; }
            }
            
            pw.println("");
            pw.println("  --- CALLERS (XREFS IN) ---");
            List<Function> callers = func.getCallingFunctions(monitor);
            cnt = 0;
            for (Function caller : callers) {
                pw.println("    <- 0x" + Long.toHexString(caller.getEntryPoint().getOffset()).toUpperCase() + " " + caller.getName());
                cnt++;
                if (cnt > 25) { pw.println("    ... truncated"); break; }
            }
            
            pw.println("");
            pw.println("  --- REFERENCES FROM ---");
            Reference[] refsFrom = getReferencesFrom(addr);
            cnt = 0;
            for (Reference r : refsFrom) {
                pw.println("    0x" + Long.toHexString(addr).toUpperCase() + " -> 0x" + Long.toHexString(r.getToAddress().getOffset()).toUpperCase() + " [" + r.getReferenceType() + "]");
                cnt++;
                if (cnt > 30) break;
            }
            
            pw.println("");
            pw.println("  --- DECOMPILED C ---");
            try {
                DecompResults results = decompiler.decompileFunction(func, 120, monitor);
                if (results != null && results.decompileCompleted()) {
                    String cCode = results.getDecompiledFunction().getC();
                    if (cCode != null) {
                        for (String line : cCode.split("\n")) {
                            pw.println("    " + line);
                        }
                    }
                } else {
                    String err = (results != null) ? results.getErrorMessage() : "unknown";
                    pw.println("    Decompilation failed: " + err);
                }
            } catch (Exception e) {
                pw.println("    Error: " + e.getMessage());
            }
            
            pw.println("");
        }
        
        pw.println("=== ANALYSIS COMPLETE ===");
        pw.close();
        decompiler.dispose();
        println("Output saved to: " + outPath);
    }
}
