// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulCombatComponent.h"
#include "AbilitySystemComponent.h"
#include "SoulAttributeSet.h"
#include "SoulGameplayTags.h"
#include "SoulAnimInstance.h"
#include "SoulCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"

#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
USoulCombatComponent::USoulCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USoulCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	AttackRange = 120.f;
	AttackRadius = 50.f;
	
}


// Called every frame
void USoulCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

/*
	이 함수를 애님인스턴스의 노티파이에 바인딩해야 한다. 현재 클래스에서 할수도 있지만,
	그렇게 되면 현재 클래스가 애님인스턴스를 의존해야 하기 때문에 좋지 않은 방법이다.
	그렇기에 이 함수는 캐릭터에서 애님인스턴스에 이어지도록 한다.
*/

/*
void USoulCombatComponent::AttackCheck()
{
	UE_LOG(LogTemp, Log, TEXT("USoulCombatComponent::AttackCheck")); //함수 실행 확인
	FHitResult HitResult;
	APawn* Pawn = Cast<APawn>(GetOwner());
	FCollisionQueryParams Params(NAME_None, false, Pawn);
	bool bResult = GetWorld()->SweepSingleByChannel( //레이저 쏘는 함수
		HitResult,
		Pawn->GetActorLocation(),
		Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2, //임시 수정중!*********.
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	if (bResult)
	{
		UE_LOG(LogTemp, Log, TEXT("SoulCharacter::AttackCheck - Something Hit"));
		AActor* HitActor = HitResult.GetActor(); //부딪힌 액터의 정보를 가져온다.
		if (IsValid(HitActor))
		{
			UE_LOG(LogTemp, Log, TEXT("Hit Actor Name : %s"), *HitActor->GetName());
		}

		//어트리뷰트의 값 가져오기!
		UAbilitySystemComponent* MyASC = Pawn->FindComponentByClass<UAbilitySystemComponent>();
		if (!MyASC) return;
		const USoulAttributeSet* AttributeSet = MyASC->GetSet<USoulAttributeSet>();
		if (!AttributeSet) return;
		float FinalDamage = -(AttributeSet->GetATK());




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
				NewSpecHandle.Data.Get()->SetSetByCallerMagnitude(DamageTag, FinalDamage);

				// 4. 최종 완성된 스펙을 내 ASC의 권한으로 적의 ASC에 적용!
				MyASC->ApplyGameplayEffectSpecToTarget(*NewSpecHandle.Data.Get(), EnemyASC);

				UE_LOG(LogTemp, Log, TEXT("GE 데미지 적용 완료! 최종 수치: %f"), FinalDamage);
			}
		}


	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("AMyCharacter::AttackCheck - No Hit"));
	}

}

void USoulCombatComponent::DamageCheck(float Damage)
{
	
	UAbilitySystemComponent* ASC = nullptr;

	if (AActor* OwnerActor = GetOwner())
	{
		ASC = OwnerActor->FindComponentByClass<UAbilitySystemComponent>();
	}

	if (!ASC) return;

	if ((ASC->HasMatchingGameplayTag(SoulGameplayTags::Gameplay_State_Move_Roll)) && (Damage < 8))
	{
		return;
	}

	if (ASC->HasMatchingGameplayTag(SoulGameplayTags::Gameplay_State_Action_OnShield))
	{
		float SP = ASC->GetNumericAttribute(USoulAttributeSet::GetSPAttribute());
		
		float NewSP = SP - 30.f;
		NewSP = FMath::Max(NewSP, 0.f); //역시 SP가 0이 되지 않도록 한다.

		ASC->SetNumericAttributeBase(USoulAttributeSet::GetSPAttribute(), NewSP);

		if (NewSP == 0.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("ShieldBreak"));
			ASC->CancelAbilities();

			if (AActor* AvatarActor = ASC->GetAvatarActor())
			{
				if (USkeletalMeshComponent* Mesh = AvatarActor->FindComponentByClass<USkeletalMeshComponent>())
				{
					if (USoulAnimInstance* AnimInstance = Cast<USoulAnimInstance>(Mesh->GetAnimInstance()))
					{
						AnimInstance->Montage_Stop(0.f);
						

					}
				}
			}
		}
		else
		{
			ASC->CancelAbilities();

			if (AActor* AvatarActor = ASC->GetAvatarActor())
			{
				if (USkeletalMeshComponent* Mesh = AvatarActor->FindComponentByClass<USkeletalMeshComponent>())
				{
					if (USoulAnimInstance* AnimInstance = Cast<USoulAnimInstance>(Mesh->GetAnimInstance()))
					{
						ASC->AddLooseGameplayTag(SoulGameplayTags::Gameplay_State_Action_OnShield);
						AnimInstance->Montage_Stop(0.1f);

						FTimerHandle TimerHandle1;
						AvatarActor->GetWorld()->GetTimerManager().SetTimer( //0.01초 뒤에 내부 내용 실행
							TimerHandle1,
							[AnimInstance]()
							{
								AnimInstance->PlayGuard();
							},
							0.1f,
							false
						);

						FTimerHandle TimerHandle2;
						AvatarActor->GetWorld()->GetTimerManager().SetTimer( //1초 뒤에 내부 내용 실행
							TimerHandle2,
							[ASC]()
							{
								if (IsValid(ASC))
								{
									ASC->RemoveLooseGameplayTag(SoulGameplayTags::Gameplay_State_Action_OnShield); 
								}
							},
							0.5f,
							false
						);
						
					}
				}
			}
		}

	}
	

	float CurrentHP = ASC->GetNumericAttribute(USoulAttributeSet::GetHPAttribute()); //GetHP는 값만을 전달하고, 이렇게 선언해야 변수로 저장가능한것으로 보임
	float DEF = ASC->GetNumericAttribute(USoulAttributeSet::GetDEFAttribute());


	float FinalDamage = FMath::Max(Damage - DEF, 0.f); //최소 0으로 계산한다.

	float NewHP = CurrentHP - FinalDamage;
	NewHP = FMath::Max(NewHP, 0.f); //역시 HP가 0 이하가 되지 않도록 한다.

	if (!(ASC->HasMatchingGameplayTag(SoulGameplayTags::Gameplay_State_Action_OnShield)))
	{
		if (AActor* AvatarActor = ASC->GetAvatarActor())
		{
			if (USkeletalMeshComponent* Mesh = AvatarActor->FindComponentByClass<USkeletalMeshComponent>())
			{
				if (USoulAnimInstance* AnimInstance = Cast<USoulAnimInstance>(Mesh->GetAnimInstance()))
				{
					AnimInstance->PlayShieldBreak();
				}
			}
		}
		
	}

	if (NewHP < 1.f) //1.f 대미지가 들어올때, 코드의 0.00000의 에러에 의해 작동하지 않을때가 있다.
	{
		

		ASC->CancelAbilities();

		AActor* OwnerActor = GetOwner();

		APawn* Pawn = Cast<APawn>(ASC->GetAvatarActor());
		if (!Pawn) return;


		if (USkeletalMeshComponent* Mesh = OwnerActor->FindComponentByClass<USkeletalMeshComponent>())
		{
			if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
			{
				AnimInstance->Montage_Stop(0.f);
			}
		}
		TryGA_Death();
	}

	ASC->SetNumericAttributeBase(USoulAttributeSet::GetHPAttribute(), NewHP);
}
*/

