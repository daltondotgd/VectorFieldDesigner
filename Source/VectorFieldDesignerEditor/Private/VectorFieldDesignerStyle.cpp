// Fill out your copyright notice in the Description page of Project Settings.

#include "VectorFieldDesignerEditor.h"
#include "VectorFieldDesignerStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateTypes.h"
#include "EditorStyleSet.h"
#include "Interfaces/IPluginManager.h"
#include "SlateOptMacros.h"
#include "Brushes/SlateImageBrush.h"


#define IMAGE_PLUGIN_BRUSH( RelativePath, ... ) FSlateImageBrush( FVectorFieldDesignerStyle::InContent( RelativePath, ".png" ), __VA_ARGS__ )
#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BOX_BRUSH(RelativePath, ...) FSlateBoxBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define TTF_FONT(RelativePath, ...) FSlateFontInfo(StyleSet->RootToContentDir(RelativePath, TEXT(".ttf")), __VA_ARGS__)
#define TTF_CORE_FONT(RelativePath, ...) FSlateFontInfo(StyleSet->RootToCoreContentDir(RelativePath, TEXT(".ttf") ), __VA_ARGS__)

FString FVectorFieldDesignerStyle::InContent(const FString& RelativePath, const ANSICHAR* Extension)
{
	static FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("VectorFieldDesigner"))->GetContentDir();
	return (ContentDir / RelativePath) + Extension;
}

TSharedPtr< FSlateStyleSet > FVectorFieldDesignerStyle::StyleSet = nullptr;
TSharedPtr< class ISlateStyle > FVectorFieldDesignerStyle::Get() { return StyleSet; }

FName FVectorFieldDesignerStyle::GetStyleSetName()
{
	static FName VectorFieldDesignerStyleName(TEXT("VectorFieldDesignerStyle"));
	return VectorFieldDesignerStyleName;
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void FVectorFieldDesignerStyle::Initialize()
{
	// Const icon sizes
	const FVector2D Icon8x8(8.0f, 8.0f);
	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);
	const FVector2D Icon64x64(64.0f, 64.0f);

	// Only register once
	if (StyleSet.IsValid())
	{
		return;
	}

	StyleSet = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	StyleSet->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate"));
	StyleSet->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

	const FTextBlockStyle& NormalText = FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");

	// Force Field
	{
		StyleSet->Set("VFDesigner.CreateSphericalForceField", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_VFDesigner_CreateSphericalForceField_40x"), Icon40x40));
		StyleSet->Set("VFDesigner.CreateVortexForceField", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_VFDesigner_CreateVortexForceField_40x"), Icon40x40));
		StyleSet->Set("VFDesigner.CreateWindForceField", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_VFDesigner_CreateWindForceField_40x"), Icon40x40));
	}

	{
		StyleSet->Set("ClassIcon.CustomizableVectorField", new IMAGE_PLUGIN_BRUSH("Icons/CustomizableVectorField_16x", Icon16x16));
		StyleSet->Set("ClassThumbnail.CustomizableVectorField", new IMAGE_PLUGIN_BRUSH("Icons/CustomizableVectorField_64x", Icon64x64));
	}

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
};

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef IMAGE_PLUGIN_BRUSH
#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef TTF_FONT
#undef TTF_CORE_FONT

void FVectorFieldDesignerStyle::Shutdown()
{
	if (StyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
		ensure(StyleSet.IsUnique());
		StyleSet.Reset();
	}
}
