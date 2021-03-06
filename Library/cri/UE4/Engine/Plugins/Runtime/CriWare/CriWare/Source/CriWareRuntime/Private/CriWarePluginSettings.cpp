/****************************************************************************
*
* CRI Middleware SDK
*
* Copyright (c) 2015-2017 CRI Middleware Co., Ltd.
*
* Library  : CRIWARE plugin for Unreal Engine 4
* Module   : PluginSettings
* File     : CriWarePluginSettings.cpp
*
****************************************************************************/

/***************************************************************************
*      インクルードファイル
*      Include files
***************************************************************************/
/* モジュールヘッダ */
#include "CriWarePluginSettings.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareRuntimePrivatePCH.h"
#include "CriWareInitializer.h"

/* Unreal Engine 4関連ヘッダ */
#include "Misc/Paths.h"
#include "Misc/ConfigCacheIni.h"
#if WITH_EDITOR
#include "Misc/FileHelper.h"
#include "Misc/MessageDialog.h" 
#endif


/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "CriWarePluginSettings"

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
UCriWarePluginSettings::UCriWarePluginSettings(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
#if WITH_EDITOR
	/* Localization of unreal properties metadata with LOCTEXT markups and reflection */
	CRI_LOCCLASS(GetClass());
#endif

	FString SourceConfigPath = FPaths::SourceConfigDir();
	if (!SourceConfigPath.Contains(TEXT("Engine"))) {
		/* 各種パラメータの初期化           */
		UpdateUProperty(0);

		FString engineConfigDir = FPaths::ConvertRelativePathToFull(FPaths::EngineConfigDir());
		FString sourceConfigDir = FPaths::SourceConfigDir();

		/* Windowsの設定ファイルを読み込む */
		FString EngineIniFile;
		GConfig->LoadGlobalIniFile(EngineIniFile, TEXT("Engine"));

		FString SectionNameUClassOld = "/Script/CriWareEditor.CriWarePluginSettings";
		FString SectionNameUClassNew = "/Script/CriWareRuntime.CriWarePluginSettings";
		FString SectionNameUClass = SectionNameUClassNew;
		TArray<FString> SectionUClass;
		bool isExistSection_UClass = GConfig->GetSection(*SectionNameUClass, SectionUClass, EngineIniFile);
		if (!isExistSection_UClass) {
			SectionNameUClass = SectionNameUClassOld;
			isExistSection_UClass = GConfig->GetSection(*SectionNameUClass, SectionUClass, EngineIniFile);
		}

		if (isExistSection_UClass) {
			// Standalone: simple load of settings
			InitializeCriWarePlugins(SectionUClass);
		}
	}

}

#if WITH_EDITOR
/* Remove Section from INI file */
static void RemoveSectionFromIniFile(FString SectionName, const TCHAR* IniFile)
{	
	FString FileData = "";
	FFileHelper::LoadFileToString(FileData, IniFile);
	TArray<FString> InLines;
	int32 lineCount = FileData.ParseIntoArrayLines(InLines);
	FileData.Empty();
	bool InSection = false;
	bool FirstSection = true;
	bool EmptySection = true;
	for (FString Line : InLines) {
		if (Line.StartsWith("[") && Line.EndsWith("]")) {
			if (InSection) {
				InSection = false;
			} else if (Line.Mid(1, Line.Len() - 2).Trim() == SectionName) {
				InSection = true;
			} else if (!FirstSection && !EmptySection){
				FileData.Append(TEXT("\r\n"));
			}
			FirstSection = false;
			EmptySection = true;
		} else {
			EmptySection = false;
		}
		if (!InSection) {
			FileData.Append(Line + TEXT("\r\n"));
		}
	}
	FFileHelper::SaveStringToFile(FileData, IniFile);
}

