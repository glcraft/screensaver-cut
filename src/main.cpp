#include <filesystem>
#include <regex_literals.h>
#include "MainGame.hpp"
#ifdef _WIN32
#include <winuser.h>
#endif
int main(int argv, char** argc)
{
    #ifdef _WIN32
    SetProcessDPIAware();
    #endif
    namespace fs=std::filesystem;
    int request=0;
    if (argv>=2)
    {
        std::string jeton(argc[1]);
        if ("/\\/s/i"_rg.match(jeton))
            request=0;
        else if ("/\\/p/i"_rg.match(jeton))
            request=1;
        else if ("/\\/test/i"_rg.match(jeton))
            request=2;
    }
    SDL_Init(SDL_INIT_VIDEO);
    SDL_DisableScreenSaver();
#ifdef DEBUG
    fs::path pathRoot(fs::current_path());
#else
    fs::path pathRoot(fs::path(argc[0]).parent_path());
#endif
    MainGame m_main;
    m_main.setPathRoot(pathRoot);
    switch(request)
    {
    case 0:
        m_main.init();
        break;
    case 2:
        m_main.setWindowed(true);
        m_main.init();
        break;
    case 1:
        m_main.init(reinterpret_cast<const void*>(atoll(argc[2])));
        break;
    }
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
    m_main.display();
    SDL_EnableScreenSaver();
    SDL_Quit();
    
    
    return 0;
}