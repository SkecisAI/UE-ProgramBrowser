

#include "FProgramModuleResource.h"


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

bool FProgramModuleResource::SetIcon(void* Data, int32 DataLen)
{
    if (!UpdateResource(Handle, RT_RCDATA, MAKEINTRESOURCE(101), 1033, Data, DataLen))
    {
        UE_LOG(LogTemp, Error, TEXT("UpdateResource failed!"));
        return false;
    }
}