void UCriWarePluginSettings::InitializeSettings()
{
	FString SourceConfigPath = FPaths::SourceConfigDir();
	if (!SourceConfigPath.Contains(TEXT("Engine"))) {
		/* 各種パラメータの初期化           */
		UpdateUProperty(0);

		FString engineConfigDir = FPaths::ConvertRelativePathToFull(FPaths::EngineConfigDir());
		FString sourceConfigDir = FPaths::SourceConfigDir();

		new(EngineIniFilePath)FString(engineConfigDir + TEXT("BaseEngine.ini"));
		new(EngineIniFilePath)FString(engineConfigDir + TEXT("Windows/WindowsEngine.ini"));
		new(EngineIniFilePath)FString(sourceConfigDir + TEXT("DefaultEngine.ini"));
		new(EngineIniFilePath)FString(sourceConfigDir + TEXT("Windows/WindowsEngine.ini"));

		/* Windowsの設定ファイルを読み込む */
		FString EngineIniFile;
		GConfig->LoadGlobalIniFile(EngineIniFile, TEXT("Engine"));

		FString SectionNameUClassOld = "/Script/CriWareEditor.CriWarePluginSettings";
		FString SectionNameUClassNew = "/Script/CriWareRuntime.CriWarePluginSettings";
		FString SectionNameUClass = SectionNameUClassNew;
		TArray<FString> SectionUClass;
		bool isExistSection_UClass = GConfig->GetSection(*SectionNameUClass, SectionUClass, EngineIniFile);
		if (!isExistSection_UClass) {
			SectionNameUClass = SectionNameUClassOld;
			isExistSection_UClass = GConfig->GetSection(*SectionNameUClass, SectionUClass, EngineIniFile);
		}

		// remove old section
		if (SectionNameUClass == SectionNameUClassOld) {
			RemoveSectionFromIniFile(SectionNameUClassOld, *EngineIniFilePath[EEngineIniFileType::ProjDefaultEngineIni]);
		}
		// Any old style -> need converstion
		/* 設定ファイルのヒエラルキー通りのパスをすべて書き出し（CriWare.iniとEngine.ini） */
		new(CriWareIniFilePath)FString(engineConfigDir + TEXT("BaseCriWare.ini"));
		new(CriWareIniFilePath)FString(engineConfigDir + TEXT("Windows/WindowsCriWare.ini"));
		new(CriWareIniFilePath)FString(sourceConfigDir + TEXT("DefaultCriWare.ini"));
		new(CriWareIniFilePath)FString(sourceConfigDir + TEXT("Windows/WindowsCriWare.ini"));

		FString CriWareIniFile;
		GConfig->LoadGlobalIniFile(CriWareIniFile, TEXT("CriWare"));

		/* CriWare用のパラメータがConfigファイルに記載されている場合はUPropertyの値をConfigファイルに記載されているパラメータで上書き */
		/* CriWare用のパラメータがどのConfigファイルにも記載されていない場合はDefaultEngine.iniに現在のUPropertyの値を書き込む */
		FString SectionNameFileSystem = "FileSystem";
		FString SectionNameAtom = "Atom";
		TArray<FString> SectionFileSystem;
		TArray<FString> SectionAtom;
		bool isFileSystemCategory = GConfig->GetSection(*SectionNameFileSystem, SectionFileSystem, CriWareIniFile);
		bool isAtomCategory = GConfig->GetSection(*SectionNameAtom, SectionAtom, CriWareIniFile);

		if ((isFileSystemCategory || isAtomCategory) && !isExistSection_UClass) {
			/* 旧Configファイルが存在し新Configファイルがない場合の処理 */
			UpdateCriData(SectionFileSystem, &SectionUClass, TEXT("FileSystem"));
			UpdateCriData(SectionAtom, &SectionUClass, TEXT("Atom"));
			InitializeCriWarePlugins(SectionUClass);
		}
		else if (!(isFileSystemCategory || isAtomCategory) && isExistSection_UClass) {
			/* 新Configファイルが存在し旧Configファイルが場合の処理 */
			InitializeCriWarePlugins(SectionUClass);
			UpdateDefaultConfigFile(EngineIniFilePath[EEngineIniFileType::ProjDefaultEngineIni]);
		}
		else if (!(isFileSystemCategory || isAtomCategory) && !isExistSection_UClass) {
			/* 新Configファイルがなく、旧Configファイルもない場合の処理 */
			CheckoutConfigFile(EngineIniFilePath[EEngineIniFileType::ProjDefaultEngineIni]);
			UpdateDefaultConfigFile(EngineIniFilePath[EEngineIniFileType::ProjDefaultEngineIni]);
		}

		/* Engine/Saved以下にEngine.iniが存在していた場合そのファイルを削除 */
		FString savedConfigDir = TEXT("Saved/Config/Windows/CriWare.ini");
		FString engineSavedConfigDir = FPaths::Combine(*(FPaths::GameDir()), savedConfigDir);
		if (!FPaths::DirectoryExists(engineSavedConfigDir)) {
			engineSavedConfigDir = FPaths::Combine(*(FPaths::EngineDir()), savedConfigDir);
		}
		IFileManager::Get().Delete(*engineSavedConfigDir);

		/* ProjectSettingsからのパラメータ変更を有効にするかどうかの判定 */
		bNoExistCriWareIni = !IsExistingCriWareIniFile();
	}
}

bool UCriWarePluginSettings::IsExistingCriWareIniFile() const
{
	bool bExistingCriWareIni = false;

	if (CriWareIniFilePath.Num() < NUM_INIFILE) {
		return bExistingCriWareIni;
	}
	
	for (int iter = 0; iter < NUM_INIFILE; iter++) {
		/* CriWare.iniを使用しているか判定 */
		if (FPaths::FileExists(CriWareIniFilePath[iter])) {
			/* CriWare.iniが見つかったらfalse */
			bExistingCriWareIni = true;
		}
	}
	return bExistingCriWareIni;
}

