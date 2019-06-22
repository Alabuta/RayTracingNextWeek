#pragma once

#ifdef max
#undef max
#endif

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

    GLenum internal_format;

    std::vector<std::byte> bytes;
};

std::optional<loader::image_data> load_image(std::string_view name)
{
    fs::path contents{"images"sv};

    if (!fs::exists(fs::current_path() / contents))
        contents = fs::current_path() / "../"sv / contents;

    auto path = contents / name;

    std::ifstream file{path.native(), std::ios::in | std::ios::binary};

    if (file.bad() || file.fail())
        return { };

    auto const start_pos = file.tellg();
    file.ignore(std::numeric_limits<std::streamsize>::max());

    std::vector<stbi_uc> buffer(static_cast<std::size_t>(file.gcount()));

    file.seekg(start_pos);

    if (!buffer.empty())
        file.read(reinterpret_cast<char *>(std::data(buffer)), static_cast<std::streamsize>(std::size(buffer)));

    loader::image_data image_data;

    auto &&[width, height, internal_format, bytes] = image_data;

    std::int32_t components = -1;

    auto data = stbi_load_from_memory(std::data(buffer), static_cast<std::int32_t>(std::size(buffer)), &width, &height, &components, 1);

    auto const buffer_size_in_bytes = width * height * components;// (components == 3 ? 4 : components);

    bytes.resize(buffer_size_in_bytes);

    std::transform(data, data + buffer_size_in_bytes, std::begin(bytes), [] (auto byte)
    {
        return static_cast<std::byte>(byte);
    });

    stbi_image_free(data);

    return image_data;
}
}
