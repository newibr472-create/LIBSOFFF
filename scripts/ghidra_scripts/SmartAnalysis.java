import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompResults;
import ghidra.program.model.address.Address;
import java.io.*;

public class SmartAnalysis extends ghidra.app.script.GhidraScript {
    
    @Override
    public void run() throws Exception {
        String outPath = "/home/z/my-project/download/ghidra_smart_output.txt";
        PrintWriter pw = new PrintWriter(new BufferedWriter(new FileWriter(outPath)));
        
        FunctionManager fm = currentProgram.getFunctionManager();
        DecompInterface decompiler = new DecompInterface();
        decompiler.setOptions(new ghidra.app.decompiler.DecompileOptions());
        decompiler.openProgram(currentProgram);
        
        pw.println("============================================================");
        pw.println("  GHIDRA SMART ANALYSIS - libanogs.so (AnoSDK Anti-Cheat)");
        pw.println("  Auto-discovering hookable anti-cheat functions");
        pw.println("============================================================");
        
        pw.println("\n--- EXPORTED FUNCTIONS ---");
        FunctionIterator iter = fm.getFunctions(true);
        int totalFuncs = 0;
        int analyzed = 0;
        
        java.util.ArrayList<Function> candidates = new java.util.ArrayList<>();
        
        while (iter.hasNext() && !monitor.isCancelled()) {
            Function func = iter.next();
            totalFuncs++;
            
            long entry = func.getEntryPoint().getOffset();
            String name = func.getName();
            int bodySize = (int) func.getBody().getNumAddresses();
            int numCalls = func.getCalledFunctions(monitor).size();
            int numCallers = func.getCallingFunctions(monitor).size();
            
            if (totalFuncs <= 50 || bodySize > 200 || numCallers > 5) {
                pw.println(String.format("  0x%06X  %-40s  size=%-5d  calls=%-3d  callers=%-3d",
                    entry, name, bodySize, numCalls, numCallers));
            }
            
            if (bodySize < 50) continue;
            if (numCalls == 0) continue;
            if (name.startsWith("FUN_") && numCallers == 0 && bodySize < 500) continue;
            
            candidates.add(func);
        }
        
        pw.println(String.format("\n  Total functions: %d, Candidates: %d", totalFuncs, candidates.size()));
        
        pw.println("\n--- TOP CANDIDATES FOR HOOKS (by size, calls, callers) ---");
        java.util.Collections.sort(candidates, (a, b) -> {
            int scoreA = (int)(a.getBody().getNumAddresses() / 10 + a.getCalledFunctions(monitor).size() * 5 + a.getCallingFunctions(monitor).size() * 10);
            int scoreB = (int)(b.getBody().getNumAddresses() / 10 + b.getCalledFunctions(monitor).size() * 5 + b.getCallingFunctions(monitor).size() * 10);
            return scoreB - scoreA;
        });
        
        int decompileCount = 0;
        for (Function func : candidates) {
            if (decompileCount >= 12 || monitor.isCancelled()) break;
            
            long entry = func.getEntryPoint().getOffset();
            String name = func.getName();
            int bodySize = (int) func.getBody().getNumAddresses();
            java.util.List<Function> callers = func.getCallingFunctions(monitor);
            java.util.List<Function> called = func.getCalledFunctions(monitor);
            
            pw.println(String.format("\n============================================================"));
            pw.println("  FUNCTION: %s @ 0x%06X", name, entry));
            pw.println("  Size: %d bytes | Callers: %d | Called: %d", bodySize, callers.size(), called.size());
            pw.println("============================================================"));
            
            pw.println("\n  CALLERS:");
            for (Function c : callers) {
                pw.println(String.format("    <- 0x%06X %s", c.getEntryPoint().getOffset(), c.getName()));
            }
            
            pw.println("\n  CALLED FUNCTIONS:");
            for (Function c : called) {
                pw.println(String.format("    -> 0x%06X %s", c.getEntryPoint().getOffset(), c.getName()));
                if (c.getName().startsWith("FUN_") == false) {
                    pw.println(String.format("       (named: %s)", c.getName()));
                }
            }
            
            pw.println("\n  DECOMPILED C CODE:");
            try {
                DecompResults results = decompiler.decompileFunction(func, 180, monitor);
                if (results != null && results.decompileCompleted()) {
                    String cCode = results.getDecompiledFunction().getC();
                    if (cCode != null) {
                        String[] lines = cCode.split("\n");
                        for (String line : lines) {
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
            
            decompileCount++;
        }
        
        pw.println("\n=== SMART ANALYSIS COMPLETE ===");
        pw.close();
        decompiler.dispose();
        println("Smart analysis saved to: " + outPath);
    }
}
