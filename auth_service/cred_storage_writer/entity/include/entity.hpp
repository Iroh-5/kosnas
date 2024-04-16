#pragma once

#include <nas/CredStorageWriterEntity.edl.h>

#include <rtl/retcode.h>

namespace nas {

class CredStorageWriterEntity
{
public:
    static CredStorageWriterEntity& Get();

    Retcode Run() noexcept;

private:
    CredStorageWriterEntity() = default;

private:
    static nk_err_t AddAuthEntryImpl(
        nas_CredStorageWriter* self,
        const nas_CredStorageWriter_AddAuthEntry_req* req,
        const nk_arena* reqArena,
        nas_CredStorageWriter_AddAuthEntry_res* res,
        nk_arena* resArena);
};

} // namespace nas
