// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulHUD.h"

#include "MyBossCharacter.h"



void ASoulHUD::SetBoss(AMyBossCharacter* Character)
{
	BossCharacter = Character;
}

void ASoulHUD::ShowSoulBase()
{
	if (!SoulBaseWidget)
	{
		return;
	}

	SoulBaseWidgetInstance = CreateWidget<UUserWidget>(
		GetOwningPlayerController(),
		SoulBaseWidget
	); //인스턴스로 생성한 클래스의 정보를 받고
	if (SoulBaseWidgetInstance)
	{
		SoulBaseWidgetInstance->AddToViewport(); //뷰포트에 새로운 인스턴스 생성
	}
}

void ASoulHUD::ShowBossHP()
{
	if (!BossHPWidget)
	{
		return;
	}

	BossHPWidgetInstance = CreateWidget<UBossHPWidget>(
		GetOwningPlayerController(),
		BossHPWidget
	); //인스턴스로 생성한 클래스의 정보를 받고
	if (BossHPWidgetInstance)
	{
		BossHPWidgetInstance->SetBoss(BossCharacter);
		BossHPWidgetInstance->AddToViewport(); //뷰포트에 새로운 인스턴스 생성
	}
}

void ASoulHUD::RemoveUI(UUserWidget* WidgetInstance)
{
	if (WidgetInstance)
	{
		WidgetInstance->RemoveFromParent();
		WidgetInstance = nullptr;
	}
}
