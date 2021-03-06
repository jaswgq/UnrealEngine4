// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "TranslationEditorPrivatePCH.h"
#include "TranslationPickerEditWindow.h"
#include "Editor/Documentation/Public/SDocumentationToolTip.h"
#include "TranslationUnit.h"

#define LOCTEXT_NAMESPACE "TranslationPicker"

// Default dimensions of the Translation Picker edit window (floating window also uses these sizes, so it matches roughly)
const int32 STranslationPickerEditWindow::DefaultEditWindowWidth = 500;
const int32 STranslationPickerEditWindow::DefaultEditWindowHeight = 500;

void STranslationPickerEditWindow::Construct(const FArguments& InArgs)
{
	ParentWindow = InArgs._ParentWindow;
	PickedTexts = InArgs._PickedTexts;
	WindowContents = SNew(SBox);
	TSharedRef<SVerticalBox> TextsBox = SNew(SVerticalBox);

	// Add a new Translation Picker Edit Widget for each picked text
	for (FText PickedText : PickedTexts)
	{
		TSharedPtr<SEditableTextBox> TextBox;
		int32 DefaultPadding = 0.0f;

		TSharedRef<STranslationPickerEditWidget> NewEditWidget = 
			SNew(STranslationPickerEditWidget)
			.PickedText(PickedText)
			.bAllowEditing(true);

		EditWidgets.Add(NewEditWidget);

		TextsBox->AddSlot()
			.AutoHeight()
			.Padding(FMargin(5))
			[
				SNew(SBorder)
				[
					NewEditWidget
				]
			];
	}

	TSharedPtr<SEditableTextBox> TextBox;
	int32 DefaultPadding = 0.0f;

	// Layout the Translation Picker Edit Widgets and some save/close buttons below them
	WindowContents->SetContent(
		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
		[

			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			.Padding(DefaultPadding)
			[
				// Display name of the current language
				SNew(STextBlock)
				.Text(FText::FromString(FInternationalization::Get().GetCurrentCulture()->GetDisplayName()))
				.Justification(ETextJustify::Center)
			]
			+ SVerticalBox::Slot()
			[
				SNew(SScrollBox)
				+ SScrollBox::Slot()
				.Padding(0.0f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(FMargin(8, 5, 8, 5))
					[
						TextsBox
					]
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Right)
			.Padding(DefaultPadding)
			[
				SNew(SUniformGridPanel)
				.SlotPadding(FEditorStyle::GetMargin("StandardDialog.SlotPadding"))
				.MinDesiredSlotWidth(FEditorStyle::GetFloat("StandardDialog.MinDesiredSlotWidth"))
				.MinDesiredSlotHeight(FEditorStyle::GetFloat("StandardDialog.MinDesiredSlotHeight"))
				+ SUniformGridPanel::Slot(0, 0)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
					.OnClicked(this, &STranslationPickerEditWindow::SaveAllAndClose)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.Padding(FMargin(0, 0, 3, 0))
						.VAlign(VAlign_Center)
						.AutoWidth()
						[
							SNew(STextBlock).Text(LOCTEXT("SaveAllAndClose", "Save all and close"))
						]
					]
				]
				+ SUniformGridPanel::Slot(1, 0)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
					.OnClicked(this, &STranslationPickerEditWindow::Close)
					.VAlign(VAlign_Center)
					.Text(LOCTEXT("CancelButton", "Cancel"))
				]
			]
		]
	);

	ChildSlot
	[
		WindowContents.ToSharedRef()
	];
}

FReply STranslationPickerEditWindow::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		Close();
		
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply STranslationPickerEditWindow::Close()
{
	if (ParentWindow.IsValid())
	{
		FSlateApplication::Get().RequestDestroyWindow(ParentWindow.Pin().ToSharedRef());
		ParentWindow.Reset();
	}

	return FReply::Handled();
}

FReply STranslationPickerEditWindow::SaveAllAndClose()
{
	
	TArray<UTranslationUnit*> TempArray;

	for (TSharedRef<STranslationPickerEditWidget> EditWidget : EditWidgets)
	{
		UTranslationUnit* TranslationUnit = EditWidget->GetTranslationUnitWithAnyChanges();
		if (TranslationUnit != nullptr)
		{
			TempArray.Add(TranslationUnit);
		}
	}

	if (TempArray.Num() > 0)
	{
		// Save the data via translation data manager
		FTranslationDataManager::SaveSelectedTranslations(TempArray);
	}

	Close();

	return FReply::Handled();
}

