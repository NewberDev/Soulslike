// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/MonsterWayPointComponent.h"

// Sets default values for this component's properties
UMonsterWayPointComponent::UMonsterWayPointComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMonsterWayPointComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMonsterWayPointComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FVector UMonsterWayPointComponent::GetCurrentWayPoint()
{
	UE_LOG(LogTemp, Log, TEXT("UMonsterWayPointComponent::WayPointPatrol"));
	
	if (WayPointList.IsValidIndex(CurrentWP) && IsValid(WayPointList[CurrentWP]))
	{
		FVector PatrolWP = (WayPointList[CurrentWP])->GetActorLocation();

		return PatrolWP;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UMonsterWayPointComponent::WayPointPatrol_WayPointList[%d] Not Valid"), CurrentWP);
		return FVector();
	}

}

void UMonsterWayPointComponent::UpdateWPIndexPingPong()
{
	if (MaxWP == 1) return;

	if (bIsMovingForward)
	{
		if (CurrentWP >= (MaxWP - 1))
		{
			CurrentWP--;
			bIsMovingForward = false;
		}
		else
		{
			CurrentWP++;
		}
	}
	else
	{
		if (CurrentWP <= 0)
		{
			CurrentWP++;
			bIsMovingForward = true;
		}
		else
		{
			CurrentWP--;
		}
	}
}

void UMonsterWayPointComponent::WayPointIdleCheck()
{
	if (MaxWP == 1)
	{
		if (IsValid(WayPointList[CurrentWP]))
		{
			AActor* OwnerActor = GetOwner();

			FRotator TargetRotation = WayPointList[CurrentWP]->GetActorRotation();

			OwnerActor->SetActorRotation(TargetRotation);

		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UMonsterWayPointComponent::WayPointIdleCheck_WayPointList[%d] Not Valid"), CurrentWP);
		}
	}
}



