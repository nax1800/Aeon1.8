#pragma once

#include <windows.h>
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include <wincrypt.h>
#include <functional>
#include <map>
#include <Psapi.h>
#include <intrin.h>

#include <d3d11.h>

#include "kiero/minhook/include/MinHook.h"
#include "kiero/kiero.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "SDK.hpp"
#include "Color.hpp"

using namespace SDK;
using namespace std;

void Log(string ToLog)
{
	cout << hue::green << "LogAeon:Info: " << ToLog << hue::white;
}

static auto StaticLoadObjectInternal = reinterpret_cast<UObject * (*)(UClass * InClass, UObject * InOuter, const TCHAR * Name, const TCHAR * FileName, uint32_t LoadFlags, UPackageMap * Sandbox, bool bAllowObjectReconciliation)>(uintptr_t(GetModuleHandle(NULL)) + 0x142EE30);

template <typename T>
static T* StaticLoadObject(const TCHAR* InPath)
{
	return (T*)StaticLoadObjectInternal(T::StaticClass(), nullptr, InPath, nullptr, 0, nullptr, false);
}

template <typename T>
static T* FindObjectFast(string ObjectName, UClass* ObjectClass = UObject::StaticClass())
{
	auto OrigInName = wstring(ObjectName.begin(), ObjectName.end()).c_str();

	auto StaticFindObject = (T * (*)(UClass*, UObject * Package, const wchar_t* OrigInName, bool ExactClass))((uintptr_t)GetModuleHandleA(0) + 0x142D2E0);
	return StaticFindObject(ObjectClass, nullptr, OrigInName, false);
}

#include "Utils.h"
#include "Addresses.h"
#include "Globals.h"
#include "Helpers.h"
#include "Building.h"
#include "Replication.h"
#include "Server.h"
#include "Loot.h"
#include "Inventory.h"
#include "Player.h"
#include "Watermark.h"
#include "Abilities.h"
#include "NetHooks.h"
#include "Hooks.h"
#include "Menu.h"
