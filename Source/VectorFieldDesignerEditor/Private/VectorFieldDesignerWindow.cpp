// Fill out your copyright notice in the Description page of Project Settings.

#include "VectorFieldDesignerEditor.h"
#include "VectorFieldDesignerWindow.h"
#include "VectorFieldDesignerViewport.h"

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

		SSingleObjectDetailsPanel::Construct(SSingleObjectDetailsPanel::FArguments().HostCommandList(InVectorFieldDesigner->GetToolkitCommands()).HostTabManager(InVectorFieldDesigner->GetTabManager()), /*bAutomaticallyObserveViaGetObjectToObserve=*/ true, /*bAllowSearch=*/ true);
	}

	virtual UObject* GetObjectToObserve() const override
	{
		return VectorFieldDesignerPtr.Pin()->GetVectorFieldBeingEdited();
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

FLinearColor FVectorFieldDesignerWindow::GetWorldCentricTabColorScale() const
{
	return FLinearColor::Red;
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

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("DetailsTab", "Details"))
		[
			SNew(SCustomizableVectorFieldPropertiesTabBody, VectorFieldDesignerWindowPtr)
		];
}

TSharedRef<SDockTab> FVectorFieldDesignerWindow::SpawnTab_Viewport(const FSpawnTabArgs & Args)
{
	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Viewport"))
		.Label(LOCTEXT("ViewportTab", "Viewport"))
		[
			SNew(SVectorFieldDesignerViewport)
			.VectorFieldBeingEdited(VectorFieldBeingEdited)
		];
}

void FVectorFieldDesignerWindow::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(VectorFieldBeingEdited);
}

#undef LOCTEXT_NAMESPACE