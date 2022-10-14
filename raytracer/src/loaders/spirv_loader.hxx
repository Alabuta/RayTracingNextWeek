#pragma once

#include <cstddef>
#include <vector>
#include <string_view>

namespace loader
{
    std::vector<std::byte> load_spirv(std::string_view name);
}
