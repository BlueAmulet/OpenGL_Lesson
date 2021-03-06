/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2013-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : CriWareEditor Module Implementation
 * File     : CriWareEditorModule.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
#include "CriWareEditorPrivatePCH.h"
#include "ICriWareEditor.h"
#include "CriWarePluginSettings.h"
#include "CriWarePluginSettingsDetails.h"
#include "CriWareEditorStyle.h"

#include "CriWareInitializer.h"
#include "AtomTriggerTableFunctionLibrary.h"
#include "AtomComponent.h"
#include "AtomComponentVisualizer.h"
#include "ActorFactoryAtomSound.h"
#include "ActorFactoryAtomSoundData.h"
#include "AtomCueSheetDetailsCustomization.h"

#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "PersonaModule.h"
#include "MultiBoxBuilder.h"
#include "IAnimationEditorModule.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "Dialogs/DlgPickAssetPath.h"
#include "AssetRegistryModule.h"
#include "ISettingsModule.h"
#include "TextLocalizationManager.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "FileManager.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "CriWareEditorModule"

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
/* ログ出力用 */
DEFINE_LOG_CATEGORY(LogCriWareEditor);

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
struct FCriWareMetaSettingGatherer
{
	// Ref: HardwareTargetingModule.cpp
	FTextBuilder DescriptionBuffer;

	TMap<UObject*, FTextBuilder> DescriptionBuffers;
	TMap<UObject*, FText> CategoryNames;

	// Are we just displaying what would change, or actually changing things?
	bool bReadOnly;

	bool bIncludeUnmodifiedProperties;

	FCriWareMetaSettingGatherer()
		: bReadOnly(false)
		, bIncludeUnmodifiedProperties(false)
	{
	}

	/* DefaultCriWare.iniの変更点をProjectSettingで表示するための文字列を作成 */
	void AddEntry(UObject* SettingsObject, UProperty* Property, FText NewValue, bool bModified)
	{
		if (bModified || bIncludeUnmodifiedProperties) {
			FTextBuilder& SettingsDescriptionBuffer = DescriptionBuffers.FindOrAdd(SettingsObject);

			if (!bReadOnly) {
				FPropertyChangedEvent ChangeEvent(Property, EPropertyChangeType::ValueSet);
				SettingsObject->PostEditChangeProperty(ChangeEvent);
			}
			else {
				FText SettingDisplayName = Property->GetDisplayNameText();

				FFormatNamedArguments Args;
				/* SettingDisplayName(プロパティ名)を{SettingValue}に関連付ける処理*/
				Args.Add(TEXT("SettingName"), SettingDisplayName);
				/* NewValue("pending" か "not pending")を{SettingValue}に関連付ける処理 */
				Args.Add(TEXT("SettingValue"), NewValue);

				/* ProjectSettingsに表示する文字列を作成 */
				/* {SettingName}にSettingDisplayNameが入り、{SettingValue}にNewValueが入る */
				FText FormatString = bModified ?
					LOCTEXT("MetaSettingDisplayStringModified", "{SettingName} is {SettingValue} <HardwareTargets.Strong>(modified)</>") :
					LOCTEXT("MetaSettingDisplayStringUnmodified", "{SettingName} is {SettingValue}");

				/* DescriptionBufferにProjectSettingsの変更結果を格納していく */
				/* CRIWARE_META_SETTING_ENTRYを更新を確認したいパラメタ数分呼んでDescriptionBufferに変更情報を格納していく必要がある*/
				SettingsDescriptionBuffer.AppendLine(FText::Format(FormatString, Args));
			}
		}
	}

	template <typename ValueType>
	static FText ValueToString(ValueType Value);

	/* DefaultCriWare.iniの更新を判定して更新されていたらtrueを返す */
	bool Finalize()
	{
		check(!bReadOnly);

		bool bSuccess = true;
		for (auto& Pair : DescriptionBuffers){
			/* iniファイルの名前を取得してデータを取得した際のファイルの最終更新時間を取得 */
			const FString Filename = Pair.Key->GetDefaultConfigFilename();
			const FDateTime BeforeTime = IFileManager::Get().GetTimeStamp(*Filename);

			/* iniファイルのデータを最新状態に更新 */
			Pair.Key->UpdateDefaultConfigFile();

			/* 最新状態のiniファイルと直前までのiniファイルの更新時間を比較して異なっていたら更新されていると判定 */
			const FDateTime AfterTime = IFileManager::Get().GetTimeStamp(*Filename);
			bSuccess = BeforeTime != AfterTime && bSuccess;
		}

		return bSuccess;
	}
};

