﻿// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMetaChat, Log, All);

#define LOCALROLE (UEnum::GetValueAsString<ENetRole>(GetLocalRole()))
#define REMOTEROLE (UEnum::GetValueAsString<ENetRole>(GetRemoteRole()))
#define REGISTERURL "http://125.132.216.190:8126/signup"
#define LOGIN "http://125.132.216.190:8126/login"