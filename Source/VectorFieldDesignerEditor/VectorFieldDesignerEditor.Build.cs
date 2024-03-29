// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VectorFieldDesignerEditor : ModuleRules
{
	public VectorFieldDesignerEditor(ReadOnlyTargetRules Target)
        : base(Target)
    {
		PrivateIncludePaths.Add("VectorFieldDesigner/Private");

		PrivateDependencyModuleNames.AddRange(
			new string[]
            {
                "Core",
                "CoreUObject",
                "Slate",
                "SlateCore",
                "Engine",
                "InputCore",
                "UnrealEd",
                "KismetWidgets",
                "Kismet",
                "PropertyEditor",
                "WorkspaceMenuStructure",
                "EditorStyle",
                "EditorWidgets",
                "VectorFieldDesigner",
                "ContentBrowser",
                "Projects"
            }
		);

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "AssetTools"
            }
        );

        DynamicallyLoadedModuleNames.AddRange(
            new string[] {
                "AssetTools"
            }
        );
    }
}
