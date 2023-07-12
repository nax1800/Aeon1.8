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

    void AddItem(AFortPlayerControllerAthena* Player, UFortItemDefinition* ItemDef, int Count, EFortQuickBars QuickBar, int Slot)
    {
        auto A = GetInventory(Player);
        auto B = GetQuickBars(Player);

        auto Item = ItemDef->CreateTemporaryItemInstanceBP(1, 0);
        auto WorldItem = reinterpret_cast<UFortWorldItem*>(Item);
        WorldItem->ItemEntry.Count = Count;
        A->Inventory.ReplicatedEntries.Add(WorldItem->ItemEntry);
        A->Inventory.ItemInstances.Add(WorldItem);
        B->ServerAddItemInternal(WorldItem->GetItemGuid(), QuickBar, Slot);
        UpdateInventory(Player);
    }

    void AddDefaultQuickBarItems(AFortPlayerControllerAthena* Player)
    {
        auto A = GetInventory(Player);
        auto B = GetQuickBars(Player);

        static auto Pickaxe = FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
        static auto EditTool = FindObjectFast<UFortEditToolItemDefinition>("/Game/Items/Weapons/BuildingTools/EditTool.EditTool");
        static auto Wall = FindObjectFast<UFortBuildingItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall");
        static auto Floor = FindObjectFast<UFortBuildingItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor");
        static auto Stairs = FindObjectFast<UFortBuildingItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W");
        static auto Roof = FindObjectFast<UFortBuildingItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS");
        static auto Wood = UObject::FindObject<UFortResourceItemDefinition>("FortResourceItemDefinition WoodItemData.WoodItemData");

        auto EditToolItem = EditTool->CreateTemporaryItemInstanceBP(1, 0);
        auto WorldEditToolItem = (UFortWorldItem*)EditToolItem;
        WorldEditToolItem->ItemEntry.Count = 1;
        A->Inventory.ReplicatedEntries.Add(WorldEditToolItem->ItemEntry);
        A->Inventory.ItemInstances.Add(WorldEditToolItem);

        /*  auto WoodItem = Wood->CreateTemporaryItemInstanceBP(1, 0);
        auto WorldWoodItem = reinterpret_cast<UFortWorldItem*>(WoodItem);
        WorldWoodItem->ItemEntry.Count = 500;
        A->Inventory.ReplicatedEntries.Add(WorldWoodItem->ItemEntry);
        A->Inventory.ItemInstances.Add(WorldWoodItem);
        B->ServerAddItemInternal(WoodItem->GetItemGuid(), EFortQuickBars::Secondary, 0); */

        AddItem(Player, Wood, 999, EFortQuickBars::Secondary, 0);

        auto PickaxeItem = Pickaxe->CreateTemporaryItemInstanceBP(1, 0);
        auto WorldPickaxeItem = reinterpret_cast<UFortWorldItem*>(PickaxeItem);
        WorldPickaxeItem->ItemEntry.Count = 1;
        A->Inventory.ReplicatedEntries.Add(WorldPickaxeItem->ItemEntry);
        A->Inventory.ItemInstances.Add(WorldPickaxeItem);
        B->ServerAddItemInternal(PickaxeItem->GetItemGuid(), EFortQuickBars::Primary, 0);

        auto WallBuildItem = Wall->CreateTemporaryItemInstanceBP(1, 0);
        auto WallWorldBuildItem = reinterpret_cast<UFortWorldItem*>(WallBuildItem);
        A->Inventory.ReplicatedEntries.Add(WallWorldBuildItem->ItemEntry);
        A->Inventory.ItemInstances.Add(WallWorldBuildItem);
        B->ServerAddItemInternal(WallWorldBuildItem->GetItemGuid(), EFortQuickBars::Secondary, 0);

        auto FloorBuildItem = Floor->CreateTemporaryItemInstanceBP(1, 0);
        auto FloorWorldBuildItem = reinterpret_cast<UFortWorldItem*>(FloorBuildItem);
        A->Inventory.ReplicatedEntries.Add(FloorWorldBuildItem->ItemEntry);
        A->Inventory.ItemInstances.Add(FloorWorldBuildItem);
        B->ServerAddItemInternal(FloorWorldBuildItem->GetItemGuid(), EFortQuickBars::Secondary, 1);

        auto StairBuildItem = Stairs->CreateTemporaryItemInstanceBP(1, 0);
        auto StairWorldBuildItem = reinterpret_cast<UFortWorldItem*>(StairBuildItem);
        A->Inventory.ReplicatedEntries.Add(StairWorldBuildItem->ItemEntry);
        A->Inventory.ItemInstances.Add(StairWorldBuildItem);
        B->ServerAddItemInternal(StairWorldBuildItem->GetItemGuid(), EFortQuickBars::Secondary, 2);

        auto RoofBuildItem = Roof->CreateTemporaryItemInstanceBP(1, 0);
        auto RoofWorldBuildItem = reinterpret_cast<UFortWorldItem*>(RoofBuildItem);
        A->Inventory.ReplicatedEntries.Add(RoofWorldBuildItem->ItemEntry);
        A->Inventory.ItemInstances.Add(RoofWorldBuildItem);
        B->ServerAddItemInternal(RoofWorldBuildItem->GetItemGuid(), EFortQuickBars::Secondary, 3);
    }

    void SetupInventory(AFortPlayerControllerAthena* Player)
    {
        auto NewQuickBars = Globals::GameplayStatics::SpawnActor<AFortQuickBars>();
        NewQuickBars->SetOwner(Player);
        reinterpret_cast<QuickBarsPointer*>(Player)->QuickBars = NewQuickBars;
        Player->OnRep_QuickBar();

        auto B = GetQuickBars(Player);

        B->EnableSlot(EFortQuickBars::Primary, 0);
        B->EnableSlot(EFortQuickBars::Primary, 1);
        B->EnableSlot(EFortQuickBars::Primary, 2);
        B->EnableSlot(EFortQuickBars::Primary, 3);
        B->EnableSlot(EFortQuickBars::Primary, 4);
        B->EnableSlot(EFortQuickBars::Secondary, 0);
        B->EnableSlot(EFortQuickBars::Secondary, 1);
        B->EnableSlot(EFortQuickBars::Secondary, 2);
        B->EnableSlot(EFortQuickBars::Secondary, 3);
        B->EnableSlot(EFortQuickBars::Secondary, 4);

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
};
