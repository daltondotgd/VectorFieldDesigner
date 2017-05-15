// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EditorStyleSet.h"

/**
 * 
 */
class FVectorFieldDesignerCommands : public TCommands<FVectorFieldDesignerCommands>
{
public:
	FVectorFieldDesignerCommands()
		: TCommands<FVectorFieldDesignerCommands>("CheapTuneObjectEditor", NSLOCTEXT("Contexts", "VectorFieldDesigner", "Vector Field Designer"), NAME_None, FEditorStyle::GetStyleSetName())
	{

	}

	TSharedPtr<FUICommandInfo> SaveAsVectorField;

	TSharedPtr<FUICommandInfo> CreateSphericalForceField;
	TSharedPtr<FUICommandInfo> CreateVortexForceField;
	TSharedPtr<FUICommandInfo> CreateWindForceField;

	virtual void RegisterCommands() override;

};