/* AddEntryの{SettingValue}に格納される文字列を出力 */
template <>
FText FCriWareMetaSettingGatherer::ValueToString(bool Value)
{
	return Value ? LOCTEXT("Pending", "pending") : LOCTEXT("NotPending", "not pending");
}
/* ProjectSettingsでどのパラメタの変更が行われたかを判定し、判定した結果を表示するための文字列を作成 */
#define CRIWARE_META_SETTING_ENTRY(Builder, Class, PropertyName, AppliedPropertyName) \
{ \
	Class* SettingsObject = GetMutableDefault<Class>(); \
	bool bModified = SettingsObject->PropertyName != SettingsObject->AppliedPropertyName; \
if (!Builder.bReadOnly) { SettingsObject->PropertyName = SettingsObject->AppliedPropertyName; } \
	Builder.AddEntry(SettingsObject, FindFieldChecked<UProperty>(Class::StaticClass(), GET_MEMBER_NAME_CHECKED(Class, PropertyName)), FCriWareMetaSettingGatherer::ValueToString(bModified), bModified); \
}

/**
 * CriWareEditor module implementation (private)
 */
class FCriWareEditorModule : public ICriWareEditor
{
public:
	/** IModuleInterface */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/* 以下関数はProjectSettingのメニュー拡張用の関数 */
	// ->
	/* ICriWareEditor */
	virtual void ApplyCriWarePluginSetting() override;
	/* ProjectSettingsでの値の変更結果の情報を取得する関数 */
	virtual TArray<FModifiedDefaultConfig> GetPendingSettingsChanges() override;
	/**
	* Override this to set whether your module is allowed to be unloaded on the fly
	*
	* @return	Whether the module supports shutdown separate from the rest of the engine.
	*/
	virtual bool SupportsDynamicReloading() override
	{
		return true;
	}
	// <-

private:
	void RegisterComponentVisualizer(FName ComponentClassName, TSharedPtr<FComponentVisualizer> Visualizer);

	/* Personaメニュー拡張用 */
	TSharedPtr<FExtender> Extender;
	void OnPersonaMenuExtension(FMenuBuilder& MenuBuilder);
	void OnImportAtomTriggerTable();
	void OnExportAtomTriggerTable();

	/* 作成済みのアセットアクションを保持しておく（シャットダウン時に登録解除するため） */
	TArray<TSharedPtr<IAssetTypeActions>> RegisteredAssetTypeActions;

	/* 以下の関数はProjectSettingのメニュー拡張用の関数 */
	// ->
	/* ProjectSettingsでの変更結果をBuilderにすべて格納する関数 */
	void GatherSettings(FCriWareMetaSettingGatherer& Builder);

	void RegisterSettings();
	void UnregisterSettings();
	void RegisterObjectCustomizations();
	/**
	* Registers a custom class
	*
	* @param ClassName				The class name to register for property customization
	* @param DetailLayoutDelegate	The delegate to call to get the custom detail layout instance
	*/
	void RegisterCustomClassLayout(FName ClassName, FOnGetDetailCustomizationInstance DetailLayoutDelegate);
	/** List of registered class that we must unregister when the module shuts down */
	TSet< FName > RegisteredClassNames;
	TSet< FName > RegisteredPropertyTypes;
	// <-
};

IMPLEMENT_MODULE( FCriWareEditorModule, CriWareEditor );

