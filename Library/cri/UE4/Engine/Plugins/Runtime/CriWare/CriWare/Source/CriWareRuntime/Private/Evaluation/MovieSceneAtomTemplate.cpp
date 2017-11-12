/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Movie Scene Atom Template
 * File     : MovieSceneAtomTemplate.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "MovieSceneAtomTemplate.h"

/* Unreal Engine 4関連ヘッダ */
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "Sections/MovieSceneAtomSection.h"
#include "Tracks/MovieSceneAtomTrack.h"
#include "MovieScene.h"
#include "MovieSceneEvaluation.h"
#include "IMovieScenePlayer.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/

/***************************************************************************
 *      データ型宣言
 *      Data Type Declarations
 ***************************************************************************/
DECLARE_CYCLE_STAT(TEXT("Atom Track Evaluate"), MovieSceneEval_AtomTrack_Evaluate, STATGROUP_MovieSceneEval);
DECLARE_CYCLE_STAT(TEXT("Atom Track Token Execute"), MovieSceneEval_AtomTrack_TokenExecute, STATGROUP_MovieSceneEval);

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
/** Stop audio from playing */
struct FStopAtomPreAnimatedToken : IMovieScenePreAnimatedToken
{
	static FMovieSceneAnimTypeID GetAnimTypeID()
	{
		return TMovieSceneAnimTypeID<FStopAtomPreAnimatedToken>();
	}

	virtual void RestoreState(UObject& InObject, IMovieScenePlayer& Player) override
	{
		UAtomComponent* AtomComponent = CastChecked<UAtomComponent>(&InObject);
		AtomComponent->Stop();
		AtomComponent->DestroyComponent();
	}

	struct FProducer : IMovieScenePreAnimatedTokenProducer
	{
		virtual IMovieScenePreAnimatedTokenPtr CacheExistingState(UObject& Object) const override
		{
			return FStopAtomPreAnimatedToken();
		}
	};
};

/** Destroy a transient audio component */
struct FDestroyAtomPreAnimatedToken : IMovieScenePreAnimatedToken
{
	static FMovieSceneAnimTypeID GetAnimTypeID()
	{
		return TMovieSceneAnimTypeID<FDestroyAtomPreAnimatedToken>();
	}

	virtual void RestoreState(UObject& InObject, IMovieScenePlayer& Player) override
	{
		UAtomComponent* AtomComponent = CastChecked<UAtomComponent>(&InObject);
		AtomComponent->DestroyComponent();
	}

	struct FProducer : IMovieScenePreAnimatedTokenProducer
	{
		virtual IMovieScenePreAnimatedTokenPtr CacheExistingState(UObject& Object) const override
		{
			return FDestroyAtomPreAnimatedToken();
		}
	};
};

struct FCachedAtomTrackData : IPersistentEvaluationData
{
	TArray<TMap<FObjectKey, TWeakObjectPtr<UAtomComponent>>> AtomComponentsByRow;

	UAtomComponent* GetAtomComponentForRow(int32 RowIndex, FObjectKey Key)
	{
		if (AtomComponentsByRow.Num() > RowIndex) {
			return AtomComponentsByRow[RowIndex].FindRef(Key).Get();
		}

		return nullptr;
	}

