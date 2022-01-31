using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Sockets;


namespace Server {
    class PacketGetPlugins {
        public static void Handle(TcpClient client, EndianIO dataStream, List<Log.PrintQueue> logId, string IP) {
            byte[] szCPU = dataStream.Reader.ReadBytes(0x10);

            Log.Add(logId, ConsoleColor.Blue, "Command", "PacketGetPlugins", IP);
            Log.Add(logId, ConsoleColor.Cyan, "Console Key", Utils.BytesToHexString(szCPU), IP);

            List<XexInfo> xexInfos = MySQL.GetXexInfos();

            byte[] resp = new byte[0x201];
            byte[] data = new byte[0x200];

            EndianWriter writer = new EndianWriter(new MemoryStream(resp), EndianStyle.BigEndian);
            EndianWriter dataWriter = new EndianWriter(new MemoryStream(data), EndianStyle.BigEndian);

            int realCount = 0;

            foreach (var xex in xexInfos) {
                if (xex.dwTitle != 0) realCount++;
            }

            dataWriter.Write(realCount);

            foreach (var xex in xexInfos) {
                if (xex.dwTitle != 0) {
                    dataWriter.Write(xex.iID);
                    dataWriter.Write(xex.dwTitle);
                    dataWriter.Write(xex.dwTitleTimestamp);
                    dataWriter.Write(xex.bEnabled);
                }
            }

            dataWriter.Close();

            Log.Add(logId, ConsoleColor.Magenta, "Info", string.Format("Sending {0} plugin(s) to client", realCount), IP);

            writer.Write(true);
            writer.Write(data);
            writer.Close();

            dataStream.Writer.Write(resp);

            Log.Print(logId);
            client.Close();
        }
    }
}
