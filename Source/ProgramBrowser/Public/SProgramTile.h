

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

struct IProgram;

/**
 * 
*/
class PROGRAMBROWSER_API SProgramTile : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SProgramTile)
        {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, TSharedRef<IProgram> InProgram);

    FText GetProgramNameText() const;

    void HandleConfigurationChanged(FName Name);

    FText GetConfigText() const;

private:
    TSharedPtr<IProgram> Program;

    TSharedPtr<FSlateDynamicImageBrush> ProgramIcon;

    FText ConfigurationText;

    TSharedPtr<STextBlock> ContentWidget;
};