#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class FString;
class FName;

struct IProgram
{
    FName Name;
    FString Description;
    FName Author;
    FName Version;
    FName Configuration;

    IProgram(const FString& InName, const FString& InDesc, const FString& InAuthor, const FString& InVersion)
        :
    Name(InName),
    Description(InDesc),
    Author(InAuthor),
    Version(InVersion)
    {}
};

/**
 * 
*/
class SProgramBrowser : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SProgramBrowser)
        {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    const TArray<TSharedRef<IProgram>>& GetPrograms() {return Programs;}

private:
    TSharedPtr<class SProgramTileList> ProgramTileList;
    TSharedPtr<class SProgramSimpleList> ProgramSimpleList;

    TArray<TSharedRef<IProgram>> Programs;
};