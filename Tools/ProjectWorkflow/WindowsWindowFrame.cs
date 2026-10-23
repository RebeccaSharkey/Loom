using System;
using System.Runtime.InteropServices;
using System.Windows.Interop;

namespace Loom.ProjectWorkflow
{
    public static class WindowsWindowFrame
    {
        public static void Attach(IntPtr window) { HwndSource.FromHwnd(window).AddHook(HandleMessage); }
        private static IntPtr HandleMessage(IntPtr window, int message, IntPtr wparam, IntPtr lparam, ref bool handled)
        {
            if (message == 0x0024) { ConstrainMaximize(window, lparam); handled = true; }
            return IntPtr.Zero;
        }
        [StructLayout(LayoutKind.Sequential)] public struct Rect { public int Left, Top, Right, Bottom; }
        [StructLayout(LayoutKind.Sequential)] private struct Point { public int X, Y; }
        [StructLayout(LayoutKind.Sequential)] private struct MinMaxInfo
        {
            public Point Reserved, MaxSize, MaxPosition, MinTrackSize, MaxTrackSize;
        }
        [StructLayout(LayoutKind.Sequential)] private struct MonitorInfo
        {
            public int Size;
            public Rect Monitor, Work;
            public uint Flags;
        }
        [DllImport("user32.dll")] private static extern IntPtr MonitorFromWindow(IntPtr window, uint flags);
        [DllImport("user32.dll", CharSet = CharSet.Auto)] private static extern bool GetMonitorInfo(IntPtr monitor, ref MonitorInfo info);
        [DllImport("user32.dll")] private static extern bool GetWindowRect(IntPtr window, out Rect rect);
        [DllImport("kernel32.dll")] public static extern IntPtr GetConsoleWindow();

        public static Rect WorkArea(IntPtr window)
        {
            MonitorInfo info = new MonitorInfo();
            info.Size = Marshal.SizeOf(typeof(MonitorInfo));
            if (!GetMonitorInfo(MonitorFromWindow(window, 2), ref info))
                throw new InvalidOperationException("Cannot resolve the monitor work area.");
            return info.Work;
        }
        public static Rect Bounds(IntPtr window)
        {
            Rect bounds;
            if (!GetWindowRect(window, out bounds)) throw new InvalidOperationException("Cannot resolve window bounds.");
            return bounds;
        }
        public static void ConstrainMaximize(IntPtr window, IntPtr data)
        {
            MonitorInfo monitor = new MonitorInfo();
            monitor.Size = Marshal.SizeOf(typeof(MonitorInfo));
            if (!GetMonitorInfo(MonitorFromWindow(window, 2), ref monitor)) return;
            MinMaxInfo info = (MinMaxInfo)Marshal.PtrToStructure(data, typeof(MinMaxInfo));
            info.MaxPosition.X = monitor.Work.Left - monitor.Monitor.Left;
            info.MaxPosition.Y = monitor.Work.Top - monitor.Monitor.Top;
            info.MaxSize.X = monitor.Work.Right - monitor.Work.Left;
            info.MaxSize.Y = monitor.Work.Bottom - monitor.Work.Top;
            Marshal.StructureToPtr(info, data, false);
        }
    }
}
