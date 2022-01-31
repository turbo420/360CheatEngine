#pragma once

class Utils {
public:
	static char* va(const char* fmt, ...);
	static char* vaBuff(char* vaBuffer, int size, const char* fmt, ...);

	// Get a function in strModuleName from its ordinal.
	static DWORD ResolveFunction(const std::string& strModuleName, DWORD dwOrdinal);

	static void Notify(const std::string& text);

	// Start a thread.
	static void Thread(PTHREAD_START_ROUTINE pStartAddress, void* pParameters = nullptr);

	// Start a thread with special creation flags.
	static void ThreadEx(LPTHREAD_START_ROUTINE pStartAddress, void* pParameters, DWORD dwCreationFlags);

	static HRESULT DoMountPath(const char* szDrive, const char* szDevice, const char* sysStr);
	static HRESULT MountPath(const char* szDrive, const char* szDevice, bool both);
	static const char* GetMountPath();
};

