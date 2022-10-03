#include "bgfx_utils.h"
#include "common.h"
#include "entry/entry.h"
#include "imgui/imgui.h"
#include "packrect.h"
#include <bimg/decode.h>
#include <bx/rng.h>
#include <list>

struct PosTexcoordVertex {
    float m_x;
    float m_y;
    float m_z;
    float m_u;
    float m_v;
    float m_w;

    static void init()
    {
        ms_layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 3, bgfx::AttribType::Float)
            .end();
    };

    static bgfx::VertexLayout ms_layout;
};

// clang-format off
static PosTexcoordVertex s_cubeVertices[] =
{
	{-1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f },
	{ 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f },
	{-1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f },
	{ 1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f },

	{-1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f },
	{ 1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f },
	{-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f },
	{ 1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f },

	{-1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f },
	{-1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f },
	{-1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f },
	{-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f },

	{ 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f },
	{ 1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f },
	{ 1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f },
	{ 1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f },

	{-1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f },
	{ 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f },
	{-1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f },
	{ 1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f },

	{-1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f },
	{-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f },
	{ 1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f },
	{ 1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f },

	{-1.0f,  1.0f,  1.0f, -2.0f,  2.0f,  2.0f },
	{ 1.0f,  1.0f,  1.0f,  2.0f,  2.0f,  2.0f },
	{-1.0f, -1.0f,  1.0f, -2.0f, -2.0f,  2.0f },
	{ 1.0f, -1.0f,  1.0f,  2.0f, -2.0f,  2.0f },
};
BX_STATIC_ASSERT(BX_COUNTOF(s_cubeVertices) == 28);

static const uint16_t s_cubeIndices[] =
{
	 0,  1,  2, // 0
	 1,  3,  2,

	 4,  6,  5, // 2
	 5,  6,  7,

	 8, 10,  9, // 4
	 9, 10, 11,

	12, 14, 13, // 6
	14, 15, 13,

	16, 18, 17, // 8
	18, 19, 17,

	20, 22, 21, // 10
	21, 22, 23,
};
BX_STATIC_ASSERT(BX_COUNTOF(s_cubeIndices) == 36);

// clang-format on
static const uint16_t kTextureSide = 512;
static const uint32_t kTexture2dSize = 256;

class Game : public entry::AppI {
public:
    Game(char const* _name, char const* _description, char const* _url)
        : entry::AppI(_name, _description, _url)
        , m_cube(kTextureSide)
    {
    }

    void init(int32_t _argc, char const* const* _argv, uint32_t _width, uint32_t _height) override;

    virtual int shutdown() override;

    void ImGuiDescription(float _x, float _y, float _z, float const* _worldToScreen, char const* _text);

    bool update() override;

    entry::MouseState m_mouseState;

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_debug;
    uint32_t m_reset;

    uint8_t* m_texture2dData;
    uint32_t m_numTextures3d;
    bool m_texture3DSupported;
    bool m_blitSupported;
    bool m_computeSupported;
    bool m_showDescriptions;

    std::list<PackCube> m_quads;
    RectPackCubeT<256> m_cube;
    int64_t m_updateTime;
    int64_t m_timeOffset;
    bx::RngMwc m_rng;

    uint32_t m_hit;
    uint32_t m_miss;

    uint8_t m_rr;
    uint8_t m_gg;
    uint8_t m_bb;

    bgfx::TextureHandle m_textures[24];
    bgfx::TextureHandle m_textures3d[3];
    bgfx::TextureHandle m_texture2d;
    bgfx::TextureHandle m_textureCube[4];
    bgfx::TextureHandle m_blitTestA;
    bgfx::TextureHandle m_blitTestB;
    bgfx::TextureHandle m_blitTestC;
    bgfx::FrameBufferHandle m_textureCubeFaceFb[6];
    bgfx::IndexBufferHandle m_ibh;
    bgfx::VertexBufferHandle m_vbh;
    bgfx::ProgramHandle m_program3d;
    bgfx::ProgramHandle m_programCmp;
    bgfx::ProgramHandle m_programCompute;
    bgfx::ProgramHandle m_program;
    bgfx::UniformHandle u_time;
    bgfx::UniformHandle s_texColor;
    bgfx::UniformHandle s_texCube;
};
