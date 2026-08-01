// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayCueNotify_Poison.h"
#include "NativeGameplayTags.h"

UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "Input.Move");

AGameplayCueNotify_Poison::AGameplayCueNotify_Poison()
{

	GameplayCueTag = InputTag_Move; 
	/*위의 매크로를 통해서 태그 이름을 지정하고
	* 현재에서처럼 사용할수 있으나,
	* 지금처럼 코드에서 태그를 직접 사용하는건 추천하지 않는다.
	* 태그 같은 경우엔 수시로 변화가 잦기 때문에 코드상에서 하는 건 
	* 추천하지 않는 것이다. -> 위의 방법이 있음을 알기만 하자!
	*/
}



bool AGameplayCueNotify_Poison::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	bool result = Super::OnActive_Implementation(MyTarget, Parameters);
	
	
	return result;
}