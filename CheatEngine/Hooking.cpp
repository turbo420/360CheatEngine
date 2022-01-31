#include "stdafx.h"

vector<HookContext> Hooking::vHookContext;
BYTE Hooking::szHookSection[0x500];
DWORD Hooking::dwHookCount;

bool Hooking::HookModuleImport(PLDR_DATA_TABLE_ENTRY moduleHandle, const char* pImportedModuleName, DWORD dwOrdinal, void* pHookFunction) {
    if (!moduleHandle) {
        return false;
    }

    int patchCount = 0;

    DWORD ordinalAddress = Utils::ResolveFunction(pImportedModuleName, dwOrdinal);

    if (!ordinalAddress) {
        LOG_ERROR("Failed to resolve ordinal 0x%X from %s", dwOrdinal, pImportedModuleName);
        return false;
    }

    PXEX_IMPORT_DESCRIPTOR imports = (PXEX_IMPORT_DESCRIPTOR)RtlImageXexHeaderField(moduleHandle->XexHeaderBase, XEX_HEADER_IMPORTS);

    if (!imports) {
        LOG_ERROR("Failed to find import descriptors");
        return false;
    }

    PXEX_IMPORT_TABLE importTable = (PXEX_IMPORT_TABLE)((PBYTE)imports + sizeof(*imports) + imports->NameTableSize);

    for (int iTable = 0; iTable < (int)imports->ModuleCount; iTable++) {
        for (int iStub = 0; iStub < importTable->ImportCount; iStub++) {
            DWORD stubAddress = *((PDWORD)importTable->ImportStubAddr[iStub]);

            if (ordinalAddress != stubAddress)
                continue;

            stubAddress = (DWORD)importTable->ImportStubAddr[iStub + 1];

            PatchInJump((PDWORD)stubAddress, (DWORD)pHookFunction, false);

            iStub = importTable->ImportCount;
            patchCount++;
        }

        importTable = (PXEX_IMPORT_TABLE)((PBYTE)importTable + importTable->TableSize);
    }

    if (patchCount == 0) return false;

    return true;
}

bool Hooking::HookModuleImport(const char* pModuleName, const char* pImportedModuleName, DWORD dwOrdinal, void* pHookFunction, vector<DWORD>* pAddresses) {
    LDR_DATA_TABLE_ENTRY* moduleHandle = (LDR_DATA_TABLE_ENTRY*)GetModuleHandle(pModuleName);

    if (!moduleHandle) {
        LOG_ERROR("Failed to find %s", pModuleName);
        return false;
    }

    int patchCount = 0;

    DWORD ordinalAddress = Utils::ResolveFunction(pImportedModuleName, dwOrdinal);

    if (!ordinalAddress) {
        LOG_ERROR("Failed to resolve ordinal 0x%X from %s", dwOrdinal, pImportedModuleName);
        return false;
    }

    PXEX_IMPORT_DESCRIPTOR imports = (PXEX_IMPORT_DESCRIPTOR)RtlImageXexHeaderField(moduleHandle->XexHeaderBase, XEX_HEADER_IMPORTS);

    if (!imports) {
        LOG_ERROR("Failed to find import descriptors");
        return false;
    }

    PXEX_IMPORT_TABLE importTable = (PXEX_IMPORT_TABLE)((PBYTE)imports + sizeof(*imports) + imports->NameTableSize);

    for (int iTable = 0; iTable < (int)imports->ModuleCount; iTable++) {
        for (int iStub = 0; iStub < importTable->ImportCount; iStub++) {
            DWORD stubAddress = *((PDWORD)importTable->ImportStubAddr[iStub]);

            if (ordinalAddress != stubAddress)
                continue;

            stubAddress = (DWORD)importTable->ImportStubAddr[iStub + 1];

            PatchInJump((PDWORD)stubAddress, (DWORD)pHookFunction, false);

            if (pAddresses) {
                pAddresses->push_back(stubAddress);
            }

            iStub = importTable->ImportCount;
            patchCount++;
        }

        importTable = (PXEX_IMPORT_TABLE)((PBYTE)importTable + importTable->TableSize);
    }

    if (patchCount == 0) return false;

    LOG_PRINT("Hooking module inside %s, ordinal %i from %s", pModuleName, dwOrdinal, pImportedModuleName);

    return true;
}

