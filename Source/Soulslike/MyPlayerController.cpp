// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "MyHUD.h"

void AMyPlayerController::ClientRPC_OnCharacterDead_Implementation()
{
	AMyHUD* MyMyHUD = GetHUD<AMyHUD>(); //컨트롤러에서 허드를 가지고 오는 코드가있다.
	if (!MyMyHUD)
	{
		return; 
		/*
		 이런 유효성 검사는 필수로 해야 문제가 발생할떄
		 그것에 해결 속도가 빨라진다! 습관화하자!
		*/
	}
	MyMyHUD->ShowEndingUi();

}

void AMyPlayerController::ClientRPC_OnEnemyFind_Implementation()
{
	AMyHUD* MyMyHUD = GetHUD<AMyHUD>(); 
	if (!MyMyHUD)
	{
		return;
	}
	MyMyHUD->ShowEnemyFindUi();
}