void USoulCombatComponent::GiveGA_Death()
{
	if (AActor* OwnerActor = GetOwner())
	{
		
		UAbilitySystemComponent* ASC = OwnerActor->FindComponentByClass<UAbilitySystemComponent>();

		ASC->GiveAbility(FGameplayAbilitySpec(GA_Death, 1, 0));
	}

	

}

void USoulCombatComponent::TryGA_Death()
{
	UE_LOG(LogTemp, Warning, TEXT("USoulCombatComponent::TryGA_Death"));

	if (AActor* OwnerActor = GetOwner())
	{
		

		UAbilitySystemComponent* ASC = OwnerActor->FindComponentByClass<UAbilitySystemComponent>();

		ASC->AddLooseGameplayTag(SoulGameplayTags::Gameplay_Rule_Death, true);

		ASC->TryActivateAbilityByClass(GA_Death);
	}

	
}

void USoulCombatComponent::LockOn_Enemy(AActor* Enemy, float DeltaTime)
{
	if (!IsValid(Enemy))
	{
		return;
	}

	ASoulCharacter* Character = Cast<ASoulCharacter>(GetOwner());
	if (!Character) return;

	APlayerController* PC = Cast<APlayerController>(Character->GetController());
	if (!PC) return;

	FVector Start = Character->GetActorLocation();
	FVector TargetLoc = Enemy->GetActorLocation();

	FRotator LookAtRot = (TargetLoc - Start).Rotation();

	// [카메라 전용 회전] 카메라(ControlRotation)는 상하(Pitch)와 좌우(Yaw) 모두 부드럽게 고정
	FRotator TargetCameraRot(LookAtRot.Pitch, LookAtRot.Yaw, 0.f);
	FRotator CurrentRot = PC->GetControlRotation();
	FRotator SmoothRot = FMath::RInterpTo(
		CurrentRot,
		TargetCameraRot,
		DeltaTime,
		20.f
	);

	PC->SetControlRotation(SmoothRot);

	// [구르기 태그 체크] - 기존 구르기 태그 적용
	/*
	if (UAbilitySystemComponent* ASC = Character->FindComponentByClass<UAbilitySystemComponent>())
	{
		// 알려주신 태그 이름으로 매핑합니다.
		FGameplayTag RollTag = FGameplayTag::RequestGameplayTag(FName("Gameplay.State.Move.Roll"));

		if (ASC->HasMatchingGameplayTag(RollTag))
		{
			// 구르기 어빌리티가 켜져서 이 태그를 가지고 있다면
			// 캐릭터 몸체 회전(SetActorRotation)을 스킵하고 바로 리턴합니다.
			return;
		}
	}*/

	// [캐릭터 바디 전용 회전] 캐릭터 몸체는 오직 좌우(Yaw)로만 회전 (상하 꺾임 방지)
	FRotator TargetCharacterRot(0.f, LookAtRot.Yaw, 0.f);
	FRotator CurrentCharacterRot = Character->GetActorRotation();
	FRotator SmoothCharacterRot = FMath::RInterpTo(
		CurrentCharacterRot, 
		TargetCharacterRot, 
		DeltaTime, 
		20.f);

	Character->SetActorRotation(SmoothCharacterRot);

	

}

