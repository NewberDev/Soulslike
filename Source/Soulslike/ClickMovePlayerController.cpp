// Fill out your copyright notice in the Description page of Project Settings.


#include "ClickMovePlayerController.h"
#include "EnhancedInputSubsystems.h" 
#include "EnhancedInputComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

AClickMovePlayerController::AClickMovePlayerController()
{
	bShowMouseCursor = true; //뷰포트에 마우스 위치를 보여주는 설정
}

void AClickMovePlayerController::OnMouse2Pressed()
{
	isMouse2Pressed = true;
}
void AClickMovePlayerController::OnMouse2Released()
{
	isMouse2Pressed = false;
}

void AClickMovePlayerController::SetNewDestination(const FVector Destination)
{
	APawn* MyPawn = GetPawn(); //이 클래스의 액터를 가져온다.
	if (MyPawn)
	{
		//밑에 함수는 특정 좌표로 현재 해당 액터를 이동시키는 함수이다.
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, Destination);
		//이걸 위해서 "AIModule"이라는 모듈이 추가로 필요하다!
	}
}

void AClickMovePlayerController::MoveToMouseCursor()
{
	FHitResult Hit;
	//마우스가 지금 어딜 눌렀는지를 찾아주는 함수.
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		SetNewDestination(Hit.ImpactPoint);
		//즉, 마우스가 클릭한 위치 정보를 받아서 그쪽으로 움직이는 함수에게 전달해준다.
	}


}


void AClickMovePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();


	//헤더 추가 필요 "EnhancedInputComponent.h"
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if (Mouse2PressedAction) //연결이 잘되어 있으면 실행
	{
		EnhancedInputComponent->BindAction(Mouse2PressedAction, ETriggerEvent::Started, this, &AClickMovePlayerController::OnMouse2Pressed);
		EnhancedInputComponent->BindAction(Mouse2PressedAction, ETriggerEvent::Completed, this, &AClickMovePlayerController::OnMouse2Released);
		//위의 경우 버튼을 눌렀을때와 땠을떄의 다른 함수 실행이다.
	}

}

//델타타임마다 마우스가 찍혔는지 확인 전달 함수!
void AClickMovePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	if (isMouse2Pressed)
	{
		MoveToMouseCursor();
	}
}



void AClickMovePlayerController::OnPossess(APawn* InPawn)
{

	Super::OnPossess(InPawn);
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	/*예를 들어 멀티플레이일때, 컨트롤이 여러개중에 내꺼를 가져와야하기에
	LocalPlayer로 가져오는 것이다.*/
	if (LocalPlayer) //잘 가져왔는지 확인
	{

		//Subsystem이 무엇인지는 나중에 더 배울것이다. - 헤더 추가 필요 "EnhancedInputSubsystems.h"
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

		if (InputSubsystem) //잘 가져왔나 확인 - 언리얼은 안전 추구가 중요하기에 항상 가져오면 잘 가져왔는지 확인해야한다.
		{
			InputSubsystem->AddMappingContext(InputMappingContext, 0);
			
		}
	}
}
