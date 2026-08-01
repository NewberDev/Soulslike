// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/ANS_AttackCollisionCheck.h"
#include "AbilitySystemComponent.h"
#include "SoulAttributeSet.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"


UANS_AttackCollisionCheck::UANS_AttackCollisionCheck()
{
	SphereRadius = 20.0f;
	CheckInterval = 0.01f;
	TimeSinceLastCheck = 0.0f;
	InterpolationStepDistance = SphereRadius * 2.f;
	// 이전과 현재 사이의 충돌체를 추가하는 간격으로, 가장 중요한 값이다.
}

void UANS_AttackCollisionCheck::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	TimeSinceLastCheck = 0.0f;

	HitActors.Empty();

	bHasPreviousSocketSample = false;

	PreviousSocketStartPos = FVector::ZeroVector;
	PreviousSocketEndPos = FVector::ZeroVector;

	if (CollisionSocketStartName.IsNone() || CollisionSocketEndName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("UCollisionCheckAnimNotifyState::NotifyBegin_CollisionSocketName_Is Not Valid"));
	}
	
}

int32 UANS_AttackCollisionCheck::CalculateInterpolationSteps(const FVector& PrevStart, const FVector& PrevEnd, const FVector& CurStart, const FVector& CurEnd) const
{
	const float StartTravel = FVector::Dist(PrevStart, CurStart);
	const float EndTravel = FVector::Dist(PrevEnd, CurEnd);
	const float MaxTravel = FMath::Max(StartTravel, EndTravel);
	if (MaxTravel <= KINDA_SMALL_NUMBER || InterpolationStepDistance <= KINDA_SMALL_NUMBER)
	{ // KINDA_SMALL_NUMBER는 float의 0을 판단하기 위한 값이다. (거의 0임을 확인하는 값으로, 더 정확한 값도 있다.)
		return 1;
	}
	return FMath::Max(1, FMath::CeilToInt(MaxTravel / InterpolationStepDistance));
}

void UANS_AttackCollisionCheck::PerformCollisionCheck(USkeletalMeshComponent* MeshComp, AActor* OwnerActor, const FVector& StartPos, const FVector& EndPos)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerActor);
	for (const TWeakObjectPtr<AActor>& WeakActor : HitActors)
	{
		if (WeakActor.IsValid())
		{
			ActorsToIgnore.Add(WeakActor.Get());
		}
	}
	FHitResult HitResult;
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2); //사용자화한 채널이다.
	const EDrawDebugTrace::Type DebugTraceType = bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	const bool bHit = UKismetSystemLibrary::SphereTraceSingle(
		MeshComp->GetWorld(),
		StartPos,
		EndPos,
		SphereRadius,
		TraceType,
		false,
		ActorsToIgnore,
		DebugTraceType,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		0.5f
	);
	if (!bHit || !HitResult.GetActor())
	{
		return;
	}

	AActor* HitActor = HitResult.GetActor();

	const bool bIsPlayerAttackToPlayer =
		OwnerActor->ActorHasTag(TEXT("Player")) && HitActor->ActorHasTag(TEXT("Player"));
	const bool bIsMonsterAttackToMonster =
		OwnerActor->ActorHasTag(TEXT("Monster")) && HitActor->ActorHasTag(TEXT("Monster"));

	if (bIsPlayerAttackToPlayer || bIsMonsterAttackToMonster)
	{
		HitActors.Add(HitActor);
		return;
	}

	HitActors.Add(HitActor);
	APawn* Pawn = Cast<APawn>(OwnerActor);
	if (!Pawn) return;
	UAbilitySystemComponent* MyASC = Pawn->FindComponentByClass<UAbilitySystemComponent>();
	if (!MyASC) return;
	const USoulAttributeSet* AttributeSet = MyASC->GetSet<USoulAttributeSet>();
	if (!AttributeSet) return;
	const float FinalDamage = -(AttributeSet->GetATK());

	DamageApplication(HitResult, FinalDamage, MyASC);

	// Hit Stop : 플레이어 전용 코드로, 공격에서 성공시에 공격 모션을 잠시 멈춰서 시각적 타격감을 준다.
	if (OwnerActor->ActorHasTag(TEXT("Player"))) 
	{
		UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
		if (AnimInstance && AnimInstance->IsAnyMontagePlaying())
		{
			AnimInstance->Montage_SetPlayRate(AnimInstance->GetCurrentActiveMontage(), 0.001f);
			OwnerActor->GetWorld()->GetTimerManager().ClearTimer(HitStopTimerHandle);
			TWeakObjectPtr<UAnimInstance> WeakAnimInstance = AnimInstance;
			OwnerActor->GetWorld()->GetTimerManager().SetTimer(
				HitStopTimerHandle,
				FTimerDelegate::CreateUObject(this, &UANS_AttackCollisionCheck::ResetAnimInstancePlayRate, WeakAnimInstance),
				HitStopDuration,
				false
			);
		}
	}
}

