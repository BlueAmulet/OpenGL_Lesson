/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2015-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Atom Config File
 * File     : SoundAtomConfig.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "SoundAtomConfig.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareRuntimePrivatePCH.h"

/* Unreal Engine 4関連ヘッダ */
#include "UObject/Package.h"
#include "Misc/Paths.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "SoundAtomConfig"

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
USoundAtomConfig::USoundAtomConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	/* Localization of unreal properties metadata with LOCTEXT markups and reflection */
	CRI_LOCCLASS(GetClass());
#endif

	ResourceData = NULL;
}

USoundAtomConfig* USoundAtomConfig::LoadAcfAsset(FString AcfFileName)
{
	/* パスの分解 */
	FString FileDir, FileName, FileExt;
	FPaths::Split(AcfFileName, FileDir, FileName, FileExt);

	/* 拡張子を含まないフルパスを作成 */
	FString PackageName;
	if (FPaths::IsRelative(AcfFileName)) {
		PackageName = "/Game/" + FileDir + "/" + FileName;
	} else {
		PackageName = FileDir + "/" + FileName;
	}

	/* 「/」の重複を回避 */
	/* 注意）「/」の重複があるとLoadObject内で例外が発生する。 */
	FPaths::RemoveDuplicateSlashes(PackageName);

	/* ACFアセットのロード */
	/* 備考）UE4.8以降は以下の処理だけでアセットがロードできる模様。 */
	USoundAtomConfig* AcfObject = LoadObject<USoundAtomConfig>(NULL, *PackageName);

	/* LoadObject失敗時（UE4.7以前）は別の方法でロードを試みる */
	if (AcfObject == NULL) {
		/* パッケージのロード */
		UPackage* Package = LoadPackage(NULL, *PackageName, 0);
		if (Package != NULL) {
			/* パッケージが完全にロード済みかどうかチェック */
			bool IsLoaded = Package->IsFullyLoaded();
			if (IsLoaded == false) {
				/* 完全にロードされていない場合はロード */
				Package->FullyLoad();
			}

			/* ACFアセットの取得 */
			AcfObject = FindObject<USoundAtomConfig>(Package, *FileName);
		}
	}

	/* エラーチェック */
	if (AcfObject == NULL) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("Could not load uasset %s."), *PackageName);
		return NULL;
	}

	/* ACFの登録 */
	AcfObject->RegisterAcf();

	return AcfObject;
}

void USoundAtomConfig::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	/* ACFデータのシリアライズ */
	RawData.Serialize(Ar, this);
}

void USoundAtomConfig::RegisterAcf()
{
	/* RawデータからACFを展開 */
	if (ResourceData == NULL) {
		if (RawData.GetBulkDataSize() > 0) {
			RawData.GetCopy((void**)&ResourceData, true);
		}
	}

	/* エラーチェック */
	if (ResourceData == NULL) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("Failed to copy acf data."));
		return;
	}

	/* オンメモリACFの登録 */
	criAtomEx_RegisterAcfData(
		(void *)ResourceData, (CriSint32)RawData.GetBulkDataSize(), NULL, 0);

	/* GCを回避 */
	AddToRoot();
}

void USoundAtomConfig::UnregisterAcf()
{
	/* ACFが登録済みかどうかチェック */
	if (ResourceData == NULL) {
		/* 未登録時は何もしない */
		return;
	}

	/* オンメモリACFの登録解除 */
	criAtomEx_UnregisterAcf();

	/* メモリの解放 */
	FMemory::Free(ResourceData);
	ResourceData = NULL;

	/* GCを許可 */
	RemoveFromRoot();
}

bool USoundAtomConfig::IsAcfRegistered()
{
	if (ResourceData == NULL) {
		return false;
	} else {
		return true;
	}
}

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/
#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
