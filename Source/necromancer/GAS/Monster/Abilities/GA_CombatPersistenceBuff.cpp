// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_CombatPersistenceBuff.h"
#include "AbilitySystemComponent.h"
#include "GAS/Monster/MonsterGameplayTags.h"
#include "TimerManager.h"

UGA_CombatPersistenceBuff::UGA_CombatPersistenceBuff()
{
	// Passive Ability 설정 - 한 번 활성화되면 계속 유지
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	// Ability 태그 설정 (새 API 사용)
	const FMonsterGameplayTags& MonsterTags = FMonsterGameplayTags::Get();
	FGameplayTagContainer Tags;
	Tags.AddTag(MonsterTags.Monster_Ability_Buff_CombatPersistence);
	SetAssetTags(Tags);
}

void UGA_CombatPersistenceBuff::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Combat 태그 변화 감지 등록
	const FMonsterGameplayTags& MonsterTags = FMonsterGameplayTags::Get();
	TagChangedDelegateHandle = ASC->RegisterGameplayTagEvent(
		MonsterTags.Monster_State_Combat,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(this, &UGA_CombatPersistenceBuff::OnCombatTagChanged);

	// 이미 Combat 상태면 바로 타이머 시작
	if (ASC->HasMatchingGameplayTag(MonsterTags.Monster_State_Combat))
	{
		OnCombatEntered();
	}

	UE_LOG(LogTemp, Log, TEXT("[%s] GA_CombatPersistenceBuff Activated - Listening for Combat tag changes"),
		*GetAvatarActorFromActorInfo()->GetName());

	// Passive Ability이므로 EndAbility 호출하지 않음 - 계속 활성화 상태 유지
}

void UGA_CombatPersistenceBuff::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 정리: 델리게이트 해제
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		const FMonsterGameplayTags& MonsterTags = FMonsterGameplayTags::Get();
		ASC->RegisterGameplayTagEvent(
			MonsterTags.Monster_State_Combat,
			EGameplayTagEventType::NewOrRemoved
		).Remove(TagChangedDelegateHandle);
	}

	// 타이머 정리
	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		if (UWorld* World = Avatar->GetWorld())
		{
			World->GetTimerManager().ClearTimer(ActivationTimerHandle);
			World->GetTimerManager().ClearTimer(RemovalTimerHandle);
		}
	}

	// 버프 제거
	RemoveBuff();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_CombatPersistenceBuff::OnCombatTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		OnCombatEntered();
	}
	else
	{
		OnCombatExited();
	}
}

void UGA_CombatPersistenceBuff::OnCombatEntered()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
	{
		return;
	}

	UWorld* World = Avatar->GetWorld();
	if (!World)
	{
		return;
	}

	// 버프 제거 타이머가 돌고 있으면 취소 (전투 재돌입)
	World->GetTimerManager().ClearTimer(RemovalTimerHandle);

	// 이미 버프가 활성화되어 있으면 아무것도 하지 않음
	if (ActiveBuffHandle.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] Combat re-entered - Buff already active, removal timer cancelled"),
			*Avatar->GetName());
		return;
	}

	// 활성화 타이머가 이미 돌고 있으면 아무것도 하지 않음
	if (World->GetTimerManager().IsTimerActive(ActivationTimerHandle))
	{
		return;
	}

	// 10초 후 버프 적용 타이머 시작
	World->GetTimerManager().SetTimer(
		ActivationTimerHandle,
		this,
		&UGA_CombatPersistenceBuff::ApplyBuff,
		BuffActivationDelay,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("[%s] Combat entered - Buff activation timer started (%.1fs)"),
		*Avatar->GetName(), BuffActivationDelay);
}

void UGA_CombatPersistenceBuff::OnCombatExited()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
	{
		return;
	}

	UWorld* World = Avatar->GetWorld();
	if (!World)
	{
		return;
	}

	// 활성화 대기 타이머 취소 (10초 안 되어서 전투 종료)
	World->GetTimerManager().ClearTimer(ActivationTimerHandle);

	// 버프가 활성화되어 있지 않으면 아무것도 하지 않음
	if (!ActiveBuffHandle.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] Combat exited - No active buff, activation timer cancelled"),
			*Avatar->GetName());
		return;
	}

	// 3초 후 버프 제거 타이머 시작
	World->GetTimerManager().SetTimer(
		RemovalTimerHandle,
		this,
		&UGA_CombatPersistenceBuff::RemoveBuff,
		BuffRemovalDelay,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("[%s] Combat exited - Buff removal timer started (%.1fs)"),
		*Avatar->GetName(), BuffRemovalDelay);
}

void UGA_CombatPersistenceBuff::ApplyBuff()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC || !BuffEffectClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ApplyBuff failed - ASC or BuffEffectClass is null"));
		return;
	}

	// GameplayEffect 적용
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
		BuffEffectClass,
		1.0f,
		ContextHandle
	);

	if (SpecHandle.IsValid())
	{
		ActiveBuffHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		UE_LOG(LogTemp, Warning, TEXT("[%s] === Combat Persistence Buff ACTIVATED! === AttackPower +10"),
			*GetAvatarActorFromActorInfo()->GetName());
	}
}

void UGA_CombatPersistenceBuff::RemoveBuff()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC || !ActiveBuffHandle.IsValid())
	{
		return;
	}

	ASC->RemoveActiveGameplayEffect(ActiveBuffHandle);
	ActiveBuffHandle.Invalidate();

	UE_LOG(LogTemp, Warning, TEXT("[%s] === Combat Persistence Buff REMOVED! ==="),
		*GetAvatarActorFromActorInfo()->GetName());
}
