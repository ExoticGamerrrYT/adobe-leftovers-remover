using System.IO.Compression;

namespace AdobeLeftoversRemoverpackager
{
    class Program
    {
        static int Main(string[] args)
        {
            if (args.Length != 1 || args[0] == "-h" || args[0] == "-help")
            {
                ShowHelp();
                return args.Length == 1 && (args[0] == "-h" || args[0] == "-help")
                    ? 0 : 1;
            }

            string version = args[0];
            string rootDir = AppDomain.CurrentDomain.BaseDirectory;
            // Assuming scripts are in <SolutionRoot>\adobe-leftovers-remover\scripts\bin\Debug\netX\
            string solutionRoot = Path.GetFullPath(Path.Combine(rootDir, "..", ".."));
            Console.WriteLine($"Solution root: {solutionRoot}");
            string qtReleasePath = Path.Combine(solutionRoot, "x64", "Release");
            Console.WriteLine($"Qt release path: {qtReleasePath}");

            if (!Directory.Exists(qtReleasePath))
            {
                Console.Error.WriteLine($"ERROR: Qt release path not found: {qtReleasePath}");
                return 1;
            }

            string stagingFolderName = $"adobe-leftovers-remover-v{version}";
            string stagingPath = Path.Combine(Path.GetTempPath(), stagingFolderName);
            string zipName = stagingFolderName + ".zip";
            string zipOutputPath = Path.Combine(solutionRoot, zipName);

            try
            {
                // Clean any leftover staging
                if (Directory.Exists(stagingPath))
                    Directory.Delete(stagingPath, true);

                // Copy everything
                Console.WriteLine($"Copying files from '{qtReleasePath}' to staging folder...");
                CopyAll(new DirectoryInfo(qtReleasePath), new DirectoryInfo(stagingPath));

                // Create ZIP
                Console.WriteLine($"Creating ZIP '{zipOutputPath}'...");
                if (File.Exists(zipOutputPath))
                    File.Delete(zipOutputPath);
                ZipFile.CreateFromDirectory(stagingPath, zipOutputPath, CompressionLevel.Optimal, false);

                Console.WriteLine("Done.");
                return 0;
            }
            catch (Exception ex)
            {
                Console.Error.WriteLine("ERROR: " + ex.Message);
                return 1;
            }
            finally
            {
                // Cleanup staging
                if (Directory.Exists(stagingPath))
                    Directory.Delete(stagingPath, true);
            }
        }

        static void ShowHelp()
        {
            Console.WriteLine("Usage:");
            Console.WriteLine("  packager.exe <version>");
            Console.WriteLine();
            Console.WriteLine("Example:");
            Console.WriteLine("  packager.exe 1.2.3");
            Console.WriteLine();
            Console.WriteLine("What it does:");
            Console.WriteLine("  1) Reads Qt release files from '../x64/Release'");
            Console.WriteLine("  2) Copies them into a folder named 'adobe-leftovers-remover-v<version>'");
            Console.WriteLine("  3) Creates 'adobe-leftovers-remover-v<version>.zip' next to your solution");
            Console.WriteLine();
        }

        static void CopyAll(DirectoryInfo source, DirectoryInfo target)
        {
            // Create target directory
            Directory.CreateDirectory(target.FullName);

            // Copy files
            foreach (FileInfo fi in source.GetFiles())
            {
                string destFile = Path.Combine(target.FullName, fi.Name);
                fi.CopyTo(destFile, true);
            }

            // Copy subdirectories
            foreach (DirectoryInfo di in source.GetDirectories())
            {
                CopyAll(di, new DirectoryInfo(Path.Combine(target.FullName, di.Name)));
            }
        }
    }
}
