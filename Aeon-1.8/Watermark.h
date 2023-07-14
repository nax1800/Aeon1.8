#pragma once
#include "framework.h"

namespace Watermark
{
	UFont* Font;

	void AddToPlayerScreen(APlayerController* Player)
	{

	}

	void Init()
	{
		Font = UObject::FindObject<UFont>("Font NotoSans.NotoSans");
	}
}