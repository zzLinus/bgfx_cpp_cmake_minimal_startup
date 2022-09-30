#include "Game.h"

bgfx::VertexLayout PosColorVertex::vrt_layout;

void Game::init(int32_t _argc, char const* const* _argv, uint32_t _width, uint32_t _height)
{
    Args args(_argc, _argv);

    m_width = _width;
    m_height = _height;
    m_debug = BGFX_DEBUG_TEXT;
    m_reset = BGFX_RESET_VSYNC;

    bgfx::Init init;
    init.type = args.m_type;
    init.vendorId = args.m_pciId;
    init.platformData.nwh = entry::getNativeWindowHandle(entry::kDefaultWindowHandle);
    init.platformData.ndt = entry::getNativeDisplayHandle();
    init.resolution.width = m_width;
    init.resolution.height = m_height;
    init.resolution.reset = m_reset;
    bgfx::init(init);

    // Enable debug text.
    bgfx::setDebug(m_debug);

    // Set view 0 clear state.
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x022c43ff, 1.0f, 0);

    // Create vertex stream declaration.
    PosColorVertex::init();

    // Create static vertex buffer.
    m_vbh = bgfx::createVertexBuffer(
        // Static data can be passed with bgfx::makeRef
        bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), PosColorVertex::vrt_layout);

    // Create static index buffer for triangle list rendering.
    m_ibh[0] = bgfx::createIndexBuffer(
        // Static data can be passed with bgfx::makeRef
        bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));

    // Create static index buffer for triangle strip rendering.
    m_ibh[1] = bgfx::createIndexBuffer(
        // Static data can be passed with bgfx::makeRef
        bgfx::makeRef(cubeTriStrip, sizeof(cubeTriStrip)));

    // Create static index buffer for line list rendering.
    m_ibh[2] = bgfx::createIndexBuffer(
        // Static data can be passed with bgfx::makeRef
        bgfx::makeRef(cubeLineList, sizeof(cubeLineList)));

    // Create static index buffer for line strip rendering.
    m_ibh[3] = bgfx::createIndexBuffer(
        // Static data can be passed with bgfx::makeRef
        bgfx::makeRef(cubeLineStrip, sizeof(cubeLineStrip))

    );

    // Create static index buffer for point list rendering.
    m_ibh[4] = bgfx::createIndexBuffer(
        // Static data can be passed with bgfx::makeRef
        bgfx::makeRef(cubePoints, sizeof(cubePoints)));

    m_program = loadProgram("vs_cubes", "fs_cubes");

    m_timeOffset = bx::getHPCounter();

    imguiCreate();
}

int Game::shutdown()
{
    imguiDestroy();

    // clean up
    for (uint32_t ii = 0; ii < BX_COUNTOF(m_ibh); ii++) {
        bgfx::destroy(m_ibh[ii]);
    }

    bgfx::destroy(m_vbh);
    bgfx::destroy(m_program);

    // Shutdown bgfx.
    bgfx::shutdown();

    return 0;
}

bool Game::update()
{
    if (!entry::processEvents(m_width, m_height, m_debug, m_reset, &m_mouseState)) {
        imguiBeginFrame(m_mouseState.m_mx, m_mouseState.m_my, (m_mouseState.m_buttons[entry::MouseButton::Left] ? IMGUI_MBUT_LEFT : 0) | (m_mouseState.m_buttons[entry::MouseButton::Right] ? IMGUI_MBUT_RIGHT : 0) | (m_mouseState.m_buttons[entry::MouseButton::Middle] ? IMGUI_MBUT_MIDDLE : 0), m_mouseState.m_mz, uint16_t(m_width), uint16_t(m_height));

        showExampleDialog(this);

        ImGui::SetNextWindowPos(
            ImVec2(m_width - m_width / 5.0f - 10.0f, 10.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(
            ImVec2(m_width / 5.0f, m_height / 3.5f), ImGuiCond_FirstUseEver);
        ImGui::Begin("Settings", NULL, 0);

        ImGui::Checkbox("Write R", &m_r);
        ImGui::Checkbox("Write G", &m_g);
        ImGui::Checkbox("Write B", &m_b);
        ImGui::Checkbox("Write A", &m_a);

        ImGui::Text("Primitive topology:");
        ImGui::Combo("##topology", (int*)&m_pt, ptNames, BX_COUNTOF(ptNames));

        ImGui::End();

        imguiEndFrame();

        float time = (float)((bx::getHPCounter() - m_timeOffset) / double(bx::getHPFrequency()));

        const bx::Vec3 at = { 0.0f, 0.0f, 0.0f };
        const bx::Vec3 eye = { 0.0f, 0.0f, -35.0f };

        // Set view and projection matrix for view 0.
        {
            float view[16];
            bx::mtxLookAt(view, eye, at);

            float proj[16];
            bx::mtxProj(proj, 60.0f, float(m_width) / float(m_height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
            bgfx::setViewTransform(0, view, proj);

            // Set view 0 default viewport.
            bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));
        }

        // This dummy draw call is here to make sure that view 0 is cleared
        // if no other draw calls are submitted to view 0.
        bgfx::touch(0);

        bgfx::IndexBufferHandle ibh = m_ibh[m_pt];
        uint64_t state = 0
            | (m_r ? BGFX_STATE_WRITE_R : 0)
            | (m_g ? BGFX_STATE_WRITE_G : 0)
            | (m_b ? BGFX_STATE_WRITE_B : 0)
            | (m_a ? BGFX_STATE_WRITE_A : 0)
            | BGFX_STATE_WRITE_Z
            | BGFX_STATE_DEPTH_TEST_LESS
            | BGFX_STATE_CULL_CW
            | BGFX_STATE_MSAA
            | ptState[m_pt];

        // Submit 11x11 cubes.
        for (uint32_t yy = 0; yy < 11; ++yy) {
            for (uint32_t xx = 0; xx < 11; ++xx) {
                float mtx[16];
                bx::mtxRotateXY(mtx, time + xx * 0.21f, time + yy * 0.37f);
                mtx[12] = -15.0f + float(xx) * 3.0f;
                mtx[13] = -15.0f + float(yy) * 3.0f;
                mtx[14] = 0.0f;

                // Set model matrix for rendering.
                bgfx::setTransform(mtx);

                // Set vertex and index buffer.
                bgfx::setVertexBuffer(0, m_vbh);
                bgfx::setIndexBuffer(ibh);

                // Set render states.
                bgfx::setState(state);

                // Submit primitive for rendering to view 0.
                bgfx::submit(0, m_program);
            }
        }

        // Advance to next frame. Rendering thread will be kicked to
        // process submitted rendering primitives.
        bgfx::frame();

        return true;
    }

    return false;
}

