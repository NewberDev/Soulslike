// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BossProjectile.generated.h"

UCLASS()
class SOULSLIKE_API ABossProjectile : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	float Speed = 1000.f;

	UPROPERTY(EditAnywhere)
	float FollowTimer;

	UPROPERTY(EditAnywhere)
	float ImpactTimer;

	UPROPERTY(EditAnywhere)
	bool OnFollow;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SpawnActorClass;
	//해당 클래스만 블루프린트에서 참조가능하다
public:	
	// Sets default values for this actor's properties
	ABossProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(BlueprintReadWrite)
	AActor* TargetCharacter;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void MagicImpactSpawn(float DeltaTime);

	UFUNCTION()
	void MagicRInterpToMove(float DeltaTime);

};
