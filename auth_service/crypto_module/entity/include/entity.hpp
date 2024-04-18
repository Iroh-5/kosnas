#pragma once

#include <nas/CryptoModuleEntity.edl.h>

#include <rtl/retcode.h>

namespace nas {

class CryptoModuleEntity
{
public:
    static CryptoModuleEntity& Get();

    Retcode Run() noexcept;

private:
    CryptoModuleEntity() = default;

private:
    static nk_err_t GetSHA256HashImpl(
        nas_CryptoModule* self,
        const nas_CryptoModule_GetSHA256Hash_req* req,
        const nk_arena* reqArena,
        nas_CryptoModule_GetSHA256Hash_res* res,
        nk_arena* resArena);
};

} // namespace nas
