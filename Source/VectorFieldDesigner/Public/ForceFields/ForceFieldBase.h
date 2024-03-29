// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SceneManagement.h"

#include "PhysicsEngine/ConvexElem.h"
#include "PhysicsEngine/BoxElem.h"
#include "PhysicsEngine/SphereElem.h"
#include "PhysicsEngine/SphylElem.h"

#include "UObject/NoExportTypes.h"
#include "ForceFieldBase.generated.h"

/**
 * 
 */
UCLASS()
class VECTORFIELDDESIGNER_API UForceFieldBase : public UObject
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual FVector ForceAtLocation(const FVector& VectorLocation) const;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FColor& Color) const;
	virtual bool IsInRange(const FVector& VectorLocation) const;

	UPROPERTY(EditAnywhere, Category = "Transform")
	FTransform Transform;

	UPROPERTY(EditAnywhere, Category = "Force")
	float Force;

	//UPROPERTY(EditAnywhere, Category = "Force")
	//bool bInvertDirection;

	UPROPERTY(EditAnywhere, Category = "Falloff")
	float FalloffExponent;

};
