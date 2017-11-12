/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2013-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Atom Sound Data (Cue)
 * File     : SoundAtomCue.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "SoundAtomCue.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareRuntimePrivatePCH.h"

/* Unreal Engine 4関連ヘッダ */
#include "Serialization/CustomVersion.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "SoundAtomCue"

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
/* バージョン番号 */
namespace SoundAtomCue
{
	enum Type
	{
		/* バージョン番号が付与される前 */
		BeforeCustomVersionWasAdded = 0,

		/* バージョン情報を追加 */
		StoreVersionInfo,

		/* キュー名をアセットに保持 */
		StoreCueName,

		/* 最新バージョンの番号を取得するための番兵 */
		VersionPlusOne,

		/* 最新バージョンの番号を指す */
		LatestVersion = VersionPlusOne - 1,
	};
}

/* バージョン番号 */
const int32 FAtomCueVersion::AssetVersion = SoundAtomCue::LatestVersion;

/* GUID */
const FGuid FAtomCueVersion::AssetGuid(0x0FAD527A, 0xEEC1492E, 0x8819E1BB, 0xE3B780B9);

/* バージョン登録 */
FCustomVersionRegistration GAtomCueVersion(
	FAtomCueVersion::AssetGuid, FAtomCueVersion::AssetVersion, TEXT("AtomCueAssetVersion"));

/***************************************************************************
 *      定数定義
 *      Constant Definition
 ***************************************************************************/
namespace {
	float ATTENUATION_DISTANCE_NOT_INITIALIZED = -1.0f;
}

/***************************************************************************
 *      クラス定義
 *      Class Definition
 ***************************************************************************/
USoundAtomCue::USoundAtomCue(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	/* Localization of unreal properties metadata with LOCTEXT markups and reflection */
	CRI_LOCCLASS(GetClass());
#endif

	CueSheet = nullptr;

#if WITH_EDITOR
	MaxAttenuationDistance = ATTENUATION_DISTANCE_NOT_INITIALIZED;
	MinAttenuationDistance = ATTENUATION_DISTANCE_NOT_INITIALIZED;
#endif
}

int32 USoundAtomCue::GetLength()
{
	if (CueSheet == NULL) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("CueSheet is not set."));
		return -1;
	}

	int64 Length = -1;

	/* キューシートからキューの長さを取得 */
	CueSheet->AddRef();
	{
		Length = criAtomExAcb_GetLengthByName(CueSheet->GetAcbHn(), TCHAR_TO_UTF8(*CueName));
	}
	CueSheet->Release();

	/* Blueprintで扱えるのは32bit値まで */
	if (Length > MAX_int32) {
		UE_LOG(LogCriWareRuntime, Warning, TEXT("Length of Cue is longer than MAX_int32."));
		Length = MAX_int32;
	}

	return (int32)Length;
}

void USoundAtomCue::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	/* アセットバージョンの使用を通知 */
	Ar.UsingCustomVersion(FAtomCueVersion::AssetGuid);

	/* バージョンチェック */
	if (Ar.IsLoading()) {
		/* アーカイブのバージョンをチェック */
		int32 archiveVer = Ar.CustomVer(FAtomCueVersion::AssetGuid);

		/* 初期バージョン専用の処理 */
		if (archiveVer < SoundAtomCue::StoreVersionInfo) {
			SerializeOldVersion(Ar);
			return;
		}

		/* キューシート名がない場合は補完 */
		if (archiveVer < SoundAtomCue::StoreCueName) {
			CueName = GetFName().ToString();
		}
	}
}

/* 旧バージョンデータの読み込み */
void USoundAtomCue::SerializeOldVersion(FArchive& Ar)
{
	UE_LOG(LogCriWareRuntime, Warning, TEXT("Atom Cue asset is not compatible. (%s)"), *GetFName().ToString());

	/* キューシート名を保存 */
	/* 備考）2014/03以前のデータを扱うための処理。 */
	Ar << CueSheetName;
}

#if WITH_EDITOR
float USoundAtomCue::GetMaxAttenuationDistance()
{
	if (MaxAttenuationDistance == ATTENUATION_DISTANCE_NOT_INITIALIZED) {
		if (ReadCueInfo() == false) {
			return 0.0f;
		}
	}
	return MaxAttenuationDistance;
}

float USoundAtomCue::GetMinAttenuationDistance()
{
	if (MinAttenuationDistance == ATTENUATION_DISTANCE_NOT_INITIALIZED) {
		if (ReadCueInfo() == false) {
			return 0.0f;
		}
	}
	return MinAttenuationDistance;
}

bool USoundAtomCue::ReadCueInfo()
{
	if (CueSheet == NULL) {
		return false;
	}

	CriAtomExCueInfo cueInfo = {0};

	CriBool isGetCueInfoSucceed = CRI_FALSE;
	CueSheet->AddRef();
	{
		if (criAtomExAcb_ExistsName(static_cast<CriAtomExAcbHn>(CueSheet->GetAcbHn()), TCHAR_TO_UTF8(*(CueName))) == CRI_TRUE) {
			isGetCueInfoSucceed = criAtomExAcb_GetCueInfoByName(static_cast<CriAtomExAcbHn>(CueSheet->GetAcbHn()), TCHAR_TO_UTF8(*(CueName)), &cueInfo);
		}
	}
	CueSheet->Release();

	if (isGetCueInfoSucceed == CRI_FALSE) {
		return false;
	}

	MaxAttenuationDistance = cueInfo.pos3d_info.max_distance;
	MinAttenuationDistance = cueInfo.pos3d_info.min_distance;

	return true;
}
#endif // WITH_EDITOR

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/
#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
