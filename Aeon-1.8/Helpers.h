#pragma once
#include "framework.h"

namespace Helpers
{
	static bool AreGuidsTheSame(FGuid guidA, FGuid guidB)
	{
		if (guidA.A == guidB.A && guidA.B == guidB.B && guidA.C == guidB.C && guidA.D == guidB.D)
			return true;
		else
			return false;
	}

	string GetPlayerName(APlayerState* Player)
	{
		auto A = Player->PlayerName.ToString();
		return A;
	}

	string GetPlayerName(APlayerController* Player)
	{
		auto A = Player->PlayerState;
		return GetPlayerName(A);
	}

	string GetPlayerName(APawn* Player)
	{
		auto A = Player->PlayerState;
		return GetPlayerName(A);
	}

	string GetObjectName(UObject* Object)
	{
		if (Object)
		{
			auto ObjectName = Object->GetFullName();
			return ObjectName;
		}
		return "";
	}
}