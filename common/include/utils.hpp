#pragma once

#include <coresrv/nk/transport-kos.h>

#include <rtl/retcode.h>

#include <string>

namespace nas::utils {

std::string GetArenaString(const nk_arena* arena, const nk_ptr_t* src);
Retcode     StringToArena(const std::string& str, nk_arena* arena, nk_ptr_t* dst);

} // namespace nas::utils
