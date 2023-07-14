#pragma once
#include "framework.h"

namespace Player
{
	void RemoveRegenEffects(APlayerPawn_Athena_C* Pawn)
	{
		Pawn->ShieldRegenDelayGameplayEffect = nullptr;
		Pawn->ShieldRegenGameplayEffect = nullptr;
		Pawn->HealthRegenDelayGameplayEffect = nullptr;
		Pawn->HealthRegenGameplayEffect = nullptr;
	}

	void FixShield(APlayerPawn_Athena_C* Pawn)
	{
		auto HealthSet = Pawn->HealthSet;
		HealthSet->CurrentShield.Minimum = 0;
		HealthSet->CurrentShield.Maximum = 100;
		HealthSet->CurrentShield.BaseValue = 0;
		HealthSet->CurrentShield.CurrentValue = 0;
		HealthSet->Shield.Minimum = 0;
		HealthSet->Shield.Maximum = 100;
		HealthSet->Shield.BaseValue = 100;
		HealthSet->Shield.CurrentValue = 100;
		HealthSet->OnRep_Shield();
		HealthSet->OnRep_CurrentShield();
	}

	void FixHealth(APlayerPawn_Athena_C* Pawn)
	{
		Pawn->SetMaxHealth(100.0f);
		Pawn->SetHealth(100.0f);
	}

	APlayerPawn_Generic_C* SpawnSTW(AFortPlayerController* PlayerController, FVector Location)
	{
		auto Pawn = Globals::GameplayStatics::SpawnActor<APlayerPawn_Generic_C>(Location);
		Pawn->SetOwner(PlayerController);
		PlayerController->Possess(Pawn);

		PlayerController->ClientForceProfileQuery();

		((AFortPlayerStateOutpost*)PlayerController->PlayerState)->OnRep_CharacterParts();

		Pawn->CharacterMovement->bReplicates = true;
		Pawn->SetReplicateMovement(true);
		Pawn->OnRep_ReplicatedBasedMovement();
		Pawn->OnRep_ReplicatedMovement();

		return Pawn;
	}

	//Athena
	APlayerPawn_Athena_C* Spawn(AFortPlayerController* PlayerController, FVector Location)
	{
		auto Pawn = Globals::GameplayStatics::SpawnActor<APlayerPawn_Athena_C>(Location);
		Pawn->SetOwner(PlayerController);
		PlayerController->Possess(Pawn);
 
		FixHealth(Pawn);
		FixShield(Pawn);
		RemoveRegenEffects(Pawn);

		PlayerController->ClientForceProfileQuery();

		Pawn->ServerChoosePart(EFortCustomPartType::Body, Globals::BodyPart);
		Pawn->ServerChoosePart(EFortCustomPartType::Head, Globals::HeadPart);
		((AFortPlayerStateAthena*)PlayerController->PlayerState)->OnRep_CharacterParts();

		Pawn->CharacterMovement->bReplicates = true;
		Pawn->SetReplicateMovement(true);
		Pawn->OnRep_ReplicatedBasedMovement();
		Pawn->OnRep_ReplicatedMovement();

		return Pawn;
	}

	void HandlePickup(APlayerPawn_Athena_C* Pawn, AFortPlayerPawn_ServerHandlePickup_Params* Parameters)
	{
			auto Player = (AFortPlayerControllerAthena*)Pawn->Controller;
			if (Player)
			{
				auto WorldInventory = reinterpret_cast<InventoryPointer*>(Player)->WorldInventory;
				auto QuickBars = reinterpret_cast<QuickBarsPointer*>(Player)->QuickBars;
				if (WorldInventory)
				{
					auto PickupEntry = Parameters->Pickup->PrimaryPickupItemEntry;
					auto PickupDef = PickupEntry.ItemDefinition;

					int Count = 0;

					for (int i = 0; i < WorldInventory->Inventory.ItemInstances.Num(); i++)
					{
						auto ItemInstance = WorldInventory->Inventory.ItemInstances[i];

						if (ItemInstance->GetItemDefinitionBP() == PickupDef && !PickupDef->IsA(UFortWeaponItemDefinition::StaticClass()))
						{
							WorldInventory->Inventory.ItemInstances.Remove(i);

							for (int j = 0; j < WorldInventory->Inventory.ReplicatedEntries.Num(); j++)
							{
								auto Entry = WorldInventory->Inventory.ReplicatedEntries[j];

								if (Entry.ItemDefinition == PickupDef && !PickupDef->IsA(UFortWeaponItemDefinition::StaticClass()))
								{
									WorldInventory->Inventory.ReplicatedEntries.Remove(j);
									Count = Entry.Count;
								}
							}
						}
					}

					auto NewPickupWorldItem = (UFortWorldItem*)PickupDef->CreateTemporaryItemInstanceBP(PickupEntry.Count + Count, 1);
					NewPickupWorldItem->ItemEntry = PickupEntry;
					NewPickupWorldItem->ItemEntry.Count = PickupEntry.Count + Count;
					NewPickupWorldItem->bTemporaryItemOwningController = true;
					NewPickupWorldItem->SetOwningControllerForTemporaryItem(Player);

					WorldInventory->Inventory.ItemInstances.Add(NewPickupWorldItem);
					WorldInventory->Inventory.ReplicatedEntries.Add(NewPickupWorldItem->ItemEntry);

					Inventory::UpdateInventory(Player);

					auto statval = new FFortItemEntryStateValue;
					statval->IntValue = NewPickupWorldItem->ItemEntry.Count;
					statval->StateType = EFortItemEntryState::NewItemCount;
					statval->NameValue = FName("Item");
					Player->ServerSetInventoryStateValue(NewPickupWorldItem->GetItemGuid(), (*statval));
					QuickBars->ServerAddItemInternal(NewPickupWorldItem->GetItemGuid(), EFortQuickBars::Primary, QuickBars->PrimaryQuickBar.SecondaryFocusedSlot);
				}
			}
	}

	void DropItem(AFortPlayerControllerAthena* Player, AFortPlayerController_ServerSpawnInventoryDrop_Params* Parameters)
	{
		auto A = Inventory::GetInventory(Player);
		auto Count = Parameters->Count;
		auto ItemGuid = Parameters->ItemGuid;
		auto ItemInstances = A->Inventory.ItemInstances;
		auto ReplicatedEntries = A->Inventory.ReplicatedEntries;
		
		for (int i = 0; i < ItemInstances.Num(); i++)
		{
			auto ItemInstance = ItemInstances[i];
			auto InstanceGuid = ItemInstance->GetItemGuid();

			if (Helpers::AreGuidsTheSame(ItemGuid, InstanceGuid))
			{
				ItemInstances.Remove(i);
				for (int j = 0; j < ReplicatedEntries.Num(); j++)
				{
					auto ReplicatedEntry = ReplicatedEntries[j];
					if (Helpers::AreGuidsTheSame(ReplicatedEntry.ItemGuid, ItemGuid))
					{
						ReplicatedEntries.Remove(j);
						Loot::SpawnPickup(Player->GetPlayerPawn()->K2_GetActorLocation(), ReplicatedEntry.ItemDefinition, Count);
					}
				}
			}
		}
	}
}