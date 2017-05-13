// Fill out your copyright notice in the Description page of Project Settings.

#include "VectorFieldDesignerEditor.h"
#include "VFDesignerViewportClient.h"
#include "CustomizableVectorField.h"
#include "AssetEditorModeManager.h"

#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "Utils.h"

#include "SceneManagement.h" // for primitive drawing
#include "PhysicsEngine/ConvexElem.h"
#include "PhysicsEngine/BoxElem.h"
#include "PhysicsEngine/SphereElem.h"
#include "PhysicsEngine/SphylElem.h"

#define LOCTEXT_NAMESPACE "FVFDesignerViewportClient"

struct HSMEForceFieldProxy : public HHitProxy
{
	DECLARE_HIT_PROXY();

	int ForceFieldIndex;

	HSMEForceFieldProxy(int InForceFieldIndex) :
		HHitProxy(HPP_UI),
		ForceFieldIndex(InForceFieldIndex) {}

};
IMPLEMENT_HIT_PROXY(HSMEForceFieldProxy, HHitProxy);

FVFDesignerViewportClient::FVFDesignerViewportClient(TWeakPtr<FVectorFieldDesignerWindow> InVectorFieldDesignerEditor, const TAttribute<UCustomizableVectorField*>& InVectorFieldBeingEdited, const TWeakPtr<SEditorViewport>& InEditorViewportWidget)
	: FEditorViewportClient(new FAssetEditorModeManager(), nullptr, InEditorViewportWidget)
	, VectorFieldDesignerEditorPtr(InVectorFieldDesignerEditor)
{
	VectorFieldBeingEdited = InVectorFieldBeingEdited;
	PreviewScene = &OwnedPreviewScene;

	bOwnsModeTools = true;
	WidgetMode = FWidget::WM_None;

	SetRealtime(true);

	DrawHelper.bDrawGrid = true;

	EngineShowFlags.DisableAdvancedFeatures();
	EngineShowFlags.SetCompositeEditorPrimitives(true);

	Invalidate();

	SetInitialViewTransform(LVT_Perspective, FVector(150.0f, 150.0f, 100.0f), FRotator(-30.0f, -135.0f, 0.0f), 0.0f);
}

void FVFDesignerViewportClient::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	FEditorViewportClient::Draw(View, PDI);

	auto VectorFieldDesignerEditor = VectorFieldDesignerEditorPtr.Pin();
	if (!VectorFieldDesignerEditor.IsValid())
	{
		return;
	}

	const FColor SelectedColor(149, 223, 157);
	const FColor UnselectedColor(157, 149, 223);

	for (int32 Index = 0; Index < VectorFieldBeingEdited.Get()->ForceFields.Num(); ++Index)
	{
		HSMEForceFieldProxy* HHitProxy = new HSMEForceFieldProxy(Index);
		PDI->SetHitProxy(HHitProxy);
		VectorFieldBeingEdited.Get()->ForceFields[Index]->Draw(PDI, VectorFieldDesignerEditor->IsSelectedForceField(Index) ? SelectedColor : UnselectedColor);
		PDI->SetHitProxy(NULL);
	}

	FBox Bounds = VectorFieldBeingEdited.Get()->Bounds;
	DrawWireBox(PDI, Bounds, FColor::Blue, SDPG_World, 1.0f);

	int Resolution = VectorFieldBeingEdited.Get()->Resolution + 1;

	for (int i = 0; i < Resolution; ++i)
	{
		for (int j = 0; j < Resolution; ++j)
		{
			for (int k = 0; k < Resolution; ++k)
			{
				FVector Location = Bounds.GetExtent() * 2.0f * (FVector((float)i, (float)j, (float)k) + 0.5f) / FVector((float)Resolution) + Bounds.Min;
				FVector Direction = FVector(0.0f);
				FColor Color = FColor::Green;
				float Thickness = 0.0f;
				uint8 Depth = SDPG_World;

				for (int32 Index = 0; Index < VectorFieldBeingEdited.Get()->ForceFields.Num(); ++Index)
				{
					UForceFieldBase* ForceField = VectorFieldBeingEdited.Get()->ForceFields[Index];
					if (ForceField->IsInRange(Location))
					{
						Direction += ForceField->ForceAtLocation(Location) * ForceField->Force;
						if (VectorFieldDesignerEditor->IsSelectedForceField(Index))
						{
							Color = FColor::Cyan;
							Thickness = 0.5f;
							Depth = SDPG_Foreground;
						}
					}
				}

				if (Direction.Size() < KINDA_SMALL_NUMBER)
				{
					PDI->DrawPoint(Location, Color, 1.0f, SDPG_World);
				}
				else
				{
					//PDI->DrawLine(Location, Location + Direction, Color, SDPG_World, Thickness);
					FTransform Transform;
					Transform.SetLocation(Location);
					Transform.SetRotation(FRotationMatrix::MakeFromX(Direction).ToQuat());
					DrawDirectionalArrow(PDI, Transform.ToMatrixNoScale(), Color, Direction.Size(), 1.0f, Depth, Thickness);
				}
			}
		}
	}
}

