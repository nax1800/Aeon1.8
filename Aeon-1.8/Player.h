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

	void Syphon(APlayerPawn_Athena_C* Pawn) //fucking dumb code but whatever
	{
		float Health = Pawn->GetHealth();
		auto HealthSet = Pawn->HealthSet;
		float Shield = HealthSet->CurrentShield.CurrentValue;
		if (Health <= 100.0f)
		{
			Health += 50;
			float ExtraHealth = Health -= 100;
			if (ExtraHealth > 0.0f)
			{
				Health -= ExtraHealth;
				if (Shield <= 100.0f)
				{
					Shield += ExtraHealth;
					float ExtraShield = Shield - 100;
					if (ExtraShield > 0.0f)
						Shield -= ExtraShield;

					HealthSet->CurrentShield.CurrentValue = Shield;
				}
			}
		}
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

		auto PlayerState = ((AFortPlayerStateAthena*)PlayerController->PlayerState);

		Pawn->ServerChoosePart(EFortCustomPartType::Body, Globals::BodyPart);
		Pawn->ServerChoosePart(EFortCustomPartType::Head, Globals::HeadPart);
		PlayerState->OnRep_CharacterParts();

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
				int Slot = Inventory::GetOpenSlot(Player);

				if (Slot > -1)
				{
					auto WorldInventory = Inventory::GetInventory(Player);
					auto QuickBars = Inventory::GetQuickBars(Player);
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
						QuickBars->ServerAddItemInternal(NewPickupWorldItem->GetItemGuid(), EFortQuickBars::Primary, Slot);
					}
				}
			}
	}
}