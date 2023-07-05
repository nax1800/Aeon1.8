#pragma once
#include "framework.h"

namespace Abilities
{
	FGameplayAbilitySpec* FindAbilitySpecFromHandle(UAbilitySystemComponent* AbilitySystem, FGameplayAbilitySpecHandle Handle)
	{
		for (int i = 0; i < AbilitySystem->ActivatableAbilities.Items.Num(); i++)
		{
			auto Spec = AbilitySystem->ActivatableAbilities.Items[i];

			if (Spec.Handle.Handle == Handle.Handle)
			{
				return &Spec;
			}
		}

		return nullptr;
	}

	char (*InternalTryActivateAbilityLong)(UAbilitySystemComponent* AbilitySystemComp, FGameplayAbilitySpecHandle AbilityToActivate, FPredictionKey InPredictionKey, UGameplayAbility** OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData);

	static void GrantGameplayAbility(AFortPlayerPawnAthena* Player, UClass* GameplayAbilityClass)
	{
		auto AbilitySystemComponent = Player->AbilitySystemComponent;

		static UGameplayEffect* DefaultGameplayEffect = UObject::FindObject<UGameplayEffect>("GE_Constructor_ContainmentUnit_Applied_C GE_Constructor_ContainmentUnit_Applied.Default__GE_Constructor_ContainmentUnit_Applied_C");

		if (!DefaultGameplayEffect)
			return;

		TArray<FGameplayAbilitySpecDef> GrantedAbilities = DefaultGameplayEffect->GrantedAbilities;

		GrantedAbilities[0].Ability = GameplayAbilityClass;

		DefaultGameplayEffect->DurationPolicy = EGameplayEffectDurationType::Infinite;

		static auto GameplayEffectClass = UObject::FindObject<UClass>("BlueprintGeneratedClass GE_Constructor_ContainmentUnit_Applied.GE_Constructor_ContainmentUnit_Applied_C");

		if (!GameplayEffectClass)
			return;

		auto handle = FGameplayEffectContextHandle();

		AbilitySystemComponent->BP_ApplyGameplayEffectToTarget(GameplayEffectClass, AbilitySystemComponent, 1, handle);
	}

	void GrantGameplayAbilities(AFortPlayerPawnAthena* Player)
	{
		static auto AbilitySet = FindObjectFast<UFortAbilitySet>("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_DefaultPlayer.GAS_DefaultPlayer");

		for (int i = 0; i < AbilitySet->GameplayAbilities.Num(); i++)
		{
			auto Ability = AbilitySet->GameplayAbilities[i];
			Abilities::GrantGameplayAbility(Player, Ability);
		}

		static auto ShootingAbility = FindObjectFast<UClass>("/Game/Abilities/Weapons/Ranged/GA_Ranged_GenericDamage.GA_Ranged_GenericDamage_C");
		if (ShootingAbility)
		{
			Abilities::GrantGameplayAbility(Player, ShootingAbility);
		}
	}

	void Init()
	{
		InternalTryActivateAbilityLong = decltype(InternalTryActivateAbilityLong)(Addresses::InternalTryActivateAbility);
	}
}