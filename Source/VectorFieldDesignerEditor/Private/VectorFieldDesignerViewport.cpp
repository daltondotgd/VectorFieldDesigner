// Fill out your copyright notice in the Description page of Project Settings.

#include "VectorFieldDesignerEditor.h"
#include "VectorFieldDesignerViewport.h"

void SVectorFieldDesignerViewportToolbar::Construct(const FArguments& InArgs, TSharedPtr<class ICommonEditorViewportToolbarInfoProvider> InInfoProvider)
{
	SCommonEditorViewportToolbarBase::Construct(SCommonEditorViewportToolbarBase::FArguments(), InInfoProvider);
}

TSharedRef<SWidget> SVectorFieldDesignerViewportToolbar::GenerateShowMenu() const
{
	GetInfoProvider().OnFloatingButtonClicked();

	TSharedRef<SEditorViewport> ViewportRef = GetInfoProvider().GetViewportWidget();

	const bool bInShouldCloseWindowAfterMenuSelection = true;
	FMenuBuilder ShowMenuBuilder(bInShouldCloseWindowAfterMenuSelection, ViewportRef->GetCommandList());
	{
		//ShowMenuBuilder.AddMenuEntry(FFlipbookEditorCommands::Get().SetShowSockets);
		//ShowMenuBuilder.AddMenuEntry(FFlipbookEditorCommands::Get().SetShowPivot);

		ShowMenuBuilder.AddMenuSeparator();

		//ShowMenuBuilder.AddMenuEntry(FFlipbookEditorCommands::Get().SetShowGrid);
		//ShowMenuBuilder.AddMenuEntry(FFlipbookEditorCommands::Get().SetShowBounds);

		ShowMenuBuilder.AddMenuSeparator();

		//ShowMenuBuilder.AddMenuEntry(FFlipbookEditorCommands::Get().SetShowCollision);
	}

	return ShowMenuBuilder.MakeWidget();
}

void SVectorFieldDesignerViewport::Construct(const FArguments& InArgs)
{
	VectorFieldBeingEdited = InArgs._VectorFieldBeingEdited;
	VectorFieldDesignerWindowPtr = InArgs._VectorFieldDesignerWindow;

	SEditorViewport::Construct(SEditorViewport::FArguments());
}

void SVectorFieldDesignerViewport::BindCommands()
{
	SEditorViewport::BindCommands();

}

TSharedRef<FEditorViewportClient> SVectorFieldDesignerViewport::MakeEditorViewportClient()
{
	EditorViewportClient = MakeShareable(new FVFDesignerViewportClient(VectorFieldDesignerWindowPtr, VectorFieldBeingEdited));

	return EditorViewportClient.ToSharedRef();
}

TSharedPtr<SWidget> SVectorFieldDesignerViewport::MakeViewportToolbar()
{
	return SNew(SVectorFieldDesignerViewportToolbar, SharedThis(this));
}

EVisibility SVectorFieldDesignerViewport::GetTransformToolbarVisibility() const
{
	return EVisibility::Visible;
}

void SVectorFieldDesignerViewport::OnFocusViewportToSelection()
{
	EditorViewportClient->FocusViewportOnBox(VectorFieldBeingEdited.Get()->Bounds);
}

TSharedRef<class SEditorViewport> SVectorFieldDesignerViewport::GetViewportWidget()
{
	return SharedThis(this);
}

TSharedPtr<FExtender> SVectorFieldDesignerViewport::GetExtenders() const
{
	TSharedPtr<FExtender> Result(MakeShareable(new FExtender));
	return Result;
}

void SVectorFieldDesignerViewport::OnFloatingButtonClicked()
{
}
