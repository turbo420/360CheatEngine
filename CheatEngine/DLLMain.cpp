#include "stdafx.h"

void SystemThread() {
	auto mountPath = Utils::GetMountPath();
	if (!SUCCEEDED(Utils::MountPath("CHEATENGINE:", mountPath, true))) {
		FreeLibraryAndExitThread((HMODULE)CheatEngine.hMyModule, 0);
		*(WORD*)((DWORD)CheatEngine.hMyModule + 64) = 1;
		return;
	}

	delete[] mountPath;

	Utils::MountPath("HDD:", "\\Device\\Harddisk0\\Partition1\\", true);
	Utils::MountPath("USB:", "\\Device\\Mass0\\", true);

	Log::Initialize();

	if (!Hypervisor::Initialize()) {
		LOG_PRINT("Failed to Initialize Hypervisor!");
		Sleep(4000);
		HalReturnToFirmware(HalFatalErrorRebootRoutine);
	}

	if (FAILED(SystemHooks::Initialize())) {
		LOG_PRINT("Failed to Initialize SystemHooks!");
		Sleep(4000);
		HalReturnToFirmware(HalFatalErrorRebootRoutine);
	}

	CheatEngine.bLoadedProperly = true;

	HANDLE hThread; DWORD hThreadID;
	ExCreateThread(&hThread, 0, &hThreadID, (PVOID)XapiThreadStartup, (LPTHREAD_START_ROUTINE)Requests::InitThread, NULL, 0x1c000427);
	XSetThreadProcessor(hThread, 4);
	ResumeThread(hThread);
	CloseHandle(hThread);

	if (!Requests::PacketWelcome()) {
		LOG_PRINT("Failed #3dab9896");
		Sleep(4000);
		HalReturnToFirmware(HalFatalErrorRebootRoutine);
	}

	if (!Requests::PacketGetPlugins()) {
		LOG_PRINT("Failed #a465cf27");
	}
}

BOOL WINAPI DllMain(HANDLE Handler, DWORD dwReason, LPVOID Reserved) {
	switch (dwReason) {
		case DLL_PROCESS_ATTACH:
			if (!SMC::IsTrayOpen() && XboxKrnlVersion->Build == 0x4497) {
				CheatEngine.hMyModule = Handler;

				XexPcToFileHeader((PVOID)0x91C10000, &CheatEngine.pDataTableEntry);

				HANDLE hThread; DWORD hThreadID;
				ExCreateThread(&hThread, 0, &hThreadID, (PVOID)XapiThreadStartup, (LPTHREAD_START_ROUTINE)SystemThread, NULL, 0x1c000427);
				XSetThreadProcessor(hThread, 4);
				ResumeThread(hThread);
				CloseHandle(hThread);
				return TRUE;
			}
			break;
		case DLL_PROCESS_DETACH:
			HalReturnToFirmware(HalFatalErrorRebootRoutine);
			break;
	}

	*(WORD*)((DWORD)Handler + 64) = 1;
	return FALSE;
}