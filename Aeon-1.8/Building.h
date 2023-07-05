#pragma once
#include "framework.h"

namespace Building
{
	void ShowBuildPreviews(AFortWeap_BuildingTool* BuildingTool, UFortWeaponItemDefinition* WeaponData)
	{
		if (WeaponData)
		{
			UBuildingEditModeMetadata* MetaData = nullptr;
			auto DataName = WeaponData->GetFullName();
			if (DataName == "FortBuildingItemDefinition BuildingItemData_RoofS.BuildingItemData_RoofS")
			{
				static auto MD = UObject::FindObject<UBuildingEditModeMetadata>("BuildingEditModeMetadata_Roof EMP_Roof_RoofC.EMP_Roof_RoofC");
				MetaData = MD;
			}
			else if (DataName == "FortBuildingItemDefinition BuildingItemData_Stair_W.BuildingItemData_Stair_W")
			{
				static auto MD = UObject::FindObject<UBuildingEditModeMetadata>("BuildingEditModeMetadata_Stair EMP_Stair_StairW.EMP_Stair_StairW");
				MetaData = MD;
			}
			else if (DataName == "FortBuildingItemDefinition BuildingItemData_Wall.BuildingItemData_Wall")
			{
				static auto MD = UObject::FindObject<UBuildingEditModeMetadata>("BuildingEditModeMetadata_Wall EMP_Wall_Solid.EMP_Wall_Solid");
				MetaData = MD;
			}
			else if (DataName == "FortBuildingItemDefinition BuildingItemData_Floor.BuildingItemData_Floor")
			{
				static auto MD = UObject::FindObject<UBuildingEditModeMetadata>("BuildingEditModeMetadata_Floor EMP_Floor_Floor.EMP_Floor_Floor");
				MetaData = MD;
			}
			if (MetaData) {
				BuildingTool->DefaultMetadata = MetaData;
				BuildingTool->OnRep_DefaultMetadata();
			}
		}
	}
}