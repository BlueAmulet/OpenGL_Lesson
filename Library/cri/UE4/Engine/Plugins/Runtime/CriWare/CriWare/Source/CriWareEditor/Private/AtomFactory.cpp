/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2013-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Factory of Atom Data
 * File     : AtomFactory.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "AtomFactory.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareEditorPrivatePCH.h"
#include "CriWareInitializer.h"
#include "SoundAtomCueSheet.h"
#include "SoundAtomCue.h"
#include "AtomComponent.h"
#include "AssetTypeActions_SoundAtomCueSheet.h"
#include "AssetTypeActions_SoundAtomCue.h"
#include "AtomComponentBroker.h"

/* Unreal Engine 4関連ヘッダ */
#include "PackageTools.h"
#include "MessageDialog.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "AtomFactory"

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
bool UAtomFactory::bIsActionRegistered = false;

/***************************************************************************
 *      クラス定義
 *      Class Definition
 ***************************************************************************/
UAtomFactory::UAtomFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/* Content BrowserでACBファイルをImportできるようにする */
	Formats.Add(TEXT("acb;Atom CueSheet File"));

	/* バイナリとして読み込む */
	bText = false;
	bCreateNew = false;
	bEditorImport = true;

	SupportedClass = USoundAtomCueSheet::StaticClass();
}

void UAtomFactory::PostInitProperties()
{
	Super::PostInitProperties();

	/* アクションの登録 */
	if (UAtomFactory::bIsActionRegistered == false) {
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		AssetToolsModule.Get().RegisterAssetTypeActions(MakeShareable(new FAssetTypeActions_SoundAtomCueSheet));
		AssetToolsModule.Get().RegisterAssetTypeActions(MakeShareable(new FAssetTypeActions_SoundAtomCue));
		FComponentAssetBrokerage::RegisterBroker(MakeShareable(new FAtomComponentBroker), UAtomComponent::StaticClass(), true, true);
		UAtomFactory::bIsActionRegistered = true;
	}
}

void UAtomFactory::BeginDestroy()
{
	Super::BeginDestroy();
}

