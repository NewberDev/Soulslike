// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MonsterWayPointComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSLIKE_API UMonsterWayPointComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "WayPoint", meta = (AllowPrivateAccess = true))
	TArray<AActor*> WayPointList;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "WayPoint", meta = (AllowPrivateAccess = true))
	int32 MaxWP = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "WayPoint", meta = (AllowPrivateAccess = true))
	int32 CurrentWP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "WayPoint", meta = (AllowPrivateAccess = true))
	bool bIsMovingForward = true;
public:	
	// Sets default values for this component's properties
	UMonsterWayPointComponent();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual FVector GetCurrentWayPoint();

	UFUNCTION(BlueprintCallable)
	virtual void UpdateWPIndexPingPong();
	
	UFUNCTION(BlueprintCallable)
	virtual void WayPointIdleCheck();

};
