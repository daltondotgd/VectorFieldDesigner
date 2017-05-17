// Fill out your copyright notice in the Description page of Project Settings.

#include "VectorFieldDesignerEditor.h"
#include "VectorFieldDesignerWindow.h"
#include "VectorFieldDesignerViewport.h"
#include "VectorFieldDesignerCommands.h"

#include "Widgets/Docking/SDockTab.h"
#include "SSingleObjectDetailsPanel.h"
#include "EditorStyleSet.h"
#include "MultiBoxBuilder.h"
#include "Framework/Commands/Commands.h"
#include "Editor.h"

#include "AssetRegistryModule.h"
#include "VectorField/VectorFieldStatic.h"
#include "Factories/VectorFieldStaticFactory.h"
#include "FileHelpers.h"
#include "IContentBrowserSingleton.h"
#include "ModuleManager.h"
#include "ContentBrowserModule.h"
#include "ObjectTools.h"

#define LOCTEXT_NAMESPACE "FVectorFieldDesignerWindow"

static bool OpenSaveAsDialog(UClass* SavedClass, const FString& InDefaultPath, const FString& InNewNameSuggestion, FString& OutPackageName, FString& OutObjectPath)
{
	FString DefaultPath = InDefaultPath;

	if (DefaultPath.IsEmpty())
	{
		DefaultPath = TEXT("/Game/VectorFields");
	}

	FString NewNameSuggestion = InNewNameSuggestion;
	check(!NewNameSuggestion.IsEmpty());

	FSaveAssetDialogConfig SaveAssetDialogConfig;
	{
		SaveAssetDialogConfig.DefaultPath = DefaultPath;
		SaveAssetDialogConfig.DefaultAssetName = NewNameSuggestion;
		SaveAssetDialogConfig.AssetClassNames.Add(SavedClass->GetFName());
		SaveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;
		SaveAssetDialogConfig.DialogTitleOverride = LOCTEXT("SaveAssetDialogTitle", "Save Asset As");
	}

	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	FString SaveObjectPath = ContentBrowserModule.Get().CreateModalSaveAssetDialog(SaveAssetDialogConfig);

	if (!SaveObjectPath.IsEmpty())
	{
		OutPackageName = FPackageName::ObjectPathToPackageName(SaveObjectPath);
		OutObjectPath = SaveObjectPath;
		return true;
	}

	return false;
}

class SCustomizableVectorFieldPropertiesTabBody : public SSingleObjectDetailsPanel
{
public:
	SLATE_BEGIN_ARGS(SCustomizableVectorFieldPropertiesTabBody) {}
	SLATE_END_ARGS()

private:
	TWeakPtr<class FVectorFieldDesignerWindow> VectorFieldDesignerPtr;

public:
	void Construct(const FArguments& InArgs, TSharedPtr<FVectorFieldDesignerWindow> InVectorFieldDesigner)
	{
		VectorFieldDesignerPtr = InVectorFieldDesigner;

		SSingleObjectDetailsPanel::Construct(SSingleObjectDetailsPanel::FArguments().HostCommandList(InVectorFieldDesigner->GetToolkitCommands()).HostTabManager(InVectorFieldDesigner->GetTabManager()), /*bAutomaticallyObserveViaGetObjectToObserve=*/ false, /*bAllowSearch=*/ true);
	}

	//virtual UObject* GetObjectToObserve() const override
	//{
	//	if (VectorFieldDesignerPtr.Pin()->HasSelectedForceFields())
	//	{
	//		return VectorFieldDesignerPtr.Pin()->GetFocusedForceField();
	//	}
	//	else
	//	{
	//		return VectorFieldDesignerPtr.Pin()->GetVectorFieldBeingEdited();
	//	}
	//}

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override
	{
		SetPropertyWindowContents(VectorFieldDesignerPtr.Pin()->GetObjectsToObserve());
	}

