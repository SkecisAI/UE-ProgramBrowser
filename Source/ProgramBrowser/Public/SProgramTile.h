

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class FProgramBrowserWorker;
struct FProgram;

/**
 * 
*/
class PROGRAMBROWSER_API SProgramTile : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SProgramTile)
        {}
    SLATE_END_ARGS()

    ~SProgramTile();

    void Construct(const FArguments& InArgs, TSharedRef<FProgram> InProgram);

    FText GetProgramNameText() const;

    void HandleConfigurationChanged(FName Name);

    FText GetConfigText() const;

    FReply OnBuildProgramClicked();

    FReply OnPackageProgramClicked();
    
private:
    TSharedPtr<FProgram> Program;

    TSharedPtr<FSlateDynamicImageBrush> ProgramIcon;

    FText ConfigurationText;

    TSharedPtr<STextBlock> ContentWidget;

    TArray<TSharedPtr<FProgramBrowserWorker>> BuildWorkers;
};