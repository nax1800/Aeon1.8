#pragma once
#include "framework.h"

namespace NetHooks
{
	__int64(*WelcomePlayer)(UWorld* World, UNetConnection* NetConnection);
	APlayerController* (*SpawnPlayActor)(UWorld* World, UPlayer* Player, ENetRole NetRole, FURL URL, void* a5, FString& Source, uint8_t a7);
	void(*UWorld_NotifyControlMessage)(UWorld* World, UNetConnection* NetConnection, uint8_t a3, void* a4);
	void (*TickFlush)(UNetDriver*, float DeltaSeconds);
	void (*NotifyActorDestroyed)(UNetDriver*, AActor*, bool);
	LPVOID(*CollectGarbageInternal)(uint32_t, bool);

	void __fastcall AOnlineBeaconHost_NotifyControlMessageHook(AOnlineBeaconHost* BeaconHost, UNetConnection* NetConnection, uint8_t a3, void* a4)
	{
		if (Server::bBusStarted)
			return;

		if (std::to_string(a3) == "4") {
			NetConnection->CurrentNetSpeed = 30000;
			return;
		}

		Log("AOnlineBeaconHost::NotifyControlMessage Called: " + std::to_string(a3) + "\n");
		Log("OpenChannels Count: " + std::to_string(NetConnection->OpenChannels.Num()) + "\n");
		return UWorld_NotifyControlMessage(Globals::World::Get(), NetConnection, a3, a4);
	}

	void TickFlushHook(UNetDriver* NetDriver, float DeltaSeconds)
	{
		Replication::ReplicateActors(NetDriver);

		return TickFlush(NetDriver, DeltaSeconds);
	}

	__int64 KickPatch(__int64, __int64)
	{
		return 0;
	}

	__int64 __fastcall WelcomePlayerHook(UWorld*, UNetConnection* NetConnection)
	{
		return WelcomePlayer(Globals::World::Get(), NetConnection);
	}

	APlayerController* SpawnPlayActorHook(UWorld*, UNetConnection* Connection, ENetRole NetRole, FURL a4, void* a5, FString& Src, uint8_t a7)
	{
		auto PlayerController = (AFortPlayerControllerAthena*)SpawnPlayActor(Globals::World::Get(), Connection, NetRole, a4, a5, Src, a7);
		Connection->PlayerController = PlayerController;
		PlayerController->NetConnection = Connection;
		Connection->OwningActor = PlayerController;

		auto Pawn = Player::Spawn(PlayerController, Server::GetPlayerStart()->K2_GetActorLocation());

		PlayerController->bHasServerFinishedLoading = true;
		PlayerController->OnRep_bHasServerFinishedLoading();

		auto PlayerState = (AFortPlayerStateAthena*)(PlayerController->PlayerState);

		PlayerState->TeamIndex = EFortTeam::HumanPvP_Team69;
		PlayerState->OnRep_TeamIndex();

		PlayerState->bHasFinishedLoading = true;
		PlayerState->bHasStartedPlaying = true;
		PlayerState->bIsGameSessionAdmin = true;
		PlayerState->bIsGameSessionOwner = true;
		PlayerState->bIsWorldDataOwner = true;
		PlayerState->bIsReadyToContinue = true;
		PlayerState->OnRep_bHasStartedPlaying();
		PlayerState->OnRep_CharacterParts();

		auto CheatManager = (UFortCheatManager*)(Globals::GameplayStatics::Get()->STATIC_SpawnObject(UFortCheatManager::StaticClass(), PlayerController));
		PlayerController->CheatManager = CheatManager;
		CheatManager->BackpackSetSize(5);

		if(Globals::bInfiniteAmmo)
		CheatManager->ToggleInfiniteAmmo();

		if (Globals::bInfiniteResources)
			CheatManager->BuildFree();

		Inventory::SetupInventory(PlayerController);
		Inventory::UpdateInventory(PlayerController);

		PlayerState->OnRep_HeroType();
		PlayerState->OnRep_PlayerTeam();

		auto DisplayName = PlayerState->PlayerName.ToString();
		Log(DisplayName + " Joined.\n");

		return PlayerController;
	}

	LPVOID CollectGarbageInternalHook(uint32_t KeepFlags, bool bPerformFullPurge)
	{
		return 0;
	}


	void Init()
	{
		Log("NetHooks Init.\n");	

		UWorld_NotifyControlMessage = decltype(UWorld_NotifyControlMessage)(Utils::BaseAddress() + 0x251ADA0);
		TickFlush = decltype(TickFlush)(Addresses::TickFlush);
		SpawnPlayActor = decltype(SpawnPlayActor)(Addresses::SpawnPlayActor);
		NotifyActorDestroyed = decltype(NotifyActorDestroyed)(Addresses::NotifyActorDestroyed);

		MH_CreateHook((void*)(Addresses::BeaconNotifyControlMessage), AOnlineBeaconHost_NotifyControlMessageHook, nullptr);
		MH_EnableHook((void*)(Addresses::BeaconNotifyControlMessage));

		MH_CreateHook((void*)(Addresses::WelcomePlayer), WelcomePlayerHook, (void**)(&WelcomePlayer));
		MH_EnableHook((void*)(Addresses::WelcomePlayer));

		MH_CreateHook((void*)(Utils::BaseAddress() + 0x224CD40), SpawnPlayActorHook, (void**)(&SpawnPlayActor));
		MH_EnableHook((void*)(Utils::BaseAddress() + 0x224CD40));

		MH_CreateHook((void*)(Addresses::KickPatch), KickPatch, nullptr);
		MH_EnableHook((void*)(Addresses::KickPatch));

		MH_CreateHook((LPVOID)(Addresses::CollectGarbageInternal), CollectGarbageInternalHook, (LPVOID*)(CollectGarbageInternal));
		MH_EnableHook((LPVOID)(Addresses::CollectGarbageInternal));

		MH_CreateHook((void*)(Addresses::TickFlush), TickFlushHook, (void**)(&TickFlush));
		MH_EnableHook((void*)(Addresses::TickFlush));
	}
}