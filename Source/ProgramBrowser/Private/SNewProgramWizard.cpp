// Fill out your copyright notice in the Description page of Project Settings.


#include "SNewProgramWizard.h"

#include "ProgramBrowser.h"
#include "ProgramBrowserBlueprintLibrary.h"
#include "SlateOptMacros.h"
#include "SPrimaryButton.h"
#include "ProgramData.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "NewProgramWizard"

void SNewProgramWizard::Construct(const FArguments& InArgs, const TArray<TSharedRef<FNewProgramTemplate>>& InTemplates)
{
	NewProgramTemplates = InTemplates;
	
	TemplateListView = SNew(SListView<TSharedRef<FNewProgramTemplate>>)
		.ListItemsSource(&NewProgramTemplates)
		.OnGenerateRow(this, &SNewProgramWizard::OnGenerateTemplateRow)
		.OnSelectionChanged(this, &SNewProgramWizard::OnSelectedTemplate);
	
	TSharedRef<SVerticalBox> MainContent = SNew(SVerticalBox)
	+ SVerticalBox::Slot()
	.Padding(5.0f)
	.AutoHeight()
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(5.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("InstructionsText", "Choose a template to create program."))
			.AutoWrapText(true)
		]
	]

	+ SVerticalBox::Slot()
	.Padding(5.0f)
	[
		TemplateListView.ToSharedRef()
	]

	+ SVerticalBox::Slot()
	.AutoHeight()
	.HAlign(HAlign_Center)
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBox)
			.MinDesiredWidth(100.0f)
			.HeightOverride(30)
			[
				SAssignNew(ProgramNameTextBox, SEditableTextBox)
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
				.HintText(LOCTEXT("CreateProgramText", "Program Name..."))
				.OnTextChanged(this, &SNewProgramWizard::OnProgramNameChanged)
			]
		]
	]

	+ SVerticalBox::Slot()
	.HAlign(HAlign_Right)
	.Padding(10.0f)
	.AutoHeight()
	[
		SNew(SPrimaryButton)
		.Text(LOCTEXT("CreateProgramText", "Create Program"))
		.IsEnabled(this, &SNewProgramWizard::IsCreateProgramEnabled)
		.OnClicked(this, &SNewProgramWizard::OnCreateNewProgramClicked)
	];
	
	ChildSlot
	[
		// Populate the widget
		MainContent
	];
	
}

TSharedRef<ITableRow> SNewProgramWizard::OnGenerateTemplateRow(TSharedRef<FNewProgramTemplate> NewProgramTemplate, const TSharedRef<STableViewBase>& TableViewBase)
{
	return SNew(STableRow<TSharedRef<FNewProgramTemplate>>, TableViewBase)
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::Get().GetBrush("NoBorder"))
		.Padding(5.0f)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::Get().GetBrush("ToolPanel.GroupBorder"))
			.Padding(5.0f)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5.0f)
				[
					SNew(SBox)
					.WidthOverride(70.0f)
					.HeightOverride(70.0f)
					[
						SNew(SBorder)
						[
							SNew(SImage)
							.Image(NewProgramTemplate->IconBrush.Get())
						]
					]
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SVerticalBox)

					+ SVerticalBox::Slot()
					[
						SNew(STextBlock)
						.Text(FText::FromName(NewProgramTemplate->Name))
					]

					+ SVerticalBox::Slot()
					[
						SNew(STextBlock)
						.Text(FText::FromString(NewProgramTemplate->Path))
					]
				]
			]
		]
	];
}

void SNewProgramWizard::OnSelectedTemplate(TSharedPtr<FNewProgramTemplate> NewProgramTemplate, ESelectInfo::Type Arg)
{
	SelectedTemplate = nullptr;
	if (!TemplateListView.IsValid()) return;
	if (TemplateListView->GetSelectedItems().Num() > 0)
	{
		SelectedTemplate = TemplateListView->GetSelectedItems()[0];
	}
}

void SNewProgramWizard::OnProgramNameChanged(const FText& Text)
{
	bool bValidPath = true;
	FString NewProgramPath = FPaths::ConvertRelativePathToFull(FPaths::Combine(FProgramBrowserModule::ProgramsDir, Text.ToString()));
	FText PathError;

	if (!FPaths::ValidatePath(NewProgramPath, &PathError))
	{
		bValidPath = false;
	}

	if (bValidPath) return;

	ProgramNameTextBox->SetError(PathError);
}

bool SNewProgramWizard::IsCreateProgramEnabled() const
{
	return SelectedTemplate.IsValid() && !ProgramNameTextBox->GetText().ToString().IsEmpty();
}

FReply SNewProgramWizard::OnCreateNewProgramClicked()
{
	FText ProgramNameText = ProgramNameTextBox->GetText();
	if (!ProgramNameText.ToString().Len())
	{
		UE_LOG(LogTemp, Error, TEXT("Program name is empty!"));
		return FReply::Handled();
	}

	FString NewProgramPath = FPaths::Combine(FProgramBrowserModule::ProgramsDir, ProgramNameText.ToString());

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	ProgramUtils::FCopyProgramFileAndDirs CopyProgramFileAndDirs(PlatformFile, SelectedTemplate->Path, NewProgramPath, ProgramNameText.ToString());
	PlatformFile.IterateDirectoryRecursively(*SelectedTemplate->Path, CopyProgramFileAndDirs);

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
