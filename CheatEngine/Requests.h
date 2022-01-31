#pragma once

enum Packets {
	PACKET_CONNECT = 1,
	PACKET_WELCOME,
	PACKET_GET_PLUGINS
};

enum eWelcomePacketStatus {
	WELCOME_STATUS_SUCCESS = 1,
};

namespace Request {
	struct ServerPacketConnect {
		BYTE szCPU[0x10];
	};

	struct ServerPacketWelcome {
		BYTE szCPU[0x10];
	};

	struct ServerPacketGetPlugins {
		BYTE szCPU[0x10];
	};
}

namespace Response {

	struct ServerPacketConnect {
		bool bConnected;
	};

	struct ServerPacketWelcome {
		eWelcomePacketStatus Status;
	};

	struct ServerPacketGetPlugins {
		bool bHasPlugins;
		BYTE szPluginBuffer[0x200];
	};
}

class Requests {
public:
	static BOOL PacketConnect();
	static BOOL PacketWelcome();
	static BOOL PacketGetPlugins();

	/* Web Requests */
	static BOOL DownloadEngine(DWORD TitleID);

	static void InitThread();

	static BYTE szToken[0x20];
};

