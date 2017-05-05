// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "VectorFieldDesignerEditor.h"
#include "CustomizableVectorField.h"
#include "CustomizableVectorFieldActions.h"

#define LOCTEXT_NAMESPACE "FVectorFieldDesignerEditorModule"

void FVectorFieldDesignerEditorModule::StartupModule()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	TSharedRef<IAssetTypeActions> CustomizableVectorFieldAssetTypeActions = MakeShareable(new FCustomizableVectorFieldActions());
	RegisterAssetTypeActions(AssetTools, CustomizableVectorFieldAssetTypeActions);
}

void FVectorFieldDesignerEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (int32 Index = 0; Index < CreatedAssetTypeActions.Num(); ++Index)
		{
			AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[Index].ToSharedRef());
		}
	}
	CreatedAssetTypeActions.Empty();
}

void FVectorFieldDesignerEditorModule::RegisterAssetTypeActions(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> AssetTypeActions)
{
	AssetTools.RegisterAssetTypeActions(AssetTypeActions);
	CreatedAssetTypeActions.Add(AssetTypeActions);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVectorFieldDesignerEditorModule, VectorFieldDesignerEditor)