// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ForceFieldBase.h"

#include "VectorField/VectorField.h"
#include "CustomizableVectorField.generated.h"

/**
 * 
 */
UCLASS()
class VECTORFIELDDESIGNER_API UCustomizableVectorField : public UVectorField
{
	GENERATED_UCLASS_BODY()

public:
	virtual void InitInstance(class FVectorFieldInstance* Instance, bool bPreviewInstance) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Customizable VectorField", meta = (ClampMin = "0", ClampMax = "64", UIMin = "1", UIMax = "32"))
	int32 Resolution;

	UPROPERTY()
	TArray<UForceFieldBase*> ForceFields;

	void CreateSphericalForceField();
	void CreateVortexForceField();
	void CreateWindForceField();

private:
	int32 GenerateNewUniqueForceFieldId();
	int32 UniqueForceFieldIdCounter;

};
