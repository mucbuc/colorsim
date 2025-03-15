#pragma once

#include <dawn_wrapper/src/dawn_wrapper.h>
#include <memory>

namespace ns {

struct ColorSim {
    ColorSim(dawn_wrapper::dawn_plugin dawn, unsigned count, unsigned padding = 0);
    ~ColorSim();

    void simulate(float_t ft);
    dawn_wrapper::buffer_wrapper elements_buffer();
    void compute(dawn_wrapper::encoder_wrapper encoder);

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> m_pimpl;
};

}
