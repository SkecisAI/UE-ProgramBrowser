

#include "SProgramBrowser.h"

#include "ProgramBrowser.h"
#include "SlateOptMacros.h"
#include "SProgramSimpleList.h"
#include "SProgramTileList.h"
#include "Widgets/Input/SSearchBox.h"
#include "ProgramData.h"
#include "Misc/FileHelper.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "ProgramEditor"

void ProgramItemToString(const FProgram* Program, TArray<FString>& OutFilterString)
{
    OutFilterString.Add(Program->Name.ToString());
}


void SProgramBrowser::Construct(const FArguments& InArgs)
{
    InitilizeProgramsData();

    ProgramTextFilter = MakeShareable(new FProgramFilter(FProgramFilter::FItemToStringArray::CreateStatic(&ProgramItemToString)));

    TSharedRef<SBorder> MainContent = SNew(SBorder)
    .BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
    [
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            .VAlign(VAlign_Center)
            .HAlign(HAlign_Left)
            .Padding(FMargin(10))
            .AutoWidth()
            [
                SNew(SButton)
                .OnClicked(this, &SProgramBrowser::OnCreateProgramClicked)
                .Content()
                [
                    SNew(SHorizontalBox)

                    + SHorizontalBox::Slot()
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SImage)
                        .Image(FAppStyle::Get().GetBrush("Icons.Plus"))
                    ]
                    
                    + SHorizontalBox::Slot()
                    .VAlign(VAlign_Center)
                    .AutoWidth()
                    [
                        SNew(STextBlock)
                        .TextStyle(FAppStyle::Get(), "SmallButtonText")
                        .Text(LOCTEXT("NewProgramLabel", "Add"))
                    ]
                ]
            ]
            
            + SHorizontalBox::Slot()
            .Padding(5.0f)
            .FillWidth(1.0)
            .HAlign(HAlign_Fill)
            [
                SNew(SSearchBox)
                .OnTextChanged(this, &SProgramBrowser::SearchBox_OnSearchProgramTextChanged)
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5.0f)
            .HAlign(HAlign_Right)
            [
                SNew(SButton)
                .OnClicked(this, &SProgramBrowser::OnOpenProgramPackageDir)
                .ToolTipText(LOCTEXT("OpenProgramsPackageDirToolTipText", "open the directory of packaged programs."))
                .Content()
                [
                    SNew(SHorizontalBox)

                    + SHorizontalBox::Slot()
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                    .Padding(2.0f)
                    [
                        SNew(SImage)
                        .Image(FAppStyle::Get().GetBrush("Icons.Package"))
                    ]
                        
                    + SHorizontalBox::Slot()
                    .VAlign(VAlign_Center)
                    .AutoWidth()
                    [
                        SNew(STextBlock)
                        .TextStyle(FAppStyle::Get(), "SmallButtonText")
                        .Text(LOCTEXT("ProgramPackagesLabel", "Program Packages"))
                    ]
                ]
            ]
        ]

        + SVerticalBox::Slot()
        [
            SNew(SSplitter)
            .Style(FAppStyle::Get(), "SplitterPanel")
            
            + SSplitter::Slot()
            .Value(0.3f)
            [
                SAssignNew(ProgramSimpleList, SProgramSimpleList, SharedThis(this))
            ]

            + SSplitter::Slot()
            [
                SAssignNew(ProgramTileList, SProgramTileList, SharedThis(this))
            ]
        ]
    ];

    ChildSlot
    [
        MainContent
    ];
}

FReply SProgramBrowser::OnCreateProgramClicked()
{
    FGlobalTabmanager::Get()->TryInvokeTab(FProgramBrowserModule::ProgramBrowserCreatorTabName);

    return FReply::Handled();
}

FReply SProgramBrowser::OnOpenProgramPackageDir()
{
    FPlatformProcess::ExploreFolder(*FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectSavedDir(), "Programs")));

    return FReply::Handled();
}

void SProgramBrowser::SearchBox_OnSearchProgramTextChanged(const FText& Text)
{
    ProgramTextFilter->SetRawFilterText(Text);
}

void SProgramBrowser::InitilizeProgramsData()
{
    TArray<FString> Files;
    IFileManager::Get().FindFiles(Files, *(FProgramBrowserModule::ProgramsDir / TEXT("*")), false, true);
    for (const FString& File : Files)
    {
        FString Desc;
        FFileHelper::LoadFileToString(Desc, *(FProgramBrowserModule::ProgramsDir / File / TEXT("Resources/Desc.txt")));
        
        Programs.Add(MakeShareable(new FProgram(
            File,
            Desc,
            "",
            "1.0",
            FProgramBrowserModule::ProgramsDir / File
            )));
    }
}

#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION