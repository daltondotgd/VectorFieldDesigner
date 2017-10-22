// Fill out your copyright notice in the Description page of Project Settings.

#include "VectorFieldDesigner.h"
#include "WindForceField.h"

UWindForceField::UWindForceField(const FObjectInitializer& ObjectInitializer)
	: UForceFieldBase(ObjectInitializer)
{
#if WITH_EDITOR
	FString SpriteTexturePath = TEXT("/VectorFieldDesigner/Sprites/WindForceField.WindForceField");
	WindSpriteTexture = LoadObject<UTexture2D>(this, *SpriteTexturePath, NULL, LOAD_None, NULL);
#endif
}

FVector UWindForceField::ForceAtLocation(const FVector& VectorLocation) const
{
	return Transform.GetRotation().RotateVector(FVector(1, 0, 0));
}

void UWindForceField::Draw(FPrimitiveDrawInterface* PDI, const FColor& Color) const
{
	//PDI->View->ViewLocation;
	PDI->DrawSprite(Transform.GetLocation(), 10.0f, 10.0f, WindSpriteTexture->Resource, FColor::White, SDPG_World, 0.0f, 0.0f, 0.0f, 0.0f);
	DrawDirectionalArrow(PDI, Transform.ToMatrixNoScale(), Color, Force, 1.0f, SDPG_World, 1.0f);
}

bool UWindForceField::IsInRange(const FVector& VectorLocation) const
{
	return true;
}