void FVFDesignerViewportClient::DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas)
{
	FEditorViewportClient::DrawCanvas(InViewport, View, Canvas);

	//const bool bIsHitTesting = Canvas.IsHitTesting();
	//if (!bIsHitTesting)
	//{
	//	Canvas.SetHitProxy(nullptr);
	//}
}

void FVFDesignerViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);

	if (VectorFieldDesignerEditorPtr.Pin()->HasSelectedForceFields() && WidgetMode == FWidget::WM_None)
	{
		WidgetMode = FWidget::WM_Translate;
	}

	if (!GIntraFrameDebuggingGameThread)
	{
		OwnedPreviewScene.GetWorld()->Tick(LEVELTICK_All, DeltaSeconds);
	}
}

FWidget::EWidgetMode FVFDesignerViewportClient::GetWidgetMode() const
{
	if (VectorFieldDesignerEditorPtr.Pin()->HasSelectedForceFields())
		return WidgetMode;
	else
		return FWidget::WM_None;
}

bool FVFDesignerViewportClient::InputWidgetDelta(FViewport* InViewport, EAxisList::Type CurrentAxis, FVector& Drag, FRotator& Rot, FVector& Scale)
{
	bool bHandled = false;
	if (bManipulating)
	{
		if (WidgetMode == FWidget::WM_Translate)
		{
			VectorFieldDesignerEditorPtr.Pin()->TranslateSelectedForceFields(Drag);
		}

		else if (WidgetMode == FWidget::WM_Rotate)
		{
			VectorFieldDesignerEditorPtr.Pin()->RotateSelectedForceFields(Rot);
		}

		else if (WidgetMode == FWidget::WM_Scale)
		{
			VectorFieldDesignerEditorPtr.Pin()->ScaleSelectedForceFields(Scale);
		}

		Invalidate();
		bHandled = true;
	}

	return bHandled;
}

void FVFDesignerViewportClient::TrackingStarted(const FInputEventState& InInputState, bool bIsDragging, bool bNudge)
{
	if (!bManipulating && bIsDragging)
	{
		bManipulating = true;

		FText TransactionText;
		switch (WidgetMode)
		{
			case FWidget::WM_Translate:
				TransactionText = LOCTEXT("VectorFieldTransaction", "Translate ForceField");
				break;
			case FWidget::WM_Rotate:
				TransactionText = LOCTEXT("VectorFieldTransaction", "Rotate ForceField");
				break;
			case FWidget::WM_Scale:
				TransactionText = LOCTEXT("VectorFieldTransaction", "Scale ForceField");
				break;
		}

		GEditor->BeginTransaction(TransactionText);
	}
}

void FVFDesignerViewportClient::TrackingStopped()
{
	if (bManipulating)
	{
		bManipulating = false;
		GEditor->EndTransaction();
	}
}

