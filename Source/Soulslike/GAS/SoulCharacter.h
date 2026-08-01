// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputMappingContext.h"
#include "SoulAbilitySystemComponent.h"
#include "SoulCharacterComponent.h"
#include "SoulInputComponent.h"
#include "SoulAbilitySet.h"
#include "SoulAttributeSet.h"
#include "SoulCombatComponent.h"
#include "PlayerCameraComponent.h"
#include "SoulCharacter.generated.h"

UCLASS()
class SOULSLIKE_API ASoulCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* InputMappingContext;


	//GAS Input
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	USoulAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere)
	USoulCharacterComponent* CharacterComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USoulCombatComponent* CombatComponent;

	UPROPERTY(EditDefaultsOnly)
	USoulAbilitySet* AbilitySet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPlayerCameraComponent* PlayerCameraComponent;

	FSoulAbilitySet_GrantedHandles GrantedHandles;

	UPROPERTY()
	USoulAttributeSet* SoulAttributeSet;

	USkeletalMeshComponent* CharacterMesh;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SavedWeapon;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> UseWeapon;

	AActor* savesword;
	AActor* usesword;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enemy", meta = (AllowPrivateAccess = "true"))
	AActor* Enemy;

	UPROPERTY()
	bool bIsDead = false;

public:
	// Sets default values for this character's properties
	ASoulCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UFUNCTION()
	void OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController);

	void GiveAbilitySet();

	UFUNCTION()
	void WeaponGeneration();

	UFUNCTION()
	void OnWeapon();

	UFUNCTION()
	void OffWeapon();

	UFUNCTION() 
	void OnGrabed();

	UFUNCTION()
	void OnReleased();

	UFUNCTION(BlueprintCallable)
	void FindEnemy(AActor * FindEnemy);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
	void ReturnDamageImpact();

	virtual void ReturnDamageImpact_Implementation();

	UFUNCTION()
	void CharacterDeath();
};
