// Fill out your copyright notice in the Description page of Project Settings.

#include "VectorFieldDesigner.h"
#include "SphericalForceField.h"

USphericalForceField::USphericalForceField(const FObjectInitializer& ObjectInitializer)
	: UForceFieldBase(ObjectInitializer)
	, Radius(50.0f)
{
}

FVector USphericalForceField::ForceAtLocation(const FVector& VectorLocation) const
{
	FVector Direction = Transform.GetRotation() * Transform.InverseTransformPosition(VectorLocation);
	float Length = 1.0f - Direction.Size() / Radius;

	Direction.Normalize();
	Direction *= FMath::Pow(Length, FalloffExponent);

	return Direction;
}

void USphericalForceField::Draw(FPrimitiveDrawInterface* PDI, const FColor& Color) const
{
	FKSphereElem SphereElem(Radius);
	SphereElem.Center = Transform.GetLocation();
	const FTransform ElemTM = SphereElem.GetTransform();
	SphereElem.DrawElemWire(PDI, Transform, FVector(1.0f), Color);
}

bool USphericalForceField::IsInRange(const FVector& VectorLocation) const
{
	FVector VectorLocationLocalSpace = Transform.InverseTransformPosition(VectorLocation);
	return VectorLocationLocalSpace.Size() < Radius;
}
