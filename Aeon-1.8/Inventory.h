#pragma once
#include "framework.h"

struct QuickBarsPointer
{
    unsigned char pad[0x1A88];
    AFortQuickBars* QuickBars;
};

struct InventoryPointer
{
    unsigned char pad[0x1D28];
    AFortInventory* WorldInventory;
};

namespace Inventory
{
    AFortInventory* GetInventory(AFortPlayerControllerAthena* Player)
    {
        auto A = reinterpret_cast<InventoryPointer*>(Player)->WorldInventory;
        return A;
    }

    AFortQuickBars* GetQuickBars(AFortPlayerControllerAthena* Player)
    {
        auto A = reinterpret_cast<QuickBarsPointer*>(Player)->QuickBars;
        return A;
    }

    void UpdateInventory(AFortPlayerControllerAthena* Player)
    {
        auto A = GetInventory(Player);
        auto B = GetQuickBars(Player);
        A->HandleInventoryLocalUpdate();
        A->Inventory.MarkArrayDirty();
        Player->HandleWorldInventoryLocalUpdate();
        B->OnRep_PrimaryQuickBar();
        B->OnRep_SecondaryQuickBar();
    }

    int GetItemSlot(AFortPlayerControllerAthena* Player, FGuid ItemGuid)
    {
        auto PrimaryQuickBar = GetQuickBars(Player)->PrimaryQuickBar;
        auto Slots = PrimaryQuickBar.Slots;
        for (int i = 0; i < Slots.Num(); i++)
        {
            if (Slots[i].Items.Data != nullptr && Helpers::AreGuidsTheSame(Slots[i].Items[0], ItemGuid)) 
            {
                return i;
            }
        }
        return -1;
    }

    int GetOpenSlot(AFortPlayerControllerAthena* Player)
    {
        auto PrimaryQuickBar = GetQuickBars(Player)->PrimaryQuickBar;
        auto Slots = PrimaryQuickBar.Slots;

        for (int i = 0; i < Slots.Num(); i++)
        {
            auto Slot = Slots[i];
            if (Slot.Items.Data == nullptr)
            {
                return i;
            }
        }

        return -1;
    }

    bool IsSlotOpen(AFortPlayerControllerAthena* Player, int Slot)
    {
        auto PrimaryQuickBar = GetQuickBars(Player)->PrimaryQuickBar;
        auto Slots = PrimaryQuickBar.Slots;

        if (Slots[Slot].Items.Data == nullptr) 
            return true;

        return false;
    }

    void AddItem(AFortPlayerControllerAthena* Player, UFortItemDefinition* ItemDef, int Count, EFortQuickBars QuickBar, int Slot, bool bAddToQuickbar = true)
    {
        auto A = GetInventory(Player);
        auto B = GetQuickBars(Player);

        auto Item = ItemDef->CreateTemporaryItemInstanceBP(1, 0);
        auto WorldItem = reinterpret_cast<UFortWorldItem*>(Item);
        WorldItem->ItemEntry.Count = Count;
        A->Inventory.ReplicatedEntries.Add(WorldItem->ItemEntry);
        A->Inventory.ItemInstances.Add(WorldItem);
        if(bAddToQuickbar)
         B->ServerAddItemInternal(WorldItem->GetItemGuid(), QuickBar, Slot);
        UpdateInventory(Player);
    }

    void AddDefaultQuickBarItems(AFortPlayerControllerAthena* Player)
    {
        auto A = GetInventory(Player);
        auto B = GetQuickBars(Player);

        static auto Pickaxe = FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
        static auto Rifle = Loot::GetRandomRifle();
        static auto Shotgun = Loot::GetRandomShotgun();
        static auto Sniper = Loot::GetRandomSniper();
        static auto EditTool = FindObjectFast<UFortEditToolItemDefinition>("/Game/Items/Weapons/BuildingTools/EditTool.EditTool");
        static auto Wall = FindObjectFast<UFortBuildingItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall");
        static auto Floor = FindObjectFast<UFortBuildingItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor");
        static auto Stairs = FindObjectFast<UFortBuildingItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W");
        static auto Roof = FindObjectFast<UFortBuildingItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS");
        static auto Wood = UObject::FindObject<UFortResourceItemDefinition>("FortResourceItemDefinition WoodItemData.WoodItemData");

        AddItem(Player, EditTool, 1, EFortQuickBars::Secondary, 0, false);

        if(!Globals::bInfiniteResources)
        AddItem(Player, Wood, 100, EFortQuickBars::Secondary, 0);

        AddItem(Player, Pickaxe, 1, EFortQuickBars::Primary, 0);

        if (Globals::bLateGame)
        {
            AddItem(Player, Rifle, 1, EFortQuickBars::Primary, 1);
            AddItem(Player, Shotgun, 1, EFortQuickBars::Primary, 2);
            AddItem(Player, Sniper, 1, EFortQuickBars::Primary, 3);
            AddItem(Player, Loot::GetRandomCosumable(), 1, EFortQuickBars::Primary, 4);
            AddItem(Player, Loot::GetRandomCosumable(), 1, EFortQuickBars::Primary, 5);
        }

        AddItem(Player, Wall, 1, EFortQuickBars::Secondary, 0);
        AddItem(Player, Floor, 1, EFortQuickBars::Secondary, 1);
        AddItem(Player, Stairs, 1, EFortQuickBars::Secondary, 2);
        AddItem(Player, Roof, 1, EFortQuickBars::Secondary, 3);
    }

