// Fill out your copyright notice in the Description page of Project Settings.

#include "VectorFieldDesignerEditor.h"
#include "VectorFieldDesignerWindow.h"
#include "CustomizableVectorField.h"
#include "CustomizableVectorFieldActions.h"


void FCustomizableVectorFieldActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder & MenuBuilder)
{
}

uint32 FCustomizableVectorFieldActions::GetCategories()
{
	return EAssetTypeCategories::Misc;
}

void FCustomizableVectorFieldActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UCustomizableVectorField* CustomizableVectorField = Cast<UCustomizableVectorField>(*ObjIt))
		{
			TSharedRef<FVectorFieldDesignerWindow> NewVectorFieldDesignerWindow(new FVectorFieldDesignerWindow());
			NewVectorFieldDesignerWindow->InitVectorFieldDesignerWindow(Mode, EditWithinLevelEditor, CustomizableVectorField);
		}
	}
}

FText FCustomizableVectorFieldActions::GetName() const
{
	return FText::FromName(TEXT("Customizable Vector Field"));
}

FColor FCustomizableVectorFieldActions::GetTypeColor() const
{
	return FColor::Red;
}

UClass* FCustomizableVectorFieldActions::GetSupportedClass() const
{
	return UCustomizableVectorField::StaticClass();
}



