// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VectorFieldDesignerEditor.h"
#include "CustomizableVectorField.h"
#include "SlateCore.h"
#include "CoreMinimal.h"
#include "UObject/GCObject.h"
#include "EditorUndoClient.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "IDetailsView.h"

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

	virtual void ExtendToolbar();
	virtual void BindEditorCommands();

	void SelectAll();

	void Cut();
	void Copy();
	void Paste();
	void Duplicate();
	void DuplicateInternal();

	bool CanPaste() const;

	void ExportVectorField();
	void ExportVectorFieldAs();

	void CreateSphericalForceField();
	void CreateVortexForceField();
	void CreateWindForceField();

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;

	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_ForceFieldsList(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);

	// Inherited via FGCObject
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	UCustomizableVectorField* GetVectorFieldBeingEdited() const { return VectorFieldBeingEdited; }

	void AddSelectedForceField(int32 Index, bool bClearSelection);
	void RemoveSelectedForceField(int32 Index);
	void ClearSelectedForceFields();
	void RemoveInvalidForceFields();
	bool IsSelectedForceField(int32 Index) const;
	bool IsForceFieldValid(int32 Index) const;
	bool HasSelectedForceFields() const;
	bool GetLastSelectedForceFieldTransform(FTransform& OutTransform) const;
	
	void DestroySelectedForceFields();

	TArray<UObject*> GetObjectsToObserve() const;

	void TranslateSelectedForceFields(const FVector& DeltaDrag);
	void RotateSelectedForceFields(const FRotator& DeltaRotation);
	void ScaleSelectedForceFields(const FVector& DeltaScale);

	FOnFinishedChangingProperties OnFinishedChangingPropertiesDelegate;

private:
	void CreateVectorFieldStaticPackage(const FString& PackageName);
	void UpdateVectorFieldStaticPackage(const FString& ObjectPath);
	bool DeleteVectorFieldStaticPackage(const FString& ObjectPath);

	void OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent);

	UCustomizableVectorField* VectorFieldBeingEdited;
	TSharedPtr<class SVectorFieldDesignerViewport> VectorFieldDesignerViewportPtr;
	TArray<int32> SelectedForceFieldIds;

};
