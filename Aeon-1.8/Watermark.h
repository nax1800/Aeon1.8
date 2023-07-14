#pragma once
#include "framework.h"

namespace Watermark
{
	UFont* Font;

	void Init()
	{
		Font = UObject::FindObject<UFont>("Font NotoSans.NotoSans");
	}
}