// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#pragma once
#include "Sound/SoundNodeAssetReferencer.h"
#include "SoundNodeModPlayer.generated.h"

/** 
 * Sound node that contains a reference to the mod file to be played
 */
UCLASS(hidecategories=Object, editinlinenew, MinimalAPI, meta=( DisplayName="Mod Player" ))
class USoundNodeModPlayer : public USoundNodeAssetReferencer
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category=ModPlayer, meta=(DisplayName="Sound Mod"))
	TAssetPtr<USoundMod> SoundModAssetPtr;

	UPROPERTY(transient)
	USoundMod* SoundMod;

	UPROPERTY(EditAnywhere, Category=ModPlayer)
	uint32 bLooping:1;

	void OnSoundModLoaded(const FName& PackageName, UPackage * Package, EAsyncLoadingResult::Type Result);

public:	

	USoundMod* GetSoundMod() const { return SoundMod; }
	void SetSoundMod(USoundMod* SoundMod);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	// End UObject Interface

	// Begin USoundNode Interface
	virtual int32 GetMaxChildNodes() const override;
	virtual float GetDuration() override;
	virtual void ParseNodes(FAudioDevice* AudioDevice, const UPTRINT NodeWaveInstanceHash, FActiveSound& ActiveSound, const FSoundParseParameters& ParseParams, TArray<FWaveInstance*>& WaveInstances) override;
#if WITH_EDITOR
	virtual FText GetTitle() const override;
#endif
	// End USoundNode Interface

	// Begin USoundNodeAssetReferencer Interface
	virtual void LoadAsset() override;
	// End USoundNode Interface

};

