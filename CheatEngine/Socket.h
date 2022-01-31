#pragma once
class Socket {
public:
	static BOOL Connect();
	static VOID Close();
	static BOOL Send(DWORD Packet, PVOID Buffer, DWORD Size);
	static BOOL Receive(PVOID Buffer, DWORD Size);
	static BOOL DownloadFile(const std::string& Server, const std::string& FileName, unsigned char** Out, DWORD* length);
	static BOOL Process(DWORD Packet, PVOID Request, DWORD RequestSize, PVOID Response, DWORD ResponseSize, BOOL Close);
private:
	static sockaddr_in ServerHandle;
	static SOCKET SocketHandle;
};

