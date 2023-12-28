#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"


struct IProgram;
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

    TSharedRef<ITableRow> ProgramTileList_OnGenerateRow(TSharedRef<IProgram> Program, const TSharedRef<STableViewBase>& TableViewBase);

private:
    TArray<TSharedRef<IProgram>> Programs;
    
    TSharedPtr<SListView<TSharedRef<IProgram>>> ProgramListView;
    
    TWeakPtr<SProgramBrowser> Owner;
};