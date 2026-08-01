// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayActor.h"
#include "Net/UnrealNetwork.h" //Replication 구현을 위한 헤더

// Sets default values
AMultiplayActor::AMultiplayActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMultiplayActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		ServerRpcMyFunction(1);
	}
	//ServerRpcMyFunction(1);
	
}


//Replication 구현하기
void AMultiplayActor::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMultiplayActor, Hp); //이 표현으로 결국 이 변수는 실행동안 리플리케이션 된다.
}

// Called every frame
void AMultiplayActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}



//RPC 구현
void AMultiplayActor::ServerRpcMyFunction_Implementation(int a)
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Green,
		FString::Printf(TEXT("Hello Multiplay World! %d"), a)
		);
}

bool AMultiplayActor::ServerRpcMyFunction_Validate(int a)
{
	return true;
}


//리플리케이션 이벤트 함수
void AMultiplayActor::OnRep_Hp()
{
	//예를 들어 체력이 달때 애니메이션을 실행하거나, 값을 전달하는 함수로 사용가능!

}