	virtual TSharedRef<SWidget> PopulateSlot(TSharedRef<SWidget> PropertyEditorWidget) override
	{
		return SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(1.0)
			[
				PropertyEditorWidget
			];
	}
};

const FName VectorFieldDesignerDetailsID(TEXT("VectorFieldDesignerDetails"));
const FName VectorFieldDesignerViewportID(TEXT("VectorFieldDesignerViewport"));

void FVectorFieldDesignerWindow::InitVectorFieldDesignerWindow(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UCustomizableVectorField* InitVectorField)
{
	VectorFieldBeingEdited = InitVectorField;

	VectorFieldBeingEdited->SetFlags(RF_Transactional);
	GEditor->RegisterForUndo(this);

	FVectorFieldDesignerCommands::Register();
	BindEditorCommands();

	TSharedPtr<FVectorFieldDesignerWindow> VectorFieldDesignerWindowPtr = SharedThis(this);

	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_VectorFieldDesignerWindow_Layout_v0.0001")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.8f)
					->SetHideTabWell(true)
					->AddTab(VectorFieldDesignerViewportID, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.2f)
					->SetHideTabWell(true)
					->AddTab(VectorFieldDesignerDetailsID, ETabState::OpenedTab)
				)
			)
		);

	InitAssetEditor(Mode, InitToolkitHost, FName(TEXT("VectorFieldDesignerWindowApp")), StandaloneDefaultLayout, true, true, InitVectorField);

	ExtendToolbar();
	RegenerateMenusAndToolbars();
}

FVectorFieldDesignerWindow::~FVectorFieldDesignerWindow()
{
	GEditor->UnregisterForUndo(this);
}

FName FVectorFieldDesignerWindow::GetToolkitFName() const
{
	return FName("VectorFieldDesigner");
}

FText FVectorFieldDesignerWindow::GetBaseToolkitName() const
{
	return LOCTEXT("VectorFieldDesignerAppLabel", "VectorField Designer");
}

FText FVectorFieldDesignerWindow::GetToolkitName() const
{
	const bool bDirtyState = VectorFieldBeingEdited->GetOutermost()->IsDirty();

	FFormatNamedArguments Args;
	Args.Add(TEXT("VectorFieldName"), FText::FromString(VectorFieldBeingEdited->GetName()));
	Args.Add(TEXT("DirtyState"), bDirtyState ? FText::FromString(TEXT("*")) : FText::GetEmpty());
	return FText::Format(LOCTEXT("VectorFieldDesignerToolkitName", "{VectorFieldName}{DirtyState}"), Args);
}

FText FVectorFieldDesignerWindow::GetToolkitToolTipText() const
{
	return FAssetEditorToolkit::GetToolTipTextForObject(VectorFieldBeingEdited);
}

FString FVectorFieldDesignerWindow::GetWorldCentricTabPrefix() const
{
	return TEXT("VectorFieldDesigner");
}

