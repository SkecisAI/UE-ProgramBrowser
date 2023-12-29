// Fill out your copyright notice in the Description page of Project Settings.


#include "SNewProgramWizard.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "NewProgramWizard"

void SNewProgramWizard::Construct(const FArguments& InArgs)
{
	NewProgramTemplates.Add(MakeShareable(new FNewProgramTemplate(
		FName("Blank"), 
		FString("A blank program with console."),
		IPluginManager::Get().FindPlugin("ProgramBrowser")->GetBaseDir() / TEXT("Resource/terminal.png"))));
	NewProgramTemplates.Add(MakeShareable(new FNewProgramTemplate(
		FName("Slate Window"), 
		FString("A program with slate UI."),
		IPluginManager::Get().FindPlugin("ProgramBrowser")->GetBaseDir() / TEXT("Resource/window.png"))));
	
	TemplateListView = SNew(SListView<TSharedRef<FNewProgramTemplate>>)
		.ListItemSource(&NewProgramTemplates)
		.OnGenerateRow(this, &SNewProgramWizard::OnGenerateTemplateRow);
	
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
	.HAlign(HAlign_Right)
	.Padding(10.0f)
	.AutoHeight()
	[
		SNew(SPrimaryButton)
		.Text(LOCTEXT("CreateProgramText", "Create Program"))
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


#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
