// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestClass.generated.h"

UCLASS()
class SOULSLIKE_API ATestClass : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestClass();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Fct() PURE_VIRTUAL(ATestClass::Fct, ;);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//virtual void Fct() PURE_VIRTUAL(ATestClass::Fct(), ;);
	
};