void FVectorFieldDesignerWindow::ExtendToolbar()
{
	struct Local
	{
		static void FillToolbar(FToolBarBuilder& ToolbarBuilder)
		{
			ToolbarBuilder.BeginSection("Export");
			{
				TSharedPtr<FUICommandInfo> ExportVectorFieldCommand = FVectorFieldDesignerCommands::Get().ExportVectorField;
				ToolbarBuilder.AddToolBarButton(ExportVectorFieldCommand,
					NAME_None,
					LOCTEXT("VectorFieldEditorWindowExport", "Export"),
					ExportVectorFieldCommand->GetDescription(),
					FSlateIcon(FEditorStyle::GetStyleSetName(), "AssetEditor.SaveAsset")
				);

				TSharedPtr<FUICommandInfo> ExportVectorFieldCommandAs = FVectorFieldDesignerCommands::Get().ExportVectorFieldAs;
				ToolbarBuilder.AddToolBarButton(ExportVectorFieldCommandAs,
					NAME_None,
					LOCTEXT("VectorFieldEditorWindowExportAs", "Export As"),
					ExportVectorFieldCommand->GetDescription(),
					FSlateIcon(FEditorStyle::GetStyleSetName(), "AssetEditor.SaveAssetAs")
				);
			}
			ToolbarBuilder.EndSection();

			ToolbarBuilder.BeginSection("ForceFields");
			{
				TSharedPtr<FUICommandInfo> CreateSphericalForceFieldCommand = FVectorFieldDesignerCommands::Get().CreateSphericalForceField;
				ToolbarBuilder.AddToolBarButton(CreateSphericalForceFieldCommand,
					NAME_None,
					LOCTEXT("VectorFieldEditorWindowCreateSphericalForceField", "Spherical"),
					CreateSphericalForceFieldCommand->GetDescription(),
					FSlateIcon(FVectorFieldDesignerStyle::GetStyleSetName(), "VFDesigner.CreateSphericalForceField")
				);

				TSharedPtr<FUICommandInfo> CreateVortexForceFieldCommand = FVectorFieldDesignerCommands::Get().CreateVortexForceField;
				ToolbarBuilder.AddToolBarButton(CreateVortexForceFieldCommand,
					NAME_None,
					LOCTEXT("VectorFieldEditorWindowCreateVortexForceField", "Vortex"),
					CreateVortexForceFieldCommand->GetDescription(),
					FSlateIcon(FVectorFieldDesignerStyle::GetStyleSetName(), "VFDesigner.CreateVortexForceField")
				);

				TSharedPtr<FUICommandInfo> CreateWindForceFieldCommand = FVectorFieldDesignerCommands::Get().CreateWindForceField;
				ToolbarBuilder.AddToolBarButton(CreateWindForceFieldCommand,
					NAME_None,
					LOCTEXT("VectorFieldEditorWindowCreateWindForceField", "Wind"),
					CreateWindForceFieldCommand->GetDescription(),
					FSlateIcon(FVectorFieldDesignerStyle::GetStyleSetName(), "VFDesigner.CreateWindForceField")
				);
			}
			ToolbarBuilder.EndSection();
		}
	};

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);

	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateStatic(&Local::FillToolbar)
	);

	AddToolbarExtender(ToolbarExtender);
}

FLinearColor FVectorFieldDesignerWindow::GetWorldCentricTabColorScale() const
{
	return FLinearColor::Red;
}

void FVectorFieldDesignerWindow::BindEditorCommands()
{
	const FVectorFieldDesignerCommands& Commands = FVectorFieldDesignerCommands::Get();

	ToolkitCommands->MapAction(
		Commands.ExportVectorField,
		FExecuteAction::CreateSP(this, &FVectorFieldDesignerWindow::ExportVectorField)
	);

	ToolkitCommands->MapAction(
		Commands.ExportVectorFieldAs,
		FExecuteAction::CreateSP(this, &FVectorFieldDesignerWindow::ExportVectorFieldAs)
	);

	ToolkitCommands->MapAction(
		Commands.CreateSphericalForceField,
		FExecuteAction::CreateSP(this, &FVectorFieldDesignerWindow::CreateSphericalForceField)
	);

	ToolkitCommands->MapAction(
		Commands.CreateVortexForceField,
		FExecuteAction::CreateSP(this, &FVectorFieldDesignerWindow::CreateVortexForceField)
	);

	ToolkitCommands->MapAction(
		Commands.CreateWindForceField,
		FExecuteAction::CreateSP(this, &FVectorFieldDesignerWindow::CreateWindForceField)
	);
}

void FVectorFieldDesignerWindow::ExportVectorField()
{
	bool bCreateNewPackage = VectorFieldBeingEdited->AssetPath.IsEmpty() || FPaths::FileExists(VectorFieldBeingEdited->AssetPath);

	if (bCreateNewPackage)
	{
		ExportVectorFieldAs();
	}
	else
	{
		UpdateVectorFieldStaticPackage(VectorFieldBeingEdited->AssetPath);
	}
}

