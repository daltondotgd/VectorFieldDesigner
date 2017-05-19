// Fill out your copyright notice in the Description page of Project Settings.

#include "VectorFieldDesignerEditor.h"
#include "VFDUtils.h"

#include "CustomizableVectorField.h"
#include "VectorField/VectorFieldStatic.h"

void FVFDUtils::FillVectorFieldWithProjectData(UVectorFieldStatic* VectorFieldStaticInstance, UCustomizableVectorField* CustomizableVectrorFieldInstance)
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