void Hooking::HookFunctionStart(DWORD* pdwAddress, DWORD* pdwSaveStub, DWORD dwDestination)
{
    if (pdwSaveStub != NULL && pdwAddress != NULL)
    {
        DWORD dwAddrReloc = reinterpret_cast<DWORD>(&pdwAddress[4]);
        DWORD dwWriteBuffer;

        if (dwAddrReloc & 0x8000)
            dwWriteBuffer = 0x3D600000 + (((dwAddrReloc >> 16) & 0xFFFF) + 1);
        else
            dwWriteBuffer = 0x3D600000 + ((dwAddrReloc >> 16) & 0xFFFF);

        pdwSaveStub[0] = dwWriteBuffer;
        dwWriteBuffer = 0x396B0000 + (dwAddrReloc & 0xFFFF);
        pdwSaveStub[1] = dwWriteBuffer;
        dwWriteBuffer = 0x7D6903A6;
        pdwSaveStub[2] = dwWriteBuffer;

        for (int i = 0; i < 4; i++)
        {
            if ((pdwAddress[i] & 0x48000003) == 0x48000001)
            {
                dwWriteBuffer = RelinkGPLR((pdwAddress[i] & ~0x48000003), &pdwSaveStub[i + 3], &pdwAddress[i]);
                pdwSaveStub[i + 3] = dwWriteBuffer;
            }
            else
            {
                dwWriteBuffer = pdwAddress[i];
                pdwSaveStub[i + 3] = dwWriteBuffer;
            }
        }

        dwWriteBuffer = 0x4E800420; // bctr
        pdwSaveStub[7] = dwWriteBuffer;

        __dcbst(0, pdwSaveStub);
        __sync();
        __isync();

        PatchInJump(pdwAddress, dwDestination, false);
    }
}

void Hooking::PatchInJump(DWORD* pdwAddress, DWORD dwDestination, bool bLinked)
{
    DWORD dwWriteBuffer;

    if (dwDestination & 0x8000)
        dwWriteBuffer = 0x3D600000 + (((dwDestination >> 16) & 0xFFFF) + 1);
    else
        dwWriteBuffer = 0x3D600000 + ((dwDestination >> 16) & 0xFFFF);

    pdwAddress[0] = dwWriteBuffer;
    dwWriteBuffer = 0x396B0000 + (dwDestination & 0xFFFF);
    pdwAddress[1] = dwWriteBuffer;
    dwWriteBuffer = 0x7D6903A6;
    pdwAddress[2] = dwWriteBuffer;

    if (bLinked)
        dwWriteBuffer = 0x4E800421;
    else
        dwWriteBuffer = 0x4E800420;

    pdwAddress[3] = dwWriteBuffer;

    __dcbst(0, pdwAddress);
    __sync();
    __isync();
}

void __declspec(naked) Hooking::GLPR()
{
    __asm
    {
        std     r14, -0x98(sp)
        std     r15, -0x90(sp)
        std     r16, -0x88(sp)
        std     r17, -0x80(sp)
        std     r18, -0x78(sp)
        std     r19, -0x70(sp)
        std     r20, -0x68(sp)
        std     r21, -0x60(sp)
        std     r22, -0x58(sp)
        std     r23, -0x50(sp)
        std     r24, -0x48(sp)
        std     r25, -0x40(sp)
        std     r26, -0x38(sp)
        std     r27, -0x30(sp)
        std     r28, -0x28(sp)
        std     r29, -0x20(sp)
        std     r30, -0x18(sp)
        std     r31, -0x10(sp)
        stw     r12, -0x8(sp)
        blr
    }
}

DWORD Hooking::RelinkGPLR(int nOffset, DWORD* pdwSaveStubAddr, DWORD* pdwOrgAddr)
{
    DWORD dwInst = 0, dwRepl;
    DWORD* pdwSaver = reinterpret_cast<DWORD*>(GLPR);

    if (nOffset & 0x2000000)
        nOffset = nOffset | 0xFC000000;

    dwRepl = pdwOrgAddr[nOffset / 4];

    for (int i = 0; i < 20; i++)
    {
        if (dwRepl == pdwSaver[i])
        {
            int nNewOffset = reinterpret_cast<int>(&pdwSaver[i]) - reinterpret_cast<int>(pdwSaveStubAddr);
            dwInst = 0x48000001 | (nNewOffset & 0x3FFFFFC);
        }
    }

    return dwInst;
}