void STranslationPickerEditWidget::Construct(const FArguments& InArgs)
{
	PickedText = InArgs._PickedText;
	bAllowEditing = InArgs._bAllowEditing;
	int32 DefaultPadding = 0.0f;

	bool bCultureInvariant = PickedText.IsCultureInvariant();
	bool bShouldGatherForLocalization = FTextInspector::ShouldGatherForLocalization(PickedText);

	// Get all the data we need and format it properly
	TOptional<FString> NamespaceString = FTextInspector::GetNamespace(PickedText);
	TOptional<FString> KeyString = FTextInspector::GetKey(PickedText);
	const FString* SourceString = FTextInspector::GetSourceString(PickedText);
	const FString& TranslationString = FTextInspector::GetDisplayString(PickedText);
	FString LocresFullPath;

	FString ManifestAndArchiveNameString;
	if (NamespaceString && KeyString)
	{
		FString LocResId;
		if (FTextLocalizationManager::Get().GetLocResID(NamespaceString.GetValue(), KeyString.GetValue(), LocResId))
		{
			LocresFullPath = *LocResId;
			ManifestAndArchiveNameString = FPaths::GetBaseFilename(*LocResId);
		}
	}

	FText Namespace = FText::FromString(NamespaceString.Get(TEXT("")));
	FText Key = FText::FromString(KeyString.Get(TEXT("")));
	FText Source = SourceString != nullptr ? FText::FromString(*SourceString) : FText::GetEmpty();
	FText ManifestAndArchiveName = FText::FromString(ManifestAndArchiveNameString);
	FText Translation = FText::FromString(TranslationString);

	FText SourceLabel = LOCTEXT("SourceLabel", "Source:");
	FText NamespaceLabel = LOCTEXT("NamespaceLabel", "Namespace:");
	FText KeyLabel = LOCTEXT("KeyLabel", "Key:");
	FText ManifestAndArchiveNameLabel = LOCTEXT("LocresFileLabel", "Target :");

	// Save the necessary data in UTranslationUnit for later.  This is what we pass to TranslationDataManager to save our edits
	TranslationUnit = NewObject<UTranslationUnit>();
	TranslationUnit->Namespace = NamespaceString.Get(TEXT(""));
	TranslationUnit->Source = SourceString != nullptr ? *SourceString : "";
	TranslationUnit->Translation = TranslationString;
	TranslationUnit->LocresPath = LocresFullPath;

	// Can only save if we have all the required information
	bool bHasRequiredLocalizationInfo = NamespaceString.IsSet() && SourceString != nullptr && LocresFullPath.Len() > 0;

	TSharedPtr<SGridPanel> GridPanel;
	TSharedRef<SGridPanel> LocalizationInfoAndSaveButtonSlot = SNew(SGridPanel).FillColumn(2,1);

	// Layout all our data
	ChildSlot
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1)
			.Padding(FMargin(5))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				[
				SAssignNew(GridPanel, SGridPanel)
				.FillColumn(1,1)
				+ SGridPanel::Slot(0,0)
				.Padding(FMargin(5))
				.HAlign(HAlign_Right)
				[
						SNew(STextBlock)
						.TextStyle(FEditorStyle::Get(), "RichTextBlock.Bold")
						.Text(SourceLabel)
				]
				+ SGridPanel::Slot(0, 1)
					.Padding(FMargin(5))
					.HAlign(HAlign_Right)
				[
					SNew(SVerticalBox)
					.Visibility(!bHasRequiredLocalizationInfo && SourceString->Equals(TranslationString) ? EVisibility::Collapsed : EVisibility::Visible)
					+ SVerticalBox::Slot()
					[
						SNew(STextBlock)
						.TextStyle(FEditorStyle::Get(), "RichTextBlock.Bold")
						.Text(LOCTEXT("TranslationLabel", "Translation: "))
					]
				]
				
				+ SGridPanel::Slot(1, 0)
					.Padding(FMargin(5))
					.ColumnSpan(2)
				[
					SNew(STextBlock)
					.Text(Source)
				]
				+ SGridPanel::Slot(1, 1)
					.ColumnSpan(2)
					.Padding(FMargin(5))
					[
						SNew(SVerticalBox)
						.Visibility(!bHasRequiredLocalizationInfo && SourceString->Equals(TranslationString) ? EVisibility::Collapsed : EVisibility::Visible)
						+ SVerticalBox::Slot()
						[
							SAssignNew(TextBox, SMultiLineEditableTextBox)
							.IsEnabled(bAllowEditing && bHasRequiredLocalizationInfo)
							.Text(Translation)
							.HintText(LOCTEXT("TranslationEditTextBox_HintText", "Enter/edit translation here."))
						]
					]
				]
			+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(5))
				[
					LocalizationInfoAndSaveButtonSlot
				]
			]
		];

	if (bCultureInvariant)
	{
		LocalizationInfoAndSaveButtonSlot->AddSlot(0, 0)
			.Padding(FMargin(5))
			.ColumnSpan(2)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("CultureInvariantLabel", "This text is culture-invariant"))
				.Justification(ETextJustify::Center)
			];
	}
	else if (!bShouldGatherForLocalization)
	{
		LocalizationInfoAndSaveButtonSlot->AddSlot(0, 0)
			.Padding(FMargin(5))
			.ColumnSpan(2)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("NotGatheredForLocalizationLabel", "This text is not gathered for localization"))
				.Justification(ETextJustify::Center)
			];
	}
	else if (!bHasRequiredLocalizationInfo)
	{
		LocalizationInfoAndSaveButtonSlot->AddSlot(0, 0)
			.Padding(FMargin(5))
			.ColumnSpan(2)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("RequiredLocalizationInfoNotFound", "The required localization info for this text was not found."))
				.Justification(ETextJustify::Center)
			];
	}
	else
	{
		LocalizationInfoAndSaveButtonSlot->AddSlot(0, 0)
			.Padding(FMargin(2.5))
			.HAlign(HAlign_Right)
			[
				SNew(STextBlock)
				.TextStyle(FEditorStyle::Get(), "RichTextBlock.Bold")
				.Text(NamespaceLabel)
			];
		LocalizationInfoAndSaveButtonSlot->AddSlot(1, 0)
			.Padding(FMargin(2.5))
			[
				SNew(STextBlock)
				.Text(Namespace)
			];
		LocalizationInfoAndSaveButtonSlot->AddSlot(0, 1)
			.Padding(FMargin(2.5))
			.HAlign(HAlign_Right)
			[
				SNew(STextBlock)
				.TextStyle(FEditorStyle::Get(), "RichTextBlock.Bold")
				.Text(KeyLabel)
			];
		LocalizationInfoAndSaveButtonSlot->AddSlot(1, 1)
			.Padding(FMargin(2.5))
			[
				SNew(STextBlock)
				.Text(Key)
			];
		LocalizationInfoAndSaveButtonSlot->AddSlot(0, 2)
			.Padding(FMargin(2.5))
			.HAlign(HAlign_Right)
			[
				SNew(STextBlock)
				.TextStyle(FEditorStyle::Get(), "RichTextBlock.Bold")
				.Text(ManifestAndArchiveNameLabel)
			];
		LocalizationInfoAndSaveButtonSlot->AddSlot(1, 2)
			.Padding(FMargin(2.5))
			[
				SNew(STextBlock)
				.Text(ManifestAndArchiveName)
			];
		LocalizationInfoAndSaveButtonSlot->AddSlot(2, 2)
			.Padding(FMargin(2.5))
			[
				SNew(SButton)
				.HAlign(HAlign_Center)
				.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
				.OnClicked(this, &STranslationPickerEditWidget::SaveAndPreview)
				.IsEnabled(bHasRequiredLocalizationInfo)
				.Visibility(bAllowEditing ? EVisibility::Visible : EVisibility::Collapsed)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.Padding(FMargin(0, 0, 3, 0))
					.VAlign(VAlign_Center)
					.AutoWidth()
					[
						SNew(STextBlock)
						.Text(bHasRequiredLocalizationInfo ? LOCTEXT("SaveAndPreviewButtonText", "Save and preview") : LOCTEXT("SaveAndPreviewButtonDisabledText", "Cannot Save"))
					]
				]
			];
	}
}

FReply STranslationPickerEditWidget::SaveAndPreview()
{
	// Update translation string from entered text
	TranslationUnit->Translation = TextBox->GetText().ToString();

	// Save the data via translation data manager
	TArray<UTranslationUnit*> TempArray;
	TempArray.Add(TranslationUnit);
	FTranslationDataManager::SaveSelectedTranslations(TempArray);

	return FReply::Handled();
}

UTranslationUnit* STranslationPickerEditWidget::GetTranslationUnitWithAnyChanges()
{
	if (TranslationUnit)
	{
		// Update translation string from entered text
		TranslationUnit->Translation = TextBox->GetText().ToString();

		return TranslationUnit;
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE