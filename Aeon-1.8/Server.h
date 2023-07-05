#pragma once
#include "framework.h"

namespace Server
{
	bool (*InitHost)(AOnlineBeaconHost*);

	AFortOnlineBeaconHost* BeaconHost;
	UNetDriver* NetDriver;

	bool bBusStarted = false;

	vector<string> CheatFunctions = {
	"Function FortniteGame.FortPlayerController.ServerCheat",
	"Function FortniteGame.FortPawn.SetHealth",
	"Function FortniteGame.FortPawn.SetMaxHealth",
	"Function FortniteGame.FortHealthSet.OnRep_Shield",
	"Function FortniteGame.FortHealthSet.OnRep_CurrentShield",
	"Function Engine.CheatManager.BugItGo",
	"Function Engine.CheatManager.BugIt",
	"Function Engine.PlayerController.ServerChangeName"
	};

	void Listen()
	{
		InitHost = decltype(InitHost)(Addresses::InitHost);

		BeaconHost = Globals::GameplayStatics::SpawnActor<AFortOnlineBeaconHost>();
		BeaconHost->ListenPort = 7777;
		if (InitHost(BeaconHost))
		{
			BeaconHost->NetDriver->World = Globals::World::Get();
			Globals::World::Get()->LevelCollections[0].NetDriver = BeaconHost->NetDriver;
			Globals::World::Get()->LevelCollections[1].NetDriver = BeaconHost->NetDriver;
			NetDriver = BeaconHost->NetDriver;
			BeaconHost->BeaconState = 0;
			Log("Server is now listening on port: 7777\n");
		}
	}

	void KickPlayer(AFortPlayerController* Player, string Reason = "Nothing")
	{
		Player->ClientTravel(L"Frontend", ETravelType::TRAVEL_Absolute, false, {});
		Log("Kicked '" + Helpers::GetPlayerName(Player) + "'.\nReason: " + Reason + ".\n");
	}

	void PlayerLeave(AFortPlayerController* Player)
	{
		Player->ClientTravel(L"Frontend", ETravelType::TRAVEL_Absolute, false, {});
		Log(Helpers::GetPlayerName(Player) + " Left.\n");
	}

	APlayerStart* GetPlayerStart()
	{
		auto PlayerStarts = Globals::GameplayStatics::GetActors<APlayerStart>();
		auto RanNum = rand() & PlayerStarts.Num();
		return PlayerStarts[RanNum];
	}

	bool CheckIfCheat(string A)
	{
		for (int i = 0; i < CheatFunctions.size(); i++)
			if (CheatFunctions[i] == A)
				return true;

		return false;
	}

	void StartBus() { Globals::KismetSystemLibrary::ExecuteCommand(L"startaircraft"); }
	void StartZone() { Globals::KismetSystemLibrary::ExecuteCommand(L"startsafezone"); }
	void PauseZone() { Globals::KismetSystemLibrary::ExecuteCommand(L"pausesafezone"); }
	void Whatever() { }
}