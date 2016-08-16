#include <stdio.h>
#include <windows.h>

int g_inited = 0;

DWORD g_base;
#define ADDR(x) ((LPVOID)(g_base - 0x400000 + (DWORD)x))

typedef void(__fastcall *unpack)(const char *ext, const char *path);


#pragma pack(push, 1)

typedef struct {
	unsigned char opcode;
	DWORD rel_dst;
} STAGE_0, *PSTAGE_0;

typedef struct {
	DWORD count;
	DWORD len;
	char val[128];
} _String, *_PString;

#pragma pack(pop)


void __stdcall _unpack();

STAGE_0 g_stage_0;

void __stdcall unpack_stage_0_func()
{
	DWORD oldprt;
	PSTAGE_0 stage_0 = (PSTAGE_0)ADDR(0x2D2E4ED);
	VirtualProtect(stage_0, sizeof(STAGE_0), PAGE_EXECUTE_READWRITE, &oldprt);
	memcpy(stage_0, &g_stage_0, sizeof(g_stage_0));
	VirtualProtect(stage_0, sizeof(STAGE_0), oldprt, &oldprt);

	
	stage_0 = (PSTAGE_0)ADDR(0x2D2E3AB);
	VirtualProtect(stage_0, sizeof(STAGE_0), PAGE_EXECUTE_READWRITE, &oldprt);
	stage_0->opcode = 0xE8; //call
	stage_0->rel_dst = ((DWORD)&_unpack) - (DWORD)(stage_0 + 1);
	VirtualProtect(stage_0, sizeof(STAGE_0), oldprt, &oldprt);
}

__declspec(naked) void __stdcall unpack_stage_0_func_asm()
{
	__asm {
		add DWORD PTR[esp], -(SIZE STAGE_0);
		push eax;
		push ecx;
		push edx;

		call unpack_stage_0_func;

		pop edx;
		pop ecx;
		pop eax;

		ret;
	}
}

void __stdcall _unpack()
{
	_String path;
	_String ext = {-1, 4, ".ezp"};
	int argc = 0;
	LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	for (int i = 0; i < argc; i++) {
		if (!_wcsicmp(L"--path", argv[i])) {
			i++;
			wcstombs(path.val, argv[i], sizeof(path.val) - 1);
			path.count = -1;
			path.len = strlen(path.val);
		}
	}

	((unpack)ADDR(0x848D30))(ext.val, path.val);

	LocalFree(argv);
	TerminateProcess(GetCurrentProcess(), 0);

}

void main()
{
	g_base = (DWORD)GetModuleHandle(0);

	DWORD oldprt;
	PSTAGE_0 stage_0 = (PSTAGE_0)ADDR(0x2D2E4ED);
	VirtualProtect(stage_0, sizeof(STAGE_0), PAGE_EXECUTE_READWRITE, &oldprt);
	memcpy(&g_stage_0, stage_0, sizeof(STAGE_0));
	stage_0->opcode = 0xE8; //call
	stage_0->rel_dst = ((DWORD)&unpack_stage_0_func_asm) - (DWORD)(stage_0 + 1);
	VirtualProtect(stage_0, sizeof(STAGE_0), oldprt, &oldprt);
}

int WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	
	//AllocConsole();
	//freopen("CONOUT$", "wb", stdout);
	//freopen("CONIN$", "rb", stdin);

	if (g_inited) return 1;
	g_inited = 1;

	main();

	return 1;
}