void FVectorFieldDesignerWindow::ExportVectorFieldAs()
{
	FString PackageName;
	FString ObjectPath;
	bool bSaveFileLocationSelected = OpenSaveAsDialog(
		UVectorFieldStatic::StaticClass(),
		FPackageName::GetLongPackagePath(VectorFieldBeingEdited->GetPathName()),
		FString::Printf(TEXT("VF_%s"), *VectorFieldBeingEdited->GetName()),
		PackageName,
		ObjectPath);

	if (bSaveFileLocationSelected)
	{
		if (FPackageName::DoesPackageExist(PackageName))
		{
			UpdateVectorFieldStaticPackage(PackageName);
			return;
		}

		CreateVectorFieldStaticPackage(PackageName);
	}
}

void FVectorFieldDesignerWindow::CreateSphericalForceField()
{
	GEditor->BeginTransaction(LOCTEXT("CreateSphericalForceFieldTransaction", "Create Spherical Force Field"));
	VectorFieldBeingEdited->Modify();

	VectorFieldBeingEdited->CreateSphericalForceField();
	SelectedForceFieldIds.Empty();
	SelectedForceFieldIds.Add(VectorFieldBeingEdited->ForceFields.Find(VectorFieldBeingEdited->ForceFields.Last()));
	VectorFieldDesignerViewportPtr->GetViewportClient().Get()->Invalidate();

	GEditor->EndTransaction();
}

void FVectorFieldDesignerWindow::CreateVortexForceField()
{
	GEditor->BeginTransaction(LOCTEXT("CreateVortexForceFieldTransaction", "Create Vortex Force Field"));
	VectorFieldBeingEdited->Modify();

	VectorFieldBeingEdited->CreateVortexForceField();
	SelectedForceFieldIds.Empty();
	SelectedForceFieldIds.Add(VectorFieldBeingEdited->ForceFields.Find(VectorFieldBeingEdited->ForceFields.Last()));
	VectorFieldDesignerViewportPtr->GetViewportClient().Get()->Invalidate();

	GEditor->EndTransaction();
}

void FVectorFieldDesignerWindow::CreateWindForceField()
{
	GEditor->BeginTransaction(LOCTEXT("CreateWindForceFieldTransaction", "Create Wind Force Field"));
	VectorFieldBeingEdited->Modify();

	VectorFieldBeingEdited->CreateWindForceField();
	SelectedForceFieldIds.Empty();
	SelectedForceFieldIds.Add(VectorFieldBeingEdited->ForceFields.Find(VectorFieldBeingEdited->ForceFields.Last()));
	VectorFieldDesignerViewportPtr->GetViewportClient().Get()->Invalidate();

	GEditor->EndTransaction();
}

void FVectorFieldDesignerWindow::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("VectorFieldDesignerWorkspaceMenuCategory", "VectorField Designer"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(VectorFieldDesignerDetailsID, FOnSpawnTab::CreateSP(this, &FVectorFieldDesignerWindow::SpawnTab_Details))
		.SetDisplayName(LOCTEXT("DetailsTab", "Details"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(VectorFieldDesignerViewportID, FOnSpawnTab::CreateSP(this, &FVectorFieldDesignerWindow::SpawnTab_Viewport))
		.SetDisplayName(LOCTEXT("ViewportTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewport"));
}

void FVectorFieldDesignerWindow::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(VectorFieldDesignerDetailsID);
	InTabManager->UnregisterTabSpawner(VectorFieldDesignerViewportID);
}

TSharedRef<SDockTab> FVectorFieldDesignerWindow::SpawnTab_Details(const FSpawnTabArgs & Args)
{
	TSharedPtr<FVectorFieldDesignerWindow> VectorFieldDesignerWindowPtr = SharedThis(this);

	auto DetailsPanel = SNew(SCustomizableVectorFieldPropertiesTabBody, VectorFieldDesignerWindowPtr);

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("DetailsTab", "Details"))
		[
			DetailsPanel
		];
}

