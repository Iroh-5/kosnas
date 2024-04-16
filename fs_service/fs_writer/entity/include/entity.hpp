#pragma once

#include <nas/FsWriterEntity.edl.h>

#include <rtl/retcode.h>

namespace nas {

class FsWriterEntity
{
public:
    static FsWriterEntity& Get();

    Retcode Run() noexcept;

private:
    FsWriterEntity() = default;

private:
    static nk_err_t InitImpl(
        nas_FsWriter* self,
        const nas_FsWriter_Init_req* req,
        const nk_arena* reqArena,
        nas_FsWriter_Init_res* res,
        nk_arena* resArena);

    static nk_err_t InitiateFileReceitImpl(
        nas_FsWriter* self,
        const nas_FsWriter_InitiateFileReceit_req* req,
        const nk_arena* reqArena,
        nas_FsWriter_InitiateFileReceit_res* res,
        nk_arena* resArena);

    static nk_err_t ReceiveFileDataImpl(
        nas_FsWriter* self,
        const nas_FsWriter_ReceiveFileData_req* req,
        const nk_arena* reqArena,
        nas_FsWriter_ReceiveFileData_res* res,
        nk_arena* resArena);
};

} // namespace nas
