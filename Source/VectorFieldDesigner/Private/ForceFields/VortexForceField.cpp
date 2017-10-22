// Fill out your copyright notice in the Description page of Project Settings.

#include "VectorFieldDesigner.h"
#include "VortexForceField.h"

UVortexForceField::UVortexForceField(const FObjectInitializer& ObjectInitializer)
	: UForceFieldBase(ObjectInitializer)
	, Radius(100.0f)
	, HalfHeight(50.0f)
	, bInvertDirection(false)
{
}

FVector UVortexForceField::ForceAtLocation(const FVector& VectorLocation) const
{
	FVector VectorLocationLocalSpace = Transform.InverseTransformPosition(VectorLocation);

	FVector Swirl = FVector(VectorLocationLocalSpace.Y - VectorLocationLocalSpace.X, -VectorLocationLocalSpace.X - VectorLocationLocalSpace.Y, 0.0f);

	float Distance = 1.0f - VectorLocationLocalSpace.Size() / Radius;
	float Falloff = FMath::Pow(Distance, FalloffExponent);

	return Transform.TransformVector(Swirl * Falloff);
}

void UVortexForceField::Draw(FPrimitiveDrawInterface* PDI, const FColor& Color) const
{
	FVector X = Transform.GetRotation().GetForwardVector() * Transform.GetScale3D().X;
	FVector Y = Transform.GetRotation().GetRightVector() * Transform.GetScale3D().Y;
	FVector Z = Transform.GetRotation().GetUpVector() * Transform.GetScale3D().Z;
	DrawWireCylinder(PDI, Transform.GetLocation(), X, Y, Z, Color, Radius, HalfHeight, 16, SDPG_World);
}

bool UVortexForceField::IsInRange(const FVector& VectorLocation) const
{
	FVector VectorLocationLocalSpace = Transform.InverseTransformPosition(VectorLocation);
	FVector XYLocation = VectorLocationLocalSpace * FVector(1, 1, 0);
	return XYLocation.Size() < Radius && FMath::Abs(VectorLocationLocalSpace.Z) < HalfHeight;
}