    void SetupInventory(AFortPlayerControllerAthena* Player)
    {
        auto NewQuickBars = Globals::GameplayStatics::SpawnActor<AFortQuickBars>();
        NewQuickBars->SetOwner(Player);
        reinterpret_cast<QuickBarsPointer*>(Player)->QuickBars = NewQuickBars;
        Player->OnRep_QuickBar();

        NewQuickBars->EnableSlot(EFortQuickBars::Primary, 0);
        NewQuickBars->EnableSlot(EFortQuickBars::Primary, 1);
        NewQuickBars->EnableSlot(EFortQuickBars::Primary, 2);
        NewQuickBars->EnableSlot(EFortQuickBars::Primary, 3);
        NewQuickBars->EnableSlot(EFortQuickBars::Primary, 4);
        NewQuickBars->EnableSlot(EFortQuickBars::Secondary, 0);
        NewQuickBars->EnableSlot(EFortQuickBars::Secondary, 1);
        NewQuickBars->EnableSlot(EFortQuickBars::Secondary, 2);
        NewQuickBars->EnableSlot(EFortQuickBars::Secondary, 3);
        NewQuickBars->EnableSlot(EFortQuickBars::Secondary, 4);

        AddDefaultQuickBarItems(Player);

        Player->bHasInitializedWorldInventory = true;
    }

    AFortWeapon* ExecuteInventoryItem(AFortPlayerControllerAthena* Player, FGuid InGuid)
    {
        auto A = GetInventory(Player);
        if (A)
        {
          auto ItemInstances = A->Inventory.ItemInstances;
          for (int i = 0; i < ItemInstances.Num(); i++)
          {
            auto ItemInstance = ItemInstances[i];

            if (Helpers::AreGuidsTheSame(ItemInstance->GetItemGuid(), InGuid))
            {
                if (Player->Pawn)
                {
                    auto ItemDef = (UFortWeaponItemDefinition*)ItemInstance->GetItemDefinitionBP();
                    auto Weapon = ((AFortPlayerPawn*)Player->Pawn)->EquipWeaponDefinition(ItemDef, InGuid);
                    return Weapon;
                }

              if (ItemInstance->GetItemDefinitionBP()->IsA(UFortTrapItemDefinition::StaticClass()))
              {
                ((AFortPlayerPawn*)Player->Pawn)->PickUpActor(nullptr, (UFortDecoItemDefinition*)ItemInstance->GetItemDefinitionBP());
              }
            }
          }
        }

        return nullptr;
    }

    void DropItem(AFortPlayerControllerAthena* Player, FGuid ItemGuid, int Count)
    {
            auto A = GetInventory(Player);
            auto B = GetQuickBars(Player);
            int Slot = GetItemSlot(Player, ItemGuid);

            UFortItemDefinition* ItemDefintion = nullptr;
                auto ItemInstances = A->Inventory.ItemInstances;
                auto ReplicatedEntries = A->Inventory.ReplicatedEntries;

                for (int i = 0; i < ItemInstances.Num(); i++)
                {
                    auto ItemInstance = ItemInstances[i];
                    if (Helpers::AreGuidsTheSame(ItemInstance->GetItemGuid(), ItemGuid))
                    {
                        ItemInstances.Remove(i);

                        for (int j = 0; j < ReplicatedEntries.Num(); j++)
                        {
                            auto ReplicatedEntry = ReplicatedEntries[j];
                            if (Helpers::AreGuidsTheSame(ReplicatedEntry.ItemGuid, ItemGuid))
                            {
                                ItemDefintion = ReplicatedEntry.ItemDefinition;
                                ReplicatedEntries.Remove(j);
                                Log("Removed Item.\n");
                            }
                        }
                    }
                }

                B->ServerRemoveItemInternal(ItemGuid, false, true);
                B->EmptySlot(EFortQuickBars::Primary, Slot);
                B->PrimaryQuickBar.Slots[Slot].Items.Data = nullptr;
                UpdateInventory(Player);

                Loot::SpawnPickup(Player->K2_GetActorLocation(), ItemDefintion, Count);
            }
}
