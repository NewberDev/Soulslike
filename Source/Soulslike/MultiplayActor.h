// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MultiplayActor.generated.h"

UCLASS()
class SOULSLIKE_API AMultiplayActor : public AActor
{
	GENERATED_BODY()
	
public:	
	//Replicaion 구현
	//UPROPERTY(Replicated) //단순 리플리케이션 구성
	UPROPERTY(ReplicatedUsing = OnRep_HP) //리플리케이션 된 값이 변경되면 OnRep_fct라는 함수를 실행시키는 구성
	int Hp;
	//리플리케이션 변경시 실행함수의 이름은 OnRep_로 접두사를 두자!
	// Sets default values for this actor's properties
	AMultiplayActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	//RPC 구현
	UFUNCTION(Client, Reliable, WithValidation) //Server, Client, NetMulticast
	void ServerRpcMyFunction(int a);

	//Replication 구현
	void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override; 

protected:
	//리플리케이션 변수 변경시 실행함수
	UFUNCTION()
	void OnRep_Hp();


};
