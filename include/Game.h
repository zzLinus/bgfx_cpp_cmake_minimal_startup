#include "bgfx_utils.h"
#include "common.h"
#include "imgui/imgui.h"
#include <bx/uint32_t.h>

struct PosColorVertex {
    float m_x;
    float m_y;
    float m_z;
    uint32_t m_abgr;

    static void init()
    {
        vrt_layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
    };

    static bgfx::VertexLayout vrt_layout;
};

// clang-format off
static PosColorVertex cubeVertices[] = {
    { -1.0f, 1.0f, 1.0f, 0xff000000 },
    { 1.0f, 1.0f, 1.0f, 0xff0000ff },
    { -1.0f, -1.0f, 1.0f, 0xff00ff00 },
    { 1.0f, -1.0f, 1.0f, 0xff00ffff },
    { -1.0f, 1.0f, -1.0f, 0xffff0000 },
    { 1.0f, 1.0f, -1.0f, 0xffff00ff },
    { -1.0f, -1.0f, -1.0f, 0xffffff00 },
    { 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t cubeTriList[] =
{
	0, 1, 2, // 0
	1, 3, 2,
	4, 6, 5, // 2
	5, 6, 7,
	0, 2, 4, // 4
	4, 2, 6,
	1, 5, 3, // 6
	5, 7, 3,
	0, 4, 1, // 8
	4, 5, 1,
	2, 3, 6, // 10
	6, 3, 7,
};

static const uint16_t cubeTriStrip[] =
{
	0, 1, 2,
	3, 7, 1,
	5, 0, 4,
	2, 6, 7,
	4,
	5,
};

static const uint16_t cubeLineList[] =
{
	0, 1,
	0, 2,
	0, 4,
	1, 3,
	1, 5,
	2, 3,
	2, 6,
	3, 7,
	4, 5,
	4, 6,
	5, 7,
	6, 7,
};

static const uint16_t cubeLineStrip[] =
{
	0, 2, 3, 1, 5, 7, 6, 4,
	0, 2, 6, 4, 5, 7, 3, 1,
	0,
};

static const uint16_t cubePoints[] =
{
	0, 1, 2, 3, 4, 5, 6, 7
};

static const char* ptNames[]
{
	"Triangle List",
	"Triangle Strip",
	"Lines",
	"Line Strip",
	"Points",
};

static const uint64_t ptState[]
{
	UINT64_C(0),
	BGFX_STATE_PT_TRISTRIP,
	BGFX_STATE_PT_LINES,
	BGFX_STATE_PT_LINESTRIP,
	BGFX_STATE_PT_POINTS,
};
// clang-format on

class Game : public entry::AppI {
public:
    Game(char const* _name, char const* _description, char const* _url)
        : entry::AppI(_name, _description, _url)
        , m_r(1)
        , m_g(1)
        , m_b(1)
    {
    }

    void init(int32_t _argc, char const* const* _argv, uint32_t _width, uint32_t _height) override;

    virtual int shutdown() override;

    bool update() override;

    entry::MouseState m_mouseState;

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_debug;
    uint32_t m_reset;
    bgfx::VertexBufferHandle m_vbh;
    bgfx::IndexBufferHandle m_ibh[BX_COUNTOF(ptState)];
    bgfx::ProgramHandle m_program;
    int64_t m_timeOffset;
    int32_t m_pt;

    bool m_r;
    bool m_g;
    bool m_b;
    bool m_a;
};