// used by detail
bool UCriWarePluginSettings::ConvertConfigrationFile(){

	/* SourceControl機能が有効な場合に対して、Configファイルをチェックアウトし削除後に結果をサブミットする必要がある */
	for (int iter = 0; iter < NUM_INIFILE; iter++){
		FString AbsoluteConfigFilePath_CriWare = CriWareIniFilePath[iter];
		FString AbsoluteConfigFilePath_Engine = EngineIniFilePath[iter];

		if (!FPaths::FileExists(AbsoluteConfigFilePath_CriWare)){
			/* FilePathのConfigファイルが存在しない場合は次のファイルの操作に移る */
			continue;
		}

		/* 一度UPropertyにデフォルト値を代入する */
		UpdateUProperty(1);

		if (!CreateEngineIniFile(AbsoluteConfigFilePath_Engine, AbsoluteConfigFilePath_CriWare)){
			UpdateUProperty(0);
			return false;
		}

		TArray<FString> FilesToBeDeleted;
		FilesToBeDeleted.Add(AbsoluteConfigFilePath_CriWare);

		ISourceControlProvider& SourceControlProvider = ISourceControlModule::Get().GetProvider();
		FSourceControlStatePtr SourceControlState = SourceControlProvider.GetState(AbsoluteConfigFilePath_CriWare, EStateCacheUsage::ForceUpdate);

		/* CriWare設定ファイルが存在する場合、存在するヒエラルキー毎の設定ファイルを個別にEngine.iniにコンバートしていく */
		/* 設定ファイルが存在しない場合は新しく設定ファイルは作成しない */
		/* 設定ファイル単体の内容をSection[Category]にカテゴリごとに格納 */
		FString SectionNameFileSystem = "FileSystem";
		FString SectionNameAtom = "Atom";
		FString SectionNameUClassOld = "/Script/CriWareEditor.CriWarePluginSettings";
		FString SectionNameUClass = "/Script/CriWareRuntime.CriWarePluginSettings";
		TArray<FString> SectionFileSystem;
		TArray<FString> SectionAtom;
		TArray<FString> SectionUClass;
		bool isFileSystemCategory = GConfig->GetSection(*SectionNameFileSystem, SectionFileSystem, AbsoluteConfigFilePath_CriWare);
		bool isAtomCategory = GConfig->GetSection(*SectionNameAtom, SectionAtom, AbsoluteConfigFilePath_CriWare);
		bool isUClassCategory = GConfig->GetSection(*SectionNameUClass, SectionUClass, AbsoluteConfigFilePath_Engine);
		if (!isUClassCategory) {
			SectionNameUClass = SectionNameUClassOld;
			GConfig->GetSection(*SectionNameUClass, SectionUClass, AbsoluteConfigFilePath_Engine);
		}

		if (isFileSystemCategory || isAtomCategory){

			/* 手動でConfigファイルを記述していた場合 */
			/* 旧Configファイルを削除して新規に新フォーマットのデータを作成する */
			
			/* 旧カテゴリの内容をSectionUClassに上書き */
			
			ConvertCriData(SectionFileSystem, &SectionUClass, TEXT("FileSystem"));
			ConvertCriData(SectionAtom, &SectionUClass, TEXT("Atom"));

			/* 旧カテゴリの内容を削除するためファイルそのものを一度削除(UpdateDefaultConfigFile()関数で再度生成される) */
			if (SourceControlState.IsValid()) {
				/* SourceControlを使用している場合はConfigファイルを削除目的でマーキング */
				if (SourceControlState->CanCheckout() || SourceControlState->IsCheckedOutOther()) {
					SourceControlProvider.Execute(ISourceControlOperation::Create<FDelete>(), FilesToBeDeleted);
				}
				else if (SourceControlState->CanAdd()){
					IFileManager::Get().Delete(*AbsoluteConfigFilePath_CriWare);
				}

				/* Engine.iniがUpdateDefaultConfigFileで作成もしくは更新される前にEngine.iniの存在を確認 */
				bool isExistEngineIniFile = false;
				if (FPaths::FileExists(AbsoluteConfigFilePath_Engine)){
					isExistEngineIniFile = true;
				}

				InitializeCriWarePlugins(SectionUClass);

				/* Engine.iniを新規作成、または更新 */
				if (isExistEngineIniFile){
					/* Engine.iniが存在する場合 */
					CheckoutConfigFile(AbsoluteConfigFilePath_Engine);
				}
				UpdateDefaultConfigFile(AbsoluteConfigFilePath_Engine);
				if (!isExistEngineIniFile){
					/* Engine.iniが存在しない場合 */
					CheckoutConfigFile(AbsoluteConfigFilePath_Engine);
				}
			}
			else {
				/* SourceControlを使用していない場合はConfigファイルを削除 */
				IFileManager::Get().Delete(*AbsoluteConfigFilePath_CriWare);
				InitializeCriWarePlugins(SectionUClass);
				UpdateDefaultConfigFile(AbsoluteConfigFilePath_Engine);
			}
		}
	}
	/* ここですべての設定ファイルのコンバート完了 */

	/* Windowsの設定ファイルを読み込む */
	FString EngineIniFile;
	GConfig->LoadGlobalIniFile(EngineIniFile, TEXT("Engine"));
	FString SectionNameUClassOld = "/Script/CriWareEditor.CriWarePluginSettings";
	FString SectionNameUClass = "/Script/CriWareRuntime.CriWarePluginSettings";
	TArray<FString> Section;
	/* 新フォーマットのデータをSectionUClassに格納 */
	if (!GConfig->GetSection(*SectionNameUClass, Section, EngineIniFile)) {
		GConfig->GetSection(*SectionNameUClassOld, Section, EngineIniFile);
	}
	InitializeCriWarePlugins(Section);
	return true;
}

bool UCriWarePluginSettings::IsMixedConfigFiles() const
{
	if (!HasPendingChanges()){
		FString CriWareIniFile;
		GConfig->LoadGlobalIniFile(CriWareIniFile, TEXT("CriWare"));
		FString EngineIniFile;
		GConfig->LoadGlobalIniFile(EngineIniFile, TEXT("Engine"));

		FString SectionNameFileSystem = "FileSystem";
		FString SectionNameAtom = "Atom";
		FString SectionName_UClassOld = "/Script/CriWareEditor.CriWarePluginSettings";
		FString SectionName_UClass = "/Script/CriWareRuntime.CriWarePluginSettings";
		TArray<FString> SectionFileSystem;
		TArray<FString> SectionAtom;
		TArray<FString> SectionUClass;
		bool isFileSystemCategory = GConfig->GetSection(*SectionNameFileSystem, SectionFileSystem, CriWareIniFile);
		bool isAtomCategory = GConfig->GetSection(*SectionNameAtom, SectionAtom, CriWareIniFile);
		bool isExistSection_UClass = GConfig->GetSection(*SectionName_UClass, SectionUClass, EngineIniFile);
		if (!isExistSection_UClass) {
			SectionName_UClass = SectionName_UClassOld;
			isExistSection_UClass = GConfig->GetSection(*SectionName_UClass, SectionUClass, EngineIniFile);
		}

		return ((isFileSystemCategory || isAtomCategory) && isExistSection_UClass);
	}
	return false;

}