void FCriWareEditorModule::StartupModule()
{
	/* ProjectSettingメニュー拡張処理を登録 */
	RegisterSettings();
	ApplyCriWarePluginSetting();
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	RegisterObjectCustomizations();
	PropertyModule.NotifyCustomizationModuleChanged();

	// Load Style
	FCriWareEditorStyle::ResetToDefault();

	/* AtomComponentVisualizerをUnrealEdモジュールに登録 */
	if (GUnrealEd != NULL) {
		TSharedPtr<FComponentVisualizer> Visualizer = MakeShareable(new FAtomComponentVisualizer);
		if (Visualizer.IsValid()) {
			GUnrealEd->RegisterComponentVisualizer(UAtomComponent::StaticClass()->GetFName(), Visualizer);
		}
	}

	/* ActorFactoryをエディターに登録 */
	UActorFactoryAtomSound* AtomSoundActorFactory = NewObject<UActorFactoryAtomSound>();
	GEditor->ActorFactories.Add(AtomSoundActorFactory);
	UActorFactoryAtomSoundData* AtomSoundDataActorFactory = NewObject<UActorFactoryAtomSoundData>();
	GEditor->ActorFactories.Add(AtomSoundDataActorFactory);

	/* ACBデータをuasset化するためのインターフェースを登録 */
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditorModule.RegisterCustomClassLayout("SoundAtomCueSheet", FOnGetDetailCustomizationInstance::CreateStatic(&FAtomCueSheetDetailsCustomization::MakeInstance));

	/* メニュー拡張用に処理を登録 */
    if(!Extender.IsValid()) {
		Extender = MakeShareable(new FExtender);
		Extender->AddMenuExtension(
			"AssetEditorActions",
			EExtensionHook::After,
			NULL,
			FMenuExtensionDelegate::CreateRaw(this, &FCriWareEditorModule::OnPersonaMenuExtension)
			);
		IAnimationEditorModule& AnimationEditorModule = FModuleManager::LoadModuleChecked<IAnimationEditorModule>("AnimationEditor");
		AnimationEditorModule.GetMenuExtensibilityManager()->AddExtender(Extender);
	}
}

void FCriWareEditorModule::ShutdownModule()
{
	/* ProjectSettingのメニュー拡張処理を登録解除 */
	if (UObjectInitialized()) {
		UnregisterSettings();
	}
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor")) {
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		/* Unregister all classes customized by name */
		for (auto It = RegisteredClassNames.CreateConstIterator(); It; ++It) {
			if (It->IsValid()) {
				PropertyModule.UnregisterCustomClassLayout(*It);
			}
		}
		/* Unregister all structures */
		for (auto It = RegisteredPropertyTypes.CreateConstIterator(); It; ++It) {
			if (It->IsValid()) {
				PropertyModule.UnregisterCustomPropertyTypeLayout(*It);
			}
		}
		PropertyModule.NotifyCustomizationModuleChanged();
	}

	/* AtomComponentVisualizerをUnrealEdモジュールから登録解除 */
	if (GUnrealEd != NULL) {
		GUnrealEd->UnregisterComponentVisualizer(UAtomComponent::StaticClass()->GetFName());
	}

	/* メニュー拡張用処理の登録解除 */
	if(Extender.IsValid() && FModuleManager::Get().IsModuleLoaded("Persona")) {
		IAnimationEditorModule& AnimationEditorModule = FModuleManager::LoadModuleChecked<IAnimationEditorModule>("AnimationEditor");
		AnimationEditorModule.GetMenuExtensibilityManager()->RemoveExtender(Extender);
	}

	/* モジュール初期化時に登録したアセットアクションを登録解除 */
	if (FModuleManager::Get().IsModuleLoaded("AssetTools")) {
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (int32 Index = 0; Index < RegisteredAssetTypeActions.Num(); ++Index) {
			AssetTools.UnregisterAssetTypeActions(RegisteredAssetTypeActions[Index].ToSharedRef());
		}
	}
}

void FCriWareEditorModule::OnPersonaMenuExtension(FMenuBuilder& MenuBuilder)
{
	/* Assetメニューの拡張 */
    MenuBuilder.BeginSection("AtomMenuHook", LOCTEXT("AtomMenu", "Atom"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("ImportAtomTriggerTable", "Import Atom Trigger Table"),
			LOCTEXT("ImportAtomTriggerTableTip", "Import Atom Triggers from DataTable asset."),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateRaw(this, &FCriWareEditorModule::OnImportAtomTriggerTable)));
		MenuBuilder.AddMenuEntry(
			LOCTEXT("ExportAtomTriggerTable", "Export Atom Trigger Table"),
			LOCTEXT("ExportAtomTriggerTableTip", "Export Atom Triggers to DataTable asset."),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateRaw(this, &FCriWareEditorModule::OnExportAtomTriggerTable)));
	}
    MenuBuilder.EndSection();
}

