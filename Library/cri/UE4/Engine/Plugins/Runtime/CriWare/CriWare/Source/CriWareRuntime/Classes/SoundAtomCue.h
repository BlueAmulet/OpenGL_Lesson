/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2013-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Atom Sound Data (Cue)
 * File     : SoundAtomCue.h
 *
 ****************************************************************************/

/* 多重定義防止 */
#pragma once

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* Unreal Engine 4関連ヘッダ */
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ScriptMacros.h"

/* CRIWAREプラグインヘッダ */
#include "SoundAtomCueSheet.h"

/* モジュールヘッダ */
#include "SoundAtomCue.generated.h"

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

/***************************************************************************
 *      変数宣言
 *      Prototype Variables
 ***************************************************************************/

/***************************************************************************
 *      クラス宣言
 *      Prototype Classes
 ***************************************************************************/
UCLASS(hidecategories=Object, editinlinenew, BlueprintType, meta = (ToolTip = "SoundAtomCue class."))
class CRIWARERUNTIME_API USoundAtomCue : public UObject
{
	GENERATED_BODY()

public:
	USoundAtomCue(const FObjectInitializer& ObjectInitializer);

	/** (DEPRECATED) CueSheet name is not available. */
	FString CueSheetName;

	/* キューシートオブジェクト */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reference", meta = (ToolTip = "Cue sheet."))
	USoundAtomCueSheet* CueSheet;

	/* キュー名 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Target", meta = (ToolTip = "Cue name."))
	FString CueName;

	UFUNCTION(BlueprintCallable, Category="Sound Atom Cue", meta = (ToolTip = "Get the length of the cue sheet."))
	int32 GetLength();

	// Begin UObject interface.
	virtual void Serialize(FArchive& Ar) override;
	// End UObject interface.

#if WITH_EDITOR
	/* エディタ上に描画するため、最大、最小減衰距離を返す */
	float GetMaxAttenuationDistance();
	float GetMinAttenuationDistance();
#endif

private:
#if WITH_EDITOR
	/* エディタ上に描画するため、最大、最小減衰距離を保持しておく */

	/* CueInfoを読みこんで、最大、最小減衰距離を保持する。
	 * CueInfo読み込みに成功: true
	 * CueInfo読み込みに失敗: false
	 */
	bool ReadCueInfo();
	float MinAttenuationDistance;
	float MaxAttenuationDistance;
#endif

private:
	/* 旧データの読み込み */
	void SerializeOldVersion(FArchive& Ar);
};

/* AtomCueアセットのバージョン管理用 */
struct FAtomCueVersion
{
	/* バージョン番号 */
	static const int32 AssetVersion;

	/* GUID */
	static const FGuid AssetGuid;
};

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/* --- end of file --- */