// used by detail
TArray<FString> UCriWarePluginSettings::GetDeletedFilesName() const
{
	TArray<FString> DeletedFilesName;

	/* SourceControl機能が有効な場合に対して、Configファイルをチェックアウトし削除後に結果をサブミットする必要がある */
	for (int iter = 0; iter < NUM_INIFILE; iter++){
		FString AbsoluteConfigFilePath_CriWare = CriWareIniFilePath[iter];
		FString AbsoluteConfigFilePath_Engine = EngineIniFilePath[iter];

		if (!FPaths::FileExists(AbsoluteConfigFilePath_CriWare)){
			/* FilePathのConfigファイルが存在しない場合は次のファイルの操作に移る */
			continue;
		}

		TArray<FString> FilesToBeDeleted;
		FilesToBeDeleted.Add(AbsoluteConfigFilePath_CriWare);

		ISourceControlProvider& SourceControlProvider = ISourceControlModule::Get().GetProvider();
		FSourceControlStatePtr SourceControlState = SourceControlProvider.GetState(AbsoluteConfigFilePath_CriWare, EStateCacheUsage::ForceUpdate);

		/* CriWare設定ファイルが存在する場合、存在するヒエラルキー毎の設定ファイルを個別にEngine.iniにコンバートしていく */
		/* 設定ファイルが存在しない場合は新しく設定ファイルは作成しない */
		/* 設定ファイル単体の内容をSection[Category]にカテゴリごとに格納 */
		FString SectionNameFileSystem = "FileSystem";
		FString SectionNameAtom = "Atom";
		TArray<FString> SectionFileSystem;
		TArray<FString> SectionAtom;
		TArray<FString> SectionUClass;
		bool isFileSystemCategory = GConfig->GetSection(*SectionNameFileSystem, SectionFileSystem, AbsoluteConfigFilePath_CriWare);
		bool isAtomCategory = GConfig->GetSection(*SectionNameAtom, SectionAtom, AbsoluteConfigFilePath_CriWare);

		if (isFileSystemCategory || isAtomCategory){

			/* 削除するファイル名をDeletedFilesNameに格納する */
			if (SourceControlState.IsValid()) {
					DeletedFilesName.Add(AbsoluteConfigFilePath_CriWare);
			}
			else {
				/* SourceControlを使用していない場合はConfigファイルを削除 */
				DeletedFilesName.Add(AbsoluteConfigFilePath_CriWare);
			}
		}
	}
	return DeletedFilesName;
}

bool UCriWarePluginSettings::HasPendingChanges() const
{
	return (
			/* === FileSystem関連パラメータ === */
			ContentDir != AppliedContentDir
			|| NumBinders != AppliedNumBinders
			|| MaxBinds != AppliedMaxBinds
			|| NumLoaders != AppliedNumLoaders
			|| MaxFiles != AppliedMaxFiles
			|| OutputsLogFileSystem != AppliedOutputsLogFileSystem
			/* === Atom関連パラメータ === */
			|| MaxVirtualVoices != AppliedMaxVirtualVoices
			|| UsesInGamePreview != AppliedUsesInGamePreview
			|| OutputsLogAtom != AppliedOutputsLogAtom
			|| NumStandardMemoryVoices != AppliedNumStandardMemoryVoices
			|| StandardMemoryVoiceNumChannels != AppliedStandardMemoryVoiceNumChannels
			|| StandardMemoryVoiceSamplingRate != AppliedStandardMemoryVoiceSamplingRate
			|| NumStandardStreamingVoices != AppliedNumStandardStreamingVoices
			|| StandardStreamingVoiceNumChannels != AppliedStandardStreamingVoiceNumChannels
			|| StandardStreamingVoiceSamplingRate != AppliedStandardStreamingVoiceSamplingRate
			|| AcfFileName != AppliedAcfFileName
			|| DistanceFactor != AppliedDistanceFactor
			|| HcaMxVoiceSamplingRate != AppliedHcaMxVoiceSamplingRate
			|| NumHcaMxMemoryVoices != AppliedNumHcaMxMemoryVoices
			|| HcaMxMemoryVoiceNumChannels != AppliedHcaMxMemoryVoiceNumChannels
			|| NumHcaMxStreamingVoices != AppliedNumHcaMxStreamingVoices
			|| HcaMxStreamingVoiceNumChannels != AppliedHcaMxStreamingVoiceNumChannels
			|| WASAPI_IsExclusive != AppliedWASAPI_IsExclusive
			|| WASAPI_BitsPerSample != AppliedWASAPI_BitsPerSample
			|| WASAPI_SamplingRate != AppliedWASAPI_SamplingRate
			|| WASAPI_NumChannels != AppliedWASAPI_NumChannels
		);
}
#endif // WITH_EDITOR

#if WITH_EDITOR
void UCriWarePluginSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	SettingChangedEvent.Broadcast();
}
#endif // WITH_EDITOR

