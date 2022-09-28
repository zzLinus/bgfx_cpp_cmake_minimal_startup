#include "bgfx_utils.h"
#include "common.h"
#include "imgui/imgui.h"
#include "logo.h"
#include <bx/uint32_t.h>

class Game : public entry::AppI {
public:
    Game(const char* _name, const char* _description, const char* _url)
        : entry::AppI(_name, _description, _url)
    {
    }

    void init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) override;

    virtual int shutdown() override;

    bool update() override;

    entry::MouseState m_mouseState;

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_debug;
    uint32_t m_reset;
};
