// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TestClass.h"
#include "SubTestClass.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API ASubTestClass : public ATestClass
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	virtual void Fct() override;
};
