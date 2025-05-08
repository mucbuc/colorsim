/* example usage:

cpp:
m_colors(dawn, 27, 5)
...
m_colors.get_wgsl_code("ColorPalette", "getColor", entry_colors, 0) },

wgsl:
var color = getColor(color_factor + PHI_HALF);
*/

#pragma once

#include <dawn_wrapper/src/dawn_wrapper.h>
#include <memory>
#include <string>

namespace ns {

struct ColorSim {
    ColorSim(dawn_wrapper::dawn_plugin dawn, unsigned count, unsigned padding = 0);
    ~ColorSim();

    void simulate(float_t ft, dawn_wrapper::encoder_wrapper encoder);
    dawn_wrapper::buffer_wrapper buffer();
    static std::string get_wgsl_code(std::string class_name, std::string getter_name, unsigned entry, unsigned group);

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> m_pimpl;
};

}
