#pragma once
#include "framework.h"

namespace Hooks
{
	bool bStarted = false;

	void(*ProcessEvent)(void*, void*, void*);
	void ProcessEventHook(UObject* Object, UFunction* Function, void* Parameters)
	{
		auto FunctionName = Function->GetFullName();

		if (FunctionName.contains("BP_PlayButton"))
		{
			Globals::LocalPlayerController::SwitchLevel();
		}

		if (FunctionName == "Function Engine.GameMode.ReadyToStartMatch" && !bStarted)
		{
			Log("Starting Servers.\n");
			Globals::Init();
			Replication::Init();
			Loot::Init();
			Abilities::Init();
			Server::Listen();
			NetHooks::Init();
				
			bStarted = true;

			Globals::GameState::ChangePlaylistID(Globals::PlaylistID);
			Globals::GameSession::SetMaxPlayers(Globals::MaxPlayers);
			Globals::GameMode::StartMatch();

			Globals::LocalPlayerController::Get()->CheatManager->DestroyAll(APlayerController::StaticClass());

			StaticLoadObject<UBlueprintGeneratedClass>(L"/Game/Abilities/Player/Constructor/Perks/ContainmentUnit/GE_Constructor_ContainmentUnit_Applied.GE_Constructor_ContainmentUnit_Applied_C");
		}

		if (FunctionName == "Function GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbility")
		{
			auto Params = (UAbilitySystemComponent_ServerTryActivateAbility_Params*)Parameters;
			auto AbilitySystemComp = (UAbilitySystemComponent*)Object;

			auto AbilityToActivate = Params->AbilityToActivate;
			auto PredictionKey = Params->PredictionKey;

			UGameplayAbility* InstancedAbility = nullptr;

			Abilities::InternalTryActivateAbilityLong(AbilitySystemComp, AbilityToActivate, PredictionKey, &InstancedAbility, nullptr, nullptr);

		}

		if (FunctionName == "Function GameplayAbilities.AbilitySystemComponent.ServerAbilityRPCBatch")
		{
			auto AbilityComp = (UAbilitySystemComponent*)Object;
			auto CurrentParams = (UAbilitySystemComponent_ServerAbilityRPCBatch_Params*)Parameters;

			FGameplayAbilitySpec* FoundSpec = Abilities::FindAbilitySpecFromHandle(AbilityComp, CurrentParams->BatchInfo.AbilitySpecHandle);
			UGameplayAbility* InstancedAbility = nullptr;

			if (FoundSpec && FoundSpec->Ability)
			{
				if(auto bFailed = Abilities::InternalTryActivateAbilityLong(AbilityComp, CurrentParams->BatchInfo.AbilitySpecHandle, CurrentParams->BatchInfo.PredictionKey, &InstancedAbility, nullptr, &FoundSpec->Ability->CurrentEventData))
					AbilityComp->ClientActivateAbilityFailed(CurrentParams->BatchInfo.AbilitySpecHandle, CurrentParams->BatchInfo.PredictionKey.Base);
			}
		}

		if (FunctionName == "Function FortniteGame.FortPlayerController.ServerLoadingScreenDropped")
		{
			auto Player = (AFortPlayerController*)Object;
			auto Pawn = (AFortPlayerPawnAthena*)Player->Pawn;

			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Abilities::GrantGameplayAbilities, Pawn, 0, 0);
		}

		if (FunctionName == "Function FortniteGame.FortPlayerController.ServerAttemptInteract")
		{
			auto Player = (AFortPlayerController*)Object;
			auto Params = (AFortPlayerController_ServerAttemptInteract_Params*)Parameters;

			auto ReceivingActor = (AKeepItemContainer*)Params->ReceivingActor;
			if (ReceivingActor && ReceivingActor->GetFullName().contains("Tiered_Chest"))
			{
				ReceivingActor->InitializeContainer(2);
				ReceivingActor->bAlreadySearched = true;
				ReceivingActor->OnRep_bAlreadySearched();
				auto ps = (*Player->PlayerState);
				ReceivingActor->SearchedBy.Add((AFortPlayerState)ps);
				ReceivingActor->OnRep_SearchedBy();
				ReceivingActor->SearchBounceData.SearchAnimationCount++;
				auto Location = ReceivingActor->K2_GetActorLocation();
				Loot::SpawnPickup(Location, Loot::GetRandomWeapon(), 1);
				Loot::SpawnPickup(Location, Loot::GetRandomCosumable(), 1);
			}
		}

		if (FunctionName == "Function FortniteGame.FortPlayerControllerAthena.ServerAttemptAircraftJump")
		{
			auto Player = (AFortPlayerControllerAthena*)Object;
			auto Pawn = Player->Pawn;

			if (!Server::bBusStarted)
				Server::bBusStarted = true;

			if (Player->IsInAircraft())
			{
				if (Pawn)
					Pawn->K2_DestroyActor();

				auto Aircraft = ((AFortGameStateAthena*)Globals::World::Get()->GameState)->GetAircraft();
				if (Aircraft)
					Player::Spawn(Player, Aircraft->K2_GetActorLocation());
			}
		}

		if (FunctionName == "Function FortniteGame.FortGameModeAthena.OnAircraftExitedDropZone")
		{
			for (int i = 0; i < Server::NetDriver->ClientConnections.Num(); i++)
			{
				auto Connection = Server::NetDriver->ClientConnections[i];

				if (Connection && Connection->PlayerController)
					if(auto Player = (AFortPlayerControllerAthena*)(Connection->PlayerController))
						if (Player->IsInAircraft())
							Player->ServerAttemptAircraftJump({});
			}
		}

