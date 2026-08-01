// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ClickMoveCharacter.generated.h"

UCLASS()
class SOULSLIKE_API AClickMoveCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AClickMoveCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/*우린 카메라와 스프링암이란 컴포넌트를 헤더에 참조하지 않았기에 모른다.
	그래서  class를 붙여서 니가 모르는게 당연하다는 추가 설명을 붙인것이다.
	이렇게 코드에 참조하지 않은 클래스의 이름임을 class를 붙여서 알려주는 것을
	전방선언이라고 한다.!!!
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta=(AllowPrivateAccess="true"))
	class UCameraComponent* CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComponent;
};
