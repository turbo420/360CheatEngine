#pragma once

struct HookContext {
    DWORD dwAddress;
    BYTE szAsm[0x10];
    BYTE bWriteSize;

    HookContext() {
        bWriteSize = 0x10;
    }
};

class Hooking {
public:
    static vector<HookContext> vHookContext;
    static BYTE szHookSection[0x500];
    static DWORD dwHookCount;

    static void HookFunctionStart(DWORD* pdwAddress, DWORD* pdwSaveStub, DWORD dwDestination);
    static bool HookModuleImport(const char* pModuleName, const char* pImportedModuleName, DWORD dwOrdinal, void* pHookFunction, vector<DWORD>* pAddresses = nullptr);
    static bool HookModuleImport(PLDR_DATA_TABLE_ENTRY module, const char* pImportedModuleName, DWORD dwOrdinal, void* pHookFunction);

    // Write data at dwAddress.
    template<typename T>
    static void Write(DWORD dwAddress, T data) {
        if (!MmIsAddressValid(reinterpret_cast<DWORD*>(dwAddress))) {
            Log::Error("Invalid address: %#010x", dwAddress);
            return;
        }

        *(T*)dwAddress = data;
    }

    // Read memory at dwAddress.
    template<typename T>
    static T Read(DWORD dwAddress) {
        if (!MmIsAddressValid(reinterpret_cast<DWORD*>(dwAddress))) {
            Log::Error("Invalid address: %#010x", dwAddress);
            return 0;
        }

        return *(T*)dwAddress;
    }

    template <typename T>
    static bool HookFunction(DWORD dwAddress, void* pHookFunction, T* pTrampoline, bool addToIntegrity = false) {
        if (dwAddress) {
            HookContext cxt;
            cxt.dwAddress = dwAddress;
            memcpy(cxt.szAsm, (void*)dwAddress, 0x10);

            vHookContext.push_back(cxt);

            DWORD* startStub = (DWORD*)&szHookSection[dwHookCount * 0x20];
            dwHookCount++;

            for (int i = 0; i < 7; i++)
                startStub[i] = 0x60000000;
            startStub[7] = 0x4E800020;

            HookFunctionStart((DWORD*)dwAddress, startStub, (DWORD)pHookFunction);

            *pTrampoline = (T)startStub;

            LOG_DEV("Hooked: Address=0x%X Stub=0x%X %s", dwAddress, *pTrampoline, addToIntegrity ? "- Added to integrity manager!" : "");
            return true;
        }

        return false;
    }

private:
    // Insert a jump instruction into an existing function to jump to another function.
    static void PatchInJump(DWORD* pdwAddress, DWORD dwDestination, bool bLinked);

    static void GLPR();

    static DWORD RelinkGPLR(int nOffset, DWORD* pdwSaveStubAddr, DWORD* pdwOrgAddr);
};

