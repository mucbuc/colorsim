#pragma once

#include <memory>
#include <string>
#include <vector>

#include "app_base.hpp"

namespace ns {

struct colors_simulate
    : private AppBase {
    using vector_type = std::vector<unsigned>;

    colors_simulate(dawn_wrapper::dawn_plugin dawn, unsigned count, unsigned padding = 0);
    void simulate(float_t ft);
    dawn_wrapper::buffer_wrapper elements_buffer();
    void compute(dawn_wrapper::encoder_wrapper encoder);

private:
    struct Pimpl;
    std::shared_ptr<Pimpl> m_pimpl;
};
}