	/** Only to be called on the game thread */
	UAtomComponent* AddAtomComponentForRow(int32 RowIndex, AActor& PrincipalActor, IMovieScenePlayer& Player)
	{
		const int32 NumExtra = RowIndex + 1 - AtomComponentsByRow.Num();
		if (NumExtra > 0) {
			AtomComponentsByRow.AddDefaulted(NumExtra);
		}

		FObjectKey ActorKey(&PrincipalActor);
		UAtomComponent* ExistingComponent = AtomComponentsByRow[RowIndex].FindRef(ActorKey).Get();

		if (!ExistingComponent) {
			USoundAtomCue* TempPlaybackAtomCue = NewObject<USoundAtomCue>();
			ExistingComponent = NewObject<UAtomComponent>(PrincipalActor.GetWorld());

			if (!ExistingComponent) {
				FString ActorName =
#if WITH_EDITOR
					PrincipalActor.GetActorLabel();
#else
					PrincipalActor.GetName();
#endif
				UE_LOG(LogMovieScene, Warning, TEXT("Failed to create audio component for spatialized audio track (row %d on %s)."), RowIndex, *ActorName);
				return nullptr;
			}

			Player.SavePreAnimatedState(*ExistingComponent, FMovieSceneAnimTypeID::Unique(), FDestroyAtomPreAnimatedToken::FProducer());

			AtomComponentsByRow[RowIndex].Add(ActorKey, ExistingComponent);

			ExistingComponent->SetFlags(RF_Transient);
			ExistingComponent->AttachToComponent(PrincipalActor.GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		}

		return ExistingComponent;
	}

	/** Only to be called on the game thread */
	UAtomComponent* AddMasterAtomComponentForRow(int32 RowIndex, UWorld* World, IMovieScenePlayer& Player)
	{
		const int32 NumExtra = RowIndex + 1 - AtomComponentsByRow.Num();
		if (NumExtra > 0) {
			AtomComponentsByRow.AddDefaulted(NumExtra);
		}

		UAtomComponent* ExistingComponent = AtomComponentsByRow[RowIndex].FindRef(FObjectKey()).Get();

		if (!ExistingComponent) {
			USoundAtomCue* TempPlaybackAtomCue = NewObject<USoundAtomCue>();
			ExistingComponent = NewObject<UAtomComponent>(World);

			if (!ExistingComponent) {
				UE_LOG(LogMovieScene, Warning, TEXT("Failed to create audio component for master audio track (row %d)."), RowIndex);
				return nullptr;
			}

			Player.SavePreAnimatedState(*ExistingComponent, FMovieSceneAnimTypeID::Unique(), FDestroyAtomPreAnimatedToken::FProducer());

			ExistingComponent->SetFlags(RF_Transient);
			AtomComponentsByRow[RowIndex].Add(FObjectKey(), ExistingComponent);
		}

		return ExistingComponent;
	}

	void StopAllSounds()
	{
		for (TMap<FObjectKey, TWeakObjectPtr<UAtomComponent>>& Map : AtomComponentsByRow) {
			for (auto& Pair : Map) {
				if (UAtomComponent* AtomComponent = Pair.Value.Get()) {
					AtomComponent->Stop();
				}
			}
		}
	}
};

struct FAtomSectionExecutionToken : IMovieSceneExecutionToken
{
	FAtomSectionExecutionToken(const FMovieSceneAtomSectionTemplateData& InAtomData)
		: AtomData(InAtomData)
	{}

	virtual void Execute(const FMovieSceneContext& Context, const FMovieSceneEvaluationOperand& Operand, FPersistentEvaluationData& PersistentData, IMovieScenePlayer& Player)
	{
		FCachedAtomTrackData& TrackData = PersistentData.GetOrAddTrackData<FCachedAtomTrackData>();

		if (Context.GetStatus() != EMovieScenePlayerStatus::Playing && Context.GetStatus() != EMovieScenePlayerStatus::Scrubbing) {
			// stopped, recording, etc
			TrackData.StopAllSounds();
		} else if (!Operand.ObjectBindingID.IsValid()) {
			// Master audio track
			UObject* PlaybackContext = Player.GetPlaybackContext();

			UAtomComponent* AtomComponent = TrackData.GetAtomComponentForRow(AtomData.RowIndex, FObjectKey());
			if (!AtomComponent) {
				// Initialize the sound
				AtomComponent = TrackData.AddMasterAtomComponentForRow(AtomData.RowIndex, PlaybackContext ? PlaybackContext->GetWorld() : nullptr, Player);
			}

			if (AtomComponent) {
				AtomData.EnsureAtomIsPlaying(*AtomComponent, PersistentData, Context, false, Player);
			}
		} else {
			// Object binding audio track
			for (TWeakObjectPtr<> Object : Player.FindBoundObjects(Operand)) {
				AActor* Actor = Cast<AActor>(Object.Get());
				if (!Actor) {
					continue;
				}

				UAtomComponent* AtomComponent = TrackData.GetAtomComponentForRow(AtomData.RowIndex, Actor);
				if (!AtomComponent) {
					// Initialize the sound
					AtomComponent = TrackData.AddAtomComponentForRow(AtomData.RowIndex, *Actor, Player);
				}

				if (AtomComponent) {
					AtomData.EnsureAtomIsPlaying(*AtomComponent, PersistentData, Context, true, Player);
				}
			}
		}
	}

