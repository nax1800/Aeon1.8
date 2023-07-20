#pragma once
#include "framework.h"

namespace Addresses
{

	PBYTE CreateChannel = 0;
	PBYTE SetChannelActor = 0;
	PBYTE ReplicateActor = 0;
	PBYTE CallPreReplication = 0;
	PBYTE TickFlush = 0;
	PBYTE SendClientAdjustment = 0;
	PBYTE ActorChannelClose = 0;
	PBYTE IsNetRelevantFor = 0;
	PBYTE NotifyActorDestroyed = 0;
	PBYTE InitHost = 0;
	PBYTE WelcomePlayer = 0;
	PBYTE BeaconNotifyControlMessage = 0;
	PBYTE KickPatch = 0;
	PBYTE InternalTryActivateAbility = 0;
	PBYTE CollectGarbageInternal = 0;
	PBYTE OnReload = 0;
	uintptr_t SpawnPlayActor = 0;
	uintptr_t WorldNotifyControlMessage = 0;
	uintptr_t ProcessEvent = 0;

	bool Init()
	{
		CreateChannel = Utils::FindPattern("\x40\x56\x57\x41\x54\x41\x55\x41\x57\x48\x83\xec\x00\x48\x8b\x01", "xxxxxxxxxxxx?xxx");
		SetChannelActor = Utils::FindPattern("\x4c\x8b\xdc\x55\x53\x57\x41\x54\x49\x8d\xab\x00\x00\x00\x00\x48\x81\xec\x00\x00\x00\x00\x45\x33\xe4", "xxxxxxxxxxx????xxx????xxx");
		ReplicateActor = Utils::FindPattern("\x40\x55\x53\x57\x41\x56\x48\x8d\xac\x24\x00\x00\x00\x00\x48\x81\xec\x00\x00\x00\x00\x48\x8d\x59", "xxxxxxxxxx????xxx????xxx");
		CallPreReplication = Utils::FindPattern("\x48\x85\xd2\x0f\x84\x00\x00\x00\x00\x48\x8b\xc4\x55\x57\x41\x57", "xxxxx????xxxxxxx");
		TickFlush = Utils::FindPattern("\x4c\x8b\xdc\x55\x53\x56\x57\x49\x8d\xab\x00\x00\x00\x00\x48\x81\xec\x00\x00\x00\x00\x41\x0f\x29\x7b", "xxxxxxxxxx????xxx????xxxx");
		SendClientAdjustment = Utils::FindPattern("\x40\x53\x48\x83\xec\x00\x48\x8b\x99\x00\x00\x00\x00\x48\x39\x99", "xxxxx?xxx????xxx");
		ActorChannelClose = Utils::FindPattern("\x48\x89\x5c\x24\x00\x48\x89\x7c\x24\x00\x41\x56\x48\x83\xec\x00\x33\xff\x48\x8b\xd9\x89\x7c\x24", "xxxx?xxxx?xxxxx?xxxxxxxx");
		IsNetRelevantFor = Utils::FindPattern("\x48\x89\x5c\x24\x00\x48\x89\x74\x24\x00\x55\x57\x41\x57\x48\x8b\xec\x48\x81\xec\x00\x00\x00\x00\x48\x8b\xfa", "xxxx?xxxx?xxxxxxxxxx????xxx");
		NotifyActorDestroyed = Utils::FindPattern("\x48\x89\x54\x24\x00\x55\x53\x41\x55\x41\x56\x48\x8d\x6c\x24", "xxxx?xxxxxxxxxx");
		InitHost = Utils::FindPattern("\x48\x8b\xc4\x48\x81\xec\x00\x00\x00\x00\x48\x89\x58\x00\x4c\x8d\x05", "xxxxxx????xxx?xxx");
		WelcomePlayer = Utils::FindPattern("\x48\x8b\xc4\x55\x48\x8d\xa8\x00\x00\x00\x00\x48\x81\xec\x00\x00\x00\x00\x48\x89\x70", "xxxxxxx????xxx????xxx");
		BeaconNotifyControlMessage = Utils::FindPattern("\x40\x55\x53\x56\x57\x41\x54\x41\x56\x41\x57\x48\x8d\xac\x24\x00\x00\x00\x00\x48\x81\xec\x00\x00\x00\x00\x48\x8b\x05\x00\x00\x00\x00\x48\x33\xc4\x48\x89\x85\x00\x00\x00\x00\x33\xff", "xxxxxxxxxxxxxxx????xxx????xxx????xxxxxx????xx");			
		KickPatch = Utils::FindPattern("\x40\x53\x56\x48\x81\xec\x00\x00\x00\x00\x48\x8b\xda", "xxxxxx????xxx");
		InternalTryActivateAbility = Utils::FindPattern("\x4c\x89\x4c\x24\x00\x4c\x89\x44\x24\x00\x89\x54\x24\x00\x55\x56", "xxxx?xxxx?xxx?xx");
		OnReload = Utils::FindPattern("\x40\x53\x56\x41\x54\x41\x55\x41\x57\x48\x83\xec\x00\x44\x8b\xea", "xxxxxxxxxxxx?xxx");
		CollectGarbageInternal = Utils::FindPattern("\x48\x8B\xC4\x48\x89\x58\x08\x88\x50\x10", "xxxxxxxxxx");

		SpawnPlayActor = Utils::BaseAddress() + 0x224CD40;
		WorldNotifyControlMessage = Utils::BaseAddress() + 0x251ADA0;
		ProcessEvent = Utils::BaseAddress() + 0x1427390;

		if (!CreateChannel || !SetChannelActor || !ReplicateActor || !CallPreReplication || !TickFlush || !SendClientAdjustment
			|| !ActorChannelClose || !IsNetRelevantFor || !NotifyActorDestroyed || !InitHost || !WelcomePlayer || !SpawnPlayActor
			|| !BeaconNotifyControlMessage || !KickPatch || !InternalTryActivateAbility || !OnReload || !CollectGarbageInternal)
		{
			Log("Pattern NULL!\n");
			//return false;
		}

		Log("Addresses Init.\n");
		return true;
	}
}
