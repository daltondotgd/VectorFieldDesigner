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

//struct HSMEForceFieldProxy : public HHitProxy
//{
//	DECLARE_HIT_PROXY();
//
//	//IStaticMeshEditor::FPrimData	PrimData;
//
//	//HSMECollisionProxy(const IStaticMeshEditor::FPrimData& InPrimData) :
//	//	HHitProxy(HPP_UI),
//	//	PrimData(InPrimData) {}
//
//	//HSMECollisionProxy(EKCollisionPrimitiveType InPrimType, int32 InPrimIndex) :
//	//	HHitProxy(HPP_UI),
//	//	PrimData(InPrimType, InPrimIndex) {}
//};
//IMPLEMENT_HIT_PROXY(HSMEForceFieldProxy, HHitProxy);

FVFDesignerViewportClient::FVFDesignerViewportClient(const TAttribute<UCustomizableVectorField*>& InVectorFieldBeingEdited, const TWeakPtr<SEditorViewport>& InEditorViewportWidget)
	: FEditorViewportClient(new FAssetEditorModeManager(), nullptr, InEditorViewportWidget)
{
	VectorFieldBeingEdited = InVectorFieldBeingEdited;
	PreviewScene = &OwnedPreviewScene;

	bOwnsModeTools = true;

	SetRealtime(true);

	DrawHelper.bDrawGrid = true;

	EngineShowFlags.DisableAdvancedFeatures();
	EngineShowFlags.SetCompositeEditorPrimitives(true);

	SetInitialViewTransform(LVT_Perspective, FVector(150.0f, 150.0f, 100.0f), FRotator(-30.0f, -135.0f, 0.0f), 0.0f);
}

void FVFDesignerViewportClient::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	FEditorViewportClient::Draw(View, PDI);

	//PDI->DrawLine(FVector::ZeroVector, FVector(1.0f, 1.0f, 1.0f) * 50.0f, FColor::Cyan, 0, 5.0f);

	//HSMECollisionProxy* HitProxy = new HSMECollisionProxy(KPT_Sphere, i);
	//PDI->SetHitProxy(HitProxy);

	const FColor SelectedColor(149, 223, 157);
	const FColor UnselectedColor(157, 149, 223);

	//HHitProxy* HitProxy = new HHitProxy(HPP_Wireframe);
	//const FColor CollisionColor = SelectedPrims.Contains(HitProxy->PrimData) ? SelectedColor : UnselectedColor;
	FKSphereElem SphereElem(TestScale.X);
	SphereElem.Center = TestLocation;
	const FTransform ElemTM = SphereElem.GetTransform();
	SphereElem.DrawElemWire(PDI, ElemTM, FVector(1.0f), FColor::Red);
	//PDI->SetHitProxy(NULL);

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
				FVector Direction = FVector(5.0f);
				FColor Color = FColor::Green;
				if (FVector::Distance(Location, TestLocation) < TestScale.X)
				{
					Direction = Location - TestLocation;
					float Length = 1.0f - Direction.Size() / TestScale.X;
					
					Direction.Normalize();
					Direction *= 20.0f * Length;
					
					Color = FColor::Cyan;
				}
				PDI->DrawLine(Location, Location + Direction, Color, SDPG_World);
			}
		}
	}

	FUnrealEdUtils::DrawWidget(View, PDI, FMatrix::Identity, 0, 0, EAxisList::Screen, EWidgetMovementMode::WMM_Translate);
}

void FVFDesignerViewportClient::DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas)
{
	FEditorViewportClient::DrawCanvas(InViewport, View, Canvas);

	const bool bIsHitTesting = Canvas.IsHitTesting();
	if (!bIsHitTesting)
	{
		Canvas.SetHitProxy(nullptr);
	}
}

void FVFDesignerViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);

	if (!GIntraFrameDebuggingGameThread)
	{
		OwnedPreviewScene.GetWorld()->Tick(LEVELTICK_All, DeltaSeconds);
	}
}

FWidget::EWidgetMode FVFDesignerViewportClient::GetWidgetMode() const
{
	return WidgetMode;
}

bool FVFDesignerViewportClient::InputWidgetDelta(FViewport* InViewport, EAxisList::Type CurrentAxis, FVector& Drag, FRotator& Rot, FVector& Scale)
{
	bool bHandled = false;

	if (bManipulating)
	{
		if (WidgetMode == FWidget::WM_Translate)
		{
			TestLocation += Drag;
		}

		if (WidgetMode == FWidget::WM_Scale)
		{
			TestScale.X += Scale.GetMax() >= KINDA_SMALL_NUMBER ? Scale.GetMax() : Scale.GetMin();
		}

		VectorFieldBeingEdited.Get()->MarkPackageDirty();

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
		//GEditor->BeginTransaction(FText::FromString(TEXT("Translation")));
	}
}

void FVFDesignerViewportClient::TrackingStopped()
{
	if (bManipulating)
	{
		bManipulating = false;
		//GEditor->EndTransaction();
	}
}

void FVFDesignerViewportClient::SetWidgetMode(FWidget::EWidgetMode NewMode)
{
	WidgetMode = NewMode;
	Invalidate();
}

bool FVFDesignerViewportClient::CanSetWidgetMode(FWidget::EWidgetMode NewMode) const
{
	return true;
}

bool FVFDesignerViewportClient::CanCycleWidgetMode() const
{
	return true;
}

FVector FVFDesignerViewportClient::GetWidgetLocation() const
{
	return TestLocation;
}

FMatrix FVFDesignerViewportClient::GetWidgetCoordSystem() const
{
	return FMatrix::Identity;
}

void FVFDesignerViewportClient::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
	if (WidgetMode == FWidget::WM_None)
		WidgetMode = FWidget::WM_Translate;
	else
		WidgetMode = FWidget::WM_None;
}

bool FVFDesignerViewportClient::InputKey(FViewport* InViewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool bGamepad)
{
	bool bHandled = false;

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
