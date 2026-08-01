// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Kismet/GameplayStatics.h"
#include "MyCharacter.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	RootComponent = Mesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(Mesh);
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
	//일정 속도를 넘기지 못하도록 하는 구성
	
	//ProjectileMovement->bRotationFollowsVelocity = true; //움직이는 방향으로 회전할지 말지 설정


	//NiagaraComponent->CreateDefaultSubobhect<UNiagaraComponent>(TEXT(""));
	//NiagaraComponent->SetAttachment(Mesh);
	//NiagaraComponent->SetAttachment(Mesh);

	
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	/*
	//서버에서는 플레이어가 다수 일수 있기에 이 액터를 소환한 클라이언트를 가져온다.
	AActor* OwningActor = GetOwner();
	APlayerController* PC = Cast<APlayerController>(OwningActor);
	if (PC == nullptr)
	{
		OwningActor = OwningActor->GetOwner();
		while (OwningActor != nullptr && PC == nullptr)
		{
			PC = Cast<APlayerController>(OwningActor);
			OwningActor = OwningActor->GetOwner();
			
		}
	}
	if (PC == nullptr)
	{
		return;
	}

	if (PC)
	{
		FVector CameraLocation;
		FRotator CameraRotation;
		PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
		//플레이어 방향으로 바라보게 된다.

		FVector LookDirection = CameraRotation.Vector();
		//ProjectileMovement->Velocity = LookDirection * Speed;
		//위 방법은 총알이 앞이 아닌 다른 방향을 앞으로 날아갈수 있다.

		SetActorRotation(CameraRotation);
		//우리가 원하는 방향으로 회전해서 나가도록함
	}
	//이런 방식은 발사체를 만드는 곳에서 설정이 가능하기 굳이 여기서 할필요가 없다.
	*/

	SetLifeSpan(10.0f);

	/*Mesh->OnComponentBeginOverlap.AddDynamic(
		this,
		&ABullet::OnCompoentBeginOverlap
	); */


	//메시의 히트 컴포넌트에 델리게이트로 등록
	/*
	Mesh->OnComponentHit.AddDynamic(
		this,
		&ABullet::OnMyComponentHit
	); */
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


//이 함수는 메시(컴포넌트)에 바인딩을 해야 작동한다.
void ABullet::OnComponentBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& HitResult
)
{
	if (OtherActor == Owner)
	{
		return;
	}

	ACharacter* OwningCharacter = Cast<ACharacter>(Owner);
	if (!OwningCharacter)
	{
		return;
	}
	
	AController* OwingController = OwningCharacter->GetController();
	if (!OwingController)
	{
		return;
	}

	/*
	UGameplayStatics::ApplyDamage(
		OtherActor,
		10.f,
		OwingController,
		this,                                                                                                                                                                                                                                             
		//UDamageType::StaticClass
	); */
	//Destroy(); //현재 인스턴스 삭제!

	//HitResult.ImpactNormal; //법선 벡터
	

	//NormalImpulse.Length();

}

/*
void ABullet::OnMyComponentHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit
)
{
	float ImpulseSize = NormalImpulse.Size(); // 충격량 크기
	UE_LOG(LogTemp, Log, TEXT("Impulse = %f"), ImpulseSize);
	
	float vel = ImpulseSize / Mesh->GetMass();

	if (vel >= 300.f)
	{
		FTransform SpawnTransform;

		SpawnTransform.SetLocation(GetActorLocation());

		FActorSpawnParameters SpawnParams;

		SpawnParams.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (IsValid(SpawnActorClass))
		{
			AActor* bullet = GetWorld()->SpawnActor<AActor>(
				SpawnActorClass,
				SpawnTransform,
				SpawnParams
			);
		}
	}
	
}*/

void ABullet::CollisionImpact(FVector NormalImpulse)
{
	float ImpulseSize = NormalImpulse.Size(); // 충격량 크기
	UE_LOG(LogTemp, Log, TEXT("Impulse = %f"), ImpulseSize);

	float vel = ImpulseSize / Mesh->GetMass();

	if (vel >= 300.f)
	{
		FTransform SpawnTransform;

		SpawnTransform.SetLocation(GetActorLocation());

		FActorSpawnParameters SpawnParams;

		SpawnParams.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (IsValid(SpawnActorClass))
		{
			AActor* bullet = GetWorld()->SpawnActor<AActor>(
				SpawnActorClass,
				SpawnTransform,
				SpawnParams
			);
		}
	}
}
