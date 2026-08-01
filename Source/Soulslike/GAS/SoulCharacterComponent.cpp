// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulCharacterComponent.h"
#include "SoulInputComponent.h"
#include "SoulGameplayTags.h"
#include "SoulInputConfig.h"
#include "SoulAbilitySystemComponent.h"




// Sets default values for this component's properties
USoulCharacterComponent::USoulCharacterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USoulCharacterComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}




// Called every frame
void USoulCharacterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USoulCharacterComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	

	UE_LOG(LogTemp, Warning, TEXT("USoulCharacterComponent::InitializePlayerInput"));

	APawn* Pawn = Cast<APawn>(GetOwner()); 
	/*lyra에서는 GameFrameworkComponent 계열에서 정의해서 이 컴포넌트가 붙어있는 
	Pawn에 정보를 받아오는 함수를 구현하고 그것을 사용한다.*/
	if (!Pawn) return;

	APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	if (!PC) return;
	//위와 동일한 상황이며, 이 컴포넌트는 Player전용이기에 AI라면 막는것이다.

	/*
		Lyra에서는 PawnData라는 데이터 에셋으로 캐릭터 정보, InputConfig 등을 받아오고,
		또한 변수로 IMC를 받아와서 새로 생성한 캐릭터에 등록하는 함수도 작성되어 있으나,
		일단은 단순히 변수로 생성해서 받아오는 방식으로 대체한다.
	*/

	if (InputConfig)
	{
		USoulInputComponent* SoulIC = Cast<USoulInputComponent>(PlayerInputComponent);
		if (ensureMsgf(SoulIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to ULyraInputComponent or a subclass of it.")))
		{
			UE_LOG(LogTemp, Warning, TEXT("USoulCharacterComponent::InitializePlayerInput_SoulIC_Cast_Valid"));
			TArray<uint32> BindHandles;
			SoulIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		
			SoulIC->BindNativeAction(InputConfig, SoulGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
			SoulIC->BindNativeAction(InputConfig, SoulGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look, /*bLogIfNotFound=*/ false);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("USoulCharacterComponent::InitializePlayerInput_SoulIC_Cast_InValid"));

		}
	}


}



//AbilityInputAction에 관여하는 함수.
void USoulCharacterComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		//ASC를 불러와서 현재 InputTag를 전달한다. --> ASC를 구현하고 다시 진행한다.
		if (USoulAbilitySystemComponent* ASC = Pawn->FindComponentByClass<USoulAbilitySystemComponent>())
		{
			//UE_LOG(LogTemp, Warning, TEXT("Character -> ASC Pressed: %s"), *InputTag.ToString());
			ASC->AbilityInputTagPressed(InputTag);
		}
	}
}

void USoulCharacterComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		//ASC를 불러와서 현재 InputTag를 제거한다.
		if (USoulAbilitySystemComponent* ASC = Pawn->FindComponentByClass<USoulAbilitySystemComponent>())
		{
			//UE_LOG(LogTemp, Warning, TEXT("Character -> ASC Pressed: %s"), *InputTag.ToString());
			ASC->AbilityInputTagReleased(InputTag);
		}
	}
}





//NativeInputAction에 관여하는 함수.
void USoulCharacterComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	//UE_LOG(LogTemp, Warning, TEXT("USoulCharacterComponent::Input_Move"));
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn) return;
	APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	if (!PC) return;

	if (USoulAbilitySystemComponent* ASC = Pawn->FindComponentByClass<USoulAbilitySystemComponent>())
	{
		if (ASC->HasMatchingGameplayTag(SoulGameplayTags::Gameplay_State_OnAir))
		{
			return;
		}
	}
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	FRotator ForwardRotation(0, Pawn->GetController()->GetControlRotation().Yaw, 0);
	FRotator RightRotation(0, Pawn->GetController()->GetControlRotation().Yaw+90, 0);


	//FVector MoveDirection = Pawn->GetActorForwardVector() * Value.Y + Pawn->GetActorRightVector() * Value.X; //이전의 카메라 뱡향으로 이동!

	FVector MoveDirection = ForwardRotation.Vector() * Value.Y + RightRotation.Vector() * Value.X;

	
	MoveDirection.Normalize(); //대각선도 단위벡터로 정규화하는 코드!!!
	Pawn->AddMovementInput(MoveDirection);
}

void USoulCharacterComponent::Input_Look(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn) return;

	FVector2D Value = InputActionValue.Get<FVector2D>();
	Pawn->AddControllerPitchInput(Value.Y);
	Pawn->AddControllerYawInput(Value.X);
}

