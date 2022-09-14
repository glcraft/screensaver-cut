#include <random>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/color_space.hpp>
#include <MainGame.hpp>

double tr_cos(double x)
{
    return 0.5-cos(x*glm::pi<double>())*0.5;
}
void MainGame::setWindowed(bool windowed)
{
    m_windowed = windowed;
}
void MainGame::setPathRoot(std::filesystem::path pRoot)
{
    m_pathRoot = pRoot;
}
void MainGame::init(const void* data)
{
    SDL_DisplayMode dmode;
    gl::Object::SetAutoInstantiate(true);
    SDL_GetDisplayMode(0,0,&dmode);
    if (data)
        m_input.attachWindow((HWND)(data));
    else if (m_windowed)
        m_input.createWindow("LibMC", SDL_WINDOW_OPENGL);
    else
    {
        m_input.createWindow("LibMC", SDL_WINDOW_BORDERLESS|SDL_WINDOW_OPENGL, glm::vec2(0.f), glm::ivec2(dmode.w, dmode.h));
        SDL_ShowCursor(false);
        SDL_CaptureMouse(SDL_TRUE);
    }
        
    id = (HWND)(data);
    SDL_GL_SetSwapInterval(1);
    glewExperimental = true;
	glewInit();

    m_screenBuffer = std::make_unique<gl::ArrayBuffer<glm::vec2>>();
    m_screenBuffer->reserve(4);
    auto mapbuffer = m_screenBuffer->map(GL_WRITE_ONLY);
    mapbuffer[0] = glm::vec2(-1, -1);
    mapbuffer[1] = glm::vec2( 1, -1);
    mapbuffer[2] = glm::vec2( 1,  1);
    mapbuffer[3] = glm::vec2(-1,  1);
    m_screenBuffer->unmap();
    m_screenBuffer->attachVertexArray(std::make_shared<gl::VertexArray>());
    m_screenBuffer->set_attrib(gl::ArrayBuffer<glm::vec2>::Attrib<0>(0,2,GL_FLOAT, GL_FALSE));

    TRY_GLSL
    m_shader << gl::sl::Shader<gl::sl::Vertex>((m_pathRoot/"res/shaders/screen.vert").string())
             << gl::sl::Shader<gl::sl::Fragment>((m_pathRoot/"res/shaders/screen.frag").string())
             << gl::sl::link;
    CATCH_GLSL

    float ratio = static_cast<float>(m_input.getWindowData().size.x)/static_cast<float>(m_input.getWindowData().size.y);

    m_shader << gl::sl::use
        << gl::UniformStatic<float>("sizeLine", m_input.getWindowData().size.x/100.f)
        << gl::UniformStatic<float>("ratio", ratio)
        << gl::UniformStatic<float>("invratio", 1.f/ratio);
    
    auto sampler = std::make_shared<gl::Sampler>();
    sampler->setFiltering(gl::Sampler::Nearest);
    sampler->setWrap(gl::Sampler::MirroredClampToEdge);

    m_screens.blitter.init(sampler, m_input.getWindowData().size);
    m_screens.drawer.init(sampler, m_input.getWindowData().size);

    gl::Framebuffer::BindScreen();

    m_time = high_clock::now();
    m_timeChange = high_clock::now();
    nextTimeBound=0.2f;
    resetTime();
    currentTimeBound = glm::mix(prevTimeBound, nextTimeBound, 0.f);
}
void MainGame::resetTime()
{
    static std::random_device rd;
    static std::mt19937 mt(rd());
    static std::uniform_real_distribution<float> dist(0.07f, 0.5f);
    prevTimeBound = nextTimeBound;
    nextTimeBound = dist(mt);
}
void MainGame::drawScene(bool tex)
{
    gl::ClearColor(1.f, 0.f, 0.f, 1.f);
    
    m_shader << gl::sl::use
        << gl::UniformStatic<float>("time", currentTime)
        << gl::UniformStatic<int>("isTexture", tex);
                
    if (tex)
    {
        m_shader << gl::UniformStatic<int>("Texture0", 0);
        m_screens.drawer.texture->bindTo(0);
    }
    m_screenBuffer->draw(GL_TRIANGLE_FAN);
    m_screens.drawer.texture->unbind();
}
void MainGame::resetLine()
{
    static std::random_device rd;
    static std::mt19937 mt(rd());
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    static std::uniform_real_distribution<float> dist1(0.f, 1.0f);

    m_line.pos=glm::vec2(dist(mt), dist(mt));
    m_line.dir=normalize(glm::vec2(dist(mt), dist(mt)));
    m_line.overture = 0.015f+static_cast<float>(dist1(mt))*0.12f;
    m_line.color = glm::rgbColor(glm::vec3(dist1(mt)*360.f,1.f,1.f));

    m_shader << gl::sl::use
        << gl::UniformRef<glm::vec3>("line.color", m_line.color)
        << gl::UniformRef<glm::vec2>("line.pos", m_line.pos)
        << gl::UniformRef<glm::vec2>("line.dir", m_line.dir)
        << gl::UniformStatic<float>("line.overture", m_line.overture);
}
void MainGame::display()
{
    bool quit=false;
    resetLine();
    Input::Update(); // Init
    while (!quit)
    {
        Input::Update();
        // m_input.update();
        float timeChange = std::chrono::duration<float, std::ratio<1,1>>(high_clock::now()-m_timeChange).count();
        float realTime =std::chrono::duration<float, std::ratio<1,1>>(high_clock::now()-m_time).count();
        // currentTimeBound = glm::mix(timeChange*0.1f, prevTimeBound, nextTimeBound);
        currentTime = tr_cos(realTime/currentTimeBound);
        if (timeChange>10.f)
        {
            timeChange=10.f;
            resetTime();
            m_timeChange = high_clock::now();
        }
        if (realTime>currentTimeBound)
        {
            currentTimeBound = glm::mix(prevTimeBound, nextTimeBound, timeChange*0.1f);
            currentTime = 1.f;
            
            glm::ivec4 vSize(0.f,0.f,m_screens.drawer.texture->getSize());
            m_screens.blitter.fbo->bindTo(gl::Framebuffer::TargetFrame);
            drawScene(true);
            m_screens.blitter.fbo->bindTo(gl::Framebuffer::TargetRead);
            m_screens.drawer.fbo->bindTo(gl::Framebuffer::TargetDraw);
            m_screens.drawer.fbo->blit(*m_screens.blitter.fbo, vSize, vSize, GL_COLOR_BUFFER_BIT, gl::Sampler::Nearest);
            glBindFramebuffer(gl::Framebuffer::TargetDraw,0);
            glBindFramebuffer(gl::Framebuffer::TargetRead,0);
            gl::Framebuffer::BindScreen();
            resetLine();
            m_time = high_clock::now();
            currentTime=0.f;
        }

        
        if (m_input.getKeyPressed(SDL_SCANCODE_ESCAPE) || m_input.getWindowData().closed)
            quit=true;
        else if (!m_windowed && (m_input.getMouseMotion()!=glm::vec2(0.f)||m_input.getAnyKeyState().pressed))
            quit=true;
#if _WIN32
        if (id && !IsWindow(id))
            quit = true;
#endif
        // m_fboBlit->bindTo(gl::Framebuffer::TargetFrame);
        drawScene(true);
        // gl::Framebuffer::BindToScreen(gl::Framebuffer::TargetFrame);
        m_input.getWindowData().swapBuffers();
    }
}
void MainGame::Screen::FBO::init(gl::Sampler::sptr sampler, glm::ivec2 size)
{
    texture = std::make_unique<gl::Texture>();
    texture->setTarget(GL_TEXTURE_2D);
    texture->setFormat(GL_RGB);
    texture->load(GL_RGBA,GL_UNSIGNED_BYTE, nullptr, size);
    texture->setSampler(sampler);

    fbo = std::make_unique<gl::Framebuffer>();
    fbo->attachTexture(gl::Framebuffer::AttachColor0, *texture, 0);
}