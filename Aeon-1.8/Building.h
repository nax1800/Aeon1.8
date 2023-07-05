#pragma once
#include "framework.h"

namespace Building
{
	UBuildingEditModeMetadata* RoofMD;
	UBuildingEditModeMetadata* StairsMD;
	UBuildingEditModeMetadata* WallMD;
	UBuildingEditModeMetadata* FloorMD;

	void ShowBuildPreviews(AFortWeap_BuildingTool* BuildingTool, UFortWeaponItemDefinition* WeaponData)
	{
		if (WeaponData)
		{
			UBuildingEditModeMetadata* MetaData = nullptr;
			auto DataName = WeaponData->GetFullName();
			if (DataName == "FortBuildingItemDefinition BuildingItemData_RoofS.BuildingItemData_RoofS")
			{
				if(!RoofMD)
					 RoofMD = UObject::FindObject<UBuildingEditModeMetadata>("BuildingEditModeMetadata_Roof EMP_Roof_RoofC.EMP_Roof_RoofC");
				MetaData = RoofMD;
			}
			else if (DataName == "FortBuildingItemDefinition BuildingItemData_Stair_W.BuildingItemData_Stair_W")
			{
				if(!StairsMD)
				     StairsMD = UObject::FindObject<UBuildingEditModeMetadata>("BuildingEditModeMetadata_Stair EMP_Stair_StairW.EMP_Stair_StairW");
				MetaData = StairsMD;
			}
			else if (DataName == "FortBuildingItemDefinition BuildingItemData_Wall.BuildingItemData_Wall")
			{
				if(!WallMD)
					 WallMD = UObject::FindObject<UBuildingEditModeMetadata>("BuildingEditModeMetadata_Wall EMP_Wall_Solid.EMP_Wall_Solid");
				MetaData = WallMD;
			}
			else if (DataName == "FortBuildingItemDefinition BuildingItemData_Floor.BuildingItemData_Floor")
			{
				if(!FloorMD)
					 FloorMD = UObject::FindObject<UBuildingEditModeMetadata>("BuildingEditModeMetadata_Floor EMP_Floor_Floor.EMP_Floor_Floor");
				MetaData = FloorMD;
			}
			if (MetaData) {
				BuildingTool->DefaultMetadata = MetaData;
				BuildingTool->OnRep_DefaultMetadata();
			}
		}
	}
}