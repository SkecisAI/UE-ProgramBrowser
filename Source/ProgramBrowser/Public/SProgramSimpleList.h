#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"


struct IProgram;
class SProgramBrowser;

/**
 * 
*/
class SProgramSimpleList : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SProgramSimpleList)
        {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, const TSharedRef<SProgramBrowser> InOwner);

    TSharedRef<ITableRow> ProgramSimpleList_OnGenerateRow(TSharedRef<IProgram> Program, const TSharedRef<STableViewBase>& TableViewBase);

private:
    TWeakPtr<SProgramBrowser> Owner;

    TArray<TSharedRef<IProgram> Programs;
    
    TSharedPtr<SListView<TSharedRef<IProgram>>> ProgramSimpleListView;
    
};