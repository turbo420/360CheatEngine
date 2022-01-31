#pragma once
class Global {
public:
	bool bConnectedToServerInit;
	bool bLoadedProperly;

	HANDLE hMyModule;
	PLDR_DATA_TABLE_ENTRY pDataTableEntry;

	/*Title plugin info*/
	vector<ServerXexInfo> PluginData;

	BOOL GetPluginData(DWORD dwTitle, DWORD dwTimestamp, ServerXexInfo* pOut);
};

extern Global CheatEngine;
