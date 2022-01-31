#pragma once


typedef enum _PEEK_POKE_TYPE {
	PEEK_BYTE = 0,
	PEEK_WORD = 1,
	PEEK_DWORD = 2,
	PEEK_QWORD = 3,
	PEEK_BYTES = 4,
	POKE_BYTE = 5,
	POKE_WORD = 6,
	POKE_DWORD = 7,
	POKE_QWORD = 8,
	POKE_BYTES = 9,
	PEEK_SPR = 10
} PEEK_POKE_TYPE;

class Hypervisor
{
public:
	static DWORD PeekPokeExpID;

	static DWORD GetVersions(DWORD Magic, DWORD Mode, QWORD Dest, QWORD Src, DWORD Len, QWORD Arg_r8);
	static BOOL Initialize();

	static DWORD PeekBytes(QWORD Address, void* Buffer, DWORD Size);
	static DWORD PokeBYTE(QWORD Address, BYTE Value);
	static DWORD PokeWORD(QWORD Address, WORD Value);
	static DWORD PokeDWORD(QWORD Address, DWORD Value);
	static DWORD PokeQWORD(QWORD Address, QWORD Value);
	static DWORD PokeBytes(QWORD Address, CONST void* Buffer, DWORD Size);
	static BYTE PeekBYTE(QWORD Address);
	static WORD PeekWORD(QWORD Address);
	static DWORD PeekDWORD(QWORD Address);
	static QWORD PeekQWORD(QWORD Address);

	static QWORD GetFuseLine(BYTE FuseIndex);

	static BYTE* GetHypervisorCPU();
	static BYTE* GetFuseCPU();
	static BYTE* GetConsoleType();

	static BYTE FuseLines[];
};

