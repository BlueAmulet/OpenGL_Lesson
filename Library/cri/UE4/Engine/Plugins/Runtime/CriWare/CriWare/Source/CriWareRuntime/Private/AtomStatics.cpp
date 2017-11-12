/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2014-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Atom Statics
 * File     : AtomStatics.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "AtomStatics.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareRuntimePrivatePCH.h"
#include "AtomParameter.h"

/* Unreal Engine 4関連ヘッダ */
#include "UObject/Package.h"
#include "GameFramework/WorldSettings.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "AtomStatics"

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/

/***************************************************************************
 *      データ型宣言
 *      Data Type Declarations
 ***************************************************************************/

/***************************************************************************
 *      変数宣言
 *      Prototype Variables
 ***************************************************************************/

/***************************************************************************
 *      クラス宣言
 *      Prototype Classes
 ***************************************************************************/

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/***************************************************************************
 *      変数定義
 *      Variable Definition
 ***************************************************************************/

/***************************************************************************
 *      クラス定義
 *      Class Definition
 ***************************************************************************/
UAtomStatics::UAtomStatics()
{
#if WITH_EDITOR
	/* Localization of unreal properties metadata with LOCTEXT markups and reflection */
	CRI_LOCCLASS(GetClass());
#endif
}

void UAtomStatics::PlaySoundAtLocation(UObject* WorldContextObject, class USoundAtomCue* Sound, FVector Location, float VolumeMultiplier, float PitchMultiplier, float StartTime, class USoundAttenuation* AttenuationSettings, class USoundConcurrency* ConcurrencySettings)
{
	SpawnSoundAtLocation(WorldContextObject, Sound, Location, FRotator::ZeroRotator, VolumeMultiplier, PitchMultiplier, StartTime, AttenuationSettings, ConcurrencySettings);
}

void UAtomStatics::PlaySoundAtLocation(UObject* WorldContextObject, class USoundAtomCue* Sound, FVector Location, FRotator Rotation, float VolumeMultiplier, float PitchMultiplier, float StartTime, class USoundAttenuation* AttenuationSettings, class USoundConcurrency* ConcurrencySettings)
{
	SpawnSoundAtLocation(WorldContextObject, Sound, Location, Rotation, VolumeMultiplier, PitchMultiplier, StartTime, AttenuationSettings, ConcurrencySettings);
}

UAtomComponent* UAtomStatics::SpawnSoundAtLocation(const UObject* WorldContextObject, class USoundAtomCue* Sound, FVector Location, FRotator Rotation, float VolumeMultiplier, float PitchMultiplier, float StartTime, class USoundAttenuation* AttenuationSettings, class USoundConcurrency* ConcurrencySettings)
{
	/* サウンド未指定時は何もしない */
	if (!Sound || !GEngine) {
		return nullptr;
	}

	/* 音声再生の可否をチェック */
	UWorld* ThisWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!ThisWorld || !ThisWorld->bAllowAudioPlayback || ThisWorld->GetNetMode() == NM_DedicatedServer) {
		return nullptr;
	}

	const bool bIsInGameWorld = ThisWorld->IsGameWorld();
	AWorldSettings *WorldSettings = ThisWorld->GetWorldSettings();

	/* 音声再生用にAtomComponentを確保 */
	UAtomComponent* AtomComponent;
	if (WorldSettings) {
		AtomComponent = NewObject<UAtomComponent>(WorldSettings);
	} else {
		AtomComponent = NewObject<UAtomComponent>();
	}
	if (AtomComponent != NULL) {
		AtomComponent->SetWorldLocationAndRotation(Location, Rotation);
		AtomComponent->DefaultVolume = VolumeMultiplier;
		AtomComponent->AttenuationSettings = AttenuationSettings;
		AtomComponent->bAutoDestroy = true;
		AtomComponent->bIsUISound = !bIsInGameWorld;
		AtomComponent->SetSound(Sound);
		AtomComponent->Play(StartTime);

		/* 再生終了検知のためRegisterComponentを実行 */
		/* 注意）シーク再生をリクエスト済みなので、Auto ActivateはOFFに設定。   */
		/* 　　　→Activate時の再生でシーク再生がキャンセルされるのを防止。     */
		AtomComponent->bAutoActivate = false;
		AtomComponent->RegisterComponent();
	}

	return AtomComponent;
}

class UAtomComponent* UAtomStatics::SpawnSoundAttached(class USoundAtomCue* Sound, class USceneComponent* AttachToComponent, FName AttachPointName, FVector Location,  FRotator Rotation, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroyed, float VolumeMultiplier, float PitchMultiplier, float StartTime, class USoundAttenuation* AttenuationSettings, class USoundConcurrency* ConcurrencySettings)
{
	/* サウンド未指定時は何もしない */
	if (Sound == NULL) {
		return NULL;
	}

