

#include "SProgramTile.h"

#include "PakFileUtilities.h"
#include "ProgramBrowserBlueprintLibrary.h"
#include "SlateOptMacros.h"
#include "SPrimaryButton.h"
#include "ProgramData.h"
#include "SProgramBrowser.h"
#include "SProgramTileList.h"
#include "TargetReceipt.h"
#include "Async/Async.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"
#include "Widgets/Notifications/SNotificationList.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "ProgramTile"

SProgramTile::~SProgramTile()
{
    for (TSharedPtr<FProgramBrowserWorker>& Worker : ProcWorkers)
    {
        if (Worker.IsValid())
        {
            Worker->Join();
        }
    }
}

void SProgramTile::Construct(const FArguments& InArgs, TSharedRef<FProgram> InProgram, TSharedPtr<SProgramTileList> InOwner)
{
    Program = InProgram;
    Owner = InOwner;

    FString IconPath = InProgram->Path / TEXT("Resources/Program.ico");
    if (FPaths::FileExists(FPaths::ConvertRelativePathToFull(IconPath)))
    {
        ProgramIcon = MakeShareable(new FSlateDynamicImageBrush(FName(*IconPath), FVector2D(128, 128)));
    }
    else
    {
        FString DefaultIconPath = IPluginManager::Get().FindPlugin("ProgramBrowser")->GetBaseDir() / TEXT("Resources/DefaultProgram.png");
        ProgramIcon = MakeShareable(new FSlateDynamicImageBrush(FName(*DefaultIconPath), FVector2D(128, 128)));
    }

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
                            .HighlightText_Raw(&Owner->GetOwner().GetProgramFilter(), &FProgramFilter::GetRawFilterText)
                            .Font(FCoreStyle::GetDefaultFontStyle("Blod", 12))
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

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SButton)
                            .OnClicked(this, &SProgramTile::OnBrowserProgramSourceClicked)
                            .ButtonStyle(FAppStyle::Get(), "SimpleButton")
                            .ToolTipText(LOCTEXT("ProgramSourceBrowseTip", "Open the source folder"))
                            [

                                SNew(SImage)
                                .Image(FAppStyle::Get().GetBrush("Icons.BrowseContent"))
                                .DesiredSizeOverride(FVector2D(20.f, 20.f))
                                .ColorAndOpacity(FSlateColor::UseForeground())
                            ]
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
    FNotificationInfo Info(FText::FromString(FString::Printf(TEXT("Building %s..."), *Program->Name.ToString())));
    Info.ExpireDuration = 5.0f;
    Info.bUseThrobber = true;
    BuildNotification = FSlateNotificationManager::Get().AddNotification(Info);
    BuildNotification->SetCompletionState(SNotificationItem::CS_Pending);
    
    ProcWorkers.Add(MakeShareable(new FProgramBrowserWorker(
        FString::Printf(TEXT("BuildProgram-%s"), *(Program->Name.ToString())),
        [this]
        {
            bool bBuildRes = BuildProgramCommand();

            AsyncTask(ENamedThreads::GameThread, [this, bBuildRes]()
            {
                if (!BuildNotification.IsValid()) return;
                
                if (bBuildRes)
                {
                    BuildNotification->SetCompletionState(SNotificationItem::CS_Success);
                }
                else
                {
                    BuildNotification->SetCompletionState(SNotificationItem::CS_Fail);
                }

                BuildNotification->ExpireAndFadeout();
                BuildNotification.Reset();
            });
        })));

    return FReply::Handled();
}

FReply SProgramTile::OnPackageProgramClicked()
{
    FNotificationInfo Info(FText::FromString(FString::Printf(TEXT("Packaging %s..."), *Program->Name.ToString())));
    Info.ExpireDuration = 5.0f;
    Info.bUseThrobber = true;
    PackageNotification = FSlateNotificationManager::Get().AddNotification(Info);
    PackageNotification->SetCompletionState(SNotificationItem::CS_Pending);
    
    ProcWorkers.Add(MakeShareable(new FProgramBrowserWorker(
        FString::Printf(TEXT("PackageProgram-%s"), *Program->Name.ToString()),
        [&]()
        {
            UE_LOG(LogTemp, Display, TEXT("**************** PACKAGE PROGRAM %s START ****************"), *Program->Name.ToString());
            bool bPackageRes = PackageProgramCommand();
            UE_LOG(LogTemp, Display, TEXT("**************** PACKAGE PROGRAM %s END ****************"), *Program->Name.ToString());
            
            AsyncTask(ENamedThreads::GameThread, [this, bPackageRes]()
            {
                if (bPackageRes)
                {
                    
                    PackageNotification->SetCompletionState(SNotificationItem::CS_Success);
                }
                else
                {
                    PackageNotification->SetCompletionState(SNotificationItem::CS_Fail);
                }
                
                PackageNotification->ExpireAndFadeout();
                PackageNotification.Reset();

            });
        })));
    
    return FReply::Handled();
}

FReply SProgramTile::OnBrowserProgramSourceClicked()
{
    FPlatformProcess::ExploreFolder(*FPaths::ConvertRelativePathToFull(*Program->Path));
    
    return FReply::Handled();
}

bool SProgramTile::BuildProgramCommand()
{
    FString BuildCommandline;
    FString Configuration = ConfigurationText.ToString();
    FString OutputMessage;


    BuildCommandline += Program->Name.ToString() + TEXT(" ");
    BuildCommandline += TEXT("Win64 ");
    BuildCommandline += Configuration;
            
    return UProgramBrowserBlueprintLibrary::BuildProgram(BuildCommandline, Program->Name.ToString());
}

bool SProgramTile::PackageProgramCommand()
{
    if (!BuildProgramCommand())
    {
        UE_LOG(LogTemp, Error, TEXT("Package program %s failed: building failed"), *Program->Name.ToString());
        return false;
    }
    
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
        UE_LOG(LogTemp, Error, TEXT("Package program %s failed: target not existed."), *Program->Name.ToString());
        return false;
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

    if (!FFileHelper::SaveStringArrayToFile(PakCommandsList, *PakCommandsFile))
    {
        UE_LOG(LogTemp, Error, TEXT("Package program %s failed: save pak command file failed."), *Program->Name.ToString());
        return false;
    }
    
    FString UnrealPakCMD = FString::Printf(TEXT("\"%s\" -create=\"%s\" -compressionformats=Oodle"), *PakFilePath, *PakCommandsFile);

    if (!ExecuteUnrealPak(*UnrealPakCMD))
    {
        UE_LOG(LogTemp, Error, TEXT("Package program %s failed: execute UnrealPak failed."), *Program->Name.ToString());
        return false;
    }

    UProgramBrowserBlueprintLibrary::StageProgram(
        Program->Name.ToString(),
        ProgramTargetName,
        PakFilePath,
        FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Programs"), Program->Name.ToString(), Program->Name.ToString()),
        Program->Path / TEXT("Resources/Program.ico")
        );

    return true;
}

#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION