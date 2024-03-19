#pragma once

#include "CoreMinimal.h"
#include "Misc/TextFilter.h"
#include "Widgets/SCompoundWidget.h"

struct FProgram;
class SProgramTileList;
class SProgramSimpleList;

using FProgramFilter = TTextFilter<const FProgram*>;

/**
 * 
*/
class PROGRAMBROWSER_API SProgramBrowser : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SProgramBrowser)
        {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    
    const TArray<TSharedRef<FProgram>>& GetPrograms() {return Programs;}

    FReply OnCreateProgramClicked();

    FReply OnOpenProgramPackageDir();

    void SearchBox_OnSearchProgramTextChanged(const FText& Text);

    FProgramFilter& GetProgramFilter()
    {
        return *ProgramTextFilter;
    }
    
private:
    TSharedPtr<SProgramTileList> ProgramTileList;
    
    TSharedPtr<SProgramSimpleList> ProgramSimpleList;

    TArray<TSharedRef<FProgram>> Programs;

    void InitilizeProgramsData();

    TSharedPtr<FProgramFilter> ProgramTextFilter; 
};