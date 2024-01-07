// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FProgram
{
	FName Name;
	FString Description;
	FName Author;
	FName Version;
	FName Configuration;
	FString Path;

	FProgram(const FString& InName, const FString& InDesc, const FString& InAuthor, const FString& InVersion, const FString& InPath)
		:
	Name(InName),
	Description(InDesc),
	Author(InAuthor),
	Version(InVersion),
	Path(InPath)
	{}
};

struct FNewProgramTemplate
{
	FName Name;
	FString Path;
	FString Desc;
	FString IconPath;

	TSharedPtr<FSlateDynamicImageBrush> IconBrush;

	FNewProgramTemplate(const FName& InName, const FString& InPath, const FString& InDesc, const FString& InIconPath)
		:
	Name(InName),
	Path(InPath),
	Desc(InDesc),
	IconPath(InIconPath)
	{
		IconBrush = MakeShareable(new FSlateDynamicImageBrush(FName(*IconPath), FVector2D(128, 128)));
	}
};

