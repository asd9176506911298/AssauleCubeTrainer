// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <iomanip>
#include "mem.h"

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

bool bJump = false, bAmmo = false, bRecoil = false, bShotSpped = false, bSpeedHack = false, bTriggerbot = false, bFly = false;


struct vec3
{
    float x, y, z;
};

std::string isOpen(bool status)
{
    return status ? "ON" : "OFF";
}

vec3 recoderPostion = { 0 };

void printText(std::string string, bool status)
{
    std::cout << std::setw(40) << std::left << string << isOpen(status) << std::endl;
}

void printText(std::string string)
{
    std::cout << std::setw(40) << std::left << string << std::endl;
}


void printOnOff()
{
    system("cls");
    printText("[SPACE] Auto Jump");
    printText("[NUMPAD1] High Jump", bJump);
    printText("[NUMPAD2] Unlimited Ammo", bAmmo);
    printText("[NUMPAD3] Gun Recoil", bRecoil);
    printText("[NUMPAD4] Shoot Fast", bShotSpped);
    printText("[NUMPAD5] Set Teleport Position");
    printText("[NUMPAD6] Teleport to set Position");
    printText("[NUMPAD7] Speed Hack", bSpeedHack);
    printText("[NUMPAD8] Triggerbot", bTriggerbot);
    printText("[NUMPAD9] Fly", bFly);
    std::cout << std::setw(40) << std::left << "[END] Exit" << std::right << std::endl;
}

class playerent
{
public:

    // be sure to put all the offsets you pad this way in a union
    union
    {
        //              Type     Name    Offset
        DEFINE_MEMBER_N(int, health, 0xf8);
        DEFINE_MEMBER_N(vec3, position, 0x34);
        DEFINE_MEMBER_N(float, maxSpeed, 0x50);
        DEFINE_MEMBER_N(byte, jump, 0x6b);
        DEFINE_MEMBER_N(char, frontback, 0x80);
        DEFINE_MEMBER_N(char, leftright, 0x81);
        DEFINE_MEMBER_N(byte, invisible, 0x82);
        DEFINE_MEMBER_N(byte, left, 0x8c);
        DEFINE_MEMBER_N(byte, right, 0x8d);
        DEFINE_MEMBER_N(byte, up, 0x8e);
        DEFINE_MEMBER_N(byte, down, 0x8f);
        DEFINE_MEMBER_N(byte, attack, 0x224);
        DEFINE_MEMBER_N(int, team, 0x32c);
        DEFINE_MEMBER_N(int, spectator, 0x338);
    };
};

struct EntList
{
    playerent* ents[32];
};

typedef playerent* (__cdecl* tGetCrossHairEnt)();

tGetCrossHairEnt GetCrossHairEnt = nullptr;



DWORD WINAPI HackThread(HMODULE hmodule)
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    printOnOff();

    int* numOfPlayers = (int*)(0x50f500);
    playerent* localPlayer = *(playerent**)0x50F4F4;
    EntList* entList = *(EntList**)0x50F4F8;
    GetCrossHairEnt = (tGetCrossHairEnt)0x4607C0;

    float speed_val = localPlayer->maxSpeed * 0.2;

    while (1)
    {
        //Press End FreeLibraryAndExitThread
        if (GetAsyncKeyState(VK_END) & 1)
        {
            break;
        }
        //Press Numpad1 High Jump
        if (GetAsyncKeyState(VK_NUMPAD1) & 1)
        {
            bJump = !bJump;
            printOnOff();
            {
                if (bJump)
                    mem::Patch((BYTE*)0x45B318, (BYTE*)"\xD9\x05\x44\xE2\x4E\x00", 6);
                else
                    mem::Patch((BYTE*)0x45B318, (BYTE*)"\xD9\x05\x4C\xE2\x4E\x00", 6);
            }
        }
        //Press Numpad2 Unlimited Ammo
        if (GetAsyncKeyState(VK_NUMPAD2) & 1)
        {
            bAmmo = !bAmmo;
            printOnOff();
            if (bAmmo)
                mem::Patch((BYTE*)0x4637E9, (BYTE*)"\xff\x06", 2);
            else
                mem::Patch((BYTE*)0x4637E9, (BYTE*)"\xff\x0e", 2);
        }
        //Press Numpad3 Gun Recoil
        if (GetAsyncKeyState(VK_NUMPAD3) & 1)
        {
            bRecoil = !bRecoil;
            printOnOff();
            if (bRecoil)
            {
                mem::Nop((BYTE*)(0x463786), 10);
            }
            else
            {
                mem::Patch((BYTE*)(0x463786), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10);
            }
        }
        //Press Numpad4 Shot Fast
        if (GetAsyncKeyState(VK_NUMPAD4) & 1)
        {
            bShotSpped = !bShotSpped;
            printOnOff();
            if (bShotSpped)
            {
                mem::Nop((BYTE*)(0x4637E4), 2);
            }
            else
            {
                mem::Patch((BYTE*)(0x4637E4), (BYTE*)"\x89\x0a", 2);
            }
        }
        //Press Numpad5 Record Current Position
        if (GetAsyncKeyState(VK_NUMPAD5) & 1)
        {
            printOnOff();
            recoderPostion = localPlayer->position;
        }
        //Press Numpad6 Teleport to Record Positions
        if (GetAsyncKeyState(VK_NUMPAD6) & 1)
        {
            if (recoderPostion.x || recoderPostion.y || recoderPostion.z)
            {
                printOnOff();
                localPlayer->position = recoderPostion;
            }
        }

        if (GetAsyncKeyState(VK_NUMPAD7) & 1)
        {
            bSpeedHack = !bSpeedHack;
            printOnOff();

        }

        if (GetAsyncKeyState(VK_NUMPAD8) & 1)
        {
            bTriggerbot = !bTriggerbot;
            printOnOff();

        }

        if (GetAsyncKeyState(VK_NUMPAD9) & 1)
        {
            bFly = !bFly;
            printOnOff();
            if (bFly)
            {
                localPlayer->invisible = 11;
                localPlayer->spectator = 5;
            }
            else
            {
                localPlayer->invisible = 0;
                localPlayer->spectator = 0;
            }
        }

        if (GetAsyncKeyState(VK_SPACE))
        {
            localPlayer->jump = 1;
        }

        if (bSpeedHack)
        {
            if (!(localPlayer->up && localPlayer->down))
            {
                if (localPlayer->up)
                    localPlayer->frontback = speed_val;
                if (localPlayer->down)
                    localPlayer->frontback = -speed_val;
            }

            if (!(localPlayer->left && localPlayer->right))
            {
                if (localPlayer->left)
                    localPlayer->leftright = speed_val;
                if (localPlayer->right)
                    localPlayer->leftright = -speed_val;
            }
        }

        if (bTriggerbot)
        {
            playerent* CrossHairEnt = GetCrossHairEnt();

            if (CrossHairEnt)
            {
                if (localPlayer->team != CrossHairEnt->team)
                {
                    localPlayer->attack = 1;
                }
            }
            else
            {
                localPlayer->attack = 0;
            }
        }

        

        Sleep(10);
    }

    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hmodule, 0);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, 0));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

