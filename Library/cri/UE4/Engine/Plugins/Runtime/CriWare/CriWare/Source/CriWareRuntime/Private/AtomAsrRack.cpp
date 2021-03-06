/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2016-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Atom ASR Rack
 * File     : AtomAsrRack.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "AtomAsrRack.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareRuntimePrivatePCH.h"

/* Unreal Engine 4関連ヘッダ */
#include "UObject/Package.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "AtomAsrRack"

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
UAtomAsrRack::UAtomAsrRack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	/* Localization of unreal properties metadata with LOCTEXT markups and reflection */
	CRI_LOCCLASS(GetClass());
#endif

	RackId = CRIATOMEXASR_RACK_ILLEGAL_ID;
}

/* デフォルトASRラックの取得 */
UAtomAsrRack* UAtomAsrRack::GetDefaultAsrRack()
{
	return UAtomAsrRack::GetAsrRack(0);
}

/* 指定したIDのASRラックを取得 */
UAtomAsrRack* UAtomAsrRack::GetAsrRack(int32 AsrRackId)
{
	/* オブジェクト名の作成 */
	FString ObjectName = TEXT("AtomAsrRack_");
	ObjectName.AppendInt(AsrRackId);

	/* 既存オブジェクトの検索 */
	UAtomAsrRack* AsrRack = (UAtomAsrRack*)StaticFindObject(
		UAtomAsrRack::StaticClass(), ANY_PACKAGE, *ObjectName);
	if (AsrRack == NULL) {
		/* ACBアセットが存在しない場合はACBオブジェクトを作成 */
		AsrRack = NewObject<UAtomAsrRack>(GetTransientPackage(), *ObjectName);
	}

	/* エラーチェック */
	if (AsrRack == nullptr) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("Failed to create instance of AtomAsrRack."));
		return nullptr;
	}

	/* ASRラックIDの取得 */
	AsrRack->RackId = AsrRackId;

	return AsrRack;
}

void UAtomAsrRack::AttachDspBusSetting(FString SettingName)
{
	criAtomExAsrRack_AttachDspBusSetting((CriAtomExAsrRackId)RackId, TCHAR_TO_UTF8(*SettingName), NULL, 0);
}

void UAtomAsrRack::DetachDspBusSetting()
{
	criAtomExAsrRack_DetachDspBusSetting((CriAtomExAsrRackId)RackId);
}

void UAtomAsrRack::ApplyDspBusSnapshot(FString SnapshotName, int32 Milliseconds)
{
	criAtomExAsrRack_ApplyDspBusSnapshot(
		(CriAtomExAsrRackId)RackId, TCHAR_TO_UTF8(*SnapshotName), Milliseconds);
}

void UAtomAsrRack::SetBusVolumeByName(FString BusName, float Volume)
{
	criAtomExAsrRack_SetBusVolumeByName(
		(CriAtomExAsrRackId)RackId, TCHAR_TO_UTF8(*BusName), Volume);
}

void UAtomAsrRack::SetBusSendLevelByName(FString SourceBusName, FString DestBusName, float Level)
{
	criAtomExAsrRack_SetBusSendLevelByName(
		(CriAtomExAsrRackId)RackId, TCHAR_TO_UTF8(*SourceBusName),
		TCHAR_TO_UTF8(*DestBusName), Level);
}

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/

#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