void UANS_AttackCollisionCheck::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp || CollisionSocketStartName.IsNone() || CollisionSocketEndName.IsNone()) return;

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor) return;

	// 프레임 타임을 누적
	TimeSinceLastCheck += FrameDeltaTime;

	// 설정한 인터벌 시간이 지나면 충돌 체크 수행
	if (TimeSinceLastCheck >= CheckInterval)
	{
		// 타이머 차감 (정확한 주기를 위해 0으로 리셋하기보단 인터벌만큼 빼줍니다)
		TimeSinceLastCheck -= CheckInterval;

		
		const FVector CurStart = MeshComp->GetSocketLocation(CollisionSocketStartName);
		const FVector CurEnd = MeshComp->GetSocketLocation(CollisionSocketEndName);

		if (!bHasPreviousSocketSample)
		{
			PerformCollisionCheck(MeshComp, OwnerActor, CurStart, CurEnd);
			bHasPreviousSocketSample = true;
		}
		else //충돌체 사이를 보간한다.
		{
			const int32 NumSteps = CalculateInterpolationSteps(
				PreviousSocketStartPos, PreviousSocketEndPos, CurStart, CurEnd);
			for (int32 Step = 1; Step <= NumSteps; ++Step)
			{
				const float Alpha = static_cast<float>(Step) / NumSteps;
				const FVector InterpStart = FMath::Lerp(PreviousSocketStartPos, CurStart, Alpha);
				const FVector InterpEnd = FMath::Lerp(PreviousSocketEndPos, CurEnd, Alpha);
				PerformCollisionCheck(MeshComp, OwnerActor, InterpStart, InterpEnd);
			}
		}

		PreviousSocketStartPos = CurStart;
		PreviousSocketEndPos = CurEnd;

	}
}

void UANS_AttackCollisionCheck::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

void UANS_AttackCollisionCheck::DamageApplication(const FHitResult& HitResult, float Damage, UAbilitySystemComponent* MyASC)
{
	AActor* HitActor = HitResult.GetActor();
	if (!HitActor) return;
	UE_LOG(LogTemp, Log, TEXT("공격 적중!: %s"), *HitActor->GetName());

	//==================================================== //히트 사운드
	if (HitSound && HitResult.bBlockingHit)
	{
		UGameplayStatics::PlaySoundAtLocation(
			HitActor->GetWorld(),
			HitSound,
			HitResult.ImpactPoint // 궤적이 부딪힌 바로 그 3D 좌표
		);
	}
	//====================================================

	// 1. 피격 당한 대상의 ASC에 접근한다.
	UAbilitySystemComponent* EnemyASC = HitActor->FindComponentByClass<UAbilitySystemComponent>();

	if (EnemyASC && DamageGameplayEffectClass)
	{
		// 2. GE를 적용하기 위한 'Context' 및 'Spec(설계도)' 생성
		FGameplayEffectContextHandle EffectContext = MyASC->MakeEffectContext();
		EffectContext.AddHitResult(HitResult); // 피격 위치, 법선 등 상세 정보 저장

		// 레벨은 기본적으로 1.0f로 설정하여 스펙 생성
		FGameplayEffectSpecHandle NewSpecHandle = MyASC->MakeOutgoingSpec(DamageGameplayEffectClass, 1.0f, EffectContext);

		if (NewSpecHandle.IsValid())
		{
			// 3. [SetByCaller] 에디터에 등록한 태그 매칭 및 데미지 값 주입
			// 데이터 에셋에서 지정한 "Data.Damage.Amount" 태그와 일치해야 합니다.
			FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(TEXT("Data.Damage.Amount"));

			// 음수/양수 여부는 어트리뷰트셋(PostGameplayEffectExecute) 내부 연산 방식에 맞게 넘겨줍니다.
			// 보통 데미지는 양수로 넘겨서 내부에서 빼거나, 마이너스로 넘겨서 Add 하도록 설계합니다.
			NewSpecHandle.Data.Get()->SetSetByCallerMagnitude(DamageTag, Damage);

			// 4. 최종 완성된 스펙을 내 ASC의 권한으로 적의 ASC에 적용!
			MyASC->ApplyGameplayEffectSpecToTarget(*NewSpecHandle.Data.Get(), EnemyASC);

			UE_LOG(LogTemp, Log, TEXT("GE 데미지 적용 완료! 최종 수치: %f"), Damage);
		}
	}
}

void UANS_AttackCollisionCheck::ResetAnimInstancePlayRate(TWeakObjectPtr<UAnimInstance> AnimInstance)
{
	// 게임 플레이 도중 언로드되거나 파괴되었는지 검사 (댕글링 포인터 방지)
	if (AnimInstance.IsValid())
	{
		if (AnimInstance->IsAnyMontagePlaying())
		{
			// 몽타주의 재생 속도를 다시 정상(1.0)으로 돌려놓아 자연스럽게 공격을 이어가게 합니다.
			AnimInstance->Montage_SetPlayRate(AnimInstance->GetCurrentActiveMontage(), 1.0f);
		}
	}
}