void FCriWareEditorModule::OnImportAtomTriggerTable()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	TArray<FAssetData> SelectedAssets;

	/* AnimSequenceアセットの選択 */
	FString DefaultPath = TEXT("/Game");
	FOpenAssetDialogConfig OpenAssetDialogConfig;
	OpenAssetDialogConfig.DialogTitleOverride = LOCTEXT("OpenAnimSequenceDialogTitle", "Open Animation Sequence");
	OpenAssetDialogConfig.DefaultPath = DefaultPath;
	OpenAssetDialogConfig.AssetClassNames.Add(UAnimSequence::StaticClass()->GetFName());
	OpenAssetDialogConfig.bAllowMultipleSelection = false;
	SelectedAssets = ContentBrowserModule.Get().CreateModalOpenAssetDialog(OpenAssetDialogConfig);
	if (SelectedAssets.Num() == 0) {
		return;
	}
	UAnimSequence* Animation = Cast<UAnimSequence>(SelectedAssets[0].GetAsset());
	if (Animation == NULL) {
		UE_LOG(LogCriWareEditor, Error, TEXT("Specified asset is not Animation Sequence."));
		return;
	}

	/* トラック数の取得 */
	int32 numTracks = Animation->AnimNotifyTracks.Num();

	/* DataTableアセットの選択 */
	OpenAssetDialogConfig.DialogTitleOverride = LOCTEXT("OpenAtomTriggerTableDialogTitle", "Open Atom Trigger Table");
	OpenAssetDialogConfig.AssetClassNames.Add(UDataTable::StaticClass()->GetFName());
	OpenAssetDialogConfig.bAllowMultipleSelection = false;
	SelectedAssets = ContentBrowserModule.Get().CreateModalOpenAssetDialog(OpenAssetDialogConfig);
	if (SelectedAssets.Num() == 0) {
		return;
	}
	UDataTable* DataTable = Cast<UDataTable>(SelectedAssets[0].GetAsset());
	if (DataTable == NULL) {
		UE_LOG(LogCriWareEditor, Error, TEXT("Specified asset is not Data Table."));
		return;
	}

	/* DataTableの解析 */
	for (int32 i = 1; ; i++) {
		/* 行の取得 */
		FName RowName = *FString::Printf(TEXT("%d"), i);
		FAtomTriggerRow OutRow;
		bool result = UAtomTriggerTableFunctionLibrary::GetDataTableRowFromName(DataTable, RowName, OutRow);
		if (result == false) {
			break;
		}

		/* トリガ位置の補正 */
		/* 備考）アニメーションシーケンス範囲外のトリガ情報は範囲内にクリップする。 */
		if (OutRow.Time < 0) {
			OutRow.Time = 0.0f;
		}
		if (OutRow.Time > Animation->SequenceLength) {
			OutRow.Time = Animation->SequenceLength;
		}

		/* 空トラックの新規作成 */
		FAnimNotifyTrack & Track = Animation->AnimNotifyTracks[numTracks - 1];
		FAnimNotifyTrack NewItem;
		NewItem.TrackName = *FString::FromInt(numTracks + 1);
		NewItem.TrackColor = FLinearColor::White;
		Animation->AnimNotifyTracks.Insert(NewItem, numTracks);
		numTracks++;

		/* 空のAnimNotifyを新規作成 */
		FAnimNotifyEvent NewEvent;
		NewEvent.NotifyName = RowName;
		NewEvent.SetTime(OutRow.Time);
		NewEvent.TriggerTimeOffset = GetTriggerTimeOffsetForType(Animation->CalculateOffsetForNotify(OutRow.Time));
		NewEvent.TrackIndex = numTracks - 1;

		/* AnimNotify_PlayAtomCueの作成 */

		/* ブループリントアセットのパスを指定してLoadObject */
		TSubclassOf<UObject> BlueprintClass = NULL;
		FString BlueprintPath = TEXT("/CriWare/AnimNotify_PlayAtomCue.PlayAtomCue");
		UBlueprint* BlueprintLibPtr = LoadObject<UBlueprint>(NULL, *BlueprintPath, NULL, 0, NULL);
		BlueprintClass = Cast<UClass>(BlueprintLibPtr->GeneratedClass);
		check(BlueprintClass)
		if (BlueprintClass == NULL) {
			return;
		}

		/* ロードされたUClassを使ってConstructObject */
		class UObject* AnimNotifyClass = NewObject<UObject>(
			Animation, BlueprintClass, NAME_None, RF_Transactional);
		NewEvent.Notify = Cast<UAnimNotify>(AnimNotifyClass);
		NewEvent.NotifyName = FName(*NewEvent.Notify->GetNotifyName());

		/* DataTableから取得した値をプロパティに設定 */
		UProperty* ObjectProp = FindField<UProperty>(AnimNotifyClass->GetClass(), "Sound");
		if (ObjectProp != NULL) {
			*ObjectProp->ContainerPtrToValuePtr<USoundAtomCue*>(AnimNotifyClass) = OutRow.Cue;
		}
		ObjectProp = FindField<UProperty>(AnimNotifyClass->GetClass(), "Bone");
		if (ObjectProp != NULL) {
			*ObjectProp->ContainerPtrToValuePtr<FName>(AnimNotifyClass) = OutRow.Bone;
		}

		/* 作成したAnimNotifyをAnimSequenceに追加 */
		Animation->Notifies.Add(NewEvent);
	}

	/* アセットを編集状態に変更 */
	Animation->MarkPackageDirty();

	/* タイムラインの表示を更新 */
	Animation->RefreshCacheData();
}

