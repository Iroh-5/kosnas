#include "entity.hpp"

#include <nas/FsWriterEntity.edl.h>

#include <coresrv/nk/transport-kos.h>
#include <coresrv/sl/sl_api.h>

#include <kos/trace.h>

namespace nas {

FsWriterEntity& FsWriterEntity::Get()
{
    static FsWriterEntity instance;

    return instance;
}

Retcode FsWriterEntity::Run() noexcept
{
    ServiceId iid;
    Handle connHandle{ServiceLocatorRegister("FsWriterConnection", nullptr, 0, &iid)};
    if (connHandle == INVALID_HANDLE)
    {
        ERROR(FS_WRITER_ENTITY, "ServiceLocatorRegister failed");
        return rcFail;
    }

    NkKosTransport transport;
    NkKosTransport_Init(&transport, connHandle, nullptr, 0);

    nas_FsWriter_ops entityOps{
        .Init               = &FsWriterEntity::InitImpl,
        .InitiateFileReceit = &FsWriterEntity::InitiateFileReceitImpl,
        .ReceiveFileData    = &FsWriterEntity::ReceiveFileDataImpl
    };

    nas_FsWriter fsReader{
        .ops = &entityOps
    };

    nas_FsWriter_component entityComponent{
        .FsWriter = &fsReader
    };

    nas_FsWriterEntity_entity entity;
    nas_FsWriterEntity_entity_init(&entity, &entityComponent);

    nas_FsWriterEntity_entity_req req;
    nas_FsWriterEntity_entity_res res;

    char reqBuffer[nas_FsWriterEntity_entity_req_arena_size];
    nk_arena reqArena = NK_ARENA_INITIALIZER(reqBuffer, reqBuffer + sizeof(reqBuffer));

    char resBuffer[nas_FsWriterEntity_entity_res_arena_size];
    nk_arena resArena = NK_ARENA_INITIALIZER(resBuffer, resBuffer + sizeof(resBuffer));

    while (true)
    {
        nk_req_reset(&req);
        nk_arena_reset(&reqArena);
        nk_arena_reset(&resArena);

        auto rc{nk_transport_recv(&transport.base, &req.base_, &reqArena)};
        if (rc == NK_EOK)
        {
            nas_FsWriterEntity_entity_dispatch(&entity, &req.base_, &reqArena, &res.base_, &resArena);
        }
        else
        {
            ERROR(FS_WRITER_ENTITY, "nk_transport_recv failed: %d", rc);
            continue;
        }

        rc = nk_transport_reply(&transport.base, &res.base_, &resArena);
        if (rc != NK_EOK)
        {
            ERROR(FS_WRITER_ENTITY, "nk_transport_reply failed: %d", rc);
        }
    }

    return rcOk;
}

nk_err_t FsWriterEntity::InitImpl(
    [[maybe_unused]] nas_FsWriter* self,
    [[maybe_unused]] const nas_FsWriter_Init_req* req,
    [[maybe_unused]] const nk_arena* reqArena,
    [[maybe_unused]] nas_FsWriter_Init_res* res,
    [[maybe_unused]] nk_arena* resArena)
{
    return NK_EOK;
}

nk_err_t FsWriterEntity::InitiateFileReceitImpl(
    [[maybe_unused]] nas_FsWriter* self,
    [[maybe_unused]] const nas_FsWriter_InitiateFileReceit_req* req,
    [[maybe_unused]] const nk_arena* reqArena,
    [[maybe_unused]] nas_FsWriter_InitiateFileReceit_res* res,
    [[maybe_unused]] nk_arena* resArena)
{
    return NK_EOK;
}

nk_err_t FsWriterEntity::ReceiveFileDataImpl(
    [[maybe_unused]] nas_FsWriter* self,
    [[maybe_unused]] const nas_FsWriter_ReceiveFileData_req* req,
    [[maybe_unused]] const nk_arena* reqArena,
    [[maybe_unused]] nas_FsWriter_ReceiveFileData_res* res,
    [[maybe_unused]] nk_arena* resArena)
{
    return NK_EOK;
}

} // namespace nas
