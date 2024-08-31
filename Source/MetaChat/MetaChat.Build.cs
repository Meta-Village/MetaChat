﻿// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MetaChat : ModuleRules
{
	public MetaChat(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput", "OnlineSubsystemSteam", "OnlineSubsystem","UMG", "HTTP", "Json", "JsonUtilities", "PixelStreaming", "PixelStreamingPlayer" , "RenderCore", "RHI" });
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "PixelStreaming" });

    }
}
