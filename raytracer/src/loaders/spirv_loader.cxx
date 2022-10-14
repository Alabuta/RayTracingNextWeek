#ifdef max
#undef max
#endif

#include <cstddef>
#include <limits>
#include <vector>

#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

#include <string_view>
using namespace std::string_view_literals;

#include "loaders/spirv_loader.hxx"


namespace loader
{
    std::vector<std::byte> load_spirv(std::string_view name)
    {
        fs::path contents{ "shaders/bin"sv };

        if (!fs::exists(fs::current_path() / contents))
            contents = fs::current_path() / "../"sv / contents;

        auto path = contents / name;

        std::ifstream file{ path.c_str(), std::ios::in | std::ios::binary };

        if (file.bad() || file.fail())
            return { };

        auto const start_pos = file.tellg();
        file.ignore(std::numeric_limits<std::streamsize>::max());

        std::vector<std::byte> byte_code(static_cast<std::size_t>(file.gcount()));

        file.seekg(start_pos);

        if (!byte_code.empty())
            file.read(reinterpret_cast<char *>(std::data(byte_code)), static_cast<std::streamsize>(std::size(byte_code)));

        return byte_code;
    }
}
