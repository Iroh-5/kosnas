#include "utils.hpp"

#include <rtl/string.h>

namespace nas::utils {

std::string GetArenaString(const nk_arena *arena, const nk_ptr_t *src)
{
    nk_uint32_t msgLength = 0;

    const nk_char_t* nkMsg = nk_arena_get(nk_char_t, arena, src, &msgLength);

    return nkMsg != nullptr ? nkMsg : "";
}

Retcode StringToArena(const std::string &str, nk_arena *arena, nk_ptr_t *dst)
{
    nk_char_t *nk_str = nk_arena_alloc(nk_char_t, arena, dst, str.length() + 1);
    if (nk_str == nullptr)
    {
        return rcOutOfMemory;
    }

    rtl_strncpy(nk_str, str.c_str(), str.length() + 1);

    return rcOk;
}

} // namespace nas::utils
