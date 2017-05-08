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

#define LOCTEXT_NAMESPACE "FVectorFieldDesignerWindow"

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
			ToolbarBuilder.BeginSection("Toolbar");
			{

				TSharedPtr<FUICommandInfo> CreateSphericalForceFieldCommand = FVectorFieldDesignerCommands::Get().CreateSphericalForceField;
				ToolbarBuilder.AddToolBarButton(CreateSphericalForceFieldCommand,
					NAME_None,
					FText::FromString(TEXT("Spherical")), // CreateSphericalForceFieldCommand->GetLabel(),
					CreateSphericalForceFieldCommand->GetDescription()
					//,
					//FSlateIcon(FEditorStyle::GetStyleSetName(), "SoundCueGraphEditor.PlayCue")
				);

				TSharedPtr<FUICommandInfo> CreateVortexForceFieldCommand = FVectorFieldDesignerCommands::Get().CreateVortexForceField;
				ToolbarBuilder.AddToolBarButton(CreateVortexForceFieldCommand,
					NAME_None,
					FText::FromString(TEXT("Vortex")), // CreateVortexForceFieldCommand->GetLabel(),
					CreateVortexForceFieldCommand->GetDescription()
					//,
					//FSlateIcon(FEditorStyle::GetStyleSetName(), "SoundCueGraphEditor.StopCueNode")
				);

				TSharedPtr<FUICommandInfo> CreateWindForceFieldCommand = FVectorFieldDesignerCommands::Get().CreateWindForceField;
				ToolbarBuilder.AddToolBarButton(CreateWindForceFieldCommand,
					NAME_None,
					FText::FromString(TEXT("Wind")), // CreateWindForceFieldCommand->GetLabel(),
					CreateWindForceFieldCommand->GetDescription()
					//,
					//FSlateIcon(FEditorStyle::GetStyleSetName(), "SoundCueGraphEditor.StopCueNode")
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

void FVectorFieldDesignerWindow::CreateSphericalForceField()
{
	VectorFieldBeingEdited->CreateSphericalForceField();
	SelectedForceFieldIds.Empty();
	SelectedForceFieldIds.Add(VectorFieldBeingEdited->ForceFields.Find(VectorFieldBeingEdited->ForceFields.Last()));
}

void FVectorFieldDesignerWindow::CreateVortexForceField()
{
	VectorFieldBeingEdited->CreateVortexForceField();
	SelectedForceFieldIds.Empty();
	SelectedForceFieldIds.Add(VectorFieldBeingEdited->ForceFields.Find(VectorFieldBeingEdited->ForceFields.Last()));
}

void FVectorFieldDesignerWindow::CreateWindForceField()
{
	VectorFieldBeingEdited->CreateWindForceField();
	SelectedForceFieldIds.Empty();
	SelectedForceFieldIds.Add(VectorFieldBeingEdited->ForceFields.Find(VectorFieldBeingEdited->ForceFields.Last()));
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

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Viewport"))
		.Label(LOCTEXT("ViewportTab", "Viewport"))
		[
			SNew(SVectorFieldDesignerViewport)
			.VectorFieldBeingEdited(VectorFieldBeingEdited)
			.VectorFieldDesignerWindow(VectorFieldDesignerWindowPtr)
		];
}

void FVectorFieldDesignerWindow::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(VectorFieldBeingEdited);
}

void FVectorFieldDesignerWindow::AddSelectedForceField(int Index, bool bClearSelection)
{
	check(IsForceFieldValid(Index));

	if (bClearSelection)
	{
		ClearSelectedForceFields();
	}

	SelectedForceFieldIds.Add(Index);
}

void FVectorFieldDesignerWindow::RemoveSelectedForceField(int Index)
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

bool FVectorFieldDesignerWindow::IsSelectedForceField(int Index) const
{
	return SelectedForceFieldIds.Contains(Index);
}

bool FVectorFieldDesignerWindow::IsForceFieldValid(int Index) const
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

void FVectorFieldDesignerWindow::DestroySelectedForceFields()
{
	for (int32 Index = SelectedForceFieldIds.Num() - 1; Index >= 0; --Index)
	{
		if (IsForceFieldValid(Index))
		{
			VectorFieldBeingEdited->ForceFields[SelectedForceFieldIds[Index]]->ConditionalBeginDestroy();
			VectorFieldBeingEdited->ForceFields[SelectedForceFieldIds[Index]] = nullptr;
			VectorFieldBeingEdited->ForceFields.RemoveAt(SelectedForceFieldIds[Index]);
			SelectedForceFieldIds.RemoveAt(Index);
		}
	}
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
		for (auto& Index : SelectedForceFieldIds)
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
	for (int Index : SelectedForceFieldIds)
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
	for (int Index : SelectedForceFieldIds)
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
	for (int Index : SelectedForceFieldIds)
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

#undef LOCTEXT_NAMESPACE