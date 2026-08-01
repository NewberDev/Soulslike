// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_AttackCollisionCheck.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API UANS_AttackCollisionCheck : public UAnimNotifyState
{
	GENERATED_BODY()

	// 에디터 디테일 패널에서 체크로, 디버그 여부를 결정한다.
	UPROPERTY(EditAnywhere, Category = "Debug", meta = (ToolTip = "구체 충돌체의 반지름입니다."))
	bool bShowDebug = false;
	
	UPROPERTY(EditAnywhere, category = "CollisionCheck")
	FName CollisionSocketStartName;

	UPROPERTY(EditAnywhere, category = "CollisionCheck")
	FName CollisionSocketEndName;

	UPROPERTY(EditAnywhere, category = "CollisionCheck")
	TSubclassOf<class UGameplayEffect> DamageGameplayEffectClass;

	UPROPERTY(EditAnywhere, Category = "Collision", meta = (ToolTip = "구체 충돌체의 반지름입니다."))
	float SphereRadius;

	UPROPERTY(EditAnywhere, Category = "Collision", meta = (ToolTip = "몇 초에 한 번씩 충돌을 체크할지 설정합니다. (예: 0.1 = 0.1초마다)"))
	float CheckInterval;

	UPROPERTY(EditAnywhere, Category = "Collision", meta = (ToolTip = "보간 trace 1회당 허용 최대 이동 거리(cm).더 멀리 움직였으면 step 수 증가"))
	float InterpolationStepDistance;
	
	// 경과 시간을 누적할 변수
	float TimeSinceLastCheck;

	// 이번 공격 주기 동안 이미 맞은 액터들을 기억하는 배열 (중복 피격 방지)
	TArray<TWeakObjectPtr<AActor>> HitActors;

	FVector PreviousSocketStartPos = FVector::ZeroVector;
	FVector PreviousSocketEndPos = FVector::ZeroVector;
	bool bHasPreviousSocketSample = false;

	// 히트 스톱을 적용할 시간 (초 단위, 0.05 ~ 0.12초 사이가 엘든링 느낌의 황금 수치이다)
	UPROPERTY(EditAnywhere, Category = "Hit Stop", meta = (BlueprintProtected = "true"))
	float HitStopDuration = 0.08f;

	// 몽타주를 다시 재생시키기 위한 타이머 핸들
	FTimerHandle HitStopTimerHandle;

	int32 CalculateInterpolationSteps(
		const FVector& PrevStart,
		const FVector& PrevEnd,
		const FVector& CurStart,
		const FVector& CurEnd) const;

	void PerformCollisionCheck(
		USkeletalMeshComponent* MeshComp,
		AActor* OwnerActor,
		const FVector& StartPos,
		const FVector& EndPos);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> HitSound;

public:

	UANS_AttackCollisionCheck();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	void DamageApplication(const FHitResult& HitResult, float Damage, class UAbilitySystemComponent* AttributeSet);

	// 타이머가 작동했을 때 호출할 복구 함수
	void ResetAnimInstancePlayRate(TWeakObjectPtr<UAnimInstance> AnimInstance);

};