void FCriWareEditorModule::OnExportAtomTriggerTable()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	TArray<FAssetData> SelectedAssets;

	/* AnimSequenceアセットの選択 */
	FString DefaultPath = TEXT("/Game");
	FOpenAssetDialogConfig OpenAssetDialogConfig;
	OpenAssetDialogConfig.DialogTitleOverride = LOCTEXT("OpenAnimSequenceDialogTitle", "Open Animation Sequence");
	OpenAssetDialogConfig.DefaultPath = DefaultPath;
	OpenAssetDialogConfig.AssetClassNames.Add(UAnimSequence::StaticClass()->GetFName());
	OpenAssetDialogConfig.bAllowMultipleSelection = false;
	SelectedAssets = ContentBrowserModule.Get().CreateModalOpenAssetDialog(OpenAssetDialogConfig);
	if (SelectedAssets.Num() == 0) {
		return;
	}
	UAnimSequence* Animation = Cast<UAnimSequence>(SelectedAssets[0].GetAsset());
	if (Animation == NULL) {
		UE_LOG(LogCriWareEditor, Error, TEXT("Specified asset is not Animation Sequence."));
		return;
	}

	/* セーブダイアログの表示 */
	FString DefaultAsset = MakeUniqueObjectName(NULL, UPackage::StaticClass()).ToString();
	TSharedRef<SDlgPickAssetPath> NewDataTableDlg =
		SNew(SDlgPickAssetPath)
		.Title(LOCTEXT("ExportAtomTriggerTableDialogTitle", "Choose Location for Data Table Asset"))
		.DefaultAssetPath(FText::FromString(DefaultAsset));
	if (NewDataTableDlg->ShowModal() == EAppReturnType::Cancel) {
		return;
	}

	/* アセットの作成 */
	FString PackageName = NewDataTableDlg->GetFullAssetPath().ToString();
	FName AssetName = FName(*NewDataTableDlg->GetAssetName().ToString());
	UPackage* Package = CreatePackage(NULL, *PackageName);
	UDataTable* NewTable = NewObject<UDataTable>(
		Package, AssetName, RF_Public | RF_Standalone | RF_Transactional);
	if (NewTable == NULL) {
		UE_LOG(LogCriWareEditor, Error, TEXT("Failed to create Data Table"));
		return;
	}

	/* 行の形式を指定 */
	UScriptStruct* ImportRowStruct = FindObjectChecked<UScriptStruct>(ANY_PACKAGE, TEXT("AtomTriggerRow"));
	if (ImportRowStruct == NULL) {
		UE_LOG(LogCriWareEditor, Error, TEXT("AtomTriggerRow not found."));
		return;
	}
	NewTable->RowStruct = ImportRowStruct;

	/* 内容をCSV文字列で作成 */
	FString String = "Name,Time,Bone,Cue\n";
	for (int32 i = 0; i < Animation->Notifies.Num(); i++) {
		/* AnimNotifyの取得 */
		FAnimNotifyEvent& NotifyEvent = Animation->Notifies[i];
		if (NotifyEvent.Notify == NULL) {
			/* AnimNotifyStateの場合は無視 */
			continue;
		}

		/* プロパティの取得 */
		UProperty* SoundProp = FindField<UProperty>(NotifyEvent.Notify->GetClass(), "Sound");
		UProperty* BoneProp = FindField<UProperty>(NotifyEvent.Notify->GetClass(), "Bone");
		if ((SoundProp == NULL) || (BoneProp == NULL)) {
			/* 形式の異なるAnimNotifyは無視 */
			continue;
		}
		USoundAtomCue* Cue = *SoundProp->ContainerPtrToValuePtr<USoundAtomCue*>(NotifyEvent.Notify);;
		FName Bone = *BoneProp->ContainerPtrToValuePtr<FName>(NotifyEvent.Notify);

		/* 行の情報を追記 */
		String += (
			FString::Printf(TEXT("%d,"), i + 1)
			+ FString::SanitizeFloat(NotifyEvent.GetTime()) + ","
			+ "\"" + Bone.ToString() + "\","
			+ "\"SoundAtomCue'" + Cue->GetPathName() + "'\"\n"
		);
	}

	// Go ahead and create table from string
	TArray<FString> Problems = NewTable->CreateTableFromCSVString(String);
	UE_LOG(LogCriWareEditor, Log, TEXT("Exported DataTable '%s' - %d Problems"), *NewTable->GetName(), Problems.Num());

	// Notify the asset registry
	const UObject* NewTableAsUObject = NewTable;
	FAssetRegistryModule::AssetCreated(NewTable);

	// Mark the package dirty...
	Package->MarkPackageDirty();

	// Show in the content browser
	TArray<UObject*> Objects;
	Objects.Add(NewTable);
	GEditor->SyncBrowserToObjects(Objects);
}


