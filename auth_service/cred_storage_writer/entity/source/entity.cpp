#include "entity.hpp"

#include <nas/CredStorageWriterEntity.edl.h>

#include <coresrv/nk/transport-kos.h>
#include <coresrv/sl/sl_api.h>

#include <kos/trace.h>

namespace nas {

CredStorageWriterEntity& CredStorageWriterEntity::Get()
{
    static CredStorageWriterEntity instance;

    return instance;
}

Retcode CredStorageWriterEntity::Run() noexcept
{
    ServiceId iid;
    Handle connHandle{ServiceLocatorRegister("CredStorageConnection", nullptr, 0, &iid)};
    if (connHandle == INVALID_HANDLE)
    {
        ERROR(CRED_STORAGE_WRITER_ENTITY, "ServiceLocatorRegister failed");
        return rcFail;
    }

    NkKosTransport transport;
    NkKosTransport_Init(&transport, connHandle, nullptr, 0);

    nas_CredStorageWriter_ops entityOps{
        .AddAuthEntry = &CredStorageWriterEntity::AddAuthEntryImpl
    };

    nas_CredStorageWriter credStorageWriter{
        .ops = &entityOps
    };

    nas_CredStorageWriter_component entityComponent{
        .CredStorageWriter = &credStorageWriter
    };

    nas_CredStorageWriterEntity_entity entity;
    nas_CredStorageWriterEntity_entity_init(&entity, &entityComponent);

    nas_CredStorageWriterEntity_entity_req req;
    nas_CredStorageWriterEntity_entity_res res;

    char reqBuffer[nas_CredStorageWriterEntity_entity_req_arena_size];
    nk_arena reqArena = NK_ARENA_INITIALIZER(reqBuffer, reqBuffer + sizeof(reqBuffer));

    char resBuffer[nas_CredStorageWriterEntity_entity_res_arena_size];
    nk_arena resArena = NK_ARENA_INITIALIZER(resBuffer, resBuffer + sizeof(resBuffer));

    while (true)
    {
        nk_req_reset(&req);
        nk_arena_reset(&reqArena);
        nk_arena_reset(&resArena);

        auto rc{nk_transport_recv(&transport.base, &req.base_, &reqArena)};
        if (rc == NK_EOK)
        {
            nas_CredStorageWriterEntity_entity_dispatch(&entity, &req.base_, &reqArena, &res.base_, &resArena);
        }
        else
        {
            ERROR(CRED_STORAGE_WRITER_ENTITY, "nk_transport_recv failed: %d", rc);
            continue;
        }

        rc = nk_transport_reply(&transport.base, &res.base_, &resArena);
        if (rc != NK_EOK)
        {
            ERROR(CRED_STORAGE_WRITER_ENTITY, "nk_transport_reply failed: %d", rc);
        }
    }

    return rcOk;
}

nk_err_t CredStorageWriterEntity::AddAuthEntryImpl(
    [[maybe_unused]] nas_CredStorageWriter* self,
    [[maybe_unused]] const nas_CredStorageWriter_AddAuthEntry_req* req,
    [[maybe_unused]] const nk_arena* reqArena,
    [[maybe_unused]] nas_CredStorageWriter_AddAuthEntry_res* res,
    [[maybe_unused]] nk_arena* resArena)
{
    return NK_EOK;
}

} // namespace nas
