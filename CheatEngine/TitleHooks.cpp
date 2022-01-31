#include "stdafx.h"

void TitleHooks::RunOnTitleLoad(PLDR_DATA_TABLE_ENTRY moduleHandle) {
	auto mountPath = Utils::GetMountPath();
	Utils::MountPath("CHEATENGINE:", mountPath, true);
	delete[] mountPath;

	if (moduleHandle) {
		XEX_EXECUTION_ID* executionId = (XEX_EXECUTION_ID*)RtlImageXexHeaderField(moduleHandle->XexHeaderBase, XEX_HEADER_EXECUTION_ID);
		if (!executionId) {
			LOG_ERROR("Failed to get execution id!");
			return;
		}

		ServerXexInfo plugin;
		if (CheatEngine.GetPluginData(executionId->TitleID, moduleHandle->TimeDateStamp, &plugin)) {
			DWORD dwAddress, dwSize; //Plugin Bytes and Size

			if (Requests::DownloadEngine(plugin.dwTitle)) {
				LOG_PRINT("Cheat Engine loaded!");
			}
		}
	}
}