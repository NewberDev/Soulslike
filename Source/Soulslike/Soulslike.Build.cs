// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Soulslike : ModuleRules
{
	public Soulslike(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // 💡 [추가] 모듈 내부의 하위 폴더(GAS 등)와 루트 폴더 간의 헤더 참조를 정상화합니다.
        PublicIncludePaths.AddRange(new string[] {
            "Soulslike"
        });

        PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"NavigationSystem", //navigation모듈
			"AIModule", //navigation작동을 위해 추가 모듈 -> #include "Blueprint/AIBlueprintHelperLibrary.h"
			"UMG", //Ui를 추가하기 위한 모듈
			"Niagara", //파티클(나이아가라) 추가 모듈

			//게임플레이 어빌리티 시스템을 위한 3가지 모듈------------
			"GameplayAbilities", //기본 게임플레이 어빌리티 시스템 모듈
			"GameplayTags", //효율적인 태그 관리를 위한 모듈
			"GameplayTasks" //테스크를 위한 모듈
			//-----------------
 		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
