// Fill out your copyright notice in the Description page of Project Settings.

#include "VectorFieldDesigner.h"
#include "CustomizableVectorField.h"

#include "SphericalForceField.h"
#include "WindForceField.h"
#include "VortexForceField.h"

#include "VFDUtils.h"

UCustomizableVectorField::UCustomizableVectorField(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, GridX(8)
	, GridY(8)
	, GridZ(8)
	, AssetPath(TEXT(""))
{
	Bounds.Min = FVector(-200.0f);
	Bounds.Max = FVector(200.0f);
}

void UCustomizableVectorField::Init()
{
	VectorFieldInstance = NewObject<UVectorFieldStatic>(this, TEXT("InternalVectorFieldStaticInstance"));
	FVFDUtils::FillVectorFieldWithProjectData(VectorFieldInstance, this);
	VectorFieldInstance->InitResource();
	VectorFieldInstance->SetFlags(RF_Transactional);
	VectorFieldInstance->AddToRoot();
}

void UCustomizableVectorField::CreateSphericalForceField()
{
	FName NewName = MakeUniqueObjectName(this, USphericalForceField::StaticClass());
	UForceFieldBase* ForceField = NewObject<USphericalForceField>(this, NewName);
	ForceField->SetFlags(RF_Transactional);
	ForceFields.Add(ForceField);
	VectorFieldInstance->AddToRoot();
}

void UCustomizableVectorField::CreateVortexForceField()
{
	FName NewName = MakeUniqueObjectName(this, UVortexForceField::StaticClass());
	UForceFieldBase* ForceField = NewObject<UVortexForceField>(this, NewName);
	ForceField->SetFlags(RF_Transactional);
	ForceFields.Add(ForceField);
	VectorFieldInstance->AddToRoot();
}

void UCustomizableVectorField::CreateWindForceField()
{
	FName NewName = MakeUniqueObjectName(this, USphericalForceField::StaticClass());
	UForceFieldBase* ForceField = NewObject<UWindForceField>(this, NewName);
	ForceField->SetFlags(RF_Transactional);
	ForceFields.Add(ForceField);
	VectorFieldInstance->AddToRoot();
}

void UCustomizableVectorField::PostSaveRoot(bool bCleanupIsRequired)
{
	// TODO Capture thumbnail
}

#if WITH_EDITOR
void UCustomizableVectorField::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	FVFDUtils::FillVectorFieldWithProjectData(VectorFieldInstance, this);
	VectorFieldInstance->PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UCustomizableVectorField::InitInstance(FVectorFieldInstance* Instance, bool bPreviewInstance)
{
	VectorFieldInstance->InitInstance(Instance, bPreviewInstance);
}

FVector UCustomizableVectorField::CalculateVector(FVector Location)
{
	FVector Direction = FVector(0.0f);

	for (int32 Index = 0; Index < ForceFields.Num(); ++Index)
	{
		UForceFieldBase* ForceField = ForceFields[Index];
		if (ForceField->IsInRange(Location))
		{
			Direction += ForceField->ForceAtLocation(Location) * ForceField->Force;
		}
	}

	return Direction;
}

TArray<FVector> UCustomizableVectorField::CalculateVectorField()
{
	TArray<FVector> Result;

	FVector GridResolution((float)GridX, (float)GridY, (float)GridZ);

	for (int z = 0; z < GridZ; ++z)
	{
		for (int y = 0; y < GridY; ++y)
		{
			for (int x = 0; x < GridX; ++x)
			{
				FVector Location = Bounds.GetExtent() * 2.0f * (FVector((float)x, (float)y, (float)z) + 0.5f) / GridResolution + Bounds.Min;
				Result.Add(CalculateVector(Location));
			}
		}
	}

	return Result;
}
