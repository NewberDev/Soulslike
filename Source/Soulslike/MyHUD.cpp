// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"

void AMyHUD::ShowEndingUi()
{
	if (!EndingWidget)
	{
		return;
	}

	UUserWidget* EndingWidgetInstance = CreateWidget<UUserWidget>(
		GetOwningPlayerController(),
		EndingWidget
	); //인스턴스로 생성한 클래스의 정보를 받고
	if (EndingWidgetInstance)
	{
		EndingWidgetInstance->AddToViewport(); //뷰포트에 새로운 인스턴스 생성
	}
}

void AMyHUD::ShowEnemyFindUi()
{
	if (!EnemyFindWidget)
	{
		return;
	}

	EnemyFindWidgetInstance = CreateWidget<UUserWidget>(
		GetOwningPlayerController(),
		EnemyFindWidget
	); //인스턴스로 생성한 클래스의 정보를 받고
	if (EnemyFindWidgetInstance)
	{
		EnemyFindWidgetInstance->AddToViewport(); //뷰포트에 새로운 인스턴스 생성
	}

	FTimerHandle EndingWidgetTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		EndingWidgetTimerHandle,
		this,
		&AMyHUD::RemoveUI, // 제거 함수
		3.5f,
		false
	);
}



void AMyHUD::RemoveUI()
{
	if (EnemyFindWidgetInstance)
	{
		EnemyFindWidgetInstance->RemoveFromParent();
		EnemyFindWidgetInstance = nullptr;
	}
}
