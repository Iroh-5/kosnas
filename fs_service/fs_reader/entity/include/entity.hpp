#pragma once

#include <nas/FsReaderEntity.edl.h>

#include <rtl/retcode.h>

namespace nas {

class FsReaderEntity
{
public:
    static FsReaderEntity& Get();

    Retcode Run() noexcept;

private:
    static nk_err_t InitImpl(
        nas_FsReader* self,
        const nas_FsReader_Init_req* req,
        const nk_arena* reqArena,
        nas_FsReader_Init_res* res,
        nk_arena* resArena);

    static nk_err_t InitiateFileTransmissionImpl(
        nas_FsReader* self,
        const nas_FsReader_InitiateFileTransmission_req* req,
        const nk_arena* reqArena,
        nas_FsReader_InitiateFileTransmission_res* res,
        nk_arena* resArena);

    static nk_err_t TransmitFileDataImpl(
        nas_FsReader* self,
        const nas_FsReader_TransmitFileData_req* req,
        const nk_arena* reqArena,
        nas_FsReader_TransmitFileData_res* res,
        nk_arena* resArena);
};

} // namespace nas