void FVFDesignerViewportClient::SetWidgetMode(FWidget::EWidgetMode NewMode)
{
	WidgetMode = NewMode;
	Invalidate();
}

bool FVFDesignerViewportClient::CanSetWidgetMode(FWidget::EWidgetMode NewMode) const
{
	return !Widget->IsDragging() && VectorFieldDesignerEditorPtr.Pin()->HasSelectedForceFields();
}

bool FVFDesignerViewportClient::CanCycleWidgetMode() const
{
	return !Widget->IsDragging() && VectorFieldDesignerEditorPtr.Pin()->HasSelectedForceFields();
}

FVector FVFDesignerViewportClient::GetWidgetLocation() const
{
	FTransform Transform = FTransform::Identity;
	const bool bSelectedForceField = VectorFieldDesignerEditorPtr.Pin()->GetLastSelectedForceFieldTransform(Transform);
	if (bSelectedForceField)
	{
		return Transform.GetLocation();
	}

	return FVector::ZeroVector;
}

FMatrix FVFDesignerViewportClient::GetWidgetCoordSystem() const
{
	FTransform Transform = FTransform::Identity;
	const bool bSelectedForceField = VectorFieldDesignerEditorPtr.Pin()->GetLastSelectedForceFieldTransform(Transform);
	if (bSelectedForceField)
	{
		return FRotationMatrix(Transform.GetRotation().Rotator());
	}

	return FMatrix::Identity;
}

void FVFDesignerViewportClient::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
	auto VectorFieldDesignerEditor = VectorFieldDesignerEditorPtr.Pin();
	if (!VectorFieldDesignerEditor.IsValid())
	{
		return;
	}

	const bool bCtrlDown = Viewport->KeyState(EKeys::LeftControl) || Viewport->KeyState(EKeys::RightControl);
	const bool bShiftDown = Viewport->KeyState(EKeys::LeftShift) || Viewport->KeyState(EKeys::RightShift);
	const bool bShouldMultiSelect = bCtrlDown && bShiftDown;

	bool bClearSelectedForceFields = true;

	if (HitProxy)
	{
		if (HitProxy->IsA(HSMEForceFieldProxy::StaticGetType()))
		{
			HSMEForceFieldProxy* ForceFieldProxy = (HSMEForceFieldProxy*)HitProxy;

			if (VectorFieldDesignerEditor->IsSelectedForceField(ForceFieldProxy->ForceFieldIndex))
			{
				if (!bShouldMultiSelect)
				{
					VectorFieldDesignerEditor->AddSelectedForceField(ForceFieldProxy->ForceFieldIndex, true);
				}
				else
				{
					VectorFieldDesignerEditor->RemoveSelectedForceField(ForceFieldProxy->ForceFieldIndex);
				}
			}
			else
			{
				VectorFieldDesignerEditor->AddSelectedForceField(ForceFieldProxy->ForceFieldIndex, !bShouldMultiSelect);
			}
		}

		if (WidgetMode == FWidget::WM_None)
			WidgetMode = FWidget::WM_Translate;

		bClearSelectedForceFields = false;
	}

	if (bClearSelectedForceFields)
	{
		VectorFieldDesignerEditor->ClearSelectedForceFields();
	}

	Invalidate();
}

bool FVFDesignerViewportClient::InputKey(FViewport* InViewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool bGamepad)
{
	bool bHandled = false;

	if (Key == EKeys::Delete)
	{
		VectorFieldDesignerEditorPtr.Pin()->DestroySelectedForceFields();
		bHandled = true;
	}

	return bHandled ? true : FEditorViewportClient::InputKey(InViewport, ControllerId, Key, Event, AmountDepressed, bGamepad);
}

FLinearColor FVFDesignerViewportClient::GetBackgroundColor() const
{
	return FColor(55, 55, 55);
}

void FVFDesignerViewportClient::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEditorViewportClient::AddReferencedObjects(Collector);
}

#undef LOCTEXT_NAMESPACE 