TSharedRef<SDockTab> FVectorFieldDesignerWindow::SpawnTab_Viewport(const FSpawnTabArgs & Args)
{
	TSharedPtr<FVectorFieldDesignerWindow> VectorFieldDesignerWindowPtr = SharedThis(this);

	VectorFieldDesignerViewportPtr = SNew(SVectorFieldDesignerViewport)
		.VectorFieldBeingEdited(VectorFieldBeingEdited)
		.VectorFieldDesignerWindow(VectorFieldDesignerWindowPtr);

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Viewport"))
		.Label(LOCTEXT("ViewportTab", "Viewport"))
		[
			VectorFieldDesignerViewportPtr.ToSharedRef()
		];
}

void FVectorFieldDesignerWindow::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(VectorFieldBeingEdited);
}

void FVectorFieldDesignerWindow::AddSelectedForceField(int32 Index, bool bClearSelection)
{
	//check(IsForceFieldValid(Index));

	if (bClearSelection)
	{
		ClearSelectedForceFields();
	}

	if (IsForceFieldValid(Index))
		SelectedForceFieldIds.Add(Index);
}

void FVectorFieldDesignerWindow::RemoveSelectedForceField(int32 Index)
{
	SelectedForceFieldIds.Remove(Index);
}

void FVectorFieldDesignerWindow::ClearSelectedForceFields()
{
	SelectedForceFieldIds.Empty();
}

void FVectorFieldDesignerWindow::RemoveInvalidForceFields()
{
	for (int32 Index = SelectedForceFieldIds.Num() - 1; Index >= 0; --Index)
	{
		if (!IsForceFieldValid(Index))
		{
			SelectedForceFieldIds.RemoveAt(Index);
		}
	}
}

bool FVectorFieldDesignerWindow::IsSelectedForceField(int32 Index) const
{
	return SelectedForceFieldIds.Contains(Index);
}

bool FVectorFieldDesignerWindow::IsForceFieldValid(int32 Index) const
{
	return VectorFieldBeingEdited->ForceFields.IsValidIndex(Index);
}

bool FVectorFieldDesignerWindow::HasSelectedForceFields() const
{
	return SelectedForceFieldIds.Num() > 0;
}

bool FVectorFieldDesignerWindow::GetLastSelectedForceFieldTransform(FTransform& OutTransform) const
{
	if (HasSelectedForceFields())
	{
		int Index = SelectedForceFieldIds.Last();
		
		//IsForceFieldValid(Index);
		if (!IsForceFieldValid(Index))
			return false;

		OutTransform = VectorFieldBeingEdited->ForceFields[Index]->Transform;
	}

	return HasSelectedForceFields();
}

//inline static bool DescendingSortPredicate(const int32& e1, const int32& e2)
//{
//	return e1 > e2;
//}

void FVectorFieldDesignerWindow::DestroySelectedForceFields()
{
	GEditor->BeginTransaction(LOCTEXT("DestroySelectedForceFieldsTransaction", "Destroy Selected Force Fields"));
	VectorFieldBeingEdited->Modify();

	//SelectedForceFieldIds.Sort([](const int32& e1, const int32& e2)
	//{
	//	return e1 > e2;
	//});

	for (int32 i = SelectedForceFieldIds.Num() - 1; i >= 0; --i)
	{
		int Index = SelectedForceFieldIds[i];
		if (IsForceFieldValid(Index))
		{
			VectorFieldBeingEdited->ForceFields[Index]->ConditionalBeginDestroy();
			VectorFieldBeingEdited->ForceFields[Index] = nullptr;
			VectorFieldBeingEdited->ForceFields.RemoveAt(Index); // Here our force field is deleted and it alters all other ids
			SelectedForceFieldIds.RemoveAt(i);
		}
	}

	VectorFieldDesignerViewportPtr->GetViewportClient().Get()->Invalidate();
	GEditor->EndTransaction();
}

