// Fill out your copyright notice in the Description page of Project Settings.

#include "VectorFieldDesignerEditor.h"
#include "VectorFieldDesignerCommands.h"

#define LOCTEXT_NAMESPACE "VectorFieldDesignerCommands"

void FVectorFieldDesignerCommands::RegisterCommands()
{
	UI_COMMAND(ExportVectorField, "Export Vector Field", "Exports a Vector Field asset", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ExportVectorFieldAs, "Export Vector Field As", "Exports a Vector Field asset as...", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(CreateSphericalForceField, "Create Spherical Force Field", "Creates Spherical Force Field", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CreateVortexForceField, "Create Vortex Force Field", "Creates Vortex Force Field", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CreateWindForceField, "Create Wind Force Field", "Creates Wind Force Field", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE