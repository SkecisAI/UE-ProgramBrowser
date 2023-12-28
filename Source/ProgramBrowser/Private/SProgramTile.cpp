

#include "SProgramTile.h"
#include "SlateOptMacros.h"
#include "SPrimaryButton.h"
#include "SProgramBrowser.h"
#include "Interfaces/IPluginManager.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "ProgramTile"

void SProgramTile::Construct(const FArguments& InArgs, TSharedRef<IProgram> InProgram)
{
    Program = InProgram;

    FString IconPath = IPluginManager::Get().FindPlugin("ProgramBrowser")->GetBaseDir() / TEXT("Resources/DefaultProgram.png");
    ProgramIcon = MakeShareable(new FSlateDynamicImageBrush(FName(*IconPath), FVector2D(128, 128)));

    TArray Configuration = {FName("Debug"), FName("Development"), FName("Shipping")};
    ConfigurationText = FText::FromName(Configuration[1]);

    FMenuBuilder MenuBuilder(true, nullptr);
    for (const FName& Name : Configuration)
    {
        FUIAction Action(FExecuteAction::CreateSP(this, &SProgramTile::HandleConfigurationChanged, Name));
        MenuBuilder.AddMenuEntry(FText::FromString(Name.ToString()), FText::GetEmpty(), FSlateIcon(), Action);
    }

    ContentWidget = SNew(STextBlock)
        .Font(FCoreStyle::Get().GetFontStyle(TEXT("SmallFont")))
        .Text(this, &SProgramTile::GetConfigText);
    

    ChildSlot
    [
        SNew(SBorder)
        .BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
        .Padding(5.0f)
        [
            SNew(SBorder)
            .Padding(5.0f)
            [
                SNew(SHorizontalBox)
                
                + SHorizontalBox::Slot()
                .VAlign(VAlign_Top)
                .AutoWidth()
                [
                    SNew(SBox)
                    .WidthOverride(70.0f)
                    .HeightOverride(70.0f)
                    [
                        SNew(SBorder)
                        [
                            SNew(SImage)
                            .Image(ProgramIcon.Get())
                        ]
                    ]
                ]

                + SHorizontalBox::Slot()
                [
                    SNew(SVerticalBox)
                    // name + version
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SHorizontalBox)

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(5.0f)
                        [
                            SNew(STextBlock)
                            .Text(this, &SProgramTile::GetProgramNameText)
                        ]

                        + SHorizontalBox::Slot()
                        .HAlign(HAlign_Right)
                        [
                            SNew(SHorizontalBox)
                            
                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("ProgramVersionLabel", "Version "))
                            ]

                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                SNew(STextBlock)
                                .Text(FText::FromName(Program->Version))
                            ]
                        ]
                    ]
                    // desc
                    + SVerticalBox::Slot()
                    .Padding(5.0f)
                    .AutoHeight()
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(Program->Description))
                    ]
                    // buttons
                    + SVerticalBox::Slot()
                    .HAlign(HAlign_Right)
                    [
                        SNew(SHorizontalBox)

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SComboButton)
                            .VAlign(VAlign_Center)
                            .ComboButtonStyle(FAppStyle::Get(), "ComboButton")
                            .ButtonContent()
                            [
                                ContentWidget.ToSharedRef()
                            ]
                            .ContentPadding(1.0f)
                            .MenuContent()
                            [
                                MenuBuilder.MakeWidget()
                            ]
                        ]

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SPrimaryButton)
                            .Text(LOCTEXT("ProgramPackageLabel", "Package"))
                        ]
                    ]
                ]
            ]
        ]
    ];
}

FText SProgramTile::GetProgramNameText() const
{
    return FText::FromName(Program->Name);
}

void SProgramTile::HandleConfigurationChanged(FName Name)
{
    ConfigurationText = FText::FromName(Name);
}

FText SProgramTile::GetConfigText() const
{
    return ConfigurationText;
}

#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION