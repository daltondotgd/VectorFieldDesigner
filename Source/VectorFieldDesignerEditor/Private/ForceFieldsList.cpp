// Fill out your copyright notice in the Description page of Project Settings.

#include "VectorFieldDesignerEditor.h"
#include "ForceFieldsList.h"

#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Views/SListView.h"

#include "CustomizableVectorField.h"

#define LOCTEXT_NAMESPACE "SForceFieldsList"

void SForceFieldsList::Construct(const FArguments& InArgs, TSharedPtr<class FVectorFieldDesignerWindow>)
{
	VectorFieldBeingEdited = InArgs._VectorFieldBeingEdited;

	int Index = 0;
	for (auto& ForceField : VectorFieldBeingEdited.Get()->ForceFields)
	{
		ForceFieldsList.Add(MakeShareable(new FForceFieldsListEntry(Index++, ForceField)));
	}

	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(FilterTextBox, SSearchBox)
			.OnTextChanged(this, &SForceFieldsList::OnFilterTextChanged)
			.OnTextCommitted(this, &SForceFieldsList::OnFilterTextCommitted)
		]
		+SVerticalBox::Slot()
		.Padding(FMargin(0.0f, 2.0f, 0.0f, 0.0f))
		.FillHeight(1.0f)
		[
			SAssignNew(ListView, SListView<TSharedPtr<FForceFieldsListEntry>>)
			.ItemHeight(32.0f)
			.ListItemsSource(&ForceFieldsList)
			.SelectionMode(ESelectionMode::Multi)
			//.OnGetChildren(this, &SForceFieldsList::OnGetChildren)
			.OnGenerateRow(this, &SForceFieldsList::HandleForceFieldsListGenerateRow)
			//.OnSelectionChanged()
			//.OnMouseButtonDoubleClick(this, &SMediaPlayerEditorPlaylist::HandleMediaSourceListDoubleClick)
			//.OnMouseButtonClick(this, &SForceFieldsList::HandleForceFieldsListClick)
			.HeaderRow
			(
				SNew(SHeaderRow)

				+ SHeaderRow::Column("Icon")
				.DefaultLabel(LOCTEXT("PlaylistIconColumnHeader", " "))
				.FixedWidth(16.0f)

				+ SHeaderRow::Column("Name")
				.DefaultLabel(LOCTEXT("PlaylistSourceColumnHeader", "Name"))
				.FillWidth(0.5f)

				+ SHeaderRow::Column("Type")
				.DefaultLabel(LOCTEXT("PlaylistTypeColumnHeader", "Type"))
				.FillWidth(0.5f)
			)
		]
	];

	FilterTextBox->SetOnKeyDownHandler(FOnKeyDown::CreateSP(this, &SForceFieldsList::OnKeyDown));
}

void SForceFieldsList::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	Rebuild();
}

void SForceFieldsList::Rebuild()
{
	ListView.Get()->RebuildList();
}

void SForceFieldsList::OnFilterTextChanged(const FText& InFilteredText)
{
	ForceFieldsList.Empty();
	int Index = 0;
	for (auto& ForceField : VectorFieldBeingEdited.Get()->ForceFields.FilterByPredicate([=](UForceFieldBase* ForceField) { return ForceField->GetName().Find(InFilteredText.ToString()); }))
	{
		ForceFieldsList.Add(MakeShareable(new FForceFieldsListEntry(Index++, ForceField)));
	}
}

void SForceFieldsList::OnFilterTextCommitted(const FText& InText, ETextCommit::Type CommitInfo)
{
}

FReply SForceFieldsList::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent & KeyEvent)
{
	return FReply::Unhandled();
}

void SForceFieldsList::OnGetChildren(TSharedPtr<FForceFieldsListEntry> InItem, TArray<TSharedPtr<FForceFieldsListEntry>>& OutChildren)
{
	if (InItem->Index < 5)
	{
		int Index = 5;
		for (auto& ForceField : VectorFieldBeingEdited.Get()->ForceFields)
		{
			OutChildren.Add(MakeShareable(new FForceFieldsListEntry(Index++, ForceField)));
		}
	}
}

TSharedRef<ITableRow> SForceFieldsList::HandleForceFieldsListGenerateRow(TSharedPtr<FForceFieldsListEntry> Entry, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SForceFieldsListTableRow, OwnerTable)
		.Entry(Entry);
}

#undef LOCTEXT_NAMESPACE