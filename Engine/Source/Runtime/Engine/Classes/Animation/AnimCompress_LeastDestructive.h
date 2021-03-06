// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

/**
 * Reverts any animation compression, restoring the animation to the raw data.
 *
 */
#include "Animation/AnimCompress.h"
#include "AnimCompress_LeastDestructive.generated.h"

UCLASS()
class UAnimCompress_LeastDestructive : public UAnimCompress
{
	GENERATED_UCLASS_BODY()


protected:
	// Begin UAnimCompress Interface
	virtual void DoReduction(class UAnimSequence* AnimSeq, const TArray<class FBoneData>& BoneData) override;
	// Begin UAnimCompress Interface
};