		if (FunctionName== "Function FortniteGame.FortPlayerPawn.ServerHandlePickup")
		{
			auto Pawn = (APlayerPawn_Athena_C*)Object;
			auto Params = (AFortPlayerPawn_ServerHandlePickup_Params*)Parameters;

			if (Pawn)
				Player::HandlePickup(Pawn, Params);
		}

		if (FunctionName == "Function FortniteGame.FortPlayerController.ServerSpawnInventoryDrop")
		{
			auto Player = (AFortPlayerControllerAthena*)Object;
			auto Params = (AFortPlayerController_ServerSpawnInventoryDrop_Params*)Parameters;

			if (Player)
				Player::DropItem(Player, Params);
		}

		if (FunctionName == "Function FortniteGame.FortPlayerController.ServerExecuteInventoryItem")
		{
			auto Player = (AFortPlayerControllerAthena*)Object;
			auto Params = (AFortPlayerController_ServerExecuteInventoryItem_Params*)Parameters;

			if (Player)
			{
					Inventory::ExecuteInventoryItem(Player, Params->ItemGuid);
					auto Weapon = reinterpret_cast<AFortPlayerPawnAthena*>(Player->Pawn)->CurrentWeapon;
					if (Weapon && Weapon->Class && Weapon->IsA(AFortWeap_BuildingTool::StaticClass())) 
					{
						AFortWeap_BuildingTool* BuildingTool = reinterpret_cast<AFortWeap_BuildingTool*>(Weapon);
						Building::ShowBuildPreviews(BuildingTool, Weapon->WeaponData);
					}
			}
		}

		if (FunctionName == "Function FortniteGame.FortPlayerController.ServerReadyToStartMatch")
		{
			auto Player = (AFortPlayerController*)Object;
			Player->bReadyToStartMatch = true;
		}

		if (FunctionName == "Function FortniteGame.FortPlayerController.ServerClientPawnLoaded")
		{
			auto Player = (AFortPlayerController*)Object;
			auto Params = (AFortPlayerController_ServerClientPawnLoaded_Params*)Parameters;
			Player->bClientPawnIsLoaded = Params->bIsPawnLoaded;
		}

		if (FunctionName == "Function FortniteGame.FortPlayerController.ServerSetClientHasFinishedLoading")
		{
			auto Player = (AFortPlayerController*)Object;
			auto Params = (AFortPlayerController_ServerSetClientHasFinishedLoading_Params*)Parameters;
			Player->bHasClientFinishedLoading = Params->bInHasFinishedLoading;
		}

		if (FunctionName== "Function FortniteGame.FortPlayerController.ReturnToMainMenu")
		{
			auto Player = (AFortPlayerController*)Object;
			auto Params = (APlayerController_ClientReturnToMainMenu_Params*)Parameters;
			Server::PlayerLeave(Player);
		}

		if (FunctionName == "Function FortniteGame.FortPlayerController.ServerCreateBuildingActor")
		{
			auto Player = (AFortPlayerController*)Object;
			auto Params = (AFortPlayerController_ServerCreateBuildingActor_Params*)Parameters;
			if (Params)
			{
				auto BuildClass = Params->BuildingClassData.BuildingClass;
				auto Loc = Params->BuildLoc;
				auto Rot = Params->BuildRot;

				auto BuildingActor = Globals::GameplayStatics::SpawnActor<ABuildingSMActor>(BuildClass, Loc, Rot);
				if (BuildingActor)
				{
					BuildingActor->bPlayerPlaced = true;
					BuildingActor->SetMirrored(Params->bMirrored);
					BuildingActor->InitializeKismetSpawnedBuildingActor(BuildingActor, Player); // 0x0000000000000016 crash
				}
			}
		}

		if (FunctionName == "Function Engine.Actor.ReceiveDestroyed" && Server::BeaconHost)
		{
			auto Actor = (AActor*)Object;

			if (NetHooks::NotifyActorDestroyed && Server::NetDriver)
				NetHooks::NotifyActorDestroyed(Server::NetDriver, Actor, false);
		}

	/*	if (Server::CheckIfCheat(FunctionName))
		{
			Log(FunctionName + "\n");
			if (FunctionName.contains("PlayerController"))
			{
				auto Player = (AFortPlayerControllerAthena*)Object;
				Log("Possible cheater: " + Helpers::GetPlayerName(Player) + "\n");
			}

			if (FunctionName.contains("Pawn"))
			{
				auto Player = (APlayerPawn_Athena_C*)Object;
				Log("Possible cheater: " + Helpers::GetPlayerName(Player) + "\n");
			}

			if (FunctionName.contains("CheatManager"))
			{
				auto CheatManager = (UFortCheatManager*)Object;
				Log("Cheat Manager Function.\n");
			}
		}*/

		return ProcessEvent(Object, Function, Parameters);
	}

	void Init()
	{
		Log("Hooks Init.\n");

		auto PEAddr = ((uintptr_t)GetModuleHandle(0)) + 0x1427390;
		ProcessEvent = decltype(ProcessEvent)(PEAddr);

		MH_CreateHook(reinterpret_cast<LPVOID>(PEAddr), ProcessEventHook, reinterpret_cast<LPVOID*>(&ProcessEvent));
		MH_EnableHook(reinterpret_cast<LPVOID>(PEAddr));
	}
}