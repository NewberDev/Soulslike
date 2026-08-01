// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCameraComponent.generated.h"

class USpringArmComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSLIKE_API UPlayerCameraComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true")) 
	TObjectPtr<USpringArmComponent> CachedSpringArm;

	AActor* Enemy;

	bool bIsZooming = false;

	float ZoomEnemyLength = 0.f;

	int ZoomEnemyPattern = 0;

	float CameraTimer = 0.f;

	float OriginalLength = 0.f;

	float ZoomEnemyDuration = 1.f;

	float DeadCameraDuration = 1.f;

	bool bIsDeadMoving = false;

	// 에디터 패널에서 카메라 쉐이크 블루프린트 에셋을 할당할 슬롯입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraShakeBase> SceneCameraShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraShakeBase> DamageCameraShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraShakeBase> EarthquakeCameraShakeClass;

public:	
	// Sets default values for this component's properties
	UPlayerCameraComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Zoom Enemy Logic
	UFUNCTION(BlueprintCallable)
	void SetEnemy(AActor* NewEnemy);

	UFUNCTION(BlueprintCallable)
	void ZoomEnemyTrigger();

	void ZoomEnemy(float DeltaTime);

	void LookAtEnemy();

	void LockOwnerPlayerInput();

	void UnlockOwnerPlayerInput();

	UFUNCTION(BlueprintCallable)
	void CameraShaking_Scene();

	UFUNCTION(BlueprintCallable)
	void CameraShaking_Earthquake();

	UFUNCTION(BlueprintCallable)
	void CameraShaking_Damage();

	UFUNCTION(BlueprintCallable)
	void PlayerDeadMovingTrigger();

	//Player Dead Logic
	void PlayerDeadMoving(float DeltaTime);
};
