using System.Runtime.InteropServices;

namespace TestDotNetLoader
{
    public static class EntryPoint
    {
        public static int Main(IntPtr args, int sizeBytes)
        {
            return 0;
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    internal class TestClass
    {
        internal int X { get; set; }
    }

    public static class Class1
    {
        public delegate IntPtr CreateTestClassDelegate(int initX);
        public delegate void FreeTestClassDelegate(IntPtr testClassPtr, int expectedX);

        public static GCHandle TestClassHandle;

        public static int CallMe(IntPtr args, int sizeBytes)
        {
            return 3587;
        }

        public static IntPtr CreateTestClass(int initX)
        {
            var x = new TestClass { X = initX };
            TestClassHandle = GCHandle.Alloc(x, GCHandleType.Pinned);
            return TestClassHandle.AddrOfPinnedObject();
        }

        public static void FreeTestClass(IntPtr testClassPtr, int expectedX)
        {
            if (TestClassHandle.AddrOfPinnedObject() != testClassPtr)
                throw new Exception();

            if ((TestClassHandle.Target as TestClass)?.X != expectedX)
                throw new Exception();

            TestClassHandle.Free();
        }
    }
}