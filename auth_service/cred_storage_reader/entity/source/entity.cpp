#include "cred_storage_reader.hpp"
#include "entity.hpp"
#include "utils.hpp"

#include <nas/CredStorageReaderEntity.edl.h>

#include <coresrv/nk/transport-kos.h>
#include <coresrv/sl/sl_api.h>

#include <kos/trace.h>

namespace nas {

CredStorageReaderEntity& CredStorageReaderEntity::Get()
{
    static CredStorageReaderEntity instance;

    return instance;
}

Retcode CredStorageReaderEntity::Run() noexcept
{
    ServiceId iid;
    Handle connHandle{ServiceLocatorRegister("CredStorageReaderConnection", nullptr, 0, &iid)};
    if (connHandle == INVALID_HANDLE)
    {
        ERROR(CRED_STORAGE_READER_ENTITY, "ServiceLocatorRegister failed");
        return rcFail;
    }

    NkKosTransport transport;
    NkKosTransport_Init(&transport, connHandle, nullptr, 0);

    nas_CredStorageReader_ops entityOps{
        .Authenticate = &CredStorageReaderEntity::AuthenticateImpl
    };

    nas_CredStorageReader credStorageReader{
        .ops = &entityOps
    };

    nas_CredStorageReader_component entityComponent{
        .CredStorageReader = &credStorageReader
    };

    nas_CredStorageReaderEntity_entity entity;
    nas_CredStorageReaderEntity_entity_init(&entity, &entityComponent);

    nas_CredStorageReaderEntity_entity_req req;
    nas_CredStorageReaderEntity_entity_res res;

    char reqBuffer[nas_CredStorageReaderEntity_entity_req_arena_size];
    nk_arena reqArena = NK_ARENA_INITIALIZER(reqBuffer, reqBuffer + sizeof(reqBuffer));

    char resBuffer[nas_CredStorageReaderEntity_entity_res_arena_size];
    nk_arena resArena = NK_ARENA_INITIALIZER(resBuffer, resBuffer + sizeof(resBuffer));

    while (true)
    {
        nk_req_reset(&req);
        nk_arena_reset(&reqArena);
        nk_arena_reset(&resArena);

        auto rc{nk_transport_recv(&transport.base, &req.base_, &reqArena)};
        if (rc == NK_EOK)
        {
            nas_CredStorageReaderEntity_entity_dispatch(&entity, &req.base_, &reqArena, &res.base_, &resArena);
        }
        else
        {
            ERROR(CRED_STORAGE_READER_ENTITY, "nk_transport_recv failed: %d", rc);
            continue;
        }

        rc = nk_transport_reply(&transport.base, &res.base_, &resArena);
        if (rc != NK_EOK)
        {
            ERROR(CRED_STORAGE_READER_ENTITY, "nk_transport_reply failed: %d", rc);
        }
    }

    return rcOk;
}

nk_err_t CredStorageReaderEntity::AuthenticateImpl(
    [[maybe_unused]] nas_CredStorageReader* self,
    const nas_CredStorageReader_Authenticate_req* req,
    const nk_arena* reqArena,
    nas_CredStorageReader_Authenticate_res* res,
    [[maybe_unused]] nk_arena* resArena)
{
    const auto username{utils::GetArenaString(reqArena, &req->username)};
    const auto passwordHash{utils::GetArenaString(reqArena, &req->passwordHash)};

    res->rc = CredStorageReader::Get().Authenticate(username, passwordHash);

    return NK_EOK;
}

} // namespace nas
