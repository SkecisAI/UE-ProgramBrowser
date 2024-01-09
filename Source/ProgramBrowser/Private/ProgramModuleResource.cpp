

#include "ProgramModuleResource.h"


FProgramModuleResource::FProgramModuleResource(const FString& ExeFile, bool bDeleteExistingResources)
{
    Handle = BeginUpdateResource(*ExeFile, bDeleteExistingResources);
    if (!Handle)
    {
        UE_LOG(LogTemp, Error, TEXT("File: %s BeginUpdateResource Failed!"), *ExeFile);
    }
}

FProgramModuleResource::~FProgramModuleResource()
{
    EndUpdateResource(Handle, false);
}

bool FProgramModuleResource::SetData(int32 ResourceId, void* Data, int32 DataLen)
{
    if (!UpdateResource(Handle, RT_RCDATA, MAKEINTRESOURCE(ResourceId), 1033, Data, DataLen))
    {
        UE_LOG(LogTemp, Error, TEXT("UpdateResource failed!"));
        return false;
    }

    return true;
}

bool FProgramModuleResource::SetIcon(TArray<uint8>& GroupData, TArray<uint8>& IcoData)
{
    if (!UpdateResource(Handle, RT_GROUP_ICON, MAKEINTRESOURCE(101), 1033, GroupData.GetData(), GroupData.Num()))
    {
        UE_LOG(LogTemp, Error, TEXT("UpdateResource failed!"));
        return false;
    }

    if (!UpdateResource(Handle, RT_ICON, MAKEINTRESOURCE(1), 1033, IcoData.GetData(), IcoData.Num()))
    {
        UE_LOG(LogTemp, Error, TEXT("UpdateResource failed!"));
        return false;
    }

    return true;
}