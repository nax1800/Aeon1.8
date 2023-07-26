#pragma once
#include "framework.h"

namespace NetHooks
{
	__int64 (*WelcomePlayer)(UWorld*, UNetConnection*);
	APlayerController* (*SpawnPlayActor)(UWorld*, UPlayer*, ENetRole, FURL, void*, FString&, uint8_t);
	void(*NotifyControlMessage)(UWorld*, UNetConnection*, uint8_t, void*);
	void (*TickFlush)(UNetDriver*, float);
	void (*NotifyActorDestroyed)(UNetDriver*, AActor*, bool);
	LPVOID(*CollectGarbageInternal)(uint32_t, bool);

	void TickFlushHook(UNetDriver* NetDriver, float DeltaSeconds) { Replication::ReplicateActors(NetDriver); return TickFlush(NetDriver, DeltaSeconds); }
	__int64 KickPatch(__int64, __int64) { return 0; }
	LPVOID CollectGarbageInternalHook(uint32_t KeepFlags, bool bPerformFullPurge) { return 0; }
	__int64 WelcomePlayerHook(UWorld*, UNetConnection* NetConnection) { return WelcomePlayer(Globals::World::Get(), NetConnection); }

	void AOnlineBeaconHost_NotifyControlMessageHook(AOnlineBeaconHost* BeaconHost, UNetConnection* NetConnection, uint8_t a3, void* a4)
	{
		if (Server::bBusStarted)
			return;

		string A3 = to_string(a3);

		if (A3 == "4") {
			NetConnection->CurrentNetSpeed = Globals::NetSpeed;
			return;
		}

		Log("AOnlineBeaconHost::NotifyControlMessage Called: " + A3 + "\n");
		Log("OpenChannels Count: " + to_string(NetConnection->OpenChannels.Num()) + "\n");
		return NotifyControlMessage(Globals::World::Get(), NetConnection, a3, a4);
	}

	bool bFirstPlayerJoined = false;
	APlayerController* SpawnPlayActorHook(UWorld* World, UNetConnection* Connection, ENetRole NetRole, FURL a4, void* a5, FString& Src, uint8_t a7)
	{
		auto PlayerController = (AFortPlayerControllerAthena*)SpawnPlayActor(Globals::World::Get(), Connection, NetRole, a4, a5, Src, a7);
		Connection->PlayerController = PlayerController;
		PlayerController->NetConnection = Connection;
		Connection->OwningActor = PlayerController;

		auto Pawn = Player::Spawn(PlayerController, Server::GetPlayerStart()->K2_GetActorLocation());

		if (!bFirstPlayerJoined)
		{
			bFirstPlayerJoined = true;
			Loot::SpawnFloorLoot();
		}

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

	void Init()
	{
		Log("NetHooks Init.\n");	

		NotifyControlMessage = decltype(NotifyControlMessage)(Addresses::WorldNotifyControlMessage);
		TickFlush = decltype(TickFlush)(Addresses::TickFlush);
		SpawnPlayActor = decltype(SpawnPlayActor)(Addresses::SpawnPlayActor);
		NotifyActorDestroyed = decltype(NotifyActorDestroyed)(Addresses::NotifyActorDestroyed);

		Utils::AddHook((void*)(Addresses::BeaconNotifyControlMessage), AOnlineBeaconHost_NotifyControlMessageHook, nullptr);
		Utils::AddHook((void*)(Addresses::WelcomePlayer), WelcomePlayerHook, (void**)(&WelcomePlayer));
		Utils::AddHook((void*)(Addresses::SpawnPlayActor), SpawnPlayActorHook, (void**)(&SpawnPlayActor));
		Utils::AddHook((void*)(Addresses::KickPatch), KickPatch, nullptr);
		Utils::AddHook((LPVOID)(Addresses::CollectGarbageInternal), CollectGarbageInternalHook, (LPVOID*)(CollectGarbageInternal));
		Utils::AddHook((void*)(Addresses::TickFlush), TickFlushHook, (void**)(&TickFlush));
	}
}