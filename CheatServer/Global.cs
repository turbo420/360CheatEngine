namespace Server {
    public class Global {
        public static int iPort = 5555;
        public static int iMaximumRequestSize = 0x4800;
    }

    enum PACKETS {
        PACKET_CONNECT = 1,
        PACKET_WELCOME,
        PACKET_GET_PLUGINS
    }

    public struct XexInfo {
        public int iID;
        public string Name;
        public uint dwTitle;
        public uint dwTitleTimestamp;
        public bool bEnabled;
    }

    public enum ClientInfoStatus {
        Authed,
        Banned
    }

    public struct ClientInfo {
        public int iID;
        public string CPUKey;
        public string FirstIP;
        public string LastIP;
        public ClientInfoStatus Status;
        public int iLastConnection;
    }
}
