// Fill out your copyright notice in the Description page of Project Settings.

#include "VectorFieldDesigner.h"
#include "CustomizableVectorField.h"

#include "SphericalForceField.h"
#include "WindForceField.h"
#include "VortexForceField.h"

UCustomizableVectorField::UCustomizableVectorField(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Resolution(8)
	, UniqueForceFieldIdCounter(-1)
{
	Bounds.Min = FVector(-200.0f);
	Bounds.Max = FVector(200.0f);

	//const int32 VectorCount = Resolution * Resolution * Resolution;
	//const int32 DestBufferSize = VectorCount * sizeof(FFloat16Color);
	//SourceData.Lock(LOCK_READ_WRITE);
	//FFloat16Color* RESTRICT DestValues = (FFloat16Color*)SourceData.Realloc(DestBufferSize);
	//for (int32 VectorIndex = 0; VectorIndex < VectorCount; ++VectorIndex)
	//{
	//	DestValues->R = 20.0f;
	//	DestValues->G = 20.0f;
	//	DestValues->B = 20.0f;
	//	DestValues->A = 0.0f;
	//	DestValues++;
	//	//SrcValues++;
	//}
	//SourceData.Unlock();

	//Resource = new FVectorFieldStaticResource(this);
}

void UCustomizableVectorField::InitInstance(FVectorFieldInstance* Instance, bool bPreviewInstance)
{
	//Instance->Init(Resource, /*bInstanced=*/ false);
	UE_LOG(LogTemp, Fatal, TEXT("NOW"));
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

//void UCustomizableVectorField::CalculateSourceData()
//{
//	for (int i = 0; i < SizeX * SizeY * SizeZ; ++i)
//		SourceData.AddObject(FVector(20.0f));
//}

int32 UCustomizableVectorField::GenerateNewUniqueForceFieldId()
{
	return ++UniqueForceFieldIdCounter;
}