void UCriWarePluginSettings::UpdateUProperty(int isConvert){
	if (isConvert == 1){
		/* === FileSystem関連パラメータ === */
		ContentDir = TEXT("");
		AppliedContentDir = TEXT("");
		NumBinders = FS_NUM_BINDERS;
		AppliedNumBinders = 0;
		MaxBinds = FS_MAX_BINDS;
		AppliedMaxBinds = 0;
		NumLoaders = FS_NUM_LOADERS;
		AppliedNumLoaders = 0;
		MaxFiles = FS_MAX_FILES;
		AppliedMaxFiles = 0;
		OutputsLogFileSystem = FS_OUTPUT_LOG;
		AppliedOutputsLogFileSystem = false;
		/* === Atom関連パラメータ === */
		MaxVirtualVoices = ATOM_MAX_VIRTUAL_VOICES;
		AppliedMaxVirtualVoices = 0;
		UsesInGamePreview = ATOM_USES_INGAME_PREVIEW;
		AppliedUsesInGamePreview = true;
		OutputsLogAtom = ATOM_OUTPUT_LOG;
		AppliedOutputsLogAtom = false;
		NumStandardMemoryVoices = ATOM_NUM_STANDARD_MEMORY_VOICES;
		AppliedNumStandardMemoryVoices = 0;
		StandardMemoryVoiceNumChannels = ATOM_STANDARD_MEMORY_VOICE_NUM_CHANNELS;
		AppliedStandardMemoryVoiceNumChannels = 0;
		StandardMemoryVoiceSamplingRate = ATOM_STANDARD_MEMORY_VOICE_SAMPLING_RATE;
		AppliedStandardMemoryVoiceSamplingRate = 0;
		NumStandardStreamingVoices = ATOM_NUM_STANDARD_STREAMING_VOICES;
		AppliedNumStandardStreamingVoices = 0;
		StandardStreamingVoiceNumChannels = ATOM_STANDARD_STREAMING_VOICE_NUM_CHANNELS;
		AppliedStandardStreamingVoiceNumChannels = 0;
		StandardStreamingVoiceSamplingRate = ATOM_STANDARD_STREAMING_VOICE_SAMPLING_RATE;
		AppliedStandardStreamingVoiceSamplingRate = 0;
		AcfFileName = TEXT("");
		AppliedAcfFileName = TEXT("");
		DistanceFactor = ATOM_DISTANCE_FACTOR;
		AppliedDistanceFactor = 0;
		HcaMxVoiceSamplingRate = CRIATOM_DEFAULT_OUTPUT_SAMPLING_RATE;
		AppliedHcaMxVoiceSamplingRate = 0;
		NumHcaMxMemoryVoices = 0;
		AppliedNumHcaMxMemoryVoices = 0;
		HcaMxMemoryVoiceNumChannels = ATOM_STANDARD_MEMORY_VOICE_NUM_CHANNELS;
		AppliedHcaMxMemoryVoiceNumChannels = 0;
		NumHcaMxStreamingVoices = 0;
		AppliedNumHcaMxStreamingVoices = 0;
		HcaMxStreamingVoiceNumChannels = ATOM_STANDARD_STREAMING_VOICE_NUM_CHANNELS;
		AppliedHcaMxStreamingVoiceNumChannels = 0;
		WASAPI_IsExclusive = false;
		AppliedWASAPI_IsExclusive = false;
		WASAPI_BitsPerSample = 24;
		AppliedWASAPI_BitsPerSample = 0;
		WASAPI_SamplingRate = CRIATOM_DEFAULT_OUTPUT_SAMPLING_RATE;
		AppliedWASAPI_SamplingRate = 0;
		WASAPI_NumChannels = CRIATOM_DEFAULT_OUTPUT_CHANNELS;
		AppliedWASAPI_NumChannels = 0;
	}
	else if (isConvert == 0) {
		/* === FileSystem関連パラメータ === */
		ContentDir = TEXT("");
		AppliedContentDir = TEXT("");
		NumBinders = FS_NUM_BINDERS;
		AppliedNumBinders = FS_NUM_BINDERS;
		MaxBinds = FS_MAX_BINDS;
		AppliedMaxBinds = FS_MAX_BINDS;
		NumLoaders = FS_NUM_LOADERS;
		AppliedNumLoaders = FS_NUM_LOADERS;
		MaxFiles = FS_MAX_FILES;
		AppliedMaxFiles = FS_MAX_FILES;
		OutputsLogFileSystem = FS_OUTPUT_LOG;
		AppliedOutputsLogFileSystem = FS_OUTPUT_LOG;
		/* === Atom関連パラメータ === */
		MaxVirtualVoices = ATOM_MAX_VIRTUAL_VOICES;
		AppliedMaxVirtualVoices = ATOM_MAX_VIRTUAL_VOICES;
		UsesInGamePreview = ATOM_USES_INGAME_PREVIEW;
		AppliedUsesInGamePreview = ATOM_USES_INGAME_PREVIEW;
		OutputsLogAtom = ATOM_OUTPUT_LOG;
		AppliedOutputsLogAtom = ATOM_OUTPUT_LOG;
		NumStandardMemoryVoices = ATOM_NUM_STANDARD_MEMORY_VOICES;
		AppliedNumStandardMemoryVoices = ATOM_NUM_STANDARD_MEMORY_VOICES;
		StandardMemoryVoiceNumChannels = ATOM_STANDARD_MEMORY_VOICE_NUM_CHANNELS;
		AppliedStandardMemoryVoiceNumChannels = ATOM_STANDARD_MEMORY_VOICE_NUM_CHANNELS;
		StandardMemoryVoiceSamplingRate = ATOM_STANDARD_MEMORY_VOICE_SAMPLING_RATE;
		AppliedStandardMemoryVoiceSamplingRate = ATOM_STANDARD_MEMORY_VOICE_SAMPLING_RATE;
		NumStandardStreamingVoices = ATOM_NUM_STANDARD_STREAMING_VOICES;
		AppliedNumStandardStreamingVoices = ATOM_NUM_STANDARD_STREAMING_VOICES;
		StandardStreamingVoiceNumChannels = ATOM_STANDARD_STREAMING_VOICE_NUM_CHANNELS;
		AppliedStandardStreamingVoiceNumChannels = ATOM_STANDARD_STREAMING_VOICE_NUM_CHANNELS;
		StandardStreamingVoiceSamplingRate = ATOM_STANDARD_STREAMING_VOICE_SAMPLING_RATE;
		AppliedStandardStreamingVoiceSamplingRate = ATOM_STANDARD_STREAMING_VOICE_SAMPLING_RATE;
		AcfFileName = TEXT("");
		AppliedAcfFileName = TEXT("");
		DistanceFactor = ATOM_DISTANCE_FACTOR;
		AppliedDistanceFactor = ATOM_DISTANCE_FACTOR;
		HcaMxVoiceSamplingRate = CRIATOM_DEFAULT_OUTPUT_SAMPLING_RATE;
		AppliedHcaMxVoiceSamplingRate = CRIATOM_DEFAULT_OUTPUT_SAMPLING_RATE;
		NumHcaMxMemoryVoices = 0;
		AppliedNumHcaMxMemoryVoices = 0;
		HcaMxMemoryVoiceNumChannels = ATOM_STANDARD_MEMORY_VOICE_NUM_CHANNELS;
		AppliedHcaMxMemoryVoiceNumChannels = ATOM_STANDARD_MEMORY_VOICE_NUM_CHANNELS;
		NumHcaMxStreamingVoices = 0;
		AppliedNumHcaMxStreamingVoices = 0;
		HcaMxStreamingVoiceNumChannels = ATOM_STANDARD_STREAMING_VOICE_NUM_CHANNELS;
		AppliedHcaMxStreamingVoiceNumChannels = ATOM_STANDARD_STREAMING_VOICE_NUM_CHANNELS;
		WASAPI_IsExclusive = false;
		AppliedWASAPI_IsExclusive = false;
		WASAPI_BitsPerSample = 24;
		AppliedWASAPI_BitsPerSample = 24;
		WASAPI_SamplingRate = CRIATOM_DEFAULT_OUTPUT_SAMPLING_RATE;
		AppliedWASAPI_SamplingRate = CRIATOM_DEFAULT_OUTPUT_SAMPLING_RATE;
		WASAPI_NumChannels = CRIATOM_DEFAULT_OUTPUT_CHANNELS;
		AppliedWASAPI_NumChannels = CRIATOM_DEFAULT_OUTPUT_CHANNELS;
	}
}

