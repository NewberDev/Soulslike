// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "Bullet.generated.h"

UCLASS()
class SOULSLIKE_API ABullet : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere)
	float Speed = 3000.f;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Niagara")
	//UNiagaraComponent* NiagaraComponent;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SpawnActorClass;
public:	
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnComponentBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& HitResult
	);
	/*sweep: 이동시 텔레포트가 아닌 쓸고 지나가는 것을 말함
	0->100을 가는 동안의 중간값을 이동했다고 해주는 처리해주는 것을 말함.
	sweep을 on하면 overlap 충돌시에 겹쳐지지 않았더라도 sweep으로 인해 그 사이에 충돌로 적용해주는 것을 말한다.
	*/

	/*
	UFUNCTION()
	void OnMyComponentHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);
	*/

	UFUNCTION(BlueprintCallable)
	void CollisionImpact(FVector NormalImpulse);

};