TArray<UObject*> FVectorFieldDesignerWindow::GetObjectsToObserve() const
{
	TArray<UObject*> ObjectsToObserve;

	if (!HasSelectedForceFields())
	{
		ObjectsToObserve.Add(VectorFieldBeingEdited);
	}
	else
	{
		for (int32 Index : SelectedForceFieldIds)
		{
			if (IsForceFieldValid(Index))
			{
				ObjectsToObserve.Add(VectorFieldBeingEdited->ForceFields[Index]);
			}
		}
	}

	return ObjectsToObserve;
}

void FVectorFieldDesignerWindow::TranslateSelectedForceFields(const FVector& DeltaDrag)
{
	for (int32 Index : SelectedForceFieldIds)
	{
		if (IsForceFieldValid(Index))
		{
			UForceFieldBase* ForceField = VectorFieldBeingEdited->ForceFields[Index];
			ForceField->Modify();
			ForceField->Transform.SetLocation(ForceField->Transform.GetLocation() + DeltaDrag);
			VectorFieldBeingEdited->MarkPackageDirty();
		}
	}
}

void FVectorFieldDesignerWindow::RotateSelectedForceFields(const FRotator& DeltaRotation)
{
	for (int32 Index : SelectedForceFieldIds)
	{
		if (IsForceFieldValid(Index))
		{
			UForceFieldBase* ForceField = VectorFieldBeingEdited->ForceFields[Index];
			ForceField->Modify();
			ForceField->Transform.SetRotation(ForceField->Transform.GetRotation() * DeltaRotation.Quaternion());
			VectorFieldBeingEdited->MarkPackageDirty();
		}
	}
}

void FVectorFieldDesignerWindow::ScaleSelectedForceFields(const FVector& DeltaScale)
{
	for (int32 Index : SelectedForceFieldIds)
	{
		if (IsForceFieldValid(Index))
		{
			UForceFieldBase* ForceField = VectorFieldBeingEdited->ForceFields[Index];
			ForceField->Modify();
			ForceField->Transform.SetScale3D(ForceField->Transform.GetScale3D() + DeltaScale);
			VectorFieldBeingEdited->MarkPackageDirty();
		}
	}
}

void FVectorFieldDesignerWindow::CreateVectorFieldStaticPackage(const FString& PackageName)
{
	UPackage* Package = CreatePackage(NULL, *PackageName);
	UPackage* OutermostPkg = Package->GetOutermost();
	UVectorFieldStatic* VectorField = NewObject<UVectorFieldStatic>(OutermostPkg, UVectorFieldStatic::StaticClass(), *FPaths::GetBaseFilename(PackageName), RF_Standalone | RF_Public);

	VectorField->SizeX = VectorFieldBeingEdited->GridX;
	VectorField->SizeY = VectorFieldBeingEdited->GridY;
	VectorField->SizeZ = VectorFieldBeingEdited->GridZ;
	VectorField->Bounds = VectorFieldBeingEdited->Bounds;

	// Convert vectors to 16-bit FP and store.
	const TArray<FVector> SrcValues = VectorFieldBeingEdited->CalculateVectorField();
	const int32 VectorCount = SrcValues.Num();
	const int32 DestBufferSize = VectorCount * sizeof(FFloat16Color);
	VectorField->SourceData.Lock(LOCK_READ_WRITE);
	FFloat16Color* RESTRICT DestValues = (FFloat16Color*)VectorField->SourceData.Realloc(DestBufferSize);
	int Index = 0;
	for (int32 VectorIndex = 0; VectorIndex < VectorCount; ++VectorIndex)
	{
		DestValues->R = SrcValues[VectorIndex].X;
		DestValues->G = SrcValues[VectorIndex].Y;
		DestValues->B = SrcValues[VectorIndex].Z;
		DestValues->A = 0.0f;
		++DestValues;
	}
	VectorField->SourceData.Unlock();

	VectorField->InitResource();

	FAssetRegistryModule::AssetCreated(VectorField);
	VectorField->MarkPackageDirty();
	Package->SetDirtyFlag(true);
	VectorField->PostEditChange();
	VectorField->AddToRoot();

	VectorFieldBeingEdited->AssetPath = VectorField->GetPathName();
	VectorFieldBeingEdited->MarkPackageDirty();

	// Save Package
	//TArray<UPackage*> PackagesToSave;
	//PackagesToSave.Add(Package);
	//FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, bCheckDirtyOnAssetSave, /*bPromptToSave=*/ false);
}

