// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/MonsterBase.h"
#include "Monster/MonsterAbilitySystemComponent.h"
#include "Gas/SoulAttributeSet.h"
#include "GAS/SoulAbilitySet.h"
#include "AIController.h" 
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
AMonsterBase::AMonsterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    AbilitySystemComponent = CreateDefaultSubobject<UMonsterAbilitySystemComponent>(TEXT("MonsterAbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);

    AttributeSet = CreateDefaultSubobject<USoulAttributeSet>(TEXT("MonsterAttributeSet"));

}

// Called when the game starts or when spawned
void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMonsterBase::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this); //ASC 초기화

        GiveAbilitySet();

        AActor* This = Cast<AActor>(this);

        UE_LOG(LogTemp, Warning, TEXT("%s : Ability Num: %d"), *This->GetName(), AbilitySystemComponent->GetActivatableAbilities().Num());



        USoulAttributeSet* ActiveAttributeSet = const_cast<USoulAttributeSet*>(AbilitySystemComponent->GetSet<USoulAttributeSet>());

        /* 방법2
        USoulAttributeSet* ActiveAttributeSet = const_cast<USoulAttributeSet*>(
            Cast<USoulAttributeSet>(AbilitySystemComponent->GetAttributeSet(USoulAttributeSet::StaticClass()))
            );*/
        
        //방법3
        //const USoulAttributeSet* ActiveAttributeSet = Cast<USoulAttributeSet>(AbilitySystemComponent->GetSet<USoulAttributeSet>());

        if (ActiveAttributeSet)
        {
            // 몬스터가 쥐고 있는 변수 주소도 진짜 주소로 동기화해 줍니다.
            AttributeSet = ActiveAttributeSet;

            UE_LOG(LogTemp, Warning, TEXT("AMonsterBase::PossessedBy : 'AttributeSet - Die' Binding!"));
            AttributeSet->OnHPZeroDelegate.AddUObject(this, &AMonsterBase::Die);
            AttributeSet->OnHPChangedDelegate.AddUObject(this, &AMonsterBase::ReturnDamageImpact);
        }

    }
}

void AMonsterBase::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    
}

// Called every frame
void AMonsterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMonsterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMonsterBase::GiveAbilitySet()
{
    FSoulAbilitySet_GrantedHandles GrantedHandles;

    if (AbilitySet && AbilitySystemComponent)
    {
        AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, &GrantedHandles, this);
    }
}

// 몬스터 상태 변화 실행 함수.
void AMonsterBase::SetMonsterState(EMonsterState NewState)
{
	// 동일한 상황은 무시한다.
	if (CurrentState == NewState) return;

	EMonsterState OldState = CurrentState;
	CurrentState = NewState;

	
	OnStateChanged(OldState, NewState);
}

// 실질적 상태 변화를 위한 코드.
void AMonsterBase::OnStateChanged(EMonsterState OldState, EMonsterState NewState)
{ 
    switch (NewState)
    {
    case EMonsterState::Idle:
        break;
    case EMonsterState::Patrol:
        break;
    case EMonsterState::Chase:
        break;
    case EMonsterState::Attack:
        break;
    case EMonsterState::Hit:
        // 예: 피격 상태가 되면 이동을 일시 중지시키는 등의 공통 처리
        break;
    case EMonsterState::Die:
        break;
    }
}

void AMonsterBase::ReturnDamageImpact_Implementation()
{
    
}


void AMonsterBase::Die_Implementation()
{
    SetMonsterState(EMonsterState::Die);

    UE_LOG(LogTemp, Warning, TEXT("AMonsterBase::Die_Implementation"));

    //캐릭터 사망시 실행되는 함수.
    AAIController* AIC = Cast<AAIController>(GetController());
    if (!AIC) return;

    UBlackboardComponent* BB = AIC->GetBlackboardComponent();
    if (!BB) return;

    BB->SetValueAsBool(TEXT("bMonsterDeath"), true);
}
