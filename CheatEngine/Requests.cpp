#include "stdafx.h"

BYTE Requests::szToken[0x20];

BOOL Requests::PacketConnect() {
	Request::ServerPacketConnect* Request = (Request::ServerPacketConnect*)XEncryptedAlloc(sizeof(Request::ServerPacketConnect));
	Response::ServerPacketConnect* Response = (Response::ServerPacketConnect*)XEncryptedAlloc(sizeof(Response::ServerPacketConnect));

	auto cleanup = [&] {
		XEncryptedFree(Request);
		XEncryptedFree(Response);
	};

	memcpy(Request->szCPU, Hypervisor::GetFuseCPU(), 0x10);

	if (Socket::Process(PACKET_CONNECT, Request, sizeof(Request::ServerPacketConnect), Response, sizeof(Response::ServerPacketConnect), FALSE)) {
		if (Response->bConnected == true) {
			cleanup();
			return TRUE;
		}
	}

	cleanup();
	return FALSE;
}

void Requests::InitThread() {
	while (true) {
		if (Requests::PacketConnect()) {
			CheatEngine.bConnectedToServerInit = true;
			break;
		}

		Utils::Notify("CheatEngine - Failed to connect. Check your internet connection!");
		Sleep(10000);
	}

	ExitThread(0);
}

BOOL Requests::PacketWelcome() {
	Request::ServerPacketWelcome* Request = (Request::ServerPacketWelcome*)XEncryptedAlloc(sizeof(Request::ServerPacketWelcome));
	Response::ServerPacketWelcome* Response = (Response::ServerPacketWelcome*)XEncryptedAlloc(sizeof(Response::ServerPacketWelcome));

	auto cleanup = [&] {
		XEncryptedFree(Request);
		XEncryptedFree(Response);
	};

	memcpy(Request->szCPU, Hypervisor::GetFuseCPU(), 0x10);

	if (Socket::Process(PACKET_WELCOME, Request, sizeof(Request::ServerPacketWelcome), Response, sizeof(Response::ServerPacketWelcome), FALSE)) {
		switch (Response->Status) {
			case WELCOME_STATUS_SUCCESS: {

				cleanup();
				return TRUE;
			}
		}
	}

	cleanup();
	return FALSE;
}

BOOL Requests::PacketGetPlugins() {
	Request::ServerPacketGetPlugins* Request = (Request::ServerPacketGetPlugins*)XEncryptedAlloc(sizeof(Request::ServerPacketGetPlugins));
	Response::ServerPacketGetPlugins* Response = (Response::ServerPacketGetPlugins*)XEncryptedAlloc(sizeof(Response::ServerPacketGetPlugins));

	auto cleanup = [&] {
		XEncryptedFree(Request);
		XEncryptedFree(Response);
	};

	memcpy(Request->szCPU, Hypervisor::GetFuseCPU(), 0x10);

	if (Socket::Process(PACKET_GET_PLUGINS, Request, sizeof(Request::ServerPacketGetPlugins), Response, sizeof(Response::ServerPacketGetPlugins), FALSE)) {
		if (Response->bHasPlugins) {
			LOG_PRINT("Plugins detected...");

			int count = *(int*)Response->szPluginBuffer;
			ServerXexInfo* info = (ServerXexInfo*)(Response->szPluginBuffer + 4);

			CheatEngine.PluginData.clear();

			for (int i = 0; i < count; i++) {
				CheatEngine.PluginData.push_back(info[i]);
			}
		}
		else {
			LOG_PRINT("No plugins detected :(");
		}

		cleanup();
		return TRUE;
	}

	cleanup();

	return FALSE;
}

BOOL Requests::DownloadEngine(DWORD TitleID) {
	DWORD Attempts = 0;

	/* XEX Data */
	DWORD  xexsize = 0;
	unsigned char* xexbytes = 0;

	while (Attempts < 50) {
		if (xexbytes)
			free(xexbytes);

		if (Socket::DownloadFile("192.168.178.58", Utils::va("/Plugins/ENGINE-%x.BIN", TitleID), &xexbytes, &xexsize)) {

			bool ReceivedBytes = xexsize > 0;

			if (!ReceivedBytes) {
				Utils::Notify("Failed to receive engine!");
				break;
				return FALSE;
			}
			auto moduleLoadStatus = XexLoadImageFromMemory(xexbytes, xexsize, "soly_engine.bin", XEX_MODULE_FLAG_DLL, 0, &handle);

			if (moduleLoadStatus != ERROR_SUCCESS) {
				if (moduleLoadStatus == 0xC0000018) {
					Utils::Notify("CheatEngine - Failed to load engine, conflicting module detected!");
					break;
					return FALSE;
				}

				Utils::Notify(Utils::va("CheatEngine - Error 0x%X occured when trying to load engine", moduleLoadStatus));
				break;
				return FALSE;
			}

			Utils::Notify("CheatEngine - Engine loaded!");
			break;
			return TRUE;
		}
		else {
			Utils::Notify("CheatEngine - Cheats were not loaded. Please restart game to try again.");
			break;
		}

		Attempts++;
		Sleep(50);
	}

	return FALSE;
}