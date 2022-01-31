using System;
using System.Collections.Generic;
using System.Net.Sockets;

namespace Server {
    class PacketWelcome {
        private enum eWelcomePacketStatus
        {
            STATUS_SUCCESS = 1,
            STATUS_BANNED,
            STATUS_NOTHING
        }
        public static void Handle(TcpClient client, EndianIO dataStream, List<Log.PrintQueue> logId, string IP) {
            byte[] szCPU = dataStream.Reader.ReadBytes(0x10);

            Log.Add(logId, ConsoleColor.Blue, "Command", "PacketWelcome", IP);
            Log.Add(logId, ConsoleColor.Cyan, "Console Key", Utils.BytesToHexString(szCPU), IP);

            ClientInfo clientinfo = new ClientInfo();

            eWelcomePacketStatus status = eWelcomePacketStatus.STATUS_NOTHING;

            if (MySQL.GetClientData(Utils.BytesToHexString(szCPU), ref clientinfo)) {
                switch (clientinfo.Status) {
                    case ClientInfoStatus.Banned:
                        status = eWelcomePacketStatus.STATUS_BANNED;
                        break;
                    case ClientInfoStatus.Authed:
                        status = eWelcomePacketStatus.STATUS_SUCCESS;
                        break;
                }

                MySQL.UpdateUserInfoWelcomePacket(Utils.BytesToHexString(szCPU), IP);
            }
            else {
                MySQL.AddUserWelcomePacket(Utils.BytesToHexString(szCPU), Utils.BytesToHexString(szCPU), IP);
                status = eWelcomePacketStatus.STATUS_SUCCESS;
            }

            Log.Add(logId, ConsoleColor.Green, "Status", $"{status}", IP);

            dataStream.Writer.Write((int)status);

            Log.Add(logId, ConsoleColor.Green, "Status", "Response sent", IP);
            Log.Print(logId);
            client.Close();
        }
    }
}
