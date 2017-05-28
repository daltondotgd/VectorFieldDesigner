// Fill out your copyright notice in the Description page of Project Settings.

#include "VectorFieldDesigner.h"
#include "ForceFieldBase.h"

UForceFieldBase::UForceFieldBase(const FObjectInitializer& ObjectInitializer)
	: UObject(ObjectInitializer)
	, Force(150.0f)
{
}

FVector UForceFieldBase::ForceAtLocation(const FVector& VectorLocation) const
{
	return FVector::ZeroVector;
}

void UForceFieldBase::Draw(FPrimitiveDrawInterface* PDI, const FColor& Color) const
{

}

bool UForceFieldBase::IsInRange(const FVector& VectorLocation) const
{
	return false;
}
