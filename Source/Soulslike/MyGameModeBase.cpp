 // Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "MyCharacter.h"
#include "MyPlayerController.h"

void AMyGameModeBase::BeginPlay()
{
	//레벨에 새로운 액터가 스폰될때마다 실행되는 월드단위의 델리게이트로
	//액터 스폰시 등록한 AActor를 인자로 받는 함수를 실행시킬 수 있다.
	//이것으로 플레이어의 죽었을때 실행할 델리게이트의 등록가능 (방법1)
	
	GetWorld()->AddOnActorSpawnedHandler(
		FOnActorSpawned::FDelegate::CreateUObject(
			this,
			&AMyGameModeBase::HandelActorSpawned
		)
	); 
}

void AMyGameModeBase::HandelActorSpawned(AActor* Actor)
{

}

void AMyGameModeBase::RestartGame()
{
	UWorld* World = GetWorld();
	//UGameplayStatics::OpenLevel(World, TEXT("L_MyNav"));
	//TEXT매크로는 문자열이 깨지는 것을 방지하기 위한 선언이다.

	//UGameplayStatics::OpenLevelBySoftObjectPtr(World, Level);
	
	//아래의 방법으로 따로 이름을 받을 것 없이 지금 level의 이름을 가져올수도 잇다.
	FName LevelName = FName(World->GetMapName());
	//싱글 플레이 용도
	//UGameplayStatics::OpenLevel(World, LevelName); 
	GetWorld()->ServerTravel(TEXT("L_BossMap")); //서버와 클라이언트 같이 레벨 재시작
	
	/*
	* APlayerController* PC = GetPlayerController(n);
	* PC->ClientTravel(TEXT("PATH")); //클라이언트(플레이어 컨트롤러) 혼자 이동 
	*/
}

void AMyGameModeBase::OnCharacterDead(AMyCharacter* MyCharacter)
{
	//UI 띄우기 -> 클라이언트에서 실행
	AMyPlayerController* PC = MyCharacter->GetController<AMyPlayerController>();
	if (!PC)
	{
		UE_LOG(LogTemp, Log, TEXT("AMyGameModeBase::OnCharacterDead - PlayerController_InValid"));
		return;
	}
	PC->ClientRPC_OnCharacterDead();
	
	//게임 재시작하기 -> 서버에서 실행


	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f, FColor::Yellow,
		FString::Printf(TEXT("AMyGameModeBase::OnCharacterDead"))
	);

	FTimerHandle GameOverTimerHandle; //타이머의 취소, 조작등의 역할을 한다.
	GetWorld()->GetTimerManager().SetTimer(
		GameOverTimerHandle,
		this, //어떤 클래스의
		&AMyGameModeBase::RestartGame, //어떤 함수를 가져올지
		3.0f, //몇 초후에 실행?
		false //루프 할지(true)/ 말지(false)
	);
}



/*이 함수가 캐릭터쪽에서 실행되야 캐릭터의 델리게이트에 게임모드베이스의 
재시작함수가 있는데, 가장 간단하게 캐릭터 클래스의 BeginPlay에서 델리게이트에서 
실행하는 것이 가장 간단하지만, 간혹 캐릭터의 BeginPlay보다 게임모드의 실행이
늦어지는 경우 아래의 함수가 제대로 실행되지 않아 재시작이 안될수 있다!*/
void AMyGameModeBase::OnCharacterSpawned(AMyCharacter* MyCharacter)
{
	/*캐릭터의 정보를 받아왔다.
	  캐릭터에서 델리게이트를 만들고 거기에
	  RestartGame을 넣어서 Hp가 0이 됐을때 재시작하는 
	  구현을 만들자!
	 */
	MyCharacter->OnMyCharacterDead.AddDynamic(this, &AMyGameModeBase::OnCharacterDead);
}

//클라이언트가 서버에 접속하여 빙의된후 서버에서만 호출되는 게임모드 베이스 내 함수이다.
//이것으로 플레이어의 죽었을때 실행할 델리게이트의 등록가능 (방법2)
void AMyGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	AMyCharacter* MyCharacter = NewPlayer->GetPawn<AMyCharacter>();
	if (!MyCharacter)
	{
		return;
	}
	OnCharacterSpawned(MyCharacter);
}


void AMyGameModeBase::EnemyPlayerFind(AActor* Enemy, AActor* Player)
{
	//카메라 특정 액터 바라보기 + 줌인 + 제어 불가
	AMyCharacter* MyCharacter = Cast<AMyCharacter>(Player);
	MyCharacter->EnemyFind_CameraAction(Enemy);

	//발견 위젯 실행
}