// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class MetaChat : ModuleRules
{
	public MetaChat(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        bEnableExceptions = true; //예외 추가

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput", "OnlineSubsystemSteam", "OnlineSubsystem", "UMG", "HTTP", "Json", "JsonUtilities", "PixelStreaming", "PixelStreamingPlayer", "RenderCore", "RHI", "PixelStreamingServers", "WebSockets", "AudioCapture", "PixelStreamingEditor" }); //PixelStreamingEditor "UnrealEd","LevelEditor",  

        if (Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd", "LevelEditor", "SourceControlWindows", "UnsavedAssetsTracker", "ReplicationGraph" });
        }
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "PixelStreaming", "WebSocketNetworking",  "Sockets", "Networking", "OnlineSubsystem" }); //"UnrealEd", "LevelEditor"
    }
}
