using System;
using System.Collections.Generic;
using System.Net.Sockets;

namespace Server
{
    class Welcome
    {
        public static void Handle(TcpClient client, EndianIO dataStream, List<Log.PrintQueue> logId, string IP)
        {
            byte[] szCPU = dataStream.Reader.ReadBytes(0x10);

            Log.Add(logId, ConsoleColor.Blue, "Command", "PacketWelcome", IP);
            Log.Add(logId, ConsoleColor.Cyan, "Console Key", Utils.BytesToHexString(szCPU), IP);

            // the error buffer if an error occured, and the bool that shows if an error is present
            bool hasError = false;
            char[] error = new char[0x100];

            Log.Print(logId);
            client.Close();
        }
    }
}
