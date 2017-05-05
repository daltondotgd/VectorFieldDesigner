// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SEditorViewport.h"
#include "SCommonEditorViewportToolbarBase.h"

#include "VectorFieldDesignerEditor.h"
#include "CustomizableVectorField.h"
#include "VFDesignerViewportClient.h"

class SVectorFieldDesignerViewportToolbar : public SCommonEditorViewportToolbarBase
{
public:
	SLATE_BEGIN_ARGS(SVectorFieldDesignerViewportToolbar) {}
	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, TSharedPtr<class ICommonEditorViewportToolbarInfoProvider> InInfoProvider);

	// SCommonEditorViewportToolbarBase interface
	virtual TSharedRef<SWidget> GenerateShowMenu() const override;
	// End of SCommonEditorViewportToolbarBase
};

/**
 * 
 */
class SVectorFieldDesignerViewport : public SEditorViewport, public ICommonEditorViewportToolbarInfoProvider
{
public:
	SLATE_BEGIN_ARGS(SVectorFieldDesignerViewport)
		: _VectorFieldBeingEdited((UCustomizableVectorField*)nullptr)
	{}

		SLATE_ATTRIBUTE(UCustomizableVectorField*, VectorFieldBeingEdited)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// SEditorViewport interface
	virtual void BindCommands() override;
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	virtual TSharedPtr<SWidget> MakeViewportToolbar() override;
	virtual EVisibility GetTransformToolbarVisibility() const override;
	virtual void OnFocusViewportToSelection() override;
	// End of SEditorViewport interface

	// ICommonEditorViewportToolbarInfoProvider interface
	virtual TSharedRef<class SEditorViewport> GetViewportWidget() override;
	virtual TSharedPtr<FExtender> GetExtenders() const override;
	virtual void OnFloatingButtonClicked() override;
	// End of ICommonEditorViewportToolbarInfoProvider interface

private:
	TAttribute<UCustomizableVectorField*> VectorFieldBeingEdited;
	TSharedPtr<FVFDesignerViewportClient> EditorViewportClient;

};
