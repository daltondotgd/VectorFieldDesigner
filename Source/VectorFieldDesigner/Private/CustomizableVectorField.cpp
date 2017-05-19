// Fill out your copyright notice in the Description page of Project Settings.

#include "VectorFieldDesigner.h"
#include "CustomizableVectorField.h"

#include "SphericalForceField.h"
#include "WindForceField.h"
#include "VortexForceField.h"

#include "VectorField/VectorFieldStatic.h"

UCustomizableVectorField::UCustomizableVectorField(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, GridX(8)
	, GridY(8)
	, GridZ(8)
	, UniqueForceFieldIdCounter(-1)
	, AssetPath(TEXT(""))
{
	Bounds.Min = FVector(-200.0f);
	Bounds.Max = FVector(200.0f);
}

void FillVectorFieldWithProjectData(UVectorFieldStatic* VectorFieldStaticInstance, UCustomizableVectorField* CustomizableVectrorFieldInstance)
{
	VectorFieldStaticInstance->SizeX = CustomizableVectrorFieldInstance->GridX;
	VectorFieldStaticInstance->SizeY = CustomizableVectrorFieldInstance->GridY;
	VectorFieldStaticInstance->SizeZ = CustomizableVectrorFieldInstance->GridZ;
	VectorFieldStaticInstance->Bounds = CustomizableVectrorFieldInstance->Bounds;

	// Convert vectors to 16-bit FP and store.
	const TArray<FVector> SrcValues = CustomizableVectrorFieldInstance->CalculateVectorField();
	const int32 VectorCount = SrcValues.Num();
	const int32 DestBufferSize = VectorCount * sizeof(FFloat16Color);
	VectorFieldStaticInstance->SourceData.Lock(LOCK_READ_WRITE);
	FFloat16Color* RESTRICT DestValues = (FFloat16Color*)VectorFieldStaticInstance->SourceData.Realloc(DestBufferSize);
	int Index = 0;
	for (int32 VectorIndex = 0; VectorIndex < VectorCount; ++VectorIndex)
	{
		DestValues->R = SrcValues[VectorIndex].X;
		DestValues->G = SrcValues[VectorIndex].Y;
		DestValues->B = SrcValues[VectorIndex].Z;
		DestValues->A = 0.0f;
		++DestValues;
	}
	VectorFieldStaticInstance->SourceData.Unlock();
}


void UCustomizableVectorField::InitInstance(FVectorFieldInstance* Instance, bool bPreviewInstance)
{
	UVectorFieldStatic* VF = NewObject<UVectorFieldStatic>(this, NAME_None, RF_Transient);
	FillVectorFieldWithProjectData(VF, this);
	VF->InitResource();
	VF->AddToRoot();
	VF->InitInstance(Instance, bPreviewInstance);
	//Instance->Init(Resource, /*bInstanced=*/ false);
	//UE_LOG(LogTemp, Fatal, TEXT("NOW"));
}

void UCustomizableVectorField::CreateSphericalForceField()
{
	FString NewName = FString::Printf(TEXT("SphericalForceField%d"), GenerateNewUniqueForceFieldId());
	UForceFieldBase* ForceField = NewObject<USphericalForceField>(this, *NewName);
	ForceField->SetFlags(RF_Transactional);
	ForceFields.Add(ForceField);
}

void UCustomizableVectorField::CreateVortexForceField()
{
	FString NewName = FString::Printf(TEXT("VortexForceField%d"), GenerateNewUniqueForceFieldId());
	UForceFieldBase* ForceField = NewObject<UVortexForceField>(this, *NewName);
	ForceField->SetFlags(RF_Transactional);
	ForceFields.Add(ForceField);
}

void UCustomizableVectorField::CreateWindForceField()
{
	FString NewName = FString::Printf(TEXT("WindForceField%d"), GenerateNewUniqueForceFieldId());
	UForceFieldBase* ForceField = NewObject<UWindForceField>(this, *NewName);
	ForceField->SetFlags(RF_Transactional);
	ForceFields.Add(ForceField);
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

int32 UCustomizableVectorField::GenerateNewUniqueForceFieldId()
{
	return ++UniqueForceFieldIdCounter;
}
