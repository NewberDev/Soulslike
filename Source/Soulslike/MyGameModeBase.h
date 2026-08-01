// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"


class AMyCharacter; 
//참조 없이 이런 클래스가 있다는 것을 알려주는 선언으로 이후에 소스파일에서 헤더로 참조한다.

/**
 * 
 */
UCLASS()
class SOULSLIKE_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	//TObjectPtr<UWorld> Level; //기본적인 포인터와 같으나 더 효율적인 것이 이 오브젝트 포인터이다. 이게 표준이라고 한다.
	/*
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> Level;
	*/
	//level같이 메모리 소모가 큰 하드 참조는 소모가 크기게 나온것이 Soft참조이다.
	void BeginPlay() override;

	void HandelActorSpawned(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void RestartGame();

	UFUNCTION()
	void OnCharacterDead(AMyCharacter* MyCharacter);

	UFUNCTION(BlueprintCallable)
	void OnCharacterSpawned(AMyCharacter* MyCharacter);

	void PostLogin(APlayerController* NewPlayer) override;

	UFUNCTION(BlueprintCallable)
	void EnemyPlayerFind(AActor* Enemy, AActor* Player);
};