/* 以下の関数はProjectSettingのメニュー拡張用の関数 */

void FCriWareEditorModule::RegisterSettings()
{
	auto Settings = GetMutableDefault<UCriWarePluginSettings>();
	Settings->InitializeSettings();

	/* ProjectSettingの"Project/Plugins/CriWare"にDefaultCriWare.ini編集用エディタを表示する処理を登録 */
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings")) {
		SettingsModule->RegisterSettings("Project", "Plugins", "CriWare",
			LOCTEXT("RuntimeSettingsName", "CriWare"),
			LOCTEXT("RuntimeSettingsDescription", "Configure the CRIWARE plugin"),
			Settings
		);
	}
}

void FCriWareEditorModule::UnregisterSettings()
{
	/* ProjectSettingの"Project/Plugins/CriWare"にDefaultCriWare.ini編集用エディタを表示する処理を登録解除 */
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings")) {
		SettingsModule->UnregisterSettings("Project", "Plugins", "CriWare");
	}
}

TArray<FModifiedDefaultConfig> FCriWareEditorModule::GetPendingSettingsChanges()
{
	// Gather and stringify the modified settings
	FCriWareMetaSettingGatherer Gatherer;
	Gatherer.bReadOnly = true;
	Gatherer.bIncludeUnmodifiedProperties = true;
	GatherSettings(Gatherer);

	TArray<FModifiedDefaultConfig> OutArray;
	for (auto& Pair : Gatherer.DescriptionBuffers) {
		FModifiedDefaultConfig ModifiedConfig;
		ModifiedConfig.SettingsObject = Pair.Key;
		ModifiedConfig.Description = Pair.Value.ToText();
		ModifiedConfig.CategoryHeading = Gatherer.CategoryNames.FindChecked(Pair.Key);

		OutArray.Add(ModifiedConfig);
	}
	return OutArray;
}

