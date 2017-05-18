// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorViewportClient.h"
#include "VectorFieldDesignerWindow.h"
#include "SEditorViewport.h"
#include "PreviewScene.h"

/**
 * 
 */
class FVFDesignerViewportClient : public FEditorViewportClient
{
public:
	/** Constructor */
	FVFDesignerViewportClient(TWeakPtr<FVectorFieldDesignerWindow> InVectorFieldDesignerEditor, const TAttribute<class UCustomizableVectorField*>& InVectorFieldBeingEdited, const TWeakPtr<SEditorViewport>& InEditorViewportWidget = nullptr);

	// FViewportClient interface
	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual void DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas) override;
	virtual void Tick(float DeltaSeconds) override;

	virtual FWidget::EWidgetMode GetWidgetMode() const override;
	virtual bool InputWidgetDelta(FViewport* InViewport, EAxisList::Type CurrentAxis, FVector& Drag, FRotator& Rot, FVector& Scale) override;

	virtual void TrackingStarted(const struct FInputEventState& InInputState, bool bIsDragging, bool bNudge) override;
	virtual void TrackingStopped() override;

	virtual void SetWidgetMode(FWidget::EWidgetMode NewMode) override;
	virtual bool CanSetWidgetMode(FWidget::EWidgetMode NewMode) const override;
	virtual bool CanCycleWidgetMode() const override;
	virtual FVector GetWidgetLocation() const override;
	virtual FMatrix GetWidgetCoordSystem() const override;
	virtual ECoordSystem GetWidgetCoordSystemSpace() const override { return COORD_Local; }
	// End of FViewportClient interface

	// FEditorViewportClient interface
	virtual void ProcessClick(class FSceneView& View, class HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;
	virtual bool InputKey(FViewport* InViewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool bGamepad) override;
	virtual FLinearColor GetBackgroundColor() const override;
	// End of FEditorViewportClient interface

	// FSerializableObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	// End of FSerializableObject interface

	void SetPreviewParticleSystem(UParticleSystem* PreviewParticleSystem);

	void Invalidate() { UE_LOG(LogTemp, Warning, TEXT("Called")); FEditorViewportClient::Invalidate(); };
private:
	TAttribute<UCustomizableVectorField*> VectorFieldBeingEdited;
	TWeakPtr<FVectorFieldDesignerWindow> VectorFieldDesignerEditorPtr;
	FPreviewScene OwnedPreviewScene;

	class UParticleSystemComponent* PreviewParticleSystemComponent;
	class UVectorFieldStatic* PreviewVectorFieldStaticInstance;

	FWidget::EWidgetMode WidgetMode;

	bool bManipulating;
};
