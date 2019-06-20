#pragma once

#include <cstddef>
#include <limits>
#include <vector>
#include <optional>

#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

#include <string_view>
using namespace std::string_view_literals;

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


namespace loader {
struct image_data {
    std::int32_t width{0}, height{0};

    std::vector<std::byte> bytes;
};

std::optional<loader::image_data> load_image(std::string_view name)
{
    fs::path contents{"images"sv};

    if (!fs::exists(fs::current_path() / contents))
        contents = fs::current_path() / "../"sv / contents;

    auto path = contents / name;

    std::string str;// { std::begin(path), std::end(path) };

    loader::image_data image_data;

    auto &&[width, height, bytes] = image_data;

    std::int32_t components = -1;

    auto data = stbi_load(str.c_str(), &width, &height, &components, 1);

    return image_data;
}
}
