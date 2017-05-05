// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VectorFieldDesignerEditor.h"
#include "CustomizableVectorField.h"
#include "SlateCore.h"
#include "CoreMinimal.h"
#include "UObject/GCObject.h"
#include "EditorUndoClient.h"
#include "Toolkits/AssetEditorToolkit.h"

/**
 * 
 */
class FVectorFieldDesignerWindow : public FAssetEditorToolkit, public FGCObject, public FEditorUndoClient
{
public:
	void InitVectorFieldDesignerWindow(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class UCustomizableVectorField* InitVectorField);

	~FVectorFieldDesignerWindow();

	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;

	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);

	//TSharedPtr<class SCheapTuneCompoundWidget> CheapTuneCompoundWidgetPtr;

	// Inherited via FGCObject
	virtual void AddReferencedObjects(FReferenceCollector & Collector) override;

	UCustomizableVectorField* GetVectorFieldBeingEdited() const { return VectorFieldBeingEdited; }

private:
	UCustomizableVectorField* VectorFieldBeingEdited;
};