FString UCriWarePluginSettings::CheckIniParamString(FString ini_param_string, int default_value) {
	if (ini_param_string == "") {
		ini_param_string = FString::FromInt(default_value);
	}
	return ini_param_string;
}

FString UCriWarePluginSettings::CheckIniParamString(FString ini_param_string, float default_value) {
	if (ini_param_string == "") {
		ini_param_string = FString::SanitizeFloat(default_value);
	}
	return ini_param_string;
}

void UCriWarePluginSettings::InitializeCriWarePlugins(const TArray<FString>& Section_UClass)
{
	/* === FileSystem関連パラメータ === */

	/* コンテンツディレクトリパス指定の取得 */
	ContentDir = GetParameterString( Section_UClass, TEXT("ContentDir") );
	AppliedContentDir = ContentDir;
	
	/* バインダ情報の取得 */
	NumBinders = FCString::Atoi( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("NumBinders")), FS_NUM_BINDERS));
	AppliedNumBinders = NumBinders;
	MaxBinds = FCString::Atoi( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("MaxBinds")), FS_MAX_BINDS));
	AppliedMaxBinds = MaxBinds;

	/* ローダ情報の取得 */
	NumLoaders = FCString::Atoi( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("NumLoaders")), FS_NUM_LOADERS));
	AppliedNumLoaders = NumLoaders;
	MaxFiles = FCString::Atoi( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("MaxFiles")), FS_MAX_FILES));
	AppliedMaxFiles = MaxFiles;

	/* ログ出力を行うかどうか */
	OutputsLogFileSystem = FCString::ToBool( *GetParameterString(Section_UClass, TEXT("OutputsLogFileSystem")) );
	AppliedOutputsLogFileSystem = OutputsLogFileSystem;

	/* === Atom関連パラメータ === */
	
	/* 最大バーチャルボイス数の取得 */
	MaxVirtualVoices = FCString::Atoi( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("MaxVirtualVoices")), ATOM_MAX_VIRTUAL_VOICES));
	AppliedMaxVirtualVoices = MaxVirtualVoices;

	/* インゲームプレビューを使用するかどうか */
	UsesInGamePreview = FCString::ToBool( *GetParameterString(Section_UClass, TEXT("UsesInGamePreview")));
	AppliedUsesInGamePreview = UsesInGamePreview;

	/* ログ出力を行うかどうか */
	OutputsLogAtom = FCString::ToBool( *GetParameterString(Section_UClass, TEXT("OutputsLogAtom")) );
	AppliedOutputsLogAtom = OutputsLogAtom;

	/* メモリ再生用ボイス情報の取得 */
	NumStandardMemoryVoices = FCString::Atoi( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("NumStandardMemoryVoices")), ATOM_NUM_STANDARD_MEMORY_VOICES));
	AppliedNumStandardMemoryVoices = NumStandardMemoryVoices;
	StandardMemoryVoiceNumChannels = FCString::Atoi( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("StandardMemoryVoiceNumChannels")), ATOM_STANDARD_MEMORY_VOICE_NUM_CHANNELS));
	AppliedStandardMemoryVoiceNumChannels = StandardMemoryVoiceNumChannels;
	StandardMemoryVoiceSamplingRate = FCString::Atoi( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("StandardMemoryVoiceSamplingRate")), ATOM_STANDARD_MEMORY_VOICE_SAMPLING_RATE));
	AppliedStandardMemoryVoiceSamplingRate = StandardMemoryVoiceSamplingRate;

	/* ストリーム再生用ボイス情報の取得 */
	NumStandardStreamingVoices = FCString::Atoi( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("NumStandardStreamingVoices")), ATOM_NUM_STANDARD_STREAMING_VOICES));
	AppliedNumStandardStreamingVoices = NumStandardStreamingVoices;
	StandardStreamingVoiceNumChannels = FCString::Atoi( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("StandardStreamingVoiceNumChannels")), ATOM_STANDARD_STREAMING_VOICE_NUM_CHANNELS));
	AppliedStandardStreamingVoiceNumChannels = StandardStreamingVoiceNumChannels;
	StandardStreamingVoiceSamplingRate = FCString::Atoi( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("StandardStreamingVoiceSamplingRate")), ATOM_STANDARD_STREAMING_VOICE_SAMPLING_RATE));
	AppliedStandardStreamingVoiceSamplingRate = StandardStreamingVoiceSamplingRate;

	/* HCA-MXサンプリングレートの取得 */
	HcaMxVoiceSamplingRate = FCString::Atoi( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("HcaMxVoiceSamplingRate")), CRIATOM_DEFAULT_OUTPUT_SAMPLING_RATE));
	AppliedHcaMxVoiceSamplingRate = HcaMxVoiceSamplingRate;

	/* HCA-MXメモリ再生用ボイス情報の取得 */
	NumHcaMxMemoryVoices = FCString::Atoi( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("NumHcaMxMemoryVoices")), 0));
	AppliedNumHcaMxMemoryVoices = NumHcaMxMemoryVoices;
	HcaMxMemoryVoiceNumChannels = FCString::Atoi( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("HcaMxMemoryVoiceNumChannels")), ATOM_STANDARD_MEMORY_VOICE_NUM_CHANNELS));
	AppliedHcaMxMemoryVoiceNumChannels = HcaMxMemoryVoiceNumChannels;

	/* HCA-MXストリーム再生用ボイス情報の取得 */
	NumHcaMxStreamingVoices = FCString::Atoi( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("NumHcaMxStreamingVoices")), 0));
	AppliedNumHcaMxStreamingVoices = NumHcaMxStreamingVoices;
	HcaMxStreamingVoiceNumChannels = FCString::Atoi( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("HcaMxStreamingVoiceNumChannels")), ATOM_STANDARD_STREAMING_VOICE_NUM_CHANNELS));
	AppliedHcaMxStreamingVoiceNumChannels = HcaMxStreamingVoiceNumChannels;

	/* ACFファイル名の取得 */
	AcfFileName = GetParameterString( Section_UClass, TEXT("AcfFileName") );
	AppliedAcfFileName = AcfFileName;

	/* 距離係数の取得 */
	DistanceFactor = FCString::Atof( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("DistanceFactor")), ATOM_DISTANCE_FACTOR));
	AppliedDistanceFactor = DistanceFactor;

	/* WASAPI関連 */
	WASAPI_IsExclusive = FCString::ToBool( *GetParameterString(Section_UClass, TEXT("WASAPI_IsExclusive")) );
	AppliedWASAPI_IsExclusive = WASAPI_IsExclusive;
	WASAPI_BitsPerSample = FCString::Atoi( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("WASAPI_BitsPerSample")), 24));
	AppliedWASAPI_BitsPerSample = WASAPI_BitsPerSample;
	WASAPI_SamplingRate = FCString::Atoi( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("WASAPI_SamplingRate")), CRIATOM_DEFAULT_OUTPUT_SAMPLING_RATE));
	AppliedWASAPI_SamplingRate = WASAPI_SamplingRate;
	WASAPI_NumChannels = FCString::Atoi( *CheckIniParamString(GetParameterString(Section_UClass, TEXT("WASAPI_NumChannels")), CRIATOM_DEFAULT_OUTPUT_CHANNELS));
	AppliedWASAPI_NumChannels = WASAPI_NumChannels;
}