UObject* UAtomFactory::FactoryCreateBinary
(
	UClass*				Class,
	UObject*			InParent,
	FName				Name,
	EObjectFlags		Flags,
	UObject*			Context,
	const TCHAR*		FileType,
	const uint8*&		Buffer,
	const uint8*		BufferEnd,
	FFeedbackContext*	Warn
)
{
	/* ACBオブジェクトの作成 */
	USoundAtomCueSheet* AcbObject = NewObject<USoundAtomCueSheet>(InParent, Name, Flags);

	/* データを別のメモリ領域にコピー */
	/* 備考）メモリ領域がconstなのでそのままではロードできないため。 */
	FByteBulkData RawData;
	RawData.Lock(LOCK_READ_WRITE);
	void* LockedData = RawData.Realloc(BufferEnd - Buffer);
	FMemory::Memcpy(LockedData, Buffer, BufferEnd - Buffer);
	RawData.Unlock();

	/* ACBデータをuasset内に取り込む */
	AcbObject->RawData.Lock(LOCK_READ_WRITE);
	LockedData = AcbObject->RawData.Realloc(BufferEnd - Buffer);
	FMemory::Memcpy(LockedData, Buffer, BufferEnd - Buffer);
	AcbObject->RawData.Unlock();

	/* === ACB内のキューの列挙 === */

	/* ACBを一旦ロード */
	/* 【暫定】以下の処理は情報取得専用のAPIに置き換える予定。 */
	int DataSize = RawData.GetBulkDataSize();
	void* Data = NULL;
	RawData.GetCopy(&Data, true);
	CriAtomExAcbHn AcbHn = criAtomExAcb_LoadAcbData(Data, DataSize, NULL, NULL, NULL, 0);

	/* データ保存先のパスを取得 */
	FString PackagePath = FPackageName::GetLongPackagePath(InParent->GetOutermost()->GetName());

	/* キューの数を取得 */
	CriSint32 num_cues = criAtomExAcb_GetNumCues(AcbHn);

	/* ACBファイルの情報を列挙 */
	for (int i = 0; i < num_cues; i++) {
		/* キュー名の取得 */
		const CriChar8 *cue_name = criAtomExAcb_GetCueNameByIndex(AcbHn, i);

		/* 「/」を含むキューを除外 */
		/* 備考）インゲームプレビュー用バイナリを使用すると、       */
		/* 　　　サブシンセが「/」を含むキュー名が出力されるため。  */
		int j;
		size_t len = strlen(cue_name);
		for (j = 0; j < len; j++) {
			if (cue_name[j] == '/') {
				break;
			}
		}
		if (j != len) {
			continue;
		}

		/* アセット名、パッケージ名の生成 */
		FString AssetName = UTF8_TO_TCHAR(cue_name);
		FString PackageName = PackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);
		UPackage* Pkg = CreatePackage(NULL, *PackageName);

		/* 備考）検索前にPackageTools::HandleFullyLoadingPackages関数で強制的にロードする。 */
		{
			TArray<UPackage*> TopLevelPackages;
			TopLevelPackages.Add(Pkg);
			PackageTools::HandleFullyLoadingPackages(TopLevelPackages, LOCTEXT("CreateANewObject", "Create a new object"));
		}

		/* 既存アセットの取得 */
		USoundAtomCue* ExistingAsset = (USoundAtomCue*)StaticFindObject(USoundAtomCue::StaticClass(), Pkg, *AssetName);
		if (ExistingAsset == NULL) {
			/* 同名のアセットが存在しないかチェック */
			UObject* SameNameAsset = StaticFindObject(nullptr, InParent, *AssetName);
			if (SameNameAsset != nullptr) {
				/* 同名のアセットを作成することはできない */
				FMessageDialog::Open(EAppMsgType::Ok,
					NSLOCTEXT("UnrealEd", "Error_AssetExist", "Asset with same name exists. Can't overwrite another asset"));
			} else {
				/* アセットが存在しない場合は新規に作成 */
				/* 注意）ここではFactoryは指定してはいけない！！ */
				/* 　　　→USoundAtomCue破棄時にFactoryが一緒に削除されてしまうため。 */
				FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
				ExistingAsset = (USoundAtomCue*)AssetToolsModule.Get().CreateAsset(
					AssetName, PackagePath, USoundAtomCue::StaticClass(), NULL);
			}
		}

		/* アセットの作成に成功したかどうかチェック */
		/* 注意）アセットの上書きを行わなかった（行えなかった）場合、NULLが返される。 */
		if (ExistingAsset == NULL) {
			continue;
		}

		/* キュー情報の差し替えが必要かどうかチェック */
		if (ExistingAsset->CueSheet != AcbObject) {
			/* キュー情報の保存 */
			ExistingAsset->CueName = UTF8_TO_TCHAR(cue_name);
			ExistingAsset->CueSheet = AcbObject;

			/* アセットを編集状態に変更 */
			ExistingAsset->MarkPackageDirty();
		}
	}

	/* エラーチェック */
	if (AcbHn == NULL) {
		/* エラー発生時 */
		FString CuePackageName = InParent->GetOutermost()->GetName();
		FMessageDialog::Open(EAppMsgType::Ok,
			FText::Format(NSLOCTEXT("UnrealEd", "Error_ImportFailed_f", "Import failed for {0}"), FText::FromString(CuePackageName)));
	} else {
		/* ACBのリリース */
		criAtomExAcb_Release(AcbHn);
	}

	/* メモリの解放 */
	FMemory::Free(Data);

	/* ACBファイル情報の保存領域を確保 */
	if(AcbObject->AssetImportData == nullptr) {
		AcbObject->AssetImportData = NewObject<UAssetImportData>(AcbObject, TEXT("AssetImportData"));
	}

	/* ACBファイル再インポート用に情報を保存 */
	if (AcbObject->AssetImportData != nullptr) {
		AcbObject->AssetImportData->Update(CurrentFilename);
	}

	/* キューシート名の保存 */
	if (AcbObject->AssetImportData != nullptr) {
		FString AcbFilePath = AcbObject->AssetImportData->SourceData.SourceFiles[0].RelativeFilename;
		FString AcbDirectory;
		FString AcbFileName;
		FString AcbExtension;
		FPaths::Split(AcbFilePath, AcbDirectory, AcbFileName, AcbExtension);
		AcbObject->CueSheetName = AcbFileName;
	};
	if (AcbObject->CueSheetName.Len() <= 0) {
		AcbObject->CueSheetName = Name.ToString();
	}

	return AcbObject;
}

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/
#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
