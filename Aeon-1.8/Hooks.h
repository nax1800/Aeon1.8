#pragma once
#include "framework.h"

namespace Hooks
{
	bool bStarted = false;

	static std::vector<std::pair<std::string, std::function<bool(UObject*, UFunction*, void*)>>> Functions;

	bool ReadyToStartMatch(UObject* Object, UFunction* Function, void* Parameters)
	{
		if (!bStarted)
		{
			Globals::Init();
			Replication::Init();
			Loot::Init();
			Abilities::Init();
			Server::Listen();
			Watermark::Init();
			NetHooks::Init();

			bStarted = true;

			Globals::GameState::ChangePlaylistID(Globals::PlaylistID);
			Globals::GameSession::SetMaxPlayers(Globals::MaxPlayers);
			Globals::GameMode::StartMatch();

			Globals::LocalPlayerController::Get()->CheatManager->DestroyAll(APlayerController::StaticClass());

			StaticLoadObject<UBlueprintGeneratedClass>(L"/Game/Abilities/Player/Constructor/Perks/ContainmentUnit/GE_Constructor_ContainmentUnit_Applied.GE_Constructor_ContainmentUnit_Applied_C");
		}
		return false;
	}

	bool ServerTryActivateAbility(UObject* Object, UFunction* Function, void* Parameters)
	{
		auto Params = (UAbilitySystemComponent_ServerTryActivateAbility_Params*)Parameters;
		auto AbilitySystemComp = (UAbilitySystemComponent*)Object;

		auto AbilityToActivate = Params->AbilityToActivate;
		auto PredictionKey = Params->PredictionKey;

		UGameplayAbility* InstancedAbility = nullptr;

		Abilities::InternalTryActivateAbilityLong(AbilitySystemComp, AbilityToActivate, PredictionKey, &InstancedAbility, nullptr, nullptr);

		return false;
	}

	bool ServerAbilityRPCBatch(UObject* Object, UFunction* Function, void* Parameters)
	{
		auto AbilityComp = Helpers::CastObject<UAbilitySystemComponent>(Object);
		auto CurrentParams = (UAbilitySystemComponent_ServerAbilityRPCBatch_Params*)Parameters;

		FGameplayAbilitySpec* FoundSpec = Abilities::FindAbilitySpecFromHandle(AbilityComp, CurrentParams->BatchInfo.AbilitySpecHandle);
		UGameplayAbility* InstancedAbility = nullptr;

		if (FoundSpec && FoundSpec->Ability)
		{
			if (auto bFailed = Abilities::InternalTryActivateAbilityLong(AbilityComp, CurrentParams->BatchInfo.AbilitySpecHandle, CurrentParams->BatchInfo.PredictionKey, &InstancedAbility, nullptr, &FoundSpec->Ability->CurrentEventData))
				AbilityComp->ClientActivateAbilityFailed(CurrentParams->BatchInfo.AbilitySpecHandle, CurrentParams->BatchInfo.PredictionKey.Base);
		}

		return false;
	}

	bool ServerLoadingScreenDropped(UObject* Object, UFunction* Function, void* Parameters)
	{
		auto Player = Helpers::CastObject<AFortPlayerControllerAthena>(Object);
		auto Pawn = (AFortPlayerPawnAthena*)Player->Pawn;
		Abilities::GrantGameplayAbilities(Pawn);

		return false;
	}

	bool ServerAttemptInteract(UObject* Object, UFunction* Function, void* Parameters)
	{
		auto Player = (AFortPlayerController*)Object;
		auto Params = (AFortPlayerController_ServerAttemptInteract_Params*)Parameters;

		auto ReceivingActor = Helpers::CastObject<ABuildingContainer>(Params->ReceivingActor);
		if (ReceivingActor && Helpers::GetObjectName(ReceivingActor).contains("Tiered_Chest"))
		{
			auto Location = ReceivingActor->K2_GetActorLocation();
			Loot::SpawnPickup(Location, Loot::GetRandomWeapon(), 1);
			Loot::SpawnPickup(Location, Loot::GetRandomCosumable(), 1);
			ReceivingActor->K2_DestroyActor();
		}

		return false;
	}

