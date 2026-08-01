// Fill out your copyright notice in the Description page of Project Settings.


#include "SubTestClass.h"

void ASubTestClass::BeginPlay()
{
	Super::BeginPlay();

	//Fct();
}

void ASubTestClass::Fct()
{
	UE_LOG(LogTemp, Warning, TEXT("ASubTestClass::Fct"));
}
