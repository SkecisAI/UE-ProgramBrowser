

#include "SProgramBrowser.h"

#include "ProgramBrowser.h"
#include "SlateOptMacros.h"
#include "SProgramSimpleList.h"
#include "SProgramTileList.h"
#include "Widgets/Input/SSearchBox.h"
#include "ProgramData.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "ProgramEditor"

void SProgramBrowser::Construct(const FArguments& InArgs, const TArray<TSharedRef<FProgram>>& InPrograms)
{
    // for (TCHAR Letter = 'A'; Letter <= 'Z'; ++Letter)
    // {
    //     Programs.Add(MakeShareable(new FProgram{
    //     	FString(FTCHARToUTF8(&Letter)),
    //     	TEXT("test"),
    //     	TEXT("ABC"),
    //     	TEXT("1.0")}));
    // }
    Programs = InPrograms;

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
            [
                SNew(SSearchBox)
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

#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION