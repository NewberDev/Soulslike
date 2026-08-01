// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputMappingContext.h" //입력을 위한 추가
#include "MyAnimInstance.h"
#include "AbilitySystemComponent.h" //어빌리티 헤더
#include "AbilitySystemInterface.h"
#include "MyCharacter.generated.h" //항상 마지막으로 가야한다!



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMyCharacterDead, AMyCharacter*, MyCharacter);


//전방 선언 
class ABullet;

//class UAbilitySystemComponent;

UCLASS()
class SOULSLIKE_API AMyCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
	

	//입력으로 움직이기]
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;
	/* //BP 내에서 대신한다.
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	USkeletalMeshComponent* Arms;
	*/
	//애니메이션 몽타주로 만든 동작실행하는 적용
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* RollAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* WeaponAction;
	//UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
	//AActor* bullet;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABullet> SpawnActorClass; 
	//해당 클래스만 블루프린트에서 참조가능하다


	//무기 들었다 넣었다 하는 선택지
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SavedWeapon;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> UseWeapon;

	AActor* savesword;
	AActor* usesword;

	bool UsingWeapon;
	
	//여기까지
	


	//11/26 - 콤보 공격 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	bool IsAttacking = false; //공격중인가?

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	bool CanNextCombo; //다음 콤보 가능?

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn; //동작중에 또 키를 눌렀는가?

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo; //현재 몇번째 콤보중?

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	int32 MaxCombo; //최대 몇번째 콤보인가?

	UMyAnimInstance* AnimInstance; //앞으로 이 정보를 많이 받아올 것이기에
	//그냥 변수로 한번에 받아두고 사용하기 위함

	//12/3 - 디버깅 드로잉
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	float AttackRange;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	float AttackRadius;

	
	//12/3 - 데미지 받는 액터 만들기, 12/22 - 리플리케이션 변수 
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHP, EditAnywhere, BlueprintReadWrite, Category = "state", Meta = (AllowPrivateAccess = true) )
	float CurrentHp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "state", Meta = (AllowPrivateAccess = true))
	float MaxHp;

	//래그돌 되돌리기를 위한 트랜스폼 변수
	FTransform Mytransform;


	//코드로 어빌리티 다루기-----------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS", Meta = (AllowPrivateAccess = true))
	TSubclassOf<UGameplayAbility> AttackAbilities;

	UPROPERTY() 
	class UComboAttackAttributeSet* ComboAttackAttributeSet;

	UPROPERTY()
	class UCharacterStatAttributeSet* CharacterStatAttributeSet;

	FGameplayAbilitySpecHandle AttackAbilityHander;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> DefaultAttributeEffect;
	//----------------------------------------
	/*
	언리얼의 인터페이스는 두개의 클래스를 필요로 하며
	하는 보여지는 클래스와 다른 하나는 기능을 적용하기 위한 클래스이다.
	U로 시작하는 인터페이스는 보여지는 역할,
	I로 시작하는 인터페이스는 실제 코드를 적용하는 역할이다.
	인터페이스는 기본 상속을 받으면 안되지만, 언리얼의 모든 클래스는
	UObject를 상속하기에 그것을 위해, 언리얼에 보여지기 위해 있는 것이다.
	그렇기에 실제 상속하지 않는 I 인터페이스에 코드를 작성하는것이다.
	*/

public:
	// Sets default values for this character's properties
	AMyCharacter();

	//12/3 - 데미지 받는 액터 만들기
	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCaser
	)override;

	//어빌리티 컴포넌트를 가져오는 함수로, IAbilitySystemInterface를 상속해야 쓸수있다.
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	/*어빌리티를 가지고 있을수 있는 객체한테서 어빌리티 시스템 컴포넌트를 반환해
	주는 함수를 사용할수 있도록 제공해주기 위해서 위 함수를 사용하는 것이다.

	굳이 이런 인터페이스를 상속받지 않아도 되지만 나중에 상속을 할수도 있기에
	미리미리 상속해두는 것이 좋다. (습관화하자!)
	*/
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//델리게이트를 위한 추가(콤보 공격)- 11/24
	virtual void PostInitializeComponents() override;

	//위와 연결 11/24
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//PossessedBy함수 - 이동구현을 위한 가장 적합한 방법(외우자!!!)
	virtual void PossessedBy(AController* NewController) override;
	/*파라미터가 PlayerController가 아니라, 그냥 Controller인 이유는
	컨트롤러가 우리가 아니라 AI가 조작하도록 만들수도 있기 때문이다.*/


	//12/24 - 서버와 클라이언트 모두 적용되는 컨트롤러 적용만들기
	UFUNCTION()
	void OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController);
private: 
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void Attack(); //동작을 실행하는 함수로 파라미터가 필요는 없음
	//애니메이션 몽타주로 만든 동작실행하는 함수

	UFUNCTION() //구르기 함수
	void Roll();

	UFUNCTION() //원거리 공격
	void Fire();

	UFUNCTION() //처음 무기 생성
	void WeaponGeneration();

	UFUNCTION() //무기 On/Off
	void WeaponCheck();

	UFUNCTION() //무기 On
	void OnWeapon();

	UFUNCTION() //무기 Off
	void OffWeapon();

	USkeletalMeshComponent* CharacterMesh; // 캐릭터 메시

private:
	//직접 점프 변수 만들기
	/*
	UFUNCTION(BlueprintCallable)
	void MyJump();
	*/

public:
	void AttackStartComboState();
	void AttackEndComboState();

	//12/1 -  충돌 구현
	void AttackCheck();

	//12/22 - 리플리케이션 변수 변경시 실행함수
	UFUNCTION()
	void OnRep_CurrentHP();

	//12/22 - Replication 구현
	void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	//12/29 - 상대 Hp감소 구현
	UFUNCTION(Server, Reliable)
	void ServerRPC_ApplyDamage(AActor* HitActor);
	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowDamage(float Hp);

	//12/29 - 모두에게 플레이어 애니 적용
	UFUNCTION(Server,Reliable)
	void SeverRPC_PlayAttackMontage(int32 CurCombo);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastRPC_PlayAttackMontage(int32 CurCombo);
	

	//1/5- 래그돌 만들기
	UFUNCTION()
	void DoRagdoll();

	UFUNCTION()
	void Restrart();

	//12/29 - 모두에게 플레이어 애니 적용
	UFUNCTION(Server, Reliable)
	void SeverRPC_PlayRollMontage();

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastRPC_PlayRollMontage();

	UFUNCTION(Server, Reliable)
	void ServerRPC_Fire();

	UFUNCTION(Server, Reliable)
	void ServerRPC_BulletGeneration();

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastRPC_PlayFireMontage();

	//플레이어의 공격여부를 서버가 알도록 보내주는 RPC - 
	UFUNCTION(Server, Reliable)
	void SeverRPC_IsAttackingCheck(bool TF);
	//이 값은 서버의 AI가 알기만 하면 되기에 따로 리플리케이션 하지 않는다.


	//UPROPERTY(CPF_BlueprintAssignable, Category= "Event")
	FOnMyCharacterDead OnMyCharacterDead; //델리게이트 선언

	void EnemyFind_CameraAction(AActor* Enemy);

};