void FCriWareEditorModule::GatherSettings(FCriWareMetaSettingGatherer& Builder)
{
	UCriWarePluginSettings* Settings = GetMutableDefault<UCriWarePluginSettings>();

	if (Builder.bReadOnly) {
		// Force the category order and give nice descriptions
		Builder.CategoryNames.Add(GetMutableDefault<UCriWarePluginSettings>(), LOCTEXT("CriWareCategoryHeader", "Plugins - CriWare"));
	}

	/* ProjectSettingsでどのパラメタの変更が行われたかについて、パラメタ一つ一つを判定し、判定した結果を表示するための文字列を作成 */
	{
		/* === FileSystem関連パラメータ === */
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, ContentDir, AppliedContentDir);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, NumBinders, AppliedNumBinders);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, MaxBinds, AppliedMaxBinds);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, NumLoaders, AppliedNumLoaders);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, MaxFiles, AppliedMaxFiles);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, OutputsLogFileSystem, AppliedOutputsLogFileSystem);
		/* === Atom関連パラメータ === */
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, MaxVirtualVoices, AppliedMaxVirtualVoices);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, UsesInGamePreview, AppliedUsesInGamePreview);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, OutputsLogAtom, AppliedOutputsLogAtom);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, NumStandardMemoryVoices, AppliedNumStandardMemoryVoices);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, StandardMemoryVoiceNumChannels, AppliedStandardMemoryVoiceNumChannels);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, StandardMemoryVoiceSamplingRate, AppliedStandardMemoryVoiceSamplingRate);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, NumStandardStreamingVoices, AppliedNumStandardStreamingVoices);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, StandardStreamingVoiceNumChannels, AppliedStandardStreamingVoiceNumChannels);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, StandardStreamingVoiceSamplingRate, AppliedStandardStreamingVoiceSamplingRate);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, AcfFileName, AppliedAcfFileName);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, DistanceFactor, AppliedDistanceFactor);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, HcaMxVoiceSamplingRate, AppliedHcaMxVoiceSamplingRate);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, NumHcaMxMemoryVoices, AppliedNumHcaMxMemoryVoices);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, HcaMxMemoryVoiceNumChannels, AppliedHcaMxMemoryVoiceNumChannels);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, NumHcaMxStreamingVoices, AppliedNumHcaMxStreamingVoices);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, HcaMxStreamingVoiceNumChannels, AppliedHcaMxStreamingVoiceNumChannels);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, WASAPI_IsExclusive, AppliedWASAPI_IsExclusive);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, WASAPI_BitsPerSample, AppliedWASAPI_BitsPerSample);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, WASAPI_SamplingRate, AppliedWASAPI_SamplingRate);
		CRIWARE_META_SETTING_ENTRY(Builder, UCriWarePluginSettings, WASAPI_NumChannels, AppliedWASAPI_NumChannels);
	}
}

/* ProjectSettingでの変更値をiniファイルに適用 */
void FCriWareEditorModule::ApplyCriWarePluginSetting()
{
	UCriWarePluginSettings* Settings = GetMutableDefault<UCriWarePluginSettings>();

	// Apply the settings if they've changed
	if (Settings->HasPendingChanges()) {
		// Gather and apply the modified settings
		FCriWareMetaSettingGatherer Builder;
		Builder.bReadOnly = false;
		GatherSettings(Builder);
	}
}

void FCriWareEditorModule::RegisterObjectCustomizations()
{
	// Note: By default properties are displayed in script defined order (i.e the order in the header).  These layout detail classes are called in the order seen here which will display properties
	// in the order they are customized.  This is only relevant for inheritance where both a child and a parent have properties that are customized.
	// In the order below, Actor will get a chance to display details first, followed by USceneComponent.

	RegisterCustomClassLayout("CriWarePluginSettings", FOnGetDetailCustomizationInstance::CreateStatic(&FCriWarePluginSettingsDetails::MakeInstance));
}

void FCriWareEditorModule::RegisterCustomClassLayout(FName ClassName, FOnGetDetailCustomizationInstance DetailLayoutDelegate)
{
	check(ClassName != NAME_None);

	RegisteredClassNames.Add(ClassName);

	static FName PropertyEditor("PropertyEditor");
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditor);
	PropertyModule.RegisterCustomClassLayout(ClassName, DetailLayoutDelegate);
}

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/

#undef CRIWARE_META_SETTING_ENTRY
#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
