#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"


struct FProgram;
class SProgramBrowser;

/**
 * 
*/
class PROGRAMBROWSER_API SProgramTileList : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SProgramTileList)
        {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, const TSharedRef<SProgramBrowser> InOwner);

    TSharedRef<ITableRow> ProgramTileList_OnGenerateRow(TSharedRef<FProgram> Program, const TSharedRef<STableViewBase>& TableViewBase);

private:
    TArray<TSharedRef<FProgram>> Programs;
    
    TSharedPtr<SListView<TSharedRef<FProgram>>> ProgramListView;
    
    TWeakPtr<SProgramBrowser> Owner;
};