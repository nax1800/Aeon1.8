#pragma once
#include "framework.h"

namespace Loot
{
	vector<UFortItemDefinition*> Weapons;
	vector<UFortItemDefinition*> Cosumables;

	UFortItemDefinition* GetRandomRifle()
	{
		vector<UFortItemDefinition*> Rifles;
		for (int i = 0; i < Weapons.size(); i++)
			if(auto Weapon = Weapons[i])
				if (Weapon->GetFullName().contains("WID_Assault_"))
					Rifles.push_back(Weapon);

		int Size = Rifles.size();
		int Num = rand() % Size;
		return Rifles[Num];
	}


	UFortItemDefinition* GetRandomShotgun()
	{
		vector<UFortItemDefinition*> Shotguns;
		for (int i = 0; i < Weapons.size(); i++)
			if(auto Weapon = Weapons[i])
				if (Weapon->GetFullName().contains("WID_Shotgun_"))
					Shotguns.push_back(Weapon);

		int Size = Shotguns.size();
		int Num = rand() % Size;
		return Shotguns[Num];
	}

	UFortItemDefinition* GetRandomSniper()
	{
		vector<UFortItemDefinition*> Snipers;
		for (int i = 0; i < Weapons.size(); i++)
			if(auto Weapon = Weapons[i])
				if (Weapon->GetFullName().contains("WID_Sniper_"))
					Snipers.push_back(Weapon);

		int Size = Snipers.size();
		int Num = rand() % Size;
		return Snipers[Num];
	}

	UFortItemDefinition* GetRandomWeapon()
	{
		auto PoolSize = Weapons.size();
		auto RandomNum = rand() % PoolSize;
		return Weapons[RandomNum];
	}


	UFortItemDefinition* GetRandomCosumable()
	{
		auto PoolSize = Cosumables.size();
		auto RandomNum = rand() % PoolSize;
		return Cosumables[RandomNum];
	}

	bool IsCosumable(UFortItemDefinition* Item)
	{
		for (int i = 0; i < Cosumables.size(); i++)
			if(auto Cosumable = Cosumables[i])
				if (Item == Cosumable)
					return true;

		return false;
	}

	UFortItemDefinition* GetRandomItem()
	{
		vector<UFortItemDefinition*> Pool;
		for (int i = 0; i < Weapons.size(); i++) { Pool.push_back(Weapons[i]); }
		for (int i = 0; i < Cosumables.size(); i++) { Pool.push_back(Cosumables[i]); }

		auto PoolSize = Pool.size();
		auto RandomNum = rand() % PoolSize;
		return Pool[RandomNum];
	}

	AFortPickup* SpawnPickup(FVector Location, UFortItemDefinition* ItemDef, int Count)
	{
		auto A = Globals::GameplayStatics::SpawnActor<AFortPickup>(Location, FRotator());
		A->PrimaryPickupItemEntry.ItemDefinition = ItemDef;
		A->PrimaryPickupItemEntry.Count = Count;
		A->OnRep_PrimaryPickupItemEntry();
		A->TossPickup(Location, nullptr, 1, true);
		return A;
	}

	void SpawnFloorLoot()
	{
		auto Containers = Globals::GameplayStatics::GetActors<ATiered_Athena_FloorLoot_01_C>();
		int Count = Containers.Num();
		Log("Floor Loot Containers: " + to_string(Count));
		for (int i = 0; i < Count; i++)
		{
			auto Container = Containers[i];
			if (Container)
			{
				auto Location = Container->K2_GetActorLocation();
				auto Pickup = Globals::GameplayStatics::SpawnActor<AFortPickupAthena>(Location);
				auto Item = GetRandomItem();
				auto bConsumable = IsCosumable(Item);

				Pickup->PrimaryPickupItemEntry.ItemDefinition = Item;
				Pickup->OnRep_PrimaryPickupItemEntry();
				Pickup->TossPickup(Location, nullptr, 1, true);
				Container->K2_DestroyActor();
			}
		}
	}

	void Init()
	{
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_C_Ore_T02.WID_Assault_Auto_Athena_C_Ore_T02"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_C_Ore_T02.WID_Assault_SemiAuto_Athena_C_Ore_T02"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavy_Athena_C_Ore_T02.WID_Pistol_AutoHeavy_Athena_C_Ore_T02"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_C_Ore_T02.WID_Pistol_SemiAuto_Athena_C_Ore_T02"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_SixShooter_Athena_C_Ore_T02.WID_Pistol_SixShooter_Athena_C_Ore_T02"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_C_Ore_T03.WID_Shotgun_Standard_Athena_C_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_UC_Ore_T03.WID_Assault_Auto_Athena_UC_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_UC_Ore_T03.WID_Assault_SemiAuto_Athena_UC_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavy_Athena_UC_Ore_T03.WID_Pistol_AutoHeavy_Athena_UC_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_Scavenger_Athena_UC_Ore_T03.WID_Pistol_Scavenger_Athena_UC_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_UC_Ore_T03.WID_Pistol_SemiAuto_Athena_UC_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_SixShooter_Athena_UC_Ore_T03.WID_Pistol_SixShooter_Athena_UC_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_R_Ore_T03.WID_Assault_Auto_Athena_R_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_R_Ore_T03.WID_Assault_SemiAuto_Athena_R_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Surgical_Athena_R_Ore_T03.WID_Assault_Surgical_Athena_R_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Launcher_Grenade_Athena_R_Ore_T03.WID_Launcher_Grenade_Athena_R_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Launcher_Rocket_Athena_R_Ore_T03.WID_Launcher_Rocket_Athena_R_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavy_Athena_R_Ore_T03.WID_Pistol_AutoHeavy_Athena_R_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_Scavenger_Athena_R_Ore_T03.WID_Pistol_Scavenger_Athena_R_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_R_Ore_T03.WID_Pistol_SemiAuto_Athena_R_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_SixShooter_Athena_R_Ore_T03.WID_Pistol_SixShooter_Athena_R_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_R_Ore_T03.WID_Shotgun_SemiAuto_Athena_R_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_R_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_R_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Launcher_Rocket_Athena_R_Ore_T03.WID_Launcher_Rocket_Athena_R_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_VR_Ore_T03.WID_Assault_AutoHigh_Athena_VR_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Surgical_Athena_VR_Ore_T03.WID_Assault_AutoHigh_Athena_VR_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Launcher_Grenade_Athena_VR_Ore_T03.WID_Assault_AutoHigh_Athena_VR_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Launcher_Rocket_Athena_VR_Ore_T03.WID_Assault_AutoHigh_Athena_VR_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_Scavenger_Athena_VR_Ore_T03.WID_Assault_AutoHigh_Athena_VR_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_VR_Ore_T03.WID_Assault_AutoHigh_Athena_VR_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_VR_Ore_T03.WID_Assault_AutoHigh_Athena_VR_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_Standard_Scope_Athena_VR_Ore_T03.WID_Assault_AutoHigh_Athena_VR_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Launcher_Rocket_Athena_VR_Ore_T03.WID_Launcher_Rocket_Athena_VR_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Launcher_Grenade_Athena_SR_Ore_T03.WID_Launcher_Grenade_Athena_SR_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Launcher_Rocket_Athena_SR_Ore_T03.WID_Launcher_Rocket_Athena_SR_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_SR_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_SR_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_Standard_Scope_Athena_SR_Ore_T03.WID_Sniper_Standard_Scope_Athena_SR_Ore_T03"));
		Weapons.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Launcher_Rocket_Athena_SR_Ore_T03.WID_Launcher_Rocket_Athena_SR_Ore_T03"));

		Cosumables.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Consumables/Bandage/Athena_Bandage.Athena_Bandage"));
		Cosumables.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Consumables/Grenade/Athena_Grenade.Athena_Grenade"));
		Cosumables.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Consumables/Medkit/Athena_Medkit.Athena_Medkit"));
		Cosumables.push_back(FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Consumables/Shields/Athena_Shields.Athena_Shields"));
	}
}