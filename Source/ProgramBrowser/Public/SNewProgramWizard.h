// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

struct FNewProgramTemplate
{
	FName Name;
	FString Path;
	FString IconPath;

	TSharedPtr<FSlateDynamicImageBrush> IconBrush;

	FNewProgramTemplate(const FName& InName, const FString& InPath, const FString& InIconPath)
		:
	Name(InName),
	Path(InPath),
	IconPath(InIconPath)
	{
		IconBrush = MakeShareabel(new FSlateDynamicImageBrush(FName(*IconPath), FVector(128, 128)));
	}
};


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

private:
	TSharedPtr<SListView<TSharedRef<FNewProgramTemplate>>> TemplateListView;

	TArray<TSaredRef<FNewProgramTemplate>> NewProgramTemplates;
};
