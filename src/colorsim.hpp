#pragma once

#include <dawn_wrapper/src/dawn_wrapper.h>
#include <memory>
#include <string>

namespace ns {

struct ColorSim {
    ColorSim(dawn_wrapper::dawn_plugin dawn, unsigned count, unsigned padding = 0);
    ~ColorSim();

    void simulate(float_t ft);
    dawn_wrapper::buffer_wrapper elements_buffer();
    void compute(dawn_wrapper::encoder_wrapper encoder);
    static std::string get_wgsl_code(std::string class_name, std::string getter_name);

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> m_pimpl;
};

}
