#include "colorsim.hpp"

#include <asserter/src/asserter.hpp>
#include <text_utils/text_utils.hpp>

using namespace std;
using namespace dawn_wrapper;

namespace ns {

struct ColorSim::Pimpl {

    struct PaletteEntry {
        float_t m_base;
        float_t m_amp;
        float_t m_mod_base;
        float_t m_mod;
    };

    enum {
        BindGroupEntryUniform,
        BindGroupEntryRead,
        BindGroupEntryWrite,

        WorkGroupSize = 16,
        ColorElementCount = sizeof(PaletteEntry) / sizeof(float_t),
    };

    struct ColorsUniform {
        float_t m_age;
    };

    static float_t nrand(float_t n, float_t m)
    {
        static const unsigned steps(10000000);
        return n + m * float_t(rand() % steps) / steps;
    }

    static std::vector<PaletteEntry> make_palette_entries(unsigned count)
    {
        std::vector<PaletteEntry> entries;
        entries.reserve(count);

        for (auto i = 0; i < count; ++i) {
            PaletteEntry entry;
            entry.m_base = nrand(0.15, 0.7);

            const auto d = std::min<float_t>(entry.m_base, 1 - entry.m_base);
            entry.m_amp = nrand(d * 0.24, d * 0.99);

            entry.m_mod_base = nrand(0, 10);
            entry.m_mod = nrand(0.4, 0.05);

            entries.push_back(entry);
        }
        return entries;
    }

    static compute_wrapper compile_shader(dawn_plugin dawn)
    {
        auto result = dawn.make_compute();

        auto layout = result.make_bindgroup_layout();
        layout.add_uniform_buffer(BindGroupEntryUniform);
        layout.add_read_only_buffer(BindGroupEntryRead);
        layout.add_buffer(BindGroupEntryWrite);

        const auto template_script = R"(
            
            struct Uniform
            {
                m_age: f32
            };

            struct PaletteEntry
            {
                m_base: f32,
                m_amp: f32,
                m_mod_base: f32,
                m_mod: f32,
            };
            
            @group(0) @binding({{BindGroupEntryUniform}}) var<uniform> uniforms : Uniform;
            @group(0) @binding({{BindGroupEntryRead}}) var<storage, read> palette_entry: array<PaletteEntry>;
            @group(0) @binding({{BindGroupEntryWrite}}) var<storage, read_write> frame_state: array<f32>;
        
            @compute @workgroup_size({{WorkGroupSize}}, 1)
            fn colors_simulate(@builtin(global_invocation_id) id: vec3<u32>) {
                var index = id.x;
                var entry = palette_entry[index];
                frame_state[index] = entry.m_base + entry.m_amp * cos(entry.m_mod_base + entry.m_mod * uniforms.m_age);
            }
        )";

        auto script = text_utils::apply_variables(template_script, {
                                                                       { "WorkGroupSize", to_string(WorkGroupSize) },
                                                                       { "BindGroupEntryUniform", to_string(BindGroupEntryUniform) },
                                                                       { "BindGroupEntryRead", to_string(BindGroupEntryRead) },
                                                                       { "BindGroupEntryWrite", to_string(BindGroupEntryWrite) },
                                                                   });

        result.compile_shader(script, "colors_simulate");
        result.init_pipeline(layout);

        return result;
    }

    Pimpl(dawn_wrapper::dawn_plugin dawn, unsigned count, unsigned padding)
        : m_dawn(dawn)
        , m_count(count)
        , m_palette_entries(make_palette_entries(m_count))
        , m_uniform_colors(m_dawn.make_dst_buffer(sizeof(ColorsUniform), buffer_type::uniform))
        , m_result(m_dawn.make_dst_buffer((m_count + padding) * sizeof(float_t), buffer_type::storage))
        , m_entries(m_dawn.make_dst_buffer(m_count * sizeof(PaletteEntry), buffer_type::storage))
        , m_compute(compile_shader(m_dawn))
        , m_bindgroup(m_compute.make_bindgroup())
    {
        m_entries.write(m_palette_entries.data());

        m_bindgroup.add_buffer(BindGroupEntryUniform, m_uniform_colors);
        m_bindgroup.add_buffer(BindGroupEntryRead, m_entries);
        m_bindgroup.add_buffer(BindGroupEntryWrite, m_result);
    }

    void simulate(float_t ft)
    {
        m_age += ft;

        ColorsUniform uniform { m_age };
        m_uniform_colors.write(&uniform);
    }

    void compute(dawn_wrapper::encoder_wrapper encoder)
    {
        m_compute.compute(m_bindgroup, m_count / WorkGroupSize, 1, encoder);
    }

    buffer_wrapper elements_buffer()
    {
        return m_result;
    }

    dawn_plugin m_dawn;
    unsigned m_count;
    std::vector<PaletteEntry> m_palette_entries;
    buffer_wrapper m_uniform_colors;
    buffer_wrapper m_entries;
    buffer_wrapper m_result;
    float_t m_age;

    compute_wrapper m_compute;
    bindgroup_wrapper m_bindgroup;
};

#pragma mark - ColorSim

ColorSim::ColorSim(dawn_wrapper::dawn_plugin dawn, unsigned count, unsigned padding)
    : m_pimpl(make_unique<Pimpl>(dawn, count, padding))
{
}

ColorSim::~ColorSim() = default;

void ColorSim::simulate(float_t ft)
{
    m_pimpl->simulate(ft);
}

dawn_wrapper::buffer_wrapper ColorSim::elements_buffer()
{
    return m_pimpl->elements_buffer();
}

void ColorSim::compute(dawn_wrapper::encoder_wrapper encoder)
{
    m_pimpl->compute(encoder);
}

std::string ColorSim::get_wgsl_code(std::string class_name, std::string getter_name)
{
    return text_utils::apply_variables(R"(
        struct {{class_name}}
        {
            m_base: vec3f,
            m_amp: vec3f,
            m_mod_base: vec3f,
            m_mod: vec3f,
        };

        fn {{getter_name}}(t: f32, p:{{class_name}}) -> vec3f
        {
            const PI2 = 2 * acos(-1);
            return p.m_base + p.m_amp * cos( PI2 * (p.m_mod * t + p.m_mod_base) );
        }

    )",
        {
            { "class_name", class_name },
            { "getter_name", getter_name },
        });
}

}
