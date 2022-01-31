namespace Server
{
    public class Global
    {
        public static int iPort = 5555;
        public static int iMaximumRequestSize = 0x4800;
    }

    enum PACKETS
    {
        PACKET_CONNECT = 1,
    }
}
