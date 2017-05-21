// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ForceFieldBase.h"

#include "VectorField/VectorFieldStatic.h"

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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Customizable VectorField", meta = (ClampMin = "0", ClampMax = "64", UIMin = "1", UIMax = "32"))
	int32 GridX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Customizable VectorField", meta = (ClampMin = "0", ClampMax = "64", UIMin = "1", UIMax = "32"))
	int32 GridY;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Customizable VectorField", meta = (ClampMin = "0", ClampMax = "64", UIMin = "1", UIMax = "32"))
	int32 GridZ;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Preview")
	UParticleSystem* PreviewParticleSystem;

	UPROPERTY()
	TArray<UForceFieldBase*> ForceFields;

	UPROPERTY()
	FString AssetPath;

	UPROPERTY()
	UVectorFieldStatic* VectorFieldInstance;

	virtual void Init();

	void CreateSphericalForceField();
	void CreateVortexForceField();
	void CreateWindForceField();

	void PostSaveRoot(bool bCleanupIsRequired) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void InitInstance(class FVectorFieldInstance* Instance, bool bPreviewInstance) override;

	FVector CalculateVector(FVector Location);
	TArray<FVector> CalculateVectorField();

};