TArray<FString> UCriWarePluginSettings::GetParemeterArray(const TArray<FString>& str, FString SelectorName)
{
	TArray<FString> result;
	// get all line with "Selector"
	for (int32 index = 0; index < str.Num(); index++) {
		FString tmp, tmp2;
		if (str[index].Split(TEXT("="), &tmp, &tmp2) == true) {
			tmp = tmp.Trim();
			tmp2 = tmp2.Trim();
			tmp = tmp.TrimTrailing();
			tmp2 = tmp2.TrimTrailing();

			if (tmp == SelectorName) {
				result.Add(tmp2);
			}
		}
	}
	return result;
}

FString UCriWarePluginSettings::GetParameterString(const TArray<FString>& str, FString SelectorName)
{
	FString separate;
	int32 index = 0;
	while (index < str.Num()) {
		FString tmp;
		str[index].Split(TEXT("="), &tmp, NULL);
		if (tmp == SelectorName) {
			break;
		}
		index++;
	}

	if ( index == str.Num() ){
		return TEXT("");
	}
	if ( str[index].EndsWith(TEXT("=")) ) {
		return TEXT("");
	}
	str[index].Split(TEXT("="), NULL, &separate);

	return separate;
}

#if WITH_EDITOR
void UCriWarePluginSettings::UpdateCriData(TArray<FString> Section_Old, TArray<FString>* Section_New, FString CategoryType)
{
	for (int i = 0; i < Section_Old.Num(); i++){
		FString Old_tmp = Section_Old[i];
		FString SectionName_Old;
		FString Outputs;
		Old_tmp.Split(TEXT("="), &SectionName_Old, &Outputs);

		/* 旧フォーマットだとOutputLogだけ[Atom]、[FileSystem]で同名のパラメータとして存在しているため名前を変更 */
		if (SectionName_Old == TEXT("OutputsLog")) {
			if (CategoryType == TEXT("FileSystem")){
				Section_Old[i] = TEXT("OutputsLogFileSystem=") + Outputs;
			} else if (CategoryType == TEXT("Atom")) {
				Section_Old[i] = TEXT("OutputsLogAtom=") + Outputs;
			}
		}
		new(*Section_New)FString(Section_Old[i]);
	}
}

