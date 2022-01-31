using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace Server
{
    class ClientHandler
    {
        private TcpListener ListenerHandle;

        private Thread ListenerThread;
        public ClientHandler()
        {
            ListenerHandle = new TcpListener(IPAddress.Any, Global.iPort);
        }

        public void Setup()
        {
            ListenerThread = new Thread(new ThreadStart(() => {
                ListenerHandle.Start();

                while (true)
                {
                    Thread.Sleep(100);
                    if (ListenerHandle.Pending()) new Thread(new ParameterizedThreadStart(Handler)).Start(ListenerHandle.AcceptTcpClient());
                }
            }));
            ListenerThread.Start();
        }

        private void Handler(object client)
        {
            TcpClient tcpClient = (TcpClient)client;
            NetworkStream netStream = tcpClient.GetStream();

            string IP = tcpClient.Client.RemoteEndPoint.ToString().Split(new char[] { ':' })[0];

            if (!netStream.CanRead)
            {
                Console.WriteLine("Socket closed for {0} - CanRead was false!", IP);
                tcpClient.Close();
                return;
            }

            try
            {
                List<Log.PrintQueue> logId = Log.GetQueue();

                byte[] Header = new byte[8];
                if (netStream.Read(Header, 0, 8) != 8)
                {
                    Console.WriteLine("Wrong Header Connection Closed\n");
                    tcpClient.Close();
                    return;
                }

                EndianIO IO = new EndianIO(Header, EndianStyle.BigEndian);

                PACKETS Command = (PACKETS)IO.Reader.ReadUInt32();

                int Size = IO.Reader.ReadInt32();

                byte[] Data = new byte[Size];
                if ((Size > Global.iMaximumRequestSize) || (netStream.Read(Data, 0, Size) != Size))
                {
                    Console.WriteLine("Bad Size!\n");
                    tcpClient.Close();
                    return;
                }

                EndianIO dataStream = new EndianIO(Data, EndianStyle.BigEndian)
                {
                    Writer = new EndianWriter(netStream, EndianStyle.BigEndian)
                };

                switch ((PACKETS)Command)
                {
                    case PACKETS.PACKET_CONNECT:
                        PacketConnect.Handle(tcpClient, dataStream, logId, IP); break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error: {0} - {1}", ex.Message, ex.StackTrace);
            }
        }
    }
}
