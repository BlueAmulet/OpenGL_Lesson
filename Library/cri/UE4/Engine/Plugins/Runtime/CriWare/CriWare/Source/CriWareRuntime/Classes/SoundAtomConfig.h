/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2015-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Atom Config File
 * File     : SoundAtomConfig.h
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
#include "Serialization/BulkData.h"

/* モジュールヘッダ */
#include "SoundAtomConfig.generated.h"

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
UCLASS(hidecategories=object, meta = (ToolTip = "SoundAtomConfig class."))
class CRIWARERUNTIME_API USoundAtomConfig : public UObject
{
	GENERATED_BODY()

public:
	USoundAtomConfig(const FObjectInitializer& ObjectInitializer);

	/* ACFファイルを配置したフォルダのパス */
	UPROPERTY(EditAnywhere, Category="FilePath", meta = (DisplayName = "ACF File Path", ToolTip = "ACF file path."))
	FString AcfFilePath;

	/* ACFデータ */
	FByteBulkData RawData;

	/* ACFアセットのロード */
	static USoundAtomConfig* LoadAcfAsset(FString AssetPath);

	// Begin UObject interface.
	virtual void Serialize(FArchive& Ar) override;
	// End UObject interface.

	/* ACFの登録 */
	void RegisterAcf();

	/* ACFの登録解除 */
	void UnregisterAcf();

	/* ACF登録済みかどうか */
	bool IsAcfRegistered();

private:
	/* RawData展開先バッファ */
	uint8* ResourceData;
};

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/* --- end of file --- */
