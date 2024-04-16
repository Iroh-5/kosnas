#pragma once

#include <nas/CredStorageReaderEntity.edl.h>

#include <rtl/retcode.h>

namespace nas {

class CredStorageReaderEntity
{
public:
    static CredStorageReaderEntity& Get();

    Retcode Run() noexcept;

private:
    CredStorageReaderEntity() = default;

private:
    static nk_err_t AuthenticateImpl(
        nas_CredStorageReader* self,
        const nas_CredStorageReader_Authenticate_req* req,
        const nk_arena* reqArena,
        nas_CredStorageReader_Authenticate_res* res,
        nk_arena* resArena);
};

} // namespace nas
