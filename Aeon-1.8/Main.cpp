#include "framework.h"

DWORD WINAPI MenuThread(LPVOID lpReserved)
{
        bool init_hook = false;
        do
        {
            if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
            {
                kiero::bind(8, (void**)&Menu::oPresent, Menu::hkPresent);
                init_hook = true;
            }
        } while (!init_hook);
    return TRUE;
}

void Setup()
{
    Utils::InitLogConsole();

    std::string AEON = R"(
           ______ ____  _   _   __   ___  
     /\   |  ____/ __ \| \ | | /_ | / _ \ 
    /  \  | |__ | |  | |  \| |  | || (_) |
   / /\ \ |  __|| |  | | . ` |  | | > _ < 
  / ____ \| |___| |__| | |\  |  | || (_) |
 /_/    \_\______\____/|_| \_|  |_(_)___/ 
                                          
                                          
)";

    Log(AEON);
    auto GObjectAddress = Utils::FindPattern("\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8B\xD6", "xxx????x????x????x????xxx");
    auto GObjectOffset = *reinterpret_cast<uint32_t*>(GObjectAddress + 3);
    SDK::UObject::GObjects = reinterpret_cast<SDK::FUObjectArray*>(GObjectAddress + 7 + GObjectOffset);

    auto GNameAddress = Utils::FindPattern("\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x75\x50\xB9\x00\x00\x00\x00\x48\x89\x5C\x24", "xxx????xxxxxx????xxxx");
    auto GNameOffset = *reinterpret_cast<uint32_t*>(GNameAddress + 3);
    SDK::FName::GNames = *reinterpret_cast<SDK::TNameEntryArray**>(GNameAddress + 7 + GNameOffset);

    MH_Initialize();
    Addresses::Init();
    Hooks::Init();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulReason, LPVOID lpVoid)
{
    if (ulReason == 1)
    {
        Setup();
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MenuThread, hModule, 0, nullptr);
    }


    return TRUE;
}