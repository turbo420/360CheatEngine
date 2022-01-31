#pragma once
class SystemHooks {
public:
	static BOOL Initialize();

	static HRESULT XexStartExecutableHook(FARPROC TitleProcessInitThreadProc);
};

