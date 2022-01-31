#include "stdafx.h"

SOCKET Socket::SocketHandle = INVALID_SOCKET;

sockaddr_in Socket::ServerHandle = { 0 };

BYTE TempBuffer[0x4800] = { 0 };

BOOL Pending = FALSE, Connected = FALSE;

BOOL Socket::DownloadFile(const std::string& Server, const std::string& FileName, unsigned char** Out, DWORD* length) {
	*Out = (unsigned char*)0;

	HRESULT ret = 0;

	HttpClient* pHttpClient = new HttpClient;

	ret = pHttpClient->GET(Server.c_str(), FileName.c_str());

	if (ret == E_PENDING) {
		while (pHttpClient->GetStatus() == HttpClient::HTTP_STATUS_BUSY) {
			Sleep(1);
		}
	}

	if (pHttpClient->GetStatus() != HttpClient::HTTP_STATUS_DONE) {
		delete pHttpClient;
		return false;
	}

	BYTE* pContent = pHttpClient->GetResponseContentData();
	DWORD nLength = pHttpClient->GetResponseContentDataLength();

	if ((pContent > 0) && (nLength > 0)) {
		*length = nLength;

		nLength += 0x10000 - (nLength % 0x10000);

		*Out = (unsigned char*)XMemAlloc(nLength, XAPO_ALLOC_ATTRIBUTES);

		if (*Out > 0 && pContent && nLength) {
			memcpy(*Out, pContent, nLength);

			delete pHttpClient;
			return true;
		}
	}

	delete pHttpClient;
	return false;
}

BOOL Socket::Connect() {
	XNetStartupParams XNSP = { 0 };
	XNSP.cfgSizeOfStruct = sizeof(XNetStartupParams);
	XNSP.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
	if (NetDll_XNetStartup(XNCALLER_SYSAPP, &XNSP) != 0) {
		LOG_ERROR("Failed to start XNet");
		return FALSE;
	}

	WSADATA WsaData = { 0 };
	if (NetDll_WSAStartupEx(XNCALLER_SYSAPP, WINSOCK_VERSION, &WsaData, WINSOCK_VERSION) != 0) {
		LOG_ERROR("Failed to start WSA");
		return FALSE;
	}
	if ((SocketHandle = NetDll_socket(XNCALLER_SYSAPP, AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		LOG_ERROR("Failed to create socket. Status: %X", GetLastError());
		return FALSE;
	}

	BOOL SockOpt = TRUE;
	if (NetDll_setsockopt(XNCALLER_SYSAPP, SocketHandle, SOL_SOCKET, SO_MARKINSECURE, (CONST PCHAR) & SockOpt, sizeof(BOOL)) != 0) {
		LOG_ERROR("Failed to connect to socket, Status: %X", GetLastError());
		return FALSE;
	}

	DWORD MaxPacketSize = 0x2500;
	if (NetDll_setsockopt(XNCALLER_SYSAPP, SocketHandle, SOL_SOCKET, SO_SNDBUF, (CONST PCHAR) & MaxPacketSize, sizeof(DWORD)) != 0) {
		LOG_ERROR("Failed to connect to socket, Status: %X", GetLastError());
		return FALSE;
	}
	if (NetDll_setsockopt(XNCALLER_SYSAPP, SocketHandle, SOL_SOCKET, SO_RCVBUF, (CONST PCHAR) & MaxPacketSize, sizeof(DWORD)) != 0) {
		LOG_ERROR("Failed to connect to socket, Status: %X", GetLastError());
		return FALSE;
	}

	ServerHandle.sin_family = AF_INET;

	ServerHandle.sin_addr.S_un.S_un_b.s_b1 = 192;
	ServerHandle.sin_addr.S_un.S_un_b.s_b2 = 168;
	ServerHandle.sin_addr.S_un.S_un_b.s_b3 = 178;
	ServerHandle.sin_addr.S_un.S_un_b.s_b4 = 58;
	ServerHandle.sin_port = htons(5555);

	if (NetDll_connect(XNCALLER_SYSAPP, SocketHandle, (struct sockaddr*)&ServerHandle, sizeof(ServerHandle)) == SOCKET_ERROR) {
		LOG_ERROR("Failed to connect to socket, Status: %X", GetLastError());
		return FALSE;
	}

	return Connected = TRUE;
}

VOID Socket::Close() {
	if ((SocketHandle != INVALID_SOCKET) && Connected) {
		NetDll_closesocket(XNCALLER_SYSAPP, SocketHandle);
		SocketHandle = INVALID_SOCKET;
		Connected = FALSE;
	}
}

BOOL Socket::Send(DWORD Packet, PVOID Buffer, DWORD Size) {
	memset(TempBuffer, 0, sizeof(TempBuffer));
	memcpy(TempBuffer, &Packet, sizeof(DWORD));
	memcpy(TempBuffer + 4, &Size, sizeof(DWORD));
	memcpy(TempBuffer + 8, Buffer, Size);

	DWORD Remaining = (Size + 8);
	PCHAR Current = (PCHAR)TempBuffer;
	while (Remaining > 0) {
		DWORD Result = NetDll_send(XNCALLER_SYSAPP, SocketHandle, Current, min(0x2500, Remaining), 0);
		if (Result <= 0) return FALSE;
		Remaining -= Result;
		Current += Result;
	}
	return TRUE;
}

BOOL Socket::Receive(PVOID Buffer, DWORD Size) {
	DWORD Received = 0;
	DWORD Remaining = Size;
	while (Remaining > 0) {
		DWORD Result = NetDll_recv(XNCALLER_SYSAPP, SocketHandle, (PCHAR)Buffer + Received, min(0x2500, Remaining), 0);
		if (Result <= 0) return FALSE;
		Remaining -= Result;
		Received += Result;
	}
	return (Received == Size);
}

BOOL Socket::Process(DWORD Packet, PVOID Request, DWORD RequestSize, PVOID Response, DWORD ResponseSize, BOOL Close) {
	while (Pending) Sleep(1);
	Pending = TRUE;
	for (DWORD i = 0; i < 10; i++)
		if (Connect()) break;
	if (!Connected) return FALSE;
	if (!Send(Packet, Request, RequestSize)) return FALSE;
	if (!Receive(Response, ResponseSize)) return FALSE;
	if (Close) Socket::Close();
	Pending = FALSE;
	return TRUE;
}