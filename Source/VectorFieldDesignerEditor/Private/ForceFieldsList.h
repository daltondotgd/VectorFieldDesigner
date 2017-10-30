// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "SlateCore.h"
#include "Widgets/SCompoundWidget.h"
#include "ForceFields/ForceFieldBase.h"

#include "Misc/Attribute.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"
#include "SlateOptMacros.h"
#include "Widgets/Images/SImage.h"

#define LOCTEXT_NAMESPACE "SForceFieldsList"

struct FForceFieldsListEntry
{
	/** The media source's index in the table. */
	int32 Index;

	/** The media source. */
	TWeakObjectPtr<UForceFieldBase> ForceField;

	/** Create and initialize a new instance. */
	FForceFieldsListEntry(int32 InIndex, UForceFieldBase* InForceField)
		: Index(InIndex)
		, ForceField(InForceField)
	{ }
};

class SForceFieldsListTableRow
	: public SMultiColumnTableRow<TSharedPtr<FForceFieldsListEntry>>
{
public:

	SLATE_BEGIN_ARGS(SForceFieldsListTableRow)
	{ }
		SLATE_ARGUMENT(TSharedPtr<FForceFieldsListEntry>, Entry)
	SLATE_END_ARGS()

public:

	/**
	* Constructs the widget.
	*
	* @param InArgs The construction arguments.
	* @param InOwnerTableView The table view that owns this row.
	*/
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		check(InArgs._Entry.IsValid());

		Entry = InArgs._Entry;

		SMultiColumnTableRow<TSharedPtr<FForceFieldsListEntry>>::Construct(FSuperRowType::FArguments(), InOwnerTableView);
	}

public:

	//~ SMultiColumnTableRow interface

	BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override
	{
		if (ColumnName == "Icon")
		{
			//return SNew(SBox)
			//	.HAlign(HAlign_Center)
			//	.VAlign(VAlign_Center)
			//	[
			//		SNew(SImage)
			//		.Image(Style->GetBrush("MediaPlayerEditor.MediaSourceOpened"))
			//	];
			return SNew(STextBlock)
				.Text(FText::Format(LOCTEXT("ID", "{0}"), Entry->Index));
		}
		else if (ColumnName == "Index")
		{
			return SNew(STextBlock)
				.Text(FText::AsNumber(Entry->Index));
		}
		else if (ColumnName == "Name")
		{
			return SNew(STextBlock)
				.Text(FText::FromString(Entry->ForceField->GetName()));
		}
		else if (ColumnName == "Type")
		{
			return SNew(STextBlock)
				.Text(FText::FromString(Entry->ForceField->GetFullName()));
		}

		return SNullWidget::NullWidget;
	}
	END_SLATE_FUNCTION_BUILD_OPTIMIZATION

private:
	TSharedPtr<FForceFieldsListEntry> Entry;
};

/**
 * 
 */
class SForceFieldsList : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SForceFieldsList)
		: _VectorFieldBeingEdited(nullptr)
	{}

		SLATE_ATTRIBUTE(class UCustomizableVectorField*, VectorFieldBeingEdited)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedPtr<class FVectorFieldDesignerWindow> InVectorFieldDesignerWindow);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	void Rebuild();

	void OnFilterTextChanged(const FText& InFilteredText);
	void OnFilterTextCommitted(const FText& InText, ETextCommit::Type CommitInfo);
	FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent);

	TArray<TSharedPtr<FForceFieldsListEntry>> ForceFieldsList;
	void OnGetChildren(TSharedPtr<FForceFieldsListEntry> InItem, TArray<TSharedPtr<FForceFieldsListEntry>>& OutChildren);
	TSharedRef<ITableRow> HandleForceFieldsListGenerateRow(TSharedPtr<FForceFieldsListEntry> Entry, const TSharedRef<STableViewBase>& OwnerTable);

	TAttribute<class UCustomizableVectorField*> VectorFieldBeingEdited;
	TSharedPtr<class SSearchBox> FilterTextBox;
	TSharedPtr<SListView<TSharedPtr<FForceFieldsListEntry>>> ListView;
};

#undef LOCTEXT_NAMESPACE