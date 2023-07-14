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

	template<typename T = UObject>
	T* CastObject(UObject* Object)
	{
		return reinterpret_cast<T>(Object);
	}

	string GetPlayerName(AActor* Player)
	{
		if (Player->IsA(APlayerState::StaticClass()))
			return CastObject<APlayerState>(Player)->PlayerName.ToString();

		if (Player->IsA(APlayerController::StaticClass()))
			return CastObject<APlayerController>(Player)->PlayerState->PlayerName.ToString();

		if (Player->IsA(APawn::StaticClass()))
			return CastObject<APawn>(Player)->PlayerState->PlayerName.ToString();

		return "";
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