void FVectorFieldDesignerWindow::UpdateVectorFieldStaticPackage(const FString& ObjectPath)
{
	UVectorFieldStatic* VectorField = Cast<UVectorFieldStatic>(StaticLoadObject(UVectorFieldStatic::StaticClass(), NULL, *ObjectPath));

	VectorField->SizeX = VectorFieldBeingEdited->GridX;
	VectorField->SizeY = VectorFieldBeingEdited->GridY;
	VectorField->SizeZ = VectorFieldBeingEdited->GridZ;
	VectorField->Bounds = VectorFieldBeingEdited->Bounds;

	// Convert vectors to 16-bit FP and store.
	const TArray<FVector> SrcValues = VectorFieldBeingEdited->CalculateVectorField();
	const int32 VectorCount = SrcValues.Num();
	const int32 DestBufferSize = VectorCount * sizeof(FFloat16Color);
	VectorField->SourceData.Lock(LOCK_READ_WRITE);
	FFloat16Color* RESTRICT DestValues = (FFloat16Color*)VectorField->SourceData.Realloc(DestBufferSize);
	int Index = 0;
	for (int32 VectorIndex = 0; VectorIndex < VectorCount; ++VectorIndex)
	{
		DestValues->R = SrcValues[VectorIndex].X;
		DestValues->G = SrcValues[VectorIndex].Y;
		DestValues->B = SrcValues[VectorIndex].Z;
		DestValues->A = 0.0f;
		++DestValues;
	}
	VectorField->SourceData.Unlock();

	VectorField->MarkPackageDirty();
	VectorField->PostEditChange();
	VectorField->AddToRoot();

	// Save Package
	//TArray<UPackage*> PackagesToSave;
	//PackagesToSave.Add(VectorField->GetOutermost());
	//FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, bCheckDirtyOnAssetSave, /*bPromptToSave=*/ false);
}

bool FVectorFieldDesignerWindow::DeleteVectorFieldStaticPackage(const FString& ObjectPath)
{
	//if (FPackageName::DoesPackageExist(FPackageName::FilenameToLongPackageName(ObjectPath)))
	//{
		//if (GIsEditor)
		//{
		//	UEditorEngine* Editor = GEditor;
		//	FWorldContext* PIEWorldContext = GEditor->GetPIEWorldContext();
		//	if (PIEWorldContext)
		//	{
		//		FNotificationInfo Notification(LOCTEXT("CannotDeleteAssetInPIE", "Assets cannot be deleted while in PIE."));
		//		Notification.ExpireDuration = 3.0f;
		//		FSlateNotificationManager::Get().AddNotification(Notification);
		//		return;
		//	}
		//}

		UVectorFieldStatic* VectorField = Cast<UVectorFieldStatic>(StaticLoadObject(UVectorFieldStatic::StaticClass(), NULL, *ObjectPath));
//		UE_LOG(LogTemp, Warning, TEXT("Asset to delete: %s"), *ObjectPath);

		TArray<UObject*> ObjectsToDelete;
		ObjectsToDelete.Add(VectorField);
		int32 DeletedAssetsCount = ObjectTools::DeleteObjects(ObjectsToDelete, true);
//		UE_LOG(LogTemp, Warning, TEXT("Deleted Assets: %d"), DeletedAssetsCount);
		return DeletedAssetsCount > 0;
	//}

	//return false;
}

#undef LOCTEXT_NAMESPACE