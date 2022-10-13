#pragma once

#include <string_view>

#include "gfx/image.hxx"


namespace loader
{
    gfx::image2D load_image(std::string_view name);
}
