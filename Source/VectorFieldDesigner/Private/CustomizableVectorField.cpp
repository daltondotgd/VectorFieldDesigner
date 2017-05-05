// Fill out your copyright notice in the Description page of Project Settings.

#include "VectorFieldDesigner.h"
#include "CustomizableVectorField.h"

UCustomizableVectorField::UCustomizableVectorField(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	Resolution(8)
{
}

void UCustomizableVectorField::InitInstance(class FVectorFieldInstance* Instance, bool bPreviewInstance)
{
}