	bool ServerAttemptAircraftJump(UObject* Object, UFunction* Function, void* Parameters)
	{
		auto Player = Helpers::CastObject<AFortPlayerControllerAthena>(Object);
		auto Pawn = Player->Pawn;

		if (!Server::bBusStarted)
			Server::bBusStarted = true;

		if (Player->IsInAircraft())
		{
			if (Pawn)
				Pawn->K2_DestroyActor();

			auto Aircraft = Helpers::CastObject<AFortGameStateAthena>(Globals::World::Get()->GameState)->GetAircraft();
			if (Aircraft)
				Player::Spawn(Player, Aircraft->K2_GetActorLocation());
		}

		return false;
	}

	bool OnAircraftExitedDropZone(UObject* Object, UFunction* Function, void* Parameters)
	{
		for (int i = 0; i < Server::NetDriver->ClientConnections.Num(); i++)
		{
			auto Connection = Server::NetDriver->ClientConnections[i];

			if (Connection && Connection->PlayerController)
				if (auto Player = (AFortPlayerControllerAthena*)(Connection->PlayerController))
					if (Player->IsInAircraft())
						Player->ServerAttemptAircraftJump({});
		}

		return false;
	}

	bool ServerHandlePickup(UObject* Object, UFunction* Function, void* Parameters)
	{
		auto Pawn = Helpers::CastObject<APlayerPawn_Athena_C>(Object);
		auto Params = (AFortPlayerPawn_ServerHandlePickup_Params*)Parameters;

		if (Pawn)
			Player::HandlePickup(Pawn, Params);

		return false;
	}

	bool ServerSpawnInventoryDrop(UObject* Object, UFunction* Function, void* Parameters)
	{
		auto Player = Helpers::CastObject<AFortPlayerControllerAthena>(Object);
		auto Params = (AFortPlayerController_ServerSpawnInventoryDrop_Params*)Parameters;

		if (Player)
			Player::DropItem(Player, Params);

		return false;
	}

	bool ServerExecuteInventoryItem(UObject* Object, UFunction* Function, void* Parameters)
	{
		auto Player = Helpers::CastObject<AFortPlayerControllerAthena>(Object);
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

		return false;
	}

	bool ServerCreateBuildingActor(UObject* Object, UFunction* Function, void* Parameters)
	{
		auto Player = Helpers::CastObject<AFortPlayerControllerAthena>(Object);
		auto Params = (AFortPlayerController_ServerCreateBuildingActor_Params*)Parameters;
		if (Player && Params)
		{
			auto BuildClass = Params->BuildingClassData.BuildingClass;
			auto Location = Params->BuildLoc;
			auto Rotation = Params->BuildRot;
			auto Mirrored = Params->bMirrored;

			if (BuildClass)
			{
				Building::PlaceBuild(BuildClass, Mirrored, Location, Rotation);
			}
		}

		return false;
	}

	bool ReceiveDestroyed(UObject* Object, UFunction* Function, void* Parameters)
	{
		auto Actor = (AActor*)Object;

		if (Actor && NetHooks::NotifyActorDestroyed && Server::NetDriver)
			NetHooks::NotifyActorDestroyed(Server::NetDriver, Actor, false);

		return false;
	}

	bool BP_PlayButton_Clicked(UObject* Object, UFunction* Function, void* Parameters)
	{
		Globals::LocalPlayerController::SwitchLevel();

		return false;
	}

	bool ClientOnPawnDied(UObject* Object, UFunction* Function, void* Parameters)
	{
		auto Params = (AFortPlayerControllerZone_ClientOnPawnDied_Params*)Parameters;
		if (Params)
		{
			auto DeadController = (AFortPlayerControllerAthena*)Object;
			auto DeadPawn = (APlayerPawn_Athena_C*)DeadController->GetPlayerPawn();
			auto DeadState = (AFortPlayerStateAthena*)DeadController->PlayerState;

			auto DeathReport = Params->DeathReport;
			DeathReport.bNotifyUI = true;
			auto KillerPawn = (APlayerPawn_Athena_C*)DeathReport.KillerPawn;
			auto KillerPlayerState = (AFortPlayerStateAthena*)DeathReport.KillerPlayerState;
			if (KillerPlayerState)
			{
				DeadState->ClientReportKill(KillerPlayerState->PlayerName);
				KillerPlayerState->KillScore++;
				KillerPlayerState->OnRep_Kills();
				Player::Syphon(KillerPawn);
			}
		}
		return false;
	}

