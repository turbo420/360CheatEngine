#include "stdafx.h"

char vaBuffer[0x1000];
char* Utils::va(const char* fmt, ...) {
	memset(vaBuffer, 0, 0x1000);
	va_list ap;
	va_start(ap, fmt);
	RtlVsprintf(vaBuffer, fmt, ap); // RtlVsprintf
	va_end(ap);
	return vaBuffer;
}

char* Utils::vaBuff(char* vaBuffer, int size, const char* fmt, ...) {
	memset(vaBuffer, 0, size);
	va_list ap;
	va_start(ap, fmt);
	RtlVsprintf(vaBuffer, fmt, ap);
	va_end(ap);
	return vaBuffer;
}

DWORD Utils::ResolveFunction(const std::string& strModuleName, DWORD dwOrdinal) {
    HMODULE hModule = GetModuleHandle(strModuleName.c_str());

    return (hModule == NULL) ? NULL : reinterpret_cast<DWORD>(GetProcAddress(hModule, reinterpret_cast<const char*>(dwOrdinal)));
}

void Utils::Notify(const std::string& text) {
	wchar_t* buf = (wchar_t*)calloc(1024, 1);
	wsprintfW(buf, L"%S", text);
	XNotifyQueueUI(XNOTIFYUI_TYPE_PREFERRED_REVIEW, 0, 2, buf, 0);
	free(buf);
}

void Utils::Thread(PTHREAD_START_ROUTINE pStartAddress, void* pParameters) {
    CreateThread(nullptr, 0, pStartAddress, pParameters, 0, nullptr);
}

void Utils::ThreadEx(LPTHREAD_START_ROUTINE pStartAddress, void* pParameters, DWORD dwCreationFlags) {
    ExCreateThread(nullptr, 0, nullptr, nullptr, pStartAddress, pParameters, dwCreationFlags);
}

HRESULT Utils::DoMountPath(const char* szDrive, const char* szDevice, const char* sysStr) {
	STRING DeviceName, LinkName;
	CHAR szDestinationDrive[MAX_PATH];
	RtlSnprintf(szDestinationDrive, MAX_PATH, sysStr, szDrive);
	RtlInitAnsiString(&DeviceName, szDevice);
	RtlInitAnsiString(&LinkName, szDestinationDrive);
	ObDeleteSymbolicLink(&LinkName);
	return (HRESULT)ObCreateSymbolicLink(&LinkName, &DeviceName);
}

HRESULT Utils::MountPath(const char* szDrive, const char* szDevice, bool both) {
	HRESULT res;
	if (both) {
		res = DoMountPath(szDrive, szDevice, "\\System??\\%s");
		res = DoMountPath(szDrive, szDevice, "\\??\\%s");
	}
	else {
		if (KeGetCurrentProcessType() == 2) //SYSTEM_PROC
			res = DoMountPath(szDrive, szDevice, "\\System??\\%s");
		else
			res = DoMountPath(szDrive, szDevice, "\\??\\%s");
	}
	return res;
}

const char* Utils::GetMountPath() {
	char* DEVICE_DYNAMIC = new char[MAX_PATH];
	wstring ws;
	PLDR_DATA_TABLE_ENTRY TableEntry;
	XexPcToFileHeader((PVOID)0x91C10000, &TableEntry);

	if (TableEntry) {

		ws = TableEntry->FullDllName.Buffer;
		string FullDllName(ws.begin(), ws.end());

		ws = TableEntry->BaseDllName.Buffer;
		string BaseDllName(ws.begin(), ws.end());

		string::size_type i = FullDllName.find(BaseDllName);

		if (i != string::npos)
			FullDllName.erase(i, BaseDllName.length());

		memset(DEVICE_DYNAMIC, 0x0, MAX_PATH);
		strcpy(DEVICE_DYNAMIC, FullDllName.c_str());
	}
	else {
		LOG_ERROR("Mounting failed!");
	}

	return DEVICE_DYNAMIC;
}