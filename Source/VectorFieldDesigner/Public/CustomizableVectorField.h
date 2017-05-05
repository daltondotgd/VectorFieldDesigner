// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Customizable VectorField")
	int Resolution;

private:
	class FVectorFieldResource* Resource;
	
};
