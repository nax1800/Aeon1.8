#pragma once
#include "framework.h"

namespace Building
{
	UBuildingEditModeMetadata* RoofMD = nullptr;
	UBuildingEditModeMetadata* StairsMD = nullptr;
	UBuildingEditModeMetadata* WallMD = nullptr;
	UBuildingEditModeMetadata* FloorMD = nullptr;

	void ShowBuildPreviews(AFortWeap_BuildingTool* BuildingTool, UFortWeaponItemDefinition* WeaponData)
	{
		if (WeaponData)
		{
			if (!RoofMD)
				RoofMD = UObject::FindObject<UBuildingEditModeMetadata>("BuildingEditModeMetadata_Roof EMP_Roof_RoofC.EMP_Roof_RoofC");

			if (!StairsMD)
				StairsMD = UObject::FindObject<UBuildingEditModeMetadata>("BuildingEditModeMetadata_Stair EMP_Stair_StairW.EMP_Stair_StairW");

			if (!WallMD)
				WallMD = UObject::FindObject<UBuildingEditModeMetadata>("BuildingEditModeMetadata_Wall EMP_Wall_Solid.EMP_Wall_Solid");

			if (!FloorMD)
				FloorMD = UObject::FindObject<UBuildingEditModeMetadata>("BuildingEditModeMetadata_Floor EMP_Floor_Floor.EMP_Floor_Floor");

			UBuildingEditModeMetadata* MetaData = nullptr;
			auto DataName = WeaponData->GetFullName();

			if (DataName == "FortBuildingItemDefinition BuildingItemData_RoofS.BuildingItemData_RoofS")
				MetaData = RoofMD;
			else if (DataName == "FortBuildingItemDefinition BuildingItemData_Stair_W.BuildingItemData_Stair_W")
				MetaData = StairsMD;
			else if (DataName == "FortBuildingItemDefinition BuildingItemData_Wall.BuildingItemData_Wall")
				MetaData = WallMD;

			else if (DataName == "FortBuildingItemDefinition BuildingItemData_Floor.BuildingItemData_Floor")
				MetaData = FloorMD;

			if (MetaData) 
			{
				BuildingTool->DefaultMetadata = MetaData;
				BuildingTool->OnRep_DefaultMetadata();
			}
		}
	}

	void PlaceBuild(UClass* BuildClass = nullptr, bool bMirrored = false, FVector Location = {}, FRotator Rotation = {})
	{
		FQuat Quat;
		FTransform Transform;

		auto DEG_TO_RAD = 3.14159 / 180;
		auto DIVIDE_BY_2 = DEG_TO_RAD / 2;

		auto SP = sin(Rotation.Pitch * DIVIDE_BY_2);
		auto CP = cos(Rotation.Pitch * DIVIDE_BY_2);

		auto SY = sin(Rotation.Yaw * DIVIDE_BY_2);
		auto CY = cos(Rotation.Yaw * DIVIDE_BY_2);

		auto SR = sin(Rotation.Roll * DIVIDE_BY_2);
		auto CR = cos(Rotation.Roll * DIVIDE_BY_2);

		Quat.X = CR * SP * SY - SR * CP * CY;
		Quat.Y = -CR * SP * CY - SR * CP * SY;
		Quat.Z = CR * CP * SY - SR * SP * CY;
		Quat.W = CR * CP * CY + SR * SP * SY;

		Transform.Rotation = Quat;
		Transform.Scale3D = FVector{ 1,1,1 };
		Transform.Translation = Location;

		auto GS = Globals::GameplayStatics::Get();
		auto BuildingActor = (ABuildingSMActor*)GS->STATIC_BeginDeferredActorSpawnFromClass(Globals::World::Get(), BuildClass, Transform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, nullptr);
		GS->STATIC_FinishSpawningActor(BuildingActor, Transform);

		if (BuildingActor)
		{
			BuildingActor->DynamicBuildingPlacementType = EDynamicBuildingPlacementType::DestroyAnythingThatCollides;
			BuildingActor->bPlayerPlaced = true;
			BuildingActor->SetMirrored(bMirrored);
			//BuildingActor->InitializeKismetSpawnedBuildingActor(BuildingActor, nullptr);
		}
	}
}