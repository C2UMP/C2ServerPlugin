#include <Windows.h>
#include <MinHook/include/MinHook.h>

unsigned long main_thread(void* lpParameter) {
	MH_Initialize();

	unsigned char* module_base{ reinterpret_cast<unsigned char*>(GetModuleHandleA("Chivalry2-Win64-Shipping.exe")) };

	auto cmd_permission{ module_base + 0x1830D63 }; // Patch for command permission when executing commands (UTBLLocalPlayer::Exec)
	auto login_call{ module_base + 0x17E346B }; // Patch to prevent listen server crash on VTable call (UTBLGameInstance::HandleNativeLoginChanged)

	DWORD d;
	VirtualProtect(cmd_permission, 1, PAGE_EXECUTE_READWRITE, &d);
	*cmd_permission = 0xEB; // Patch to JMP
	VirtualProtect(cmd_permission, 1, d, NULL); //TODO: Convert patch to hook.

	VirtualProtect(login_call, 1, PAGE_EXECUTE_READWRITE, &d);
	*login_call = 0xEB; // Patch to JMP
	VirtualProtect(login_call, 1, d, NULL);

	ExitThread(0);
	return 0;
}

int __stdcall DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH: {
		HANDLE thread_handle{ CreateThread(NULL, 0, main_thread, hModule, 0, NULL) };
		if (thread_handle) CloseHandle(thread_handle);
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return 1;
}