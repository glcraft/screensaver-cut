#pragma once
#include <chrono>
#include <libglw/GLWrapper.h>
#include <libglw/Shaders.h>
#include <Input.h>
#include <filesystem>

double tr_cos(double);

struct ShaderParams
{
    glm::vec3 color;
    glm::vec2 pos, dir;
    float overture;
};
class MainGame
{
public:
    void init(const void* data = nullptr);
    void display();
    void setPathRoot(std::filesystem::path pRoot);
    void setWindowed(bool windowed);
private:
    void resetTime();
    void drawScene(bool tex);
    void resetLine();
    using high_clock = std::chrono::high_resolution_clock;

    struct Screen
    {
        struct FBO
        {
            void init(gl::Sampler::sptr sampler, glm::ivec2 size);
            gl::Texture::uptr texture;
            gl::Framebuffer::uptr fbo;
        } blitter, drawer;
    }m_screens;
#ifdef _WIN32
    HWND id;
#endif
    bool m_windowed=false;
    std::filesystem::path m_pathRoot;

    Input m_input;
    gl::ArrayBuffer<glm::vec2>::uptr m_screenBuffer;
    gl::sl::Program m_shader;
    high_clock::time_point m_time;
    high_clock::time_point m_timeChange;
    float currentTimeBound, nextTimeBound, prevTimeBound;
    float currentTime;

    ShaderParams m_line;
};