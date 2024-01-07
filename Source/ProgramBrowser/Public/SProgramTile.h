

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class FProgramBrowserWorker;
class SProgramTileList;
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

    void Construct(const FArguments& InArgs, TSharedRef<FProgram> InProgram, TSharedPtr<SProgramTileList> InOwner);

    FText GetProgramNameText() const;

    void HandleConfigurationChanged(FName Name);

    FText GetConfigText() const;

    FReply OnBuildProgramClicked();

    FReply OnPackageProgramClicked();

    FReply OnBrowserProgramSourceClicked();

    bool BuildProgramCommand();

    bool PackageProgramCommand();
    
private:
    TSharedPtr<SProgramTileList> Owner;
    
    TSharedPtr<FProgram> Program;

    TSharedPtr<FSlateDynamicImageBrush> ProgramIcon;

    FText ConfigurationText;

    TSharedPtr<STextBlock> ContentWidget;

    TArray<TSharedPtr<FProgramBrowserWorker>> ProcWorkers;

    TSharedPtr<SNotificationItem> BuildNotification;
    TSharedPtr<SNotificationItem> PackageNotification;
};