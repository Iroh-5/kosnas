#include "entity.hpp"

#include <nas/FsReaderEntity.edl.h>

#include <coresrv/nk/transport-kos.h>
#include <coresrv/sl/sl_api.h>

#include <kos/trace.h>

namespace nas {

FsReaderEntity& FsReaderEntity::Get()
{
    static FsReaderEntity instance;

    return instance;
}

Retcode FsReaderEntity::Run() noexcept
{
    ServiceId iid;
    Handle connHandle{ServiceLocatorRegister("FsReaderConnection", nullptr, 0, &iid)};
    if (connHandle == INVALID_HANDLE)
    {
        ERROR(FS_READER_ENTITY, "ServiceLocatorRegister failed");
        return rcFail;
    }

    NkKosTransport transport;
    NkKosTransport_Init(&transport, connHandle, nullptr, 0);

    nas_FsReader_ops entityOps{
        .Init                     = &FsReaderEntity::InitImpl,
        .InitiateFileTransmission = &FsReaderEntity::InitiateFileTransmissionImpl,
        .TransmitFileData         = &FsReaderEntity::TransmitFileDataImpl
    };

    nas_FsReader fsReader{
        .ops = &entityOps
    };

    nas_FsReader_component entityComponent{
        .FsReader = &fsReader
    };

    nas_FsReaderEntity_entity entity;
    nas_FsReaderEntity_entity_init(&entity, &entityComponent);

    nas_FsReaderEntity_entity_req req;
    nas_FsReaderEntity_entity_res res;

    char reqBuffer[nas_FsReaderEntity_entity_req_arena_size];
    nk_arena reqArena = NK_ARENA_INITIALIZER(reqBuffer, reqBuffer + sizeof(reqBuffer));

    char resBuffer[nas_FsReaderEntity_entity_res_arena_size];
    nk_arena resArena = NK_ARENA_INITIALIZER(resBuffer, resBuffer + sizeof(resBuffer));

    while (true)
    {
        nk_req_reset(&req);
        nk_arena_reset(&reqArena);
        nk_arena_reset(&resArena);

        auto rc{nk_transport_recv(&transport.base, &req.base_, &reqArena)};
        if (rc == NK_EOK)
        {
            nas_FsReaderEntity_entity_dispatch(&entity, &req.base_, &reqArena, &res.base_, &resArena);
        }
        else
        {
            ERROR(FS_READER_ENTITY, "nk_transport_recv failed: %d", rc);
            continue;
        }

        rc = nk_transport_reply(&transport.base, &res.base_, &resArena);
        if (rc != NK_EOK)
        {
            ERROR(FS_READER_ENTITY, "nk_transport_reply failed: %d", rc);
        }
    }

    return rcOk;
}

nk_err_t FsReaderEntity::InitImpl(
    [[maybe_unused]] nas_FsReader* self,
    [[maybe_unused]] const nas_FsReader_Init_req* req,
    [[maybe_unused]] const nk_arena* reqArena,
    [[maybe_unused]] nas_FsReader_Init_res* res,
    [[maybe_unused]] nk_arena* resArena)
{
    return NK_EOK;
}

nk_err_t FsReaderEntity::InitiateFileTransmissionImpl(
    [[maybe_unused]] nas_FsReader* self,
    [[maybe_unused]] const nas_FsReader_InitiateFileTransmission_req* req,
    [[maybe_unused]] const nk_arena* reqArena,
    [[maybe_unused]] nas_FsReader_InitiateFileTransmission_res* res,
    [[maybe_unused]] nk_arena* resArena)
{
    return NK_EOK;
}

nk_err_t FsReaderEntity::TransmitFileDataImpl(
    [[maybe_unused]] nas_FsReader* self,
    [[maybe_unused]] const nas_FsReader_TransmitFileData_req* req,
    [[maybe_unused]] const nk_arena* reqArena,
    [[maybe_unused]] nas_FsReader_TransmitFileData_res* res,
    [[maybe_unused]] nk_arena* resArena)
{
    return NK_EOK;
}

} // namespace nas
