using System;
using System.Collections.Generic;
using System.Net.Sockets;

namespace Server {
    class PacketConnect
    {
        public static void Handle(TcpClient client, EndianIO dataStream, List<Log.PrintQueue> logId, string IP)
        {
            byte[] szCPU = dataStream.Reader.ReadBytes(0x10);

            Log.Add(logId, ConsoleColor.Blue, "Command", "PacketConnect", IP);
            Log.Add(logId, ConsoleColor.Cyan, "Console Key", Utils.BytesToHexString(szCPU), IP);

            dataStream.Writer.Write(true);

            Log.Print(logId);
            client.Close();
        }
    }
}
