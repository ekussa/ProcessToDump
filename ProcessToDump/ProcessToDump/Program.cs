using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace ProcessToDump
{
    public class Program
    {
        [DllImport("ILoveToCrash")]
        static extern void AccessViolation();

        [DllImport("ILoveToCrash")]
        static extern void StackOverflow();

        [DllImport("ILoveToCrash")]
        static extern void Setup();

        static void ManagedAccessViolation()
        {
            string d = null;
            d.ToString();
        }

        public static void Main(string[] args)
        {
            AppDomain.CurrentDomain.UnhandledException += Managedhandler;
            Setup();

            Console.WriteLine("Hallo!");
            Console.ReadKey();

            //AccessViolation();
            //StackOverflow();
            ManagedAccessViolation();

            Console.ReadKey();
        }

        static void Managedhandler(object sender, UnhandledExceptionEventArgs args)
        {
            Console.Beep();
            Console.WriteLine("Olá do gerenciado!");
            Console.WriteLine(((Exception)args.ExceptionObject).Message);

            var process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = "procdump.exe",
                    Arguments = "-ma " + Process.GetCurrentProcess().Id,
                    CreateNoWindow = true,
                    ErrorDialog = false,
                    UseShellExecute = true,
                    WindowStyle = ProcessWindowStyle.Hidden,
                },
            };
            process.Start();
            process.WaitForExit();
            Console.WriteLine("Finished");
        }
    }
}