void UCriWarePluginSettings::ConvertCriData(TArray<FString> Section_Old, TArray<FString>* Section_New, FString CategoryType)
{
	for (int i = 0; i < Section_Old.Num(); i++){
		FString Old_tmp = Section_Old[i];
		FString SectionName_Old;
		FString Outputs;
		Old_tmp.Split(TEXT("="), &SectionName_Old, &Outputs);

		/* 旧フォーマットだとOutputLogだけ[Atom]、[FileSystem]で同名のパラメータとして存在しているため名前を変更 */
		if (SectionName_Old == TEXT("OutputsLog")) {
			if (CategoryType == TEXT("FileSystem")){
				Section_Old[i] = TEXT("OutputsLogFileSystem=") + Outputs;
				SectionName_Old = TEXT("OutputsLogFileSystem=");
			}
			else if (CategoryType == TEXT("Atom")) {
				Section_Old[i] = TEXT("OutputsLogAtom=") + Outputs;
				SectionName_Old = TEXT("OutputsLogAtom=");
			}
		}
		for (int iter = 0; iter < (*Section_New).Num(); iter++){
			FString SectionName;
			(*Section_New)[iter].Split(TEXT("="), &SectionName, NULL);
			if (SectionName == SectionName_Old){
				(*Section_New)[iter] = Section_Old[i];
			}
		}
	}
}

void UCriWarePluginSettings::CheckoutConfigFile(FString FileName) {

	FText ErrorMessage;

	ISourceControlProvider& SourceControlProvider = ISourceControlModule::Get().GetProvider();
	FSourceControlStatePtr SourceControlState = SourceControlProvider.GetState(FileName, EStateCacheUsage::ForceUpdate);
	TArray<FString> SourceControlFiles;
	SourceControlFiles.Add(FileName);
	
	if (SourceControlState.IsValid()) {
		if (SourceControlState->IsDeleted())
		{
			/* ファイルが削除されていた場合の処理 */
			ErrorMessage = LOCTEXT("ConfigFileMarkedForDeleteError", "Error: The configuration file is marked for deletion.");
		}
		else if (!SourceControlState->IsCurrent())
		{
			/* ファイルが最新でない場合の処理 */
			if (false)
			{
				if (SourceControlProvider.Execute(ISourceControlOperation::Create<FSync>(), SourceControlFiles) == ECommandResult::Succeeded)
				{
					ReloadConfig();
					if (SourceControlState->CanCheckout() || SourceControlState->IsCheckedOutOther()) {
						/* Engine.iniが存在した場合、Configファイルをチェックアウトする */
						if (SourceControlProvider.Execute(ISourceControlOperation::Create<FCheckOut>(), SourceControlFiles) == ECommandResult::Failed)
						{
							ErrorMessage = LOCTEXT("FailedToCheckOutConfigFileError", "Error: Failed to check out the configuration file.");
						}
					}
				}
				else
				{
					ErrorMessage = LOCTEXT("FailedToSyncConfigFileError", "Error: Failed to sync the configuration file to head revision.");
				}
			}
		}
		else if (SourceControlState->CanCheckout() || SourceControlState->IsCheckedOutOther()) {
			/* Engine.iniが存在した場合、Configファイルをチェックアウトする */
			if (SourceControlProvider.Execute(ISourceControlOperation::Create<FCheckOut>(), SourceControlFiles) == ECommandResult::Failed)
			{
				ErrorMessage = LOCTEXT("FailedToCheckOutConfigFileError", "Error: Failed to check out the configuration file.");
			}
		}
		else if (SourceControlState->CanAdd()){
			/* Engine.iniが存在しない場合、もしくはSourceControlに追加されていない場合はConfigファイルを追加目的でマーキングする */
			if (SourceControlProvider.Execute(ISourceControlOperation::Create<FMarkForAdd>(), SourceControlFiles) == ECommandResult::Failed)
			{
				ErrorMessage = LOCTEXT("FailedToMarkForAddConfigFileError", "Error: Failed to Mark For Add the configuration file.");
			}
		}
	}
	// show errors, if any
	if (!ErrorMessage.IsEmpty())
	{
		FMessageDialog::Open(EAppMsgType::Ok, ErrorMessage);
	}
}

// used by detail
bool UCriWarePluginSettings::CreateEngineIniFile(FString EngineIniFileName, FString CriWareIniFileName){
	FText ErrorMessage;

	ISourceControlProvider& SourceControlProvider = ISourceControlModule::Get().GetProvider();
	FSourceControlStatePtr SourceControlState = SourceControlProvider.GetState(CriWareIniFileName, EStateCacheUsage::ForceUpdate);

	FDateTime time = IFileManager::Get().GetTimeStamp(*EngineIniFileName);
	
	if (IFileManager::Get().IsReadOnly(*CriWareIniFileName) && !SourceControlState.IsValid()){
		ErrorMessage = LOCTEXT("FailedToRewriteCriWareIniError", "Error: Failed to delete CriWare.ini! CRIWARE configuration file comes with read-Only attribute.");
		FMessageDialog::Open(EAppMsgType::Ok, ErrorMessage);
		return false;
	}

	if (IFileManager::Get().IsReadOnly(*EngineIniFileName)){
		ErrorMessage = LOCTEXT("FailedToRewriteEngineIniError", "Error: Failed to rewrite Engine.ini! Engine configuration file comes with read-Only attribute.");
		FMessageDialog::Open(EAppMsgType::Ok, ErrorMessage);
		return false;
	}

	UpdateDefaultConfigFile(EngineIniFileName);

	/* Configファイルが更新されていなかった場合に更新されるまでループ処理を行う。max_iteration回ループしたらエラーメッセージを出して終了 */
	FDateTime UpdatedTime;

	UpdatedTime = IFileManager::Get().GetTimeStamp(*EngineIniFileName);
	if (time != UpdatedTime){
		return true;
	}

	ErrorMessage = LOCTEXT("FailedToRewriteEngineIniUnknownError", "Error: Failed to rewrite Engine.ini. Engine.ini file may be opend by other application.");
	FMessageDialog::Open(EAppMsgType::Ok, ErrorMessage);
	return false;
}

#endif // WITH_EDITOR

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/

#undef LOCTEXT_NAMESPACE