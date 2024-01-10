// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class FProgramBrowserWorker;
struct FNewProgramTemplate;


/**
 * 
 */
class PROGRAMBROWSER_API SNewProgramWizard : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SNewProgramWizard)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	TSharedRef<ITableRow> OnGenerateTemplateRow(TSharedRef<FNewProgramTemplate> NewProgramTemplate, const TSharedRef<STableViewBase>& TableViewBase);

	void OnSelectedTemplate(TSharedPtr<FNewProgramTemplate> NewProgramTemplate, ESelectInfo::Type Arg);

	void OnProgramNameChanged(const FText& Text);

	bool IsCreateProgramEnabled() const;

	FReply OnCreateNewProgramClicked();

private:
	TSharedPtr<SListView<TSharedRef<FNewProgramTemplate>>> TemplateListView;

	TArray<TSharedRef<FNewProgramTemplate>> NewProgramTemplates;

	TSharedPtr<FNewProgramTemplate> SelectedTemplate;

	TSharedPtr<SEditableTextBox> ProgramNameTextBox;

	TSharedPtr<SNotificationItem> CreateNotification;

	TSharedPtr<FProgramBrowserWorker> CreateWorker;

	void InitilizeProgramTemplatesData();
};
