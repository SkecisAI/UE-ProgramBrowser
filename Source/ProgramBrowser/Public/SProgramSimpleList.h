#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"


struct FProgram;
class SProgramBrowser;

/**
 * 
*/
class PROGRAMBROWSER_API SProgramSimpleList : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SProgramSimpleList)
        {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, const TSharedRef<SProgramBrowser> InOwner);

    TSharedRef<ITableRow> ProgramSimpleList_OnGenerateRow(TSharedRef<FProgram> Program, const TSharedRef<STableViewBase>& TableViewBase);

private:
    TWeakPtr<SProgramBrowser> Owner;

    TArray<TSharedRef<FProgram>> Programs;
    
    TSharedPtr<SListView<TSharedRef<FProgram>>> ProgramSimpleListView;
    
};