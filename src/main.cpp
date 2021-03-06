#include "pch.h"
#include <iostream>

#include "Fly.h"
#include "Debugger.h"

bool EnableDebugPrivilege() {
    HANDLE hToken;
    LUID luid;
    TOKEN_PRIVILEGES tkp;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        return false;
    }

    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
        return false;
    }

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = luid;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL)) {
        return false;
    }

    if (!CloseHandle(hToken)) {
        return false;
    }

    return true;
}


int main() {
    EnableDebugPrivilege();
    fly::Debugger dbg;
   
    dbg.RedirectStdoutToNull();
    if (FLY_IS_SUCCESS(dbg.InitDebug(L"D:\\Projects\\FlyDbg\\bin\\upx64.exe", L"", L"D:\\Projects\\FlyDbg\\bin"))) {
        std::cout << "Success start!" << std::endl;
    } else {
        std::cout << "Failed start!" << std::endl;
    }

    std::cout << "ImageBase = 0x" << std::hex << dbg.GetImageBaseFromPEB() << std::endl;
    std::cout << "Current instr address = 0x" << std::hex << dbg.GetCurrentInstructionAddress() << std::endl;
    std::cout << "Current instr size: " << (unsigned int)dbg.GetCurrentInstructionSize() << std::endl;

    fly::FlyBpxCallback t = [&](fly::FlyUI64 dwAddress) {
        std::wcout << "BP: 0x" << std::hex << dwAddress << std::endl;
        std::cout << "Current instr address = 0x" << std::hex << dbg.GetCurrentInstructionAddress() << std::endl;
        std::cout << "Current instr size: " << (unsigned int)dbg.GetCurrentInstructionSize() << std::endl;
        dbg.DropBpx(dwAddress);
    };
    dbg.SetBpx(0x608CE0, fly::FLY_BP_INT3, t); // oep

    dbg.Run();
    Sleep(1000);
    dbg.StopDebug();

    system("pause");
    return 0;
}