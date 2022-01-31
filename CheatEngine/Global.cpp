#include "stdafx.h"

Global CheatEngine;

BOOL Global::GetPluginData(DWORD dwTitle, DWORD dwTimestamp, ServerXexInfo* pOut) {
	if (PluginData.size() > 0) {
		for (int i = 0; i < PluginData.size(); i++) {
			auto data = PluginData[i];
			if (data.dwTitle == dwTitle) {
				LOG_PRINT("Found plugin for title 0x%X", dwTitle);
				if (!data.bEnabled) {
					LOG_PRINT("...Aborting, plugin is not enabled for client");
					return FALSE;
				}

				if (data.dwTitleTimestamp == dwTimestamp) {
					LOG_PRINT("...Timestamp match, successful plugin able to be loaded");
					if (pOut) *pOut = PluginData[i];
					return TRUE;
				}

				LOG_PRINT("...Aborting, timestamp doesn't match. Server: %X, Client: %X", data.dwTitleTimestamp, dwTimestamp);
				return FALSE;
			}
		}
	}

	return FALSE;
}