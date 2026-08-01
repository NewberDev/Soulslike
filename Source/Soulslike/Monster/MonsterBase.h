// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonsterBase.generated.h"


// 상태 열거형은 이 헤더와 하위 자식들이 직관적으로 공유해야 하므로 헤더에 정의한다. (따로 뺼수도 있다.)
UENUM(BlueprintType)
enum class EMonsterState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),        // 대기
	Patrol      UMETA(DisplayName = "Patrol"),      // 정찰
	Chase       UMETA(DisplayName = "Chase"),       // 추적 (플레이어 발견)
	Attack      UMETA(DisplayName = "Attack"),      // 공격 중
	Hit         UMETA(DisplayName = "Hit"),         // 피격 (경직 등)
	Die         UMETA(DisplayName = "Die")          // 사망
};


UCLASS(Abstract)
class SOULSLIKE_API AMonsterBase : public ACharacter
{
	GENERATED_BODY()

//Default
public:
	// Sets default values for this character's properties
	AMonsterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


//아래는 MonsterBase 로직	
private: 

protected:
	
	//=============================================================================
	// Components & Properties
	//=============================================================================

	//컴포넌트는 VisibleAnywhere, 일반 변수는 EditDefaultsOnly로 권장

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMonsterAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoulAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoulAbilitySet> AbilitySet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster | State")
	EMonsterState CurrentState;
					 

	//=============================================================================
	// Functions
	//=============================================================================


	/* 순수 가상 함수 [PURE_VIRTUAL] 몬스터 데이터 테이블 초기화 함수이다.
	 * 자식 클래스에서 오버라이드하여 구현해야 한다.(단, 안한다고 문제가 발생하진 않는다.)
	*/
	virtual void InitMonsterData() PURE_VIRTUAL(AMonsterBase::InitMonsterData, ;);

public:

	//MonsterState Function

	void GiveAbilitySet();
	
	UFUNCTION(BlueprintCallable, Category = "Monster | State")
	EMonsterState GetMonsterState() const { return CurrentState; }
	// 외부(BT, UI 등)에서 현재 상태를 안전하게 읽어갈 수 있는 Getter (const 필수!)

	UFUNCTION(BlueprintCallable, Category = "Monster | State")
	virtual void SetMonsterState(EMonsterState NewState);
	// 외부나 내부에서 상태를 변경할 때 사용하는 Setter

	virtual void OnStateChanged(EMonsterState OldState, EMonsterState NewState);
	//추구 State가 변할때 추가의 로직이 필요하다면 오버라이딩하자.
	//이전의 상태(OldState)가 이후의 영향을 주는 로직을 위한 파라미터이다.

	//몬스터 피격 받으면 실행되는 함수. -> HP바를 활성화한다.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Death")
	void ReturnDamageImpact();

	virtual void ReturnDamageImpact_Implementation();

	//몬스터 사망시 실행 함수.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Death")
	void Die();

	virtual void Die_Implementation();
	
};
