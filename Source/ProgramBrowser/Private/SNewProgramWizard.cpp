// Fill out your copyright notice in the Description page of Project Settings.


#include "SNewProgramWizard.h"

#include "DesktopPlatformModule.h"
#include "ProgramBrowser.h"
#include "ProgramBrowserBlueprintLibrary.h"
#include "SlateOptMacros.h"
#include "SPrimaryButton.h"
#include "ProgramData.h"
#include "Async/Async.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "NewProgramWizard"

void SNewProgramWizard::Construct(const FArguments& InArgs)
{
	InitilizeProgramTemplatesData();
	
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
				.Padding(FMargin(10, 0, 0, 0))
				[
					SNew(SVerticalBox)

					+ SVerticalBox::Slot()
					[
						SNew(STextBlock)
						.Font(FCoreStyle::GetDefaultFontStyle("Blod", 20))
						.Text(FText::FromName(NewProgramTemplate->Name))
					]

					+ SVerticalBox::Slot()
					[
						SNew(STextBlock)
						.Font(FCoreStyle::GetDefaultFontStyle("Italic", 10))
						.Text(FText::FromString(NewProgramTemplate->Desc))
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

	FNotificationInfo Info(FText::FromString(FString::Printf(TEXT("Creating Program %s..."), *ProgramNameText.ToString())));
	Info.bFireAndForget = false;
	Info.bUseThrobber = true;
	CreateNotification = FSlateNotificationManager::Get().AddNotification(Info);
	CreateNotification->SetCompletionState(SNotificationItem::CS_Pending);

	CreateWorker.Reset();
	CreateWorker = MakeShareable(new FProgramBrowserWorker(TEXT("Thread-CreateProgram"), [this, ProgramNameText]()
	{
		FString NewProgramPath = FPaths::Combine(FProgramBrowserModule::ProgramsDir, ProgramNameText.ToString());

		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		ProgramUtils::FCopyProgramFileAndDirs CopyProgramFileAndDirs(PlatformFile, SelectedTemplate->Path, NewProgramPath, ProgramNameText.ToString());
		PlatformFile.IterateDirectoryRecursively(*SelectedTemplate->Path, CopyProgramFileAndDirs);

		bool Res = UProgramBrowserBlueprintLibrary::RunUBT(TEXT("-ProjectFiles"));
		AsyncTask(ENamedThreads::GameThread, [this, Res]()
		{
			if (Res)
			{
				CreateNotification->SetCompletionState(SNotificationItem::CS_Success);
			}
			else
			{
				CreateNotification->SetCompletionState(SNotificationItem::CS_Fail);
			}

			CreateNotification->ExpireAndFadeout();
			CreateNotification.Reset();
		});
	}));
	


	return FReply::Handled();
}

void SNewProgramWizard::InitilizeProgramTemplatesData()
{
	TArray<FString> Files;
	IFileManager::Get().FindFiles(Files, *(FProgramBrowserModule::ProgramTemplatesDir / TEXT("*")), false, true);
	for (const FString& File : Files)
	{
		FString Desc = TEXT("...");
		FFileHelper::LoadFileToString(Desc, *(FProgramBrowserModule::ProgramTemplatesDir / File / TEXT("Resources/Desc.txt")));
		
		NewProgramTemplates.Add(MakeShareable(new FNewProgramTemplate(
			FName(File),
			FProgramBrowserModule::ProgramTemplatesDir / File,
			Desc,
			FProgramBrowserModule::ProgramTemplatesDir / File / TEXT("Resources/TemplateIcon.png"))));
	}
}

#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