	void(*ProcessEvent)(void*, void*, void*);
	void ProcessEventHook(UObject* Object, UFunction* Function, void* Parameters)
	{
		auto FunctionName = Function->GetFullName();

		if (FunctionName == "Function FortniteGame.FortPlayerController.ServerReadyToStartMatch")
		{
			auto Player = Helpers::CastObject<AFortPlayerControllerAthena>(Object);
			Player->bReadyToStartMatch = true;
		}

		if (FunctionName == "Function FortniteGame.FortPlayerController.ServerClientPawnLoaded")
		{
			auto Player = Helpers::CastObject<AFortPlayerControllerAthena>(Object);
			auto Params = (AFortPlayerController_ServerClientPawnLoaded_Params*)Parameters;
			Player->bClientPawnIsLoaded = Params->bIsPawnLoaded;
		}

		if (FunctionName == "Function FortniteGame.FortPlayerController.ServerSetClientHasFinishedLoading")
		{
			auto Player = Helpers::CastObject<AFortPlayerControllerAthena>(Object);
			auto Params = (AFortPlayerController_ServerSetClientHasFinishedLoading_Params*)Parameters;
			Player->bHasClientFinishedLoading = Params->bInHasFinishedLoading;
		}

		for (auto& Func : Functions)
		{
			if (FunctionName == Func.first)
			{
				if (Func.second(Object, Function, Parameters))
					return;

				break;
			}
		}

		return ProcessEvent(Object, Function, Parameters);
	}

	void AddFunction(std::string FullName, std::function<bool(UObject*, UFunction*, void*)> Function) { Functions.push_back({ FullName, Function }); }

	void Init()
	{
		Log("Hooks Init.\n");

		ProcessEvent = decltype(ProcessEvent)(Addresses::ProcessEvent);

		AddFunction("Function AthenaMatchmakingWidget.AthenaMatchmakingWidget_C.BndEvt__BP_PlayButton_K2Node_ComponentBoundEvent_1_CommonButtonClicked__DelegateSignature", BP_PlayButton_Clicked);
		AddFunction("Function Engine.GameMode.ReadyToStartMatch", ReadyToStartMatch);
		AddFunction("Function GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbility", ServerTryActivateAbility);
		AddFunction("Function GameplayAbilities.AbilitySystemComponent.ServerAbilityRPCBatch", ServerAbilityRPCBatch);
		AddFunction("Function FortniteGame.FortPlayerController.ServerLoadingScreenDropped", ServerLoadingScreenDropped);
		AddFunction("Function FortniteGame.FortPlayerController.ServerAttemptInteract", ServerAttemptInteract);
		AddFunction("Function FortniteGame.FortPlayerControllerAthena.ServerAttemptAircraftJump", ServerAttemptAircraftJump);
		AddFunction("Function FortniteGame.FortGameModeAthena.OnAircraftExitedDropZone", OnAircraftExitedDropZone);
		AddFunction("Function FortniteGame.FortPlayerPawn.ServerHandlePickup", ServerHandlePickup);
		AddFunction("Function FortniteGame.FortPlayerController.ServerSpawnInventoryDrop", ServerSpawnInventoryDrop);
		AddFunction("Function FortniteGame.FortPlayerController.ServerExecuteInventoryItem", ServerExecuteInventoryItem);
		AddFunction("Function FortniteGame.FortPlayerController.ServerCreateBuildingActor", ServerCreateBuildingActor);
		AddFunction("Function Engine.Actor.ReceiveDestroyed", ReceiveDestroyed);
		AddFunction("Function FortniteGame.FortPlayerControllerZone.ClientOnPawnDied", ClientOnPawnDied);

		Utils::AddHook(reinterpret_cast<LPVOID>(Addresses::ProcessEvent), ProcessEventHook, reinterpret_cast<LPVOID*>(&ProcessEvent));
	}
}