	/* アタッチ先の有無をチェック */
	if (AttachToComponent == NULL) {
		UE_LOG(LogCriWareRuntime, Warning, TEXT("UAtomStatics::SpawnSoundAttached: NULL AttachComponent specified!"));
		return NULL;
	}

	/* 音声再生の可否をチェック */
	/* 備考）Personaアセットブラウザ等での再生をブロック。 */
	UWorld* ThisWorld = AttachToComponent->GetWorld();
	if (ThisWorld->bAllowAudioPlayback == false) {
		return NULL;
	}

	/* 音声再生用にAtomComponentを確保 */
	UAtomComponent* AtomComponent = NULL;
	if (AttachToComponent->GetOwner() == NULL) {
		AtomComponent = NewObject<UAtomComponent>();
	} else {
		AtomComponent = NewObject<UAtomComponent>(AttachToComponent->GetOwner());
	}

	if(AtomComponent) {
		AtomComponent->bAutoDestroy = true;
		AtomComponent->RegisterComponentWithWorld(AttachToComponent->GetWorld());
		const bool bIsInGameWorld = AtomComponent->GetWorld()->IsGameWorld();

		AtomComponent->AttachToComponent(AttachToComponent, FAttachmentTransformRules::KeepRelativeTransform, AttachPointName);
		if (LocationType == EAttachLocation::KeepWorldPosition) {
			AtomComponent->SetWorldLocationAndRotation(Location, Rotation);
		} else {
			AtomComponent->SetRelativeLocationAndRotation(Location, Rotation);
		}

		AtomComponent->DefaultVolume = VolumeMultiplier;
		AtomComponent->AttenuationSettings = AttenuationSettings;
		AtomComponent->bStopWhenOwnerDestroyed = bStopWhenAttachedToDestroyed;
		AtomComponent->bIsUISound = !bIsInGameWorld;
		AtomComponent->SetSound(Sound);
		AtomComponent->Play(StartTime);
	}

	return AtomComponent;
}

UAtomComponent* UAtomStatics::SpawnSoundAttached(class USoundAtomCue* Sound, class USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroyed, float VolumeMultiplier, float PitchMultiplier, float StartTime, class USoundAttenuation* AttenuationSettings, class USoundConcurrency* ConcurrencySettings)
{
	return SpawnSoundAttached(Sound, AttachToComponent, AttachPointName, Location,  FRotator::ZeroRotator, LocationType, bStopWhenAttachedToDestroyed, VolumeMultiplier, PitchMultiplier, StartTime, AttenuationSettings, ConcurrencySettings);
}

UAtomComponent* UAtomStatics::PlaySoundAttached(USoundAtomCue* Sound, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroyed, float VolumeMultiplier, float PitchMultiplier, float StartTime, class USoundAttenuation* AttenuationSettings)
{
	return SpawnSoundAttached(Sound, AttachToComponent, AttachPointName, Location,  FRotator::ZeroRotator, LocationType, bStopWhenAttachedToDestroyed, VolumeMultiplier, PitchMultiplier, StartTime, AttenuationSettings, nullptr);
}

void UAtomStatics::PauseAudioOutput(bool bPause)
{
	if (bPause != false) {
		/* ポーズ */
		criAtomExAsr_PauseOutputVoice(CRI_TRUE);
		criAtomEx_PauseTimer(CRI_TRUE);

		/* レベル遷移時やプレビュー終了時にポーズ解除するアクタを作成 */
		AAtomParameter* ExistingActor = (AAtomParameter*)StaticFindObject(
			AAtomParameter::StaticClass(), ANY_PACKAGE, TEXT("UAtomStatics_PauseAudioOutput"));
		if (ExistingActor == NULL) {
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Name = *FString::Printf(TEXT("UAtomStatics_PauseAudioOutput"));
			AAtomParameter* NewActor = GWorld->SpawnActor<AAtomParameter>(SpawnInfo);
			NewActor->SetEndPlayCallback(UAtomStatics::ResumeAudioOutput, nullptr);
		}
	} else {
		/* ポーズ解除 */
		UAtomStatics::ResumeAudioOutput(nullptr);
	}
}

void UAtomStatics::ResumeAudioOutput(void* Object)
{
	/* ポーズ解除 */
	criAtomExAsr_PauseOutputVoice(CRI_FALSE);
	criAtomEx_PauseTimer(CRI_FALSE);
}

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/
#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
