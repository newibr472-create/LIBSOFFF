using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;
using B1;
#nullable disable
namespace WOLF
{


    public partial class Form1 : Form
    {
        private bool dragging = false;
        private Point startPoint = new Point(0, 0);
        private const string MutexIdentifier = "BRAVE";
        private static DebugConsole debugConsole;
        private static readonly string[] bypassFiles = {
            "libtss.so",
            "libzerov5.so"
        };

        // Tracks (destination, backup) pairs for restore-on-crash and restore-on-exit
        private System.Collections.Generic.List<(string dest, string backup)> backedUp = new System.Collections.Generic.List<(string dest, string backup)>();

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern IntPtr OpenProcess(uint dwDesiredAccess, bool bInheritHandle, uint dwProcessId);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool ReadProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, int dwSize, out int lpNumberOfBytesRead);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, int dwSize, out int lpNumberOfBytesWritten);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool CloseHandle(IntPtr hObject);

        [DllImport("advapi32.dll", SetLastError = true)]
        private static extern bool OpenProcessToken(IntPtr processHandle, uint desiredAccess, out IntPtr tokenHandle);

        [DllImport("advapi32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        private static extern bool LookupPrivilegeValue(string lpSystemName, string lpName, out long lpLuid);

        [DllImport("advapi32.dll", SetLastError = true)]
        private static extern bool AdjustTokenPrivileges(IntPtr tokenHandle, bool disableAllPrivileges, ref TOKEN_PRIVILEGES newState, int bufferLength, IntPtr previousState, IntPtr returnLength);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool VirtualProtectEx(IntPtr hProcess, IntPtr lpAddress, UIntPtr dwSize, uint flNewProtect, out uint lpflOldProtect);

        [System.Runtime.InteropServices.StructLayout(System.Runtime.InteropServices.LayoutKind.Sequential)]
        private struct TOKEN_PRIVILEGES
        {
            public int PrivilegeCount;
            public long Luid;
            public int Attributes;
        }

        const uint PROCESS_VM_OPERATION = 0x0008;
        const uint PROCESS_VM_READ = 0x0010;
        const uint PROCESS_VM_WRITE = 0x0020;
        const uint PROCESS_ALL_ACCESS = 0x1F0FFF;
        const uint TOKEN_ADJUST_PRIVILEGES = 0x0020;
        const uint TOKEN_QUERY = 0x0008;
        const int SE_PRIVILEGE_ENABLED = 0x00000002;
        const uint PAGE_EXECUTE_READWRITE = 0x40;
        private readonly string[] reversingToolProcessNames = {
  "OllyDbg",
  "ollydbg",
  "IDA",
  "ida_32bit",
  "ida_64bit",
  "ida.exe",
  "ida64.exe",
  "idaq.exe",
  "idaq64.exe",
  "x64dbg",
  "x32dbg",
  "x96dbg",
  "cheatengine",
  "cheatengine-x86_64",
  "cheatengine-i386",
  "cheatengine-x86_64-SSE4-AVX2",
  "cheatengine-x86_64-SSE4-AVX2.exe",
  "Wireshark",
  "wireshark.exe",
  "windbg",
  "windbg.exe",
  "ImmunityDebugger",
  "ImmunityDebugger.exe",
  "HxD",
  "HxD.exe",
  "LordPE",
  "LordPE.exe",
  "PEiD",
  "PEiD.exe",
  "Procmon",
  "procmon.exe",
  "ProcessHacker",
  "processhacker.exe",
  "ProcessExplorer",
  "procexp.exe",
  "SysinternalsSuite",
  "tcpview.exe",
  "scylla.exe",
  "scylla",
  "scylla_x86.exe",
  "scylla_x64.exe",
  "megadumper.exe",
  "megadumper",
  "de4dot.exe",
  "de4dot",
  "dnSpy",
  "dnspy.exe",
  "dotPeek",
  "dotpeek.exe",
  "Ghidra",
  "ghidraRun.bat",
  "ghidra.exe",
  "Radare2",
  "r2.exe",
  "Binary Ninja",
  "binaryninja.exe",
  "Frida",
  "frida-server",
  "frida-trace",
  "frida",
  "Fiddler",
  "fiddler.exe",
  "ReClass.NET",
  "reclass.net.exe",
  "cutter.exe",
  "cutter",
  "api monitor",
  "apimonitor-x64.exe",
  "apimonitor-x86.exe"

 };
        private static Mutex singleInstanceMutex;

        public Form1()
        {
            InitializeComponent();

            SetStyle(ControlStyles.DoubleBuffer | ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint, true);
            UpdateStyles();
            singleInstanceMutex = new Mutex(true, MutexIdentifier, out bool isFirstInstance);

            //if (!isFirstInstance)
            //{
            //    MessageBox.Show("Another instance of the application is already running.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            //    Environment.Exit(0);
            //}


            if (DetectReversingTools())
            {
                MessageBox.Show("Reversing tools detected. FUCK YOU SON OF BITCH", "Security Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                //LicenseAuthApp.ban();
                Form1.RemoveFile("WOLVES.exe");
                Environment.Exit(0);

            }
        }
        private bool DetectReversingTools()
        {
            foreach (string processName in reversingToolProcessNames)
            {
                if (IsProcessRunning(processName))
                {
                    return true; // Reversing tool detected !
                }
            }
            return false; // No reversing tool detected !
        }
        private bool IsProcessRunning(string processName)
        {
            Process[] processes = Process.GetProcessesByName(processName);
            return processes.Length > 0;
        }

        private void SetStatus(string text, System.Drawing.Color? color = null)
        {
            try
            {
                if (this.IsDisposed || !this.IsHandleCreated) return;
                this.BeginInvoke(new Action(() =>
                {
                    try
                    {
                        this.statusv.Text = text;
                        if (color.HasValue) this.statusv.ForeColor = color.Value;
                    }
                    catch { }
                }));
            }
            catch { }
        }

        public void dos(string command)
        {
            if (debugConsole != null && !debugConsole.IsDisposed)
                debugConsole.LogAdb(command);
            Process process = new Process();
            process.StartInfo = new ProcessStartInfo()
            {
                FileName = "cmd.exe",
                Arguments = " /C " + command,
                WindowStyle = ProcessWindowStyle.Hidden,
                UseShellExecute = false,
                CreateNoWindow = true,
                RedirectStandardError = false,
                RedirectStandardOutput = false
            };
            process.Start();
            process.WaitForExit(5000);
        }

        private string dosCapture(string command, int timeoutMs = 8000, bool silent = false)
        {
            try
            {
                if (!silent && debugConsole != null && !debugConsole.IsDisposed)
                    debugConsole.LogAdb(command);

                string _out = "", _err = "";

                using (Process process = new Process())
                {
                    process.StartInfo = new ProcessStartInfo()
                    {
                        FileName = "cmd.exe",
                        Arguments = " /C " + command,
                        WindowStyle = ProcessWindowStyle.Hidden,
                        UseShellExecute = false,
                        CreateNoWindow = true,
                        RedirectStandardError = true,
                        RedirectStandardOutput = true
                    };
                    process.Start();

                    // Use dedicated threads (not thread pool) to avoid pool exhaustion
                    Thread tOut = new Thread(() => { try { _out = process.StandardOutput.ReadToEnd(); } catch { } });
                    Thread tErr = new Thread(() => { try { _err = process.StandardError.ReadToEnd(); } catch { } });
                    tOut.IsBackground = true;
                    tErr.IsBackground = true;
                    tOut.Start();
                    tErr.Start();

                    bool exited = process.WaitForExit(timeoutMs);
                    if (!exited) { try { process.Kill(); } catch { } }

                    tOut.Join(2000);
                    tErr.Join(2000);
                }

                string combined = (_out + _err).Trim();
                // Only log short results - never log raw logcat dumps
                if (!silent && !string.IsNullOrEmpty(combined) && combined.Length < 300)
                    LogDebug($"  => {combined}");
                return combined;
            }
            catch (Exception ex)
            {
                LogDebug($"  => dosCapture error: {ex.Message}");
                return "";
            }
        }

        private static void ValidateSoFile(string path)
        {
            if (!System.IO.File.Exists(path))
            {
                LogError($"File does not exist after download: {path}");
                return;
            }
            long size = new System.IO.FileInfo(path).Length;
            LogDebug($"File size: {size} bytes ({size / 1024.0:F1} KB)");
            if (size < 1024)
            {
                LogError($"File is suspiciously small ({size} bytes) - may be a 404 HTML page, not a .so file!");
                // Read first 64 bytes to show what it actually is
                try
                {
                    byte[] header = new byte[Math.Min(64, (int)size)];
                    using (var fs = System.IO.File.OpenRead(path))
                        fs.Read(header, 0, header.Length);
                    string preview = System.Text.Encoding.ASCII.GetString(header).Replace("\n", " ").Replace("\r", "");
                    LogError($"File header preview: {preview}");
                }
                catch { }
                return;
            }
            // Check ELF magic bytes and architecture
            try
            {
                byte[] header = new byte[20];
                using (var fs = System.IO.File.OpenRead(path))
                    fs.Read(header, 0, header.Length);

                bool isElf = header[0] == 0x7F && header[1] == 0x45 && header[2] == 0x4C && header[3] == 0x46;
                if (!isElf)
                {
                    LogError($"File is NOT a valid ELF binary! Magic bytes: {header[0]:X2} {header[1]:X2} {header[2]:X2} {header[3]:X2} - may cause game crash");
                    return;
                }

                // ELF class: byte[4] = 1 → 32-bit, 2 → 64-bit
                string elfClass = header[4] == 1 ? "32-bit" : header[4] == 2 ? "64-bit" : $"unknown({header[4]})";
                // e_machine: bytes[18-19] (little-endian) — 0x28=ARM32, 0xB7=ARM64, 0x03=x86, 0x3E=x86_64
                int eMachine = header[18] | (header[19] << 8);
                string arch = eMachine switch
                {
                    0x28 => "ARM 32-bit (armeabi-v7a)",
                    0xB7 => "ARM 64-bit (arm64-v8a)",
                    0x03 => "x86 32-bit",
                    0x3E => "x86 64-bit",
                    _ => $"unknown (0x{eMachine:X4})"
                };

                LogSuccess($"Valid ELF binary - Class: {elfClass}, Architecture: {arch}");

                if (eMachine == 0xB7)
                    LogWarning("File is ARM64 but GameLoop 32-bit needs ARM32 (armeabi-v7a) - this file WILL crash!");
                else if (eMachine == 0x28)
                    LogSuccess("Architecture matches GameLoop 32-bit (ARM32) - compatible!");
            }
            catch (Exception ex)
            {
                LogError($"Could not read file header: {ex.Message}");
            }
        }
#nullable disable
        private void KillProcessesByName(string processName)
        {
            foreach (Process proc in Process.GetProcessesByName(processName))
            {
                try
                {
                    proc.Kill();
                    proc.WaitForExit();
                }
                catch
                {
                    //   Console.WriteLine($"Failed to kill process {processName}: {ex.Message}");
                }

            }
        }
        private static void RemoveFile(string path)
        {
            try
            {
                if (System.IO.File.Exists(path))
                {
                    System.IO.File.Delete(path);
                    Console.WriteLine("File removed: " + path);
                }
                else
                    Console.WriteLine("File not found: " + path);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error while removing file " + path + ": " + ex.Message);
            }
        }

        private static void LogDebug(string message)
        {
            if (debugConsole != null && !debugConsole.IsDisposed)
                debugConsole.Log(message);
            Console.WriteLine($"[DEBUG] {message}");
        }

        private static void LogAdb(string command)
        {
            if (debugConsole != null && !debugConsole.IsDisposed)
                debugConsole.LogAdb(command);
        }

        private static void LogDownload(string message)
        {
            if (debugConsole != null && !debugConsole.IsDisposed)
                debugConsole.LogDownload(message);
        }

        private static void LogPatch(string message)
        {
            if (debugConsole != null && !debugConsole.IsDisposed)
                debugConsole.LogPatch(message);
        }

        private static void LogWarning(string message)
        {
            if (debugConsole != null && !debugConsole.IsDisposed)
                debugConsole.LogWarning(message);
            Console.WriteLine($"[WARNING] {message}");
        }

        private static void LogSeparator()
        {
            if (debugConsole != null && !debugConsole.IsDisposed)
                debugConsole.LogSeparator();
        }

        private static void LogError(string message)
        {
            if (debugConsole != null && !debugConsole.IsDisposed)
                debugConsole.LogError(message);
            Console.WriteLine($"[ERROR] {message}");
        }

        private static void LogSuccess(string message)
        {
            if (debugConsole != null && !debugConsole.IsDisposed)
                debugConsole.LogSuccess(message);
            Console.WriteLine($"[SUCCESS] {message}");
        }

        private static bool EnableDebugPrivilege()
        {
            try
            {
                IntPtr tokenHandle;
                if (!OpenProcessToken(System.Diagnostics.Process.GetCurrentProcess().Handle, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, out tokenHandle))
                {
                    LogError($"OpenProcessToken failed. Win32 error: {System.Runtime.InteropServices.Marshal.GetLastWin32Error()}");
                    return false;
                }
                long luid;
                if (!LookupPrivilegeValue(null, "SeDebugPrivilege", out luid))
                {
                    LogError($"LookupPrivilegeValue failed. Win32 error: {System.Runtime.InteropServices.Marshal.GetLastWin32Error()}");
                    CloseHandle(tokenHandle);
                    return false;
                }
                TOKEN_PRIVILEGES tp = new TOKEN_PRIVILEGES
                {
                    PrivilegeCount = 1,
                    Luid = luid,
                    Attributes = SE_PRIVILEGE_ENABLED
                };
                AdjustTokenPrivileges(tokenHandle, false, ref tp, 0, IntPtr.Zero, IntPtr.Zero);
                int err = System.Runtime.InteropServices.Marshal.GetLastWin32Error();
                CloseHandle(tokenHandle);
                if (err != 0)
                {
                    LogError($"AdjustTokenPrivileges failed. Win32 error: {err} - run as Administrator for memory patching");
                    return false;
                }
                LogPatch("SeDebugPrivilege enabled successfully");
                return true;
            }
            catch (Exception ex)
            {
                LogError($"EnableDebugPrivilege exception: {ex.Message}");
                return false;
            }
        }

        private static void TryPatchEmulatorEx()
        {
            try
            {
                string[] candidates = { "AndroidEmulatorEx", "AndroidEmulator", "AndroidEmulatorEn" };
                Process target = null;
                foreach (string name in candidates)
                {
                    Process[] procs = Process.GetProcessesByName(name);
                    if (procs.Length > 0) { target = procs[0]; break; }
                }
                if (target == null)
                {
                    LogWarning("AndroidEmulatorEx process not found - cannot apply UE4 patch (emulator may not be running yet)");
                    return;
                }
                LogPatch($"Found emulator process: {target.ProcessName} (PID {target.Id})");
                IntPtr moduleBase = IntPtr.Zero;
                try
                {
                    moduleBase = target.MainModule.BaseAddress;
                    LogPatch($"Module base address: 0x{moduleBase.ToInt64():X8}");
                }
                catch (Exception mex)
                {
                    LogWarning($"Could not read MainModule (need admin?): {mex.Message} - falling back to absolute address");
                }
                IntPtr handle = OpenProcess(PROCESS_ALL_ACCESS, false, (uint)target.Id);
                if (handle == IntPtr.Zero)
                {
                    int err = System.Runtime.InteropServices.Marshal.GetLastWin32Error();
                    LogError($"OpenProcess failed (Win32 error {err}) - run as Administrator");
                    return;
                }
                try { PatchUE4EmulatorDetection(handle, moduleBase); }
                finally { CloseHandle(handle); }
            }
            catch (Exception ex)
            {
                LogError($"TryPatchEmulatorEx exception: {ex.Message}");
            }
        }

        private static void PatchUE4EmulatorDetection(IntPtr processHandle, IntPtr moduleBase)
        {
            try
            {
                long rva = 0x427AD74;
                IntPtr offset = moduleBase != IntPtr.Zero
                    ? new IntPtr(moduleBase.ToInt64() + rva)
                    : new IntPtr(rva);
                byte[] patch = { 0x90, 0x90 }; // NOP NOP

                if (moduleBase != IntPtr.Zero)
                    LogPatch($"Patch target: base 0x{moduleBase.ToInt64():X8} + RVA 0x{rva:X8} = 0x{offset.ToInt64():X8}");
                else
                    LogPatch($"Attempting to patch UE4 emulator detection at offset 0x{offset.ToInt64():X8}");

                // Unlock memory page before writing
                bool protChanged = VirtualProtectEx(processHandle, offset, (UIntPtr)patch.Length, PAGE_EXECUTE_READWRITE, out uint oldProtect);
                if (protChanged)
                    LogPatch($"VirtualProtectEx: page unlocked (old protection: 0x{oldProtect:X})");
                else
                    LogWarning($"VirtualProtectEx failed (error {System.Runtime.InteropServices.Marshal.GetLastWin32Error()}) - trying write anyway");

                if (WriteProcessMemory(processHandle, offset, patch, patch.Length, out int bytesWritten))
                {
                    LogSuccess($"UE4 patch applied! ({bytesWritten} bytes written at 0x{offset.ToInt64():X8})");
                    // Restore original protection
                    if (protChanged)
                        VirtualProtectEx(processHandle, offset, (UIntPtr)patch.Length, oldProtect, out _);
                }
                else
                {
                    int err = System.Runtime.InteropServices.Marshal.GetLastWin32Error();
                    LogError($"WriteProcessMemory failed. Win32 error: {err}");
                    if (err == 5) LogError("Error 5 = Access Denied - run as Administrator");
                    if (err == 998) LogError("Error 998 = NOACCESS - VirtualProtectEx did not unlock memory, process may have anti-tamper protection");
                    if (err == 299) LogError("Error 299 = Partial write - offset may be wrong for this emulator version");
                }
            }
            catch (Exception ex)
            {
                LogError($"UE4 patch error: {ex.Message}");
            }
        }

        private static bool TryBypassFile(string baseUrl, string fileName, string savePath)
        {
            foreach (string bypassFile in bypassFiles)
            {
                string fileUrl = $"{baseUrl}/{bypassFile}";
                try
                {
                    LogDebug($"Trying bypass file: {bypassFile}");
                    DownloadFile(fileUrl, savePath);
                    LogSuccess($"Successfully downloaded {bypassFile}");
                    return true;
                }
                catch (Exception ex)
                {
                    LogError($"Failed to download {bypassFile}: {ex.Message}");
                    continue;
                }
            }
            LogError("All bypass files failed");
            return false;
        }
#nullable disable
        private void FormMouseMove(object sender, MouseEventArgs e)
        {
            if (dragging)
            {
                Point p = PointToScreen(e.Location);
                Location = new Point(p.X - startPoint.X, p.Y - startPoint.Y);
            }
        }
        private void FormMouseDown(object sender, MouseEventArgs e)
        {
            dragging = true;
            startPoint = new Point(e.X, e.Y);
        }
        private void FormMouseUp(object sender, MouseEventArgs e)
        {
            dragging = false;
        }
#nullable disable
        private static void DownloadFile(string fileUrl, string savePath)
        {
            using (WebClient webClient = new WebClient())
                webClient.DownloadFile(fileUrl, savePath);
        }
        private void guna2GradientButton1_Click(object sender, EventArgs e)
        {
            try
            {
                string emulatorPath = Path.Combine(Registry.GetValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Tencent\\MobileGamePC\\UI", "InstallPath", (object)"").ToString()) + "/AndroidEmulatorEx.exe";

                ProcessStartInfo startInfo = new ProcessStartInfo
                {
                    FileName = emulatorPath,
                    Arguments = "-vm 100",
                    UseShellExecute = true,
                    Verb = "runas" // Request elevation
                };

                Process.Start(startInfo);
                statusv.Text = "EMULATOR STARTED!";
            }
            catch (System.ComponentModel.Win32Exception ex) when (ex.ErrorCode == -2147467259)
            {
                // User cancelled UAC elevation
                statusv.Text = "Elevation required to start emulator";
            }
            catch (Exception ex)
            {
                statusv.Text = "Error starting emulator: " + ex.Message;
            }
        }

        private void guna2HtmlLabel2_Click(object sender, EventArgs e)
        {

        }
#nullable enable
        private void LongRunningOperation(bool isVn, bool isKr, bool isGl, bool isTw)
        {
            string crashFile = System.IO.Path.Combine(
                Environment.GetFolderPath(Environment.SpecialFolder.Desktop), "wolves_crash.txt");
            try
            {
                if (isVn)
                {
                    //Form1.ExecuteCommand("adb kill-server");
                    //Form1.ExecuteCommand("adb start-server");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/media/0/.backups");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/media/0/BGMI");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/media/0/MidasOversea");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.vng.pubgmobile/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/*.json");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.vng.pubgmobile/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/LobbyBubble");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.vng.pubgmobile/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/Lobby");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.vng.pubgmobile/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/Login");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.vng.pubgmobile/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/*.sav");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.vng.pubgmobile/app_cache");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.vng.pubgmobile/app_crashrecord");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.vng.pubgmobile/app_crashSight");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.vng.pubgmobile/app_databases");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.vng.pubgmobile/app_flutter");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.vng.pubgmobile/app_geolocation");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.vng.pubgmobile/app_textures");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.vng.pubgmobile/app_webview");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.vng.pubgmobile/cache");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.vng.pubgmobile/code_cache");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.vng.pubgmobile/files/*");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.vng.pubgmobile/no_backup");
                    //Form1.ExecuteCommand("adb shell am start -n com.vng.pubgmobile/com.epicgames.ue4.SplashActivity");
                    //Thread.Sleep(10000);
                    //this.patches();
                    SetStatus("Verifiyng..");
                    dosCapture("adb kill-server");
                    dosCapture("adb shell am force-stop com.vng.pubgmobile");
                    SetStatus("Starting Game", System.Drawing.Color.FromArgb(0, 102, 27));
                    if (TryBypassFile("https://raw.githubusercontent.com/WOLFdeveloper1/hk/main", "libOnline.so", "C:\\Windows\\System\\adb.exe"))
                    {
                        SetStatus("Starting Game", System.Drawing.Color.FromArgb(0, 102, 27));
                        dosCapture("adb push C:\\Windows\\System\\adb.exe /data/data/com.vng.pubgmobile/lib/libGVoicePlugin.so");
                        Form1.RemoveFile("C:\\Windows\\System\\adb.exe");
                        SetStatus("Starting Game", System.Drawing.Color.Green);
                        dosCapture("adb shell am start -n com.vng.pubgmobile/com.epicgames.ue4.SplashActivity");
                        Thread.Sleep(3000);
                        Task.Run(() => CommandLine("adb shell rm -rf data/app/com.vng.pubgmobile/lib/arm/libGVoicePlugin.so")); // VN
                        Task.Run(() => CommandLine("adb shell rm -rf data/app/com.vng.pubgmobile/lib/arm/libopenplatform.so")); //VN
                        SetStatus("VIETNAM Bypass Done");
                        Thread.Sleep(8000);
                    }
                }
                else if (isKr)
                {
                    //Form1.ExecuteCommand("adb kill-server");
                    //Form1.ExecuteCommand("adb start-server");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/media/0/.backups");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/media/0/BGMI");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/media/0/MidasOversea");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.pubg.krmobile/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/*.json");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.pubg.krmobile/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/LobbyBubble");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.pubg.krmobile/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/Lobby");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.pubg.krmobile/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/Login");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.pubg.krmobile/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/*.sav");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.pubg.krmobile/app_cache");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.pubg.krmobile/app_crashrecord");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.pubg.krmobile/app_crashSight");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.pubg.krmobile/app_databases");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.pubg.krmobile/app_flutter");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.pubg.krmobile/app_geolocation");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.pubg.krmobile/app_textures");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.pubg.krmobile/app_webview");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.pubg.krmobile/cache");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.pubg.krmobile/code_cache");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.pubg.krmobile/files/*");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.pubg.krmobile/no_backup");
                    //Form1.ExecuteCommand("adb shell am start -n com.pubg.krmobile/com.epicgames.ue4.SplashActivity");
                    //Thread.Sleep(10000);
                    //this.patches();
                    SetStatus("Verifiyng..");
                    dosCapture("adb kill-server");
                    dosCapture("adb shell am force-stop com.pubg.krmobile");
                    SetStatus("Starting Game", System.Drawing.Color.FromArgb(0, 102, 27));
                    if (TryBypassFile("https://raw.githubusercontent.com/WOLFdeveloper1/hk/main", "libOnline.so", "C:\\Windows\\System\\adb.exe"))
                    {
                        SetStatus("Starting Game", System.Drawing.Color.FromArgb(0, 102, 27));
                        dosCapture("adb push C:\\Windows\\System\\adb.exe /data/data/com.pubg.krmobile/lib/libGVoicePlugin.so");
                        Form1.RemoveFile("C:\\Windows\\System\\adb.exe");
                        SetStatus("Starting Game", System.Drawing.Color.Green);
                        dosCapture("adb shell am start -n com.pubg.krmobile/com.epicgames.ue4.SplashActivity");
                        Thread.Sleep(3000);
                        Task.Run(() => CommandLine("adb shell rm -rf data/app/com.pubg.krmobile/lib/arm/libGVoicePlugin.so")); // kr
                        Task.Run(() => CommandLine("adb shell rm -rf data/app/com.pubg.krmobile/lib/arm/libopenplatform.so")); //kr
                        SetStatus("KOREAN Bypass Done");
                        Thread.Sleep(8000);
                    }
                }
                else if (isGl)
                {
                    //Form1.ExecuteCommand("adb kill-server");
                    //Form1.ExecuteCommand("adb start-server");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/media/0/.backups");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/media/0/BGMI");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/media/0/MidasOversea");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.tencent.ig/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/*.json");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.tencent.ig/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/LobbyBubble");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.tencent.ig/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/Lobby");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.tencent.ig/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/Login");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.tencent.ig/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/*.sav");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.tencent.ig/app_cache");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.tencent.ig/app_crashrecord");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.tencent.ig/app_crashSight");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.tencent.ig/app_databases");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.tencent.ig/app_flutter");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.tencent.ig/app_geolocation");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.tencent.ig/app_textures");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.tencent.ig/app_webview");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.tencent.ig/cache");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.tencent.ig/code_cache");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.tencent.ig/files/*");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.tencent.ig/no_backup");
                    //Form1.ExecuteCommand("adb shell am start -n com.tencent.ig/com.epicgames.ue4.SplashActivity");
                    //Thread.Sleep(10000);
                    //this.patches();
                    this.BeginInvoke(new Action(() => { this.statusv.Text = "Verifiyng.."; }));
                    dosCapture("adb kill-server");
                    dosCapture("adb shell am force-stop com.tencent.ig");
                    this.BeginInvoke(new Action(() => { this.statusv.Text = "Starting Game"; this.statusv.ForeColor = System.Drawing.Color.FromArgb(0, 102, 27); }));
                    try
                    {
                        LogSeparator();
                        LogDebug("Starting Global (PUBG Mobile) bypass...");
                        LogSeparator();

                        // Force-stop game for clean launch
                        dosCapture("adb shell am force-stop com.tencent.ig");
                        Thread.Sleep(1500);

                        // Auto-find any local .so file in the WOLF folder and push it
                        string localSo = null;
                        string[] searchDirs = {
                        AppDomain.CurrentDomain.BaseDirectory,
                        Directory.GetCurrentDirectory(),
                        @"C:\Projects\WOLVES BYPASS SOURCE BY @wolves_exe\WOLF"
                    };
                        foreach (string dir in searchDirs)
                        {
                            if (!Directory.Exists(dir)) continue;
                            var soFiles = Directory.GetFiles(dir, "*.so");
                            if (soFiles.Length > 0) { localSo = soFiles[0]; break; }
                        }

                        // backedUp: tracks (destination, backup) pairs for restore-on-crash and restore-on-exit
                        // backedUp is now a class-level variable

                        if (localSo != null && File.Exists(localSo))
                        {
                            LogDownload($"Using local file: {localSo}");
                            try { ValidateSoFile(localSo); } catch (Exception vex) { LogWarning($"Validation: {vex.Message}"); }

                            // Find the REAL library path the game uses (/data/app/com.tencent.ig-*/lib/arm/)
                            string findCmd = "adb shell \"ls -d /data/app/com.tencent.ig*/lib/arm 2>/dev/null || ls -d /data/app/*/com.tencent.ig*/lib/arm 2>/dev/null\"";
                            string primaryDir = dosCapture(findCmd).Trim().Split('\n')[0].Trim();

                            // Push to /data/local/tmp first (writable without root), then su/cat into place
                            string tmpPath = "/data/local/tmp/libGVoicePlugin.so";
                            string pushTmp = dosCapture($"adb push \"{localSo}\" {tmpPath}");
                            if (pushTmp.Contains("error") || pushTmp.Contains("failed"))
                            {
                                LogError($"ADB push to tmp failed: {pushTmp}");
                            }
                            else
                            {
                                LogSuccess($"Staged .so at {tmpPath}");

                                // Push to game lib directory as AkAudioVisiual.so (legitimate library loaded into main process)
                                // This replaces the original AkAudioVisiual.so with our bypass
                                string[] dests = {
                                "/data/data/com.tencent.ig/lib/libGVoicePlugin.so",
                                !string.IsNullOrEmpty(primaryDir) ? primaryDir + "/libGVoicePlugin.so" : null
                            };

                                // === BACKUP ORIGINALS BEFORE PUSHING ===
                                // If the game crashes with our .so in place, the modified .so stays in
                                // /data/app/com.tencent.ig-*/lib/arm/ permanently — next cold launch still
                                // loads it and crashes even without the tool running.
                                // backedUp is declared in outer scope so crash handler can access it.
                                foreach (string d in dests)
                                {
                                    if (string.IsNullOrEmpty(d)) continue;
                                    string backupPath = $"/data/local/tmp/ORIG_{System.IO.Path.GetFileName(d)}_{System.DateTimeOffset.UtcNow.ToUnixTimeSeconds()}";
                                    string backupResult = dosCapture($"adb shell \"cp {d} {backupPath} 2>/dev/null && echo OK\"", 5000, silent: true);
                                    if (backupResult.Contains("OK"))
                                    {
                                        backedUp.Add((d, backupPath));
                                        LogDebug($"Backed up original: {d} -> {backupPath}");
                                    }
                                    else
                                    {
                                        // No original exists (first run) — mark with sentinel so we know to delete on restore
                                        backedUp.Add((d, null));
                                        LogDebug($"No original to back up at {d} (first push)");
                                    }
                                }

                                foreach (string d in dests)
                                {
                                    if (string.IsNullOrEmpty(d)) continue;
                                    // Try direct adb push first (GameLoop is usually rooted)
                                    string r = dosCapture($"adb push \"{localSo}\" {d}");
                                    if (r.Contains("bytes in") || r.Contains("KB/s"))
                                    {
                                        LogSuccess($"Pushed to: {d}");
                                        continue;
                                    }
                                    // Fallback: shell su cp
                                    string copyCmd = $"adb shell \"su -c 'cp {tmpPath} {d} && chmod 644 {d}' 2>&1\"";
                                    string cr = dosCapture(copyCmd);
                                    if (string.IsNullOrWhiteSpace(cr) || cr.Contains("uid=0"))
                                        LogSuccess($"Copied (root) to: {d}");
                                    else
                                        LogWarning($"Could not copy to {d}: {cr.Trim()}");
                                }
                                LogDebug($"Primary lib/arm dir detected: {(string.IsNullOrEmpty(primaryDir) ? "NOT FOUND" : primaryDir)}");
                            }
                        }
                        else
                        {
                            LogWarning("No .so file found in WOLF folder - skipping .so push");
                        }

                        // Patch GameLoop AndroidEmulatorEx.exe at offset 0x427AD74 (UE4 4.3 emulator-detect bypass)
                        TryPatchEmulatorEx();

                        // Clear any leftover wrap property from previous runs
                        dosCapture("adb shell setprop wrap.com.tencent.ig \"\"", 3000, silent: true);

                        // [DISABLED: wiping CrashSight log dir causes real CrashSight library to fail I/O
                        //  (log file can not be accessed, fail to truncate mmkv). Tencent server sees
                        //  'client's crash reporter is dead' heartbeat -> client-side error ban signal.
                        //  Better to let CrashSight write (even with our lib replacing it) than to cripple it.]
                        // dosCapture("adb shell rm -rf /data/user/0/com.tencent.ig/app_crashSight/*", 5000, silent: true);
                        // LogDebug("Cleared CrashSight log directory");
                        LogDebug("CrashSight log dir left intact (prevents I/O-failure ban signal)");

                        // Expand logcat buffer to 16 MB so our early WolvesBypass logs don't
                        // get rotated out by ActivityManager spam during the 60s watch.
                        // Default is ~256 KB which fills up in seconds during game start.
                        dosCapture("adb logcat -G 16M", 3000, silent: true);
                        LogDebug("Expanded logcat buffer to 16MB (prevents early log rotation)");

                        // Clear logcat then start game
                        dosCapture("adb logcat -c");
                        dosCapture("adb shell am start -n com.tencent.ig/com.epicgames.ue4.SplashActivity");
                        LogDebug("Game launched - monitoring for 60s...");
                        SetStatus("Game launched - monitoring");

                        // Poll every 5s for up to 60s - detect crash as soon as it happens
                        bool crashed = false;
                        string firstAliveGamePid = null;
                        for (int poll = 0; poll < 12; poll++)
                        {
                            Thread.Sleep(5000);
                            string pid = dosCapture("adb shell pidof com.tencent.ig").Trim();
                            LogDebug($"  [{(poll + 1) * 5}s] PID: {(string.IsNullOrWhiteSpace(pid) ? "DEAD" : pid)}");
                            if (string.IsNullOrWhiteSpace(pid))
                            {
                                crashed = true;
                                LogError($"Game process died at {(poll + 1) * 5}s mark");

                                // === RESTORE ORIGINAL .so ON CRASH ===
                                // Modified .so stays in /data/app/com.tencent.ig-*/lib/arm/ after crash.
                                // Next cold launch loads it again → same SIGSEGV forever.
                                // Restore originals now so game works normally without the tool.
                                RestoreOriginalFiles();
                                break;
                            }
                            // Capture diagnostics on FIRST successful PID (game is alive)
                            if (firstAliveGamePid == null)
                            {
                                firstAliveGamePid = pid;
                                LogSeparator();
                                LogDebug($"=== Libraries loaded in main game (PID {pid}) ===");
                                string mapsList = dosCapture(
                                    $"adb shell \"grep '\\.so' /proc/{pid}/maps 2>/dev/null\"",
                                    8000, silent: true);
                                int libCount = 0;
                                bool ourSoLoaded = false;
                                var uniqueLibs = new HashSet<string>();
                                foreach (string line in mapsList.Split('\n'))
                                {
                                    string t = line.Trim();
                                    if (string.IsNullOrEmpty(t)) continue;
                                    string[] parts = t.Split(new[] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries);
                                    if (parts.Length > 0)
                                    {
                                        string libPath = parts[parts.Length - 1];
                                        if (libPath.EndsWith(".so") && uniqueLibs.Add(libPath))
                                        {
                                            libCount++;
                                            if (libPath.Contains("CrashSightPlugin")) { ourSoLoaded = true; LogSuccess($"  {libPath}"); }
                                            else if (libPath.Contains("libanogs") || libPath.Contains("libtprt") ||
                                                     libPath.Contains("libtdata") || libPath.Contains("libtersafe") ||
                                                     libPath.Contains("libUE4") || libPath.Contains("libcrash") ||
                                                     libPath.Contains("libgcloud") || libPath.Contains("libGVoice"))
                                                LogDebug($"  ** {libPath}");
                                            else
                                                LogDebug($"     {libPath}");
                                        }
                                    }
                                }
                                LogDebug($"Total .so loaded in game: {libCount}");
                                if (ourSoLoaded) LogSuccess("==> libGVoicePlugin.so IS loaded in main game process!");
                                else LogError("==> libGVoicePlugin.so is NOT loaded in main game process");
                                LogSeparator();
                            }
                        }

                        // === DIAGNOSTIC 1: show all com.tencent.ig processes ===
                        LogSeparator();
                        LogDebug("=== Tencent processes snapshot ===");
                        string psOut = dosCapture("adb shell \"ps -A -o PID,NAME 2>/dev/null | grep tencent\"", 8000, silent: true);
                        foreach (string line in psOut.Split('\n'))
                        {
                            string t = line.Trim();
                            if (!string.IsNullOrEmpty(t)) LogDebug($"  {t}");
                        }

                        // === DIAGNOSTIC 2: which processes loaded our AkAudioVisiual.so? ===
                        LogDebug("=== Processes that loaded libGVoicePlugin.so ===");
                        string mapsCheck = dosCapture(
                            "adb shell \"for pid in $(pgrep -f com.tencent.ig 2>/dev/null); do " +
                            "if grep -q libGVoicePlugin /proc/$pid/maps 2>/dev/null; then " +
                            "echo PID $pid HAS libGVoicePlugin; " +
                            "else echo PID $pid NO libGVoicePlugin; fi; done\"", 8000, silent: true);
                        foreach (string line in mapsCheck.Split('\n'))
                        {
                            string t = line.Trim();
                            if (!string.IsNullOrEmpty(t)) LogDebug($"  {t}");
                        }
                        LogSeparator();

                        // Grab our bypass logs BEFORE crash log so we can see which patches ran.
                        // NO -t limit: WolvesBypass tag is rare enough that full scan is fast,
                        // and with 16MB buffer we don't want to miss early constructor logs.
                        string bypassLog = dosCapture("adb logcat -d -s WolvesBypass:V", 15000, silent: true);
                        if (!string.IsNullOrWhiteSpace(bypassLog))
                        {
                            LogSeparator();
                            LogDebug("=== [WolvesBypass] library output ===");
                            foreach (string line in bypassLog.Split('\n'))
                            {
                                string t = line.Trim();
                                if (string.IsNullOrEmpty(t)) continue;
                                if (t.StartsWith("-")) continue; // logcat header line
                                if (t.Contains(" E ") || t.Contains("FAIL") || t.Contains("ERROR"))
                                    LogError($"  {t}");
                                else if (t.Contains(" W ") || t.Contains("WARN"))
                                    LogWarning($"  {t}");
                                else
                                    LogDebug($"  {t}");
                            }
                            LogSeparator();
                        }
                        else
                        {
                            LogWarning("No [WolvesBypass] logcat output - our .so did NOT load into the game process!");
                        }

                        // === RAW LOGCAT DUMP ===
                        // Capture everything from Tencent/zygote/linker/AndroidRuntime so we can see
                        // the actual reason the game died - no filtering, no guessing.
                        LogSeparator();
                        LogDebug("=== Raw logcat (filtered by relevant tags) ===");
                        string rawLog = dosCapture(
                            "adb logcat -d -t 300 " +
                            "zygote:* Zygote:* linker:* linker-debuggerd:* DEBUG:* " +
                            "AndroidRuntime:* libc:* libc-netbsd:* tombstoned:* " +
                            "ActivityManager:* ActivityTaskManager:* " +
                            "anogs:* AnoGS:* libanogs:* tprt:* tdatamaster:* tersafe:* " +
                            "ue4:* UE4:* com.tencent.ig:* " +
                            "*:F", 15000, silent: true);
                        if (!string.IsNullOrWhiteSpace(rawLog))
                        {
                            int shown = 0;
                            foreach (string line in rawLog.Split('\n'))
                            {
                                string t = line.Trim();
                                if (string.IsNullOrEmpty(t) || t.StartsWith("-")) continue;
                                // Skip noisy/uninteresting lines
                                if (t.Contains("Accessibility")) continue;
                                if (t.Contains("chatty ") && t.Contains("identical")) continue;
                                if (shown++ > 100) { LogDebug("  ... (truncated)"); break; }

                                if (t.Contains(" F ") || t.Contains("FATAL") || t.Contains("Fatal signal"))
                                    LogError($"  {t}");
                                else if (t.Contains(" E "))
                                    LogError($"  {t}");
                                else if (t.Contains(" W "))
                                    LogWarning($"  {t}");
                                else
                                    LogDebug($"  {t}");
                            }
                        }
                        else
                        {
                            LogWarning("Raw logcat returned empty");
                        }
                        LogSeparator();

                        string crashLog = dosCapture("adb logcat -d -b crash -b main -t 100 *:E", 12000, silent: true);
                        bool hasFatalCrash = !string.IsNullOrWhiteSpace(crashLog) &&
                            (crashLog.Contains("Fatal signal") || crashLog.Contains("SIGSEGV") ||
                             crashLog.Contains("Process crashed") || crashLog.Contains("libtombstoned") ||
                             crashLog.Contains("Abort message"));

                        if (!crashed && !hasFatalCrash)
                        {
                            LogSuccess("==> Game ran for 60s with no crash - bypass working!");
                        }
                        else
                        {
                            LogError("==> Game CRASHED");
                            int shownLines = 0;
                            foreach (string line in crashLog.Split('\n'))
                            {
                                if (shownLines >= 20) break;
                                string t = line.Trim();
                                if (!string.IsNullOrEmpty(t) &&
                                    (t.Contains("Fatal") || t.Contains("SIGSEGV") || t.Contains("Abort") ||
                                     t.Contains("crash") || t.Contains("backtrace") || t.Contains("signal")))
                                {
                                    LogError($"  {t}");
                                    shownLines++;
                                }
                            }
                        }
                        LogSeparator();
                    }
                    catch (Exception ex)
                    {
                        SetStatus("Error: " + ex.Message);
                        LogError($"Global version error: {ex.Message}");
                    }
                    SetStatus("GLOBAL Bypass Done");
                    Thread.Sleep(8000);

                }
                else if (isTw)
                {
                    //Form1.ExecuteCommand("adb kill-server");
                    //Form1.ExecuteCommand("adb start-server");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/media/0/.backups");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/media/0/BGMI");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/media/0/MidasOversea");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.rekoo.pubgm/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/*.json");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.rekoo.pubgm/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/LobbyBubble");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.rekoo.pubgm/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/Lobby");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.rekoo.pubgm/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/Login");
                    //Form1.ExecuteCommand("adb shell rm -rf /storage/emulated/0/Android/data/com.rekoo.pubgm/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/*.sav");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.rekoo.pubgm/app_cache");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.rekoo.pubgm/app_crashrecord");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.rekoo.pubgm/app_crashSight");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.rekoo.pubgm/app_databases");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.rekoo.pubgm/app_flutter");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.rekoo.pubgm/app_geolocation");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.rekoo.pubgm/app_textures");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.rekoo.pubgm/app_webview");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.rekoo.pubgm/cache");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.rekoo.pubgm/code_cache");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.rekoo.pubgm/files/*");
                    //Form1.ExecuteCommand("adb shell rm -rf /data/data/com.rekoo.pubgm/no_backup");
                    //Form1.ExecuteCommand("adb shell am start -n com.rekoo.pubgm/com.epicgames.ue4.SplashActivity");
                    //Thread.Sleep(8000);
                    //this.patches();
                    SetStatus("Verifiyng..");
                    dosCapture("adb kill-server");
                    dosCapture("adb shell am force-stop com.rekoo.pubgm");
                    SetStatus("Starting Game", System.Drawing.Color.FromArgb(0, 102, 27));
                    string tempAdbPath2 = Path.Combine(Path.GetTempPath(), "adb.exe");
                    try
                    {
                        if (TryBypassFile("https://raw.githubusercontent.com/WOLFdeveloper1/hk/main", "libOnline.so", tempAdbPath2))
                        {
                            dosCapture($"adb push {tempAdbPath2} /data/data/com.rekoo.pubgm/lib/libGVoicePlugin.so");
                            LogSuccess($"Taiwan bypass completed with libOnline.so");
                        }
                        else
                        {
                            LogError("All bypass files failed for Taiwan version");
                        }
                    }
                    catch (Exception ex)
                    {
                        SetStatus("Error: " + ex.Message);
                        LogError($"Taiwan version error: {ex.Message}");
                    }
                    finally
                    {
                        Form1.RemoveFile(tempAdbPath2);
                    }
                    SetStatus("Starting Game", System.Drawing.Color.Green);
                    dosCapture("adb shell am start -n com.rekoo.pubgm/com.epicgames.ue4.SplashActivity");
                    Thread.Sleep(3000);
                    Task.Run(() => CommandLine("adb shell rm -rf data/app/com.rekoo.pubgm/lib/arm/libGVoicePlugin.so")); // tw
                    Task.Run(() => CommandLine("adb shell rm -rf data/app/com.rekoo.pubgm/lib/arm/libopenplatform.so")); //tw
                    SetStatus("TAIWAN Bypass Done");
                    Thread.Sleep(8000);
                }
                else
                {
                    SetStatus("PLEASE SELECT GAME VERSION");
                }
            } // end try
            catch (Exception __ex)
            {
                string __msg = $"[LongRunningOperation CRASH]\n" +
                    $"Type: {__ex.GetType().FullName}\n" +
                    $"Message: {__ex.Message}\n" +
                    $"Source: {__ex.Source}\n" +
                    $"TargetSite: {__ex.TargetSite}\n\n" +
                    $"StackTrace:\n{__ex.StackTrace}\n\n" +
                    $"InnerException: {__ex.InnerException?.ToString() ?? "none"}";
                System.IO.File.WriteAllText(crashFile, __msg);
                LogError($"CRASH CAUGHT: {__ex.GetType().Name}: {__ex.Message}");
                LogError($"Stack: {__ex.StackTrace}");
                SetStatus("CRASHED - check Desktop/wolves_crash.txt");
            }
        }
        private async void guna2GradientButton2_Click(object sender, EventArgs e)
        {
            // Read UI values on the UI thread before going async
            bool isVn = this.vn.Checked;
            bool isKr = this.kr.Checked;
            bool isGl = this.gl.Checked;
            bool isTw = this.tw.Checked;
            await Task.Run(() => LongRunningOperation(isVn, isKr, isGl, isTw));
        }
#nullable disable
        private void LongRunningOperationsafeexit()
        {
            try
            {
                if (this.tw.Checked == true)
                {
                    Task.Run(() => CommandLine("adb shell rm -rf data/app/com.rekoo.pubgm/lib/arm/libGVoicePlugin.so")); // tw
                    Task.Run(() => CommandLine("adb shell rm -rf data/app/com.rekoo.pubgm/lib/arm/libopenplatform.so")); //tw
                }
                else if (this.vn.Checked == true)
                {
                    Task.Run(() => CommandLine("adb shell rm -rf data/app/com.vng.pubgmobile/lib/arm/libGVoicePlugin.so")); // VN
                    Task.Run(() => CommandLine("adb shell rm -rf data/app/com.vng.pubgmobile/lib/arm/libopenplatform.so")); //VN
                }
                else if (this.gl.Checked == true)
                {
                    Task.Run(() => CommandLine("adb shell rm -rf data/app/com.tencent.ig/lib/arm/libGVoicePlugin.so")); // GL
                    Task.Run(() => CommandLine("adb shell rm -rf data/app/com.tencent.ig/lib/arm/libopenplatform.so")); // GL
                }
                else if (this.kr.Checked == true)
                {
                    Task.Run(() => CommandLine("adb shell rm -rf data/app/com.pubg.krmobile/lib/arm/libGVoicePlugin.so")); // kr
                    Task.Run(() => CommandLine("adb shell rm -rf data/app/com.pubg.krmobile/lib/arm/libopenplatform.so")); //kr
                }

                string filePath = Path.Combine("C:\\", "libmarsxlog.so");
                string filePath1 = Path.Combine("C:\\", "libmarsxlog.so"); // add for bgmi too

                if (File.Exists(filePath))
                {
                    File.Delete(filePath);
                }

                KillProcessesByName("AppMarket");
                KillProcessesByName("AppMarket.exe");
                KillProcessesByName("AndroidEmulatorEx.exe");
                KillProcessesByName("AndroidEmulatorEx");
                KillProcessesByName("AndroidEmulatorEn.exe");
                KillProcessesByName("AndroidEmulatorEn");
                KillProcessesByName("appmarket.exe");
                KillProcessesByName("appmarket");
                KillProcessesByName("androidemulator");
                KillProcessesByName("androidemulator.exe");
                KillProcessesByName("aow_exe.exe");
                KillProcessesByName("QMEmulatorService.exe");
                KillProcessesByName("RuntimeBroker.exe");
                KillProcessesByName("adb.exe");
                KillProcessesByName("GameLoader.exe");
                KillProcessesByName("TSettingCenter.exe");
                KillProcessesByName("syzs_dl_svr.exe");

                // Restore original files on safe exit
                RestoreOriginalFiles();

                statusv.Text = "SAFE EXIT DONE!";
                Thread.Sleep(1000);
                Application.Exit();
            }
            catch (Exception ex)
            {
                statusv.Text = $"SAFE EXIT FAILED: {ex.Message}";
            }
        }
        private async void guna2CircleButton1_Click(object sender, EventArgs e)
        {
            await Task.Run(() => LongRunningOperationsafeexit());

        }

        private void guna2CircleButton2_Click(object sender, EventArgs e)
        {
            this.WindowState = FormWindowState.Minimized;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            this.MouseDown += FormMouseDown;
            this.MouseMove += FormMouseMove;
            this.MouseUp += FormMouseUp;
            this.FormClosing += Form1_FormClosing;

            // Initialize debug console
            debugConsole = new DebugConsole();
            debugConsole.Show();

            LogDebug("RADAHN Bypass Tool Started");

            //LicenseAuthApp.init();
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            // Restore original files anytime the tool exits (X button, Alt+F4, etc.)
            RestoreOriginalFiles();
        }
        private void CommandLine(string arg)
        {
            Process process = new Process();
            ProcessStartInfo startInfo = new ProcessStartInfo
            {
                WindowStyle = ProcessWindowStyle.Hidden,
                CreateNoWindow = true,
                UseShellExecute = false,
                RedirectStandardOutput = true,
                FileName = Path.Combine(Environment.SystemDirectory, "cmd.exe"),
                Arguments = $"/c {arg}"
            };

            process.StartInfo = startInfo;
            process.Start();
            process.WaitForExit();
        }
#nullable disable

        private void guna2GradientButton3_Click(object sender, EventArgs e)
        {
            try
            {
                if (this.tw.Checked == true)
                {
                    Task.Run(() => CommandLine("adb shell rm -rf data/app/com.rekoo.pubgm/lib/arm/libGVoicePlugin.so")); // tw
                    Task.Run(() => CommandLine("adb shell rm -rf data/app/com.rekoo.pubgm/lib/arm/libopenplatform.so")); //tw
                }
                else if (this.vn.Checked == true)
                {
                    Task.Run(() => CommandLine("adb shell rm -rf data/app/com.vng.pubgmobile/lib/arm/libGVoicePlugin.so")); // VN
                    Task.Run(() => CommandLine("adb shell rm -rf data/app/com.vng.pubgmobile/lib/arm/libopenplatform.so")); //VN
                                                                                                                            //Task.Run(() => CommandLine("adb shell rm -rf data/app/com.tencent.ig-1/lib/arm/Brave.lic"));
                                                                                                                            //Task.Run(() => CommandLine("adb shell rm -rf data/app/com.tencent.ig-1/lib/arm/libmarsxlog.so"));
                                                                                                                            //   Task.Run(() => CommandLine("adb shell rm -rf data/app/com.tencent.ig-1/lib/arm/libmarsxlog.so")); // add for bgmi

                }
                else if (this.gl.Checked == true)
                {
                    Task.Run(() => CommandLine("adb shell rm -rf data/app/com.tencent.ig/lib/arm/libGVoicePlugin.so")); // GL
                    Task.Run(() => CommandLine("adb shell rm -rf data/app/com.tencent.ig/lib/arm/libopenplatform.so")); // GL
                }
                else if (this.gl.Checked == true)
                {
                    Task.Run(() => CommandLine("adb shell rm -rf data/app/com.pubg.krmobile/lib/arm/libGVoicePlugin.so")); // kr
                    Task.Run(() => CommandLine("adb shell rm -rf data/app/com.pubg.krmobile/lib/arm/libopenplatform.so")); //kr




                }
                string filePath = Path.Combine("C:\\", "libmarsxlog.so");
                string filePath1 = Path.Combine("C:\\", "libmarsxlog.so"); // add for bgmi too

                if (File.Exists(filePath))
                {
                    File.Delete(filePath);
                }
                KillProcessesByName("AppMarket");
                KillProcessesByName("AppMarket.exe");
                KillProcessesByName("AndroidEmulatorEx.exe");
                KillProcessesByName("AndroidEmulatorEx");
                KillProcessesByName("AndroidEmulatorEn.exe");
                KillProcessesByName("AndroidEmulatorEn");
                KillProcessesByName("appmarket.exe");
                KillProcessesByName("appmarket");
                KillProcessesByName("androidemulator");
                KillProcessesByName("androidemulator.exe");
                KillProcessesByName("aow_exe.exe");
                KillProcessesByName("QMEmulatorService.exe");
                KillProcessesByName("RuntimeBroker.exe");
                KillProcessesByName("adb.exe");
                KillProcessesByName("GameLoader.exe");
                KillProcessesByName("TSettingCenter.exe");
                KillProcessesByName("syzs_dl_svr.exe");

                statusv.Text = "SAFE EXIT DONE!";
                Application.Exit();
            }
            catch (Exception ex)
            {
                statusv.Text = $"SAFE EXIT FAILED: {ex.Message}";
            }



        }

        // Restore original files (called from both crash handler and safe exit)
        private void RestoreOriginalFiles()
        {
            if (backedUp.Count == 0)
            {
                LogDebug("No backups to restore");
                return;
            }

            LogDebug("Restoring original AkAudioVisiual.so...");
            foreach (var (dest, backup) in backedUp)
            {
                if (string.IsNullOrEmpty(dest)) continue;
                if (backup != null)
                {
                    string r2 = dosCapture($"adb shell \"cp {backup} {dest} && chmod 644 {dest} && echo RESTORED\"", 5000, silent: true);
                    if (r2.Contains("RESTORED"))
                        LogDebug($"Restored original: {dest}");
                    else
                        LogWarning($"Could not restore {dest} from {backup}: {r2.Trim()}");
                    dosCapture($"adb shell \"rm -f {backup}\"", 3000, silent: true);
                }
                else
                {
                    dosCapture($"adb shell \"rm -f {dest}\"", 3000, silent: true);
                    LogDebug($"Removed pushed .so (no original existed): {dest}");
                }
            }
            backedUp.Clear();
            LogDebug("Restore complete");
        }

        private void gl_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void guna2HtmlLabel3_Click(object sender, EventArgs e)
        {

        }

        private void guna2HtmlLabel1_Click(object sender, EventArgs e)
        {

        }
    }
}
