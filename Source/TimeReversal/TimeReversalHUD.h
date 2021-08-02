// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TimeReversalHUD.generated.h"

UCLASS()
class ATimeReversalHUD : public AHUD
{
	GENERATED_BODY()

public:
	ATimeReversalHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

