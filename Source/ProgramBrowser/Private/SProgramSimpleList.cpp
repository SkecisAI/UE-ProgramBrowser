

#include "SProgramSimpleList.h"
#include "SlateOptMacros.h"
#include "SProgramBrowser.h"
#include "ProgramData.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "ProgramSimpleList"

void SProgramSimpleList::Construct(const FArguments& InArgs, const TSharedRef<SProgramBrowser> InOwner)
{
    Owner = InOwner;
    Programs = InOwner->GetPrograms();

    ProgramSimpleListView = SNew(SListView<TSharedRef<FProgram>>)
    .SelectionMode(ESelectionMode::Single)
    .ListItemsSource(&Programs)
    .ListViewStyle(&FAppStyle::Get().GetWidgetStyle<FTableViewStyle>("SimpleListView"))
    .OnGenerateRow(this, &SProgramSimpleList::ProgramSimpleList_OnGenerateRow);

    ChildSlot
    [
        ProgramSimpleListView.ToSharedRef()
    ];
}

TSharedRef<ITableRow> SProgramSimpleList::ProgramSimpleList_OnGenerateRow(TSharedRef<FProgram> Program, const TSharedRef<STableViewBase>& TableViewBase)
{
    return SNew(STableRow<TSharedRef<FProgram>>, TableViewBase)
    [
        SNew(SBorder)
        .BorderImage(FAppStyle::GetBrush("NoBorder"))
        .Padding(5.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock)
                .Text(FText::FromName(Program->Name))
            ]
        ]
    ];
}

#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION