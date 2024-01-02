

#include "SProgramTile.h"

#include "PakFileUtilities.h"
#include "ProgramBrowserBlueprintLibrary.h"
#include "SlateOptMacros.h"
#include "SPrimaryButton.h"
#include "ProgramData.h"
#include "TargetReceipt.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "ProgramTile"

SProgramTile::~SProgramTile()
{
    for (TSharedPtr<FProgramBrowserWorker>& Worker : BuildWorkers)
    {
        if (Worker.IsValid())
        {
            Worker->Join();
        }
    }
}

void SProgramTile::Construct(const FArguments& InArgs, TSharedRef<FProgram> InProgram)
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
                        // configuration
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
                            .Text(LOCTEXT("ProgramBuildLable", "Build"))
                            .OnClicked(this, &SProgramTile::OnBuildProgramClicked)
                        ]

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SPrimaryButton)
                            .Text(LOCTEXT("ProgramPackageLable", "Package"))
                            .OnClicked(this, &SProgramTile::OnPackageProgramClicked)
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
    return ConfigurationText.IsEmpty() ? FText::FromString(TEXT("Configuration")) : ConfigurationText;
}

FReply SProgramTile::OnBuildProgramClicked()
{
    BuildWorkers.Add(MakeShareable(new FProgramBrowserWorker(
        FString::Printf(TEXT("BuildProgram-%s"), *(Program->Name.ToString())),
        [this]
        {
            BuildProgramCommand();
        })));

    return FReply::Handled();
}

FReply SProgramTile::OnPackageProgramClicked()
{
    BuildWorkers.Add(MakeShareable(new FProgramBrowserWorker(
        FString::Printf(TEXT("PackageProgram-%s"), *Program->Name.ToString()),
        [&]()
        {
            UE_LOG(LogTemp, Warning, TEXT("**************** PACKAGE PROGRAM %s START ****************"), *Program->Name.ToString());

            BuildProgramCommand();
            
            TArray<FString> PakCommandsList;
            FString ProgramTargetName;
            if (ConfigurationText.ToString().Equals(TEXT("Debug")))
            {
                ProgramTargetName = Program->Name.ToString() + TEXT("-Win64-Debug");
            }
            else if (ConfigurationText.ToString().Equals(TEXT("Shipping")))
            {
                ProgramTargetName = Program->Name.ToString() + TEXT("-Win64-Shipping");
            }
            else
            {
                ProgramTargetName = Program->Name.ToString();
            }

            FString ReceiptPath = FPaths::Combine(FPaths::EngineDir(), TEXT("Binaries\\Win64"), ProgramTargetName + TEXT(".target"));
            FTargetReceipt Receipt;
            if(!Receipt.Read(ReceiptPath))
            {
                UE_LOG(LogTemp, Error, TEXT("Program %s target not existed."), *Program->Name.ToString())
                return;
            }

            for (const FRuntimeDependency& Dependency : Receipt.RuntimeDependencies)
            {
                FString DependencyRelPath = Dependency.Path;
                PakCommandsList.Add(FString::Printf(TEXT("\"%s\" \"%s\" -compress"), *FPaths::ConvertRelativePathToFull(DependencyRelPath), *DependencyRelPath));
            }

            TArray<FString> AdditionalDependicesFiles;
            UProgramBrowserBlueprintLibrary::GetProgramAdditionalDependenciesDirs(AdditionalDependicesFiles);
            for (const FString& Dir : AdditionalDependicesFiles)
            {
                TArray<FString> AdditionalFiles;
                IFileManager::Get().FindFilesRecursive(AdditionalFiles, *Dir, TEXT("*.*"), true, false, false);
                for (FString& Filepath : AdditionalFiles)
                {
                    PakCommandsList.Add(FString::Printf(TEXT("\"%s\" \"%s\" -compress"), *FPaths::ConvertRelativePathToFull(Filepath), *Filepath));
                }
            }

    
            FString PakFilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Programs"), Program->Name.ToString()) / Program->Name.ToString() + TEXT(".pak");
            FString PakCommandsFile = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Programs"), Program->Name.ToString()) / TEXT("PakCommandList.txt");
            FFileHelper::SaveStringArrayToFile(PakCommandsList, *PakCommandsFile);
            
            FString UnrealPakCMD = FString::Printf(TEXT("\"%s\" -create=\"%s\" -compressionformats=Oodle"), *PakFilePath, *PakCommandsFile);

            ExecuteUnrealPak(*UnrealPakCMD);

            UProgramBrowserBlueprintLibrary::StageProgram(
                Program->Name.ToString(),
                ProgramTargetName,
                PakFilePath,
                FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Programs"), Program->Name.ToString(), Program->Name.ToString()));

            UE_LOG(LogTemp, Warning, TEXT("**************** PACKAGE PROGRAM %s END ****************"), *Program->Name.ToString());
        })));
    
    return FReply::Handled();
}

void SProgramTile::BuildProgramCommand()
{
    UE_LOG(LogTemp, Warning, TEXT("******* BUILD PROGRAM *******"));
    
    FString BuildCommandline;
    FString Configuration = ConfigurationText.ToString();
    FString OutputMessage;


    BuildCommandline += Program->Name.ToString() + TEXT(" ");
    BuildCommandline += TEXT("Win64 ");
    BuildCommandline += Configuration;
            
    UProgramBrowserBlueprintLibrary::BuildProgram(BuildCommandline, Program->Name.ToString());

    UE_LOG(LogTemp, Warning, TEXT("******* BUILD END *******"));
}

#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION