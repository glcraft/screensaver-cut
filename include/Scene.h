#include <chrono>
#include <libglw/GLWrapper.h>
#include <libglw/Shaders.h>

struct ShaderParams
{
    glm::vec3 color;
    glm::vec2 pos, dir;
    float overture;
};

class Scene {
public:
    Scene();
    ~Scene();

    void init(uint32_t width, uint32_t height);

    void update();
    void render(bool tex = false) const;
private:
    void resetTime();
    void drawScene(bool tex) const;
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

    gl::ArrayBuffer<glm::vec2>::uptr m_screenBuffer;
    gl::sl::Program m_shader;
    high_clock::time_point m_time;
    high_clock::time_point m_timeChange;
    float currentTimeBound, nextTimeBound, prevTimeBound;
    float currentTime;

    ShaderParams m_line;
};