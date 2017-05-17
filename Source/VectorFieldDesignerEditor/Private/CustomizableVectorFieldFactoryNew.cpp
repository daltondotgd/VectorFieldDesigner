// Fill out your copyright notice in the Description page of Project Settings.

#include "VectorFieldDesignerEditor.h"
#include "CustomizableVectorFieldFactoryNew.h"
#include "CustomizableVectorField.h"


UCustomizableVectorFieldFactoryNew::UCustomizableVectorFieldFactoryNew(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UCustomizableVectorField::StaticClass();
}

UObject* UCustomizableVectorFieldFactoryNew::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	UCustomizableVectorField* NewCustomizableVectorField = NewObject<UCustomizableVectorField>(InParent, InClass, InName, Flags | RF_Transactional);
	return NewCustomizableVectorField;
}

FString UCustomizableVectorFieldFactoryNew::GetDefaultNewAssetName() const
{
	return TEXT("VFDP_VectorFieldDesignerProject");
}
