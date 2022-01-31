using System;

namespace Server
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.ForegroundColor = ConsoleColor.White;
            Console.WindowWidth = 150;
            Console.WindowHeight = 31;
            Console.Title = "CheatEngine Client Handler";

            try
            {
                new ClientHandler().Setup();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Exception: {ex}");
            }
        }
    }
}