	FMovieSceneAtomSectionTemplateData AtomData;
};

FMovieSceneAtomSectionTemplateData::FMovieSceneAtomSectionTemplateData(const UMovieSceneAtomSection& Section)
	: Sound(Section.GetSound())
	, AtomStartOffset(Section.GetStartOffset())
	, AtomRange(Section.GetAtomCueRange())
	, AtomPitchMultiplierCurve(Section.GetPitchMultiplierCurve())
	, AtomVolumeCurve(Section.GetSoundVolumeCurve())
	, RowIndex(Section.GetRowIndex())
{
}

void FMovieSceneAtomSectionTemplateData::EnsureAtomIsPlaying(UAtomComponent& AtomComponent, FPersistentEvaluationData& PersistentData, const FMovieSceneContext& Context, bool bAllowSpatialization, IMovieScenePlayer& Player) const
{
	Player.SavePreAnimatedState(AtomComponent, FStopAtomPreAnimatedToken::GetAnimTypeID(), FStopAtomPreAnimatedToken::FProducer());

	bool bIsPlaying = AtomComponent.IsPlaying();
	bool bIsSameSound = (AtomComponent.Sound == Sound);
	bool bPlaySound = !bIsPlaying || !bIsSameSound;
	float AtomVolume = AtomVolumeCurve.Eval(Context.GetTime());
	float PitchMultiplier = AtomPitchMultiplierCurve.Eval(Context.GetTime());

	AtomComponent.SetVolume(AtomVolume);
	AtomComponent.SetPitchMultiplier(PitchMultiplier);

	if (bPlaySound != false) {
		AtomComponent.bOverrideAttenuation = bAllowSpatialization;
		AtomComponent.Stop();
		AtomComponent.SetSound(Sound);
		AtomComponent.bIsUISound = true;
		AtomComponent.Play((Context.GetTime() - AtomRange.GetLowerBoundValue()) + FMath::Max(AtomStartOffset, 0.f));

		if (Context.GetStatus() == EMovieScenePlayerStatus::Scrubbing) {
			// Fade out the sound at the same volume in order to simply
			// set a short duration on the sound, far from ideal soln
			// AtomComponent.FadeOut(AtomTrackConstants::ScrubDuration, 1.f);
			AtomComponent.Stop();
		}
	}
}

FMovieSceneAtomSectionTemplate::FMovieSceneAtomSectionTemplate(const UMovieSceneAtomSection& Section)
	: AtomData(Section)
{
}

void FMovieSceneAtomSectionTemplate::Evaluate(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const
{
	MOVIESCENE_DETAILED_SCOPE_CYCLE_COUNTER(MovieSceneEval_AtomTrack_Evaluate)

	if (GEngine && GEngine->UseSound() && !Context.HasJumped() && Context.GetStatus() != EMovieScenePlayerStatus::Jumping && AtomData.AtomRange.Contains(Context.GetTime())) {
		ExecutionTokens.Add(FAtomSectionExecutionToken(AtomData));
	}
}

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/

/* --- end of file --- */
