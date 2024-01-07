

#include "SProgramTileList.h"
#include "SlateOptMacros.h"
#include "SProgramBrowser.h"
#include "SProgramTile.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "ProgramTileList"

void SProgramTileList::Construct(const FArguments& InArgs, const TSharedRef<SProgramBrowser> InOwner)
{
    Owner = InOwner;
    Programs = InOwner->GetPrograms();

    ProgramListView = SNew(SListView<TSharedRef<FProgram>>)
    .SelectionMode(ESelectionMode::None)
    .ListItemsSource(&Programs)
    .OnGenerateRow(this, &SProgramTileList::ProgramTileList_OnGenerateRow);

    ChildSlot
    [
        ProgramListView.ToSharedRef()
    ];
}

TSharedRef<ITableRow> SProgramTileList::ProgramTileList_OnGenerateRow(TSharedRef<FProgram> Program, const TSharedRef<STableViewBase>& TableViewBase)
{
    return SNew(STableRow<TSharedRef<FProgram>>, TableViewBase)
    [
        SNew(SProgramTile, Program, SharedThis(this))
    ];
}

#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION