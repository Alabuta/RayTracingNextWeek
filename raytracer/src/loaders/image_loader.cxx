#include <cstddef>
#include <limits>
#include <vector>

#include <string_view>
using namespace std::string_view_literals;

#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "gfx/image.hxx"
#include "loaders/image_loader.hxx"


namespace loader
{
    gfx::image2D load_image(std::string_view name)
    {

        fs::path contents{ "images"sv };

        if (!fs::exists(fs::current_path() / contents))
            contents = fs::current_path() / "../"sv / contents;

        auto path = contents / name;

        std::ifstream file{ path.c_str(), std::ios::in | std::ios::binary };

        if (file.bad() || file.fail())
            return { };

        auto const start_pos = file.tellg();
        file.ignore((std::numeric_limits<std::streamsize>::max)());

        std::vector<stbi_uc> buffer(static_cast<std::size_t>(file.gcount()));

        file.seekg(start_pos);

        if (!buffer.empty())
            file.read(reinterpret_cast<char *>(std::data(buffer)), static_cast<std::streamsize>(std::size(buffer)));

        std::int32_t width{ 0 }, height{ 0 };
        std::int32_t components{ -1 };

        stbi_set_flip_vertically_on_load(true);

        auto data = stbi_load_from_memory(std::data(buffer), static_cast<std::int32_t>(std::size(buffer)), &width, &height, &components, 0);

        auto image = gfx::create_image2D(width, height, GL_RGB8);

        glTextureSubImage2D(image.handle, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);

        return image;
    }
}
