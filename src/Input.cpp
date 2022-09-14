#include "Input.h"
#include "SDL_error.h"
#include "SDL_video.h"
#include <GL/glew.h>
#include <SDL2/SDL_syswm.h>
#include <sstream>
#include <iostream>
#include <stdexcept>
// #include "UTF8.h"
std::map<Uint32, Input::WindowData> Input::m_windows;

#ifdef EXPERIMENTAL_MULTIWINDOWS
SDL_GLContext Input::WindowData::sdl_glContext=nullptr;
#endif

Input::Input() : m_windowID(0)
{
}

Input::Input(const std::string& title, Uint32 flags, glm::vec2 pos, glm::ivec2 size) : Input()
{
	createWindow(title, flags, pos, size);
}

Input::~Input()
{
}
//test
void Input::createWindow(const std::string& title, Uint32 flags, glm::ivec2 pos, glm::ivec2 size)
{
	// Version d'OpenGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Double Buffer
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	if (size.x < 0)
		size.x = DM.w * 3 / 4;
	if (size.y < 0)
		size.y = DM.h * 3 / 4;
	SDL_Window* wPtr=SDL_CreateWindow(title.c_str(), pos.x, pos.y, size.x, size.y, flags | SDL_WINDOW_OPENGL);
	SDL_GetWindowPosition(wPtr, &pos.x, &pos.y);
	if (wPtr == 0) {

		std::cout << "SDL Error: " << SDL_GetError() << std::endl;
		throw std::runtime_error("SDL : Impossible de créer une fenêtre");
	}
	Uint32 wID = SDL_GetWindowID(wPtr);
	m_windowID = wID;
	auto mypair = m_windows.insert(std::make_pair<Uint32, WindowData>(std::move(wID), std::move(WindowData(wPtr))));
#ifdef EXPERIMENTAL_MULTIWINDOWS
	if (Input::WindowData::sdl_glContext==nullptr)
	{
		SDL_GLContext glcont = SDL_GL_CreateContext(wPtr);
		if (glcont == 0)
		{
			throw std::exception("SDL : Impossible de créer le contexte OpenGL de la fenêtre", 1);
		}
		Input::WindowData::sdl_glContext = std::move(glcont);
	}
	SDL_GL_MakeCurrent(wPtr, Input::WindowData::sdl_glContext);
#else
	SDL_GLContext glcont = SDL_GL_CreateContext(wPtr);
	if (glcont == 0)
	{
		std::cout << "SDL Error: " << SDL_GetError() << std::endl;
		throw std::runtime_error("SDL : Impossible de créer le contexte OpenGL de la fenêtre");
	}
	mypair.first->second.sdl_glContext = std::move(glcont);
#endif
	
	
#ifdef __WINDOWS__
	mypair.first->second.glrc = wglGetCurrentContext();
	mypair.first->second.dc = wglGetCurrentDC();
#endif
	mypair.first->second.reset();
	mypair.first->second.size = size;
	mypair.first->second.position = pos;
	for (auto& key : mypair.first->second.io.keys)
		key.pressing = false;
	for (auto& button : mypair.first->second.io.mouseButtons)
		button.pressing = false;
	mypair.first->second.attached = false;
}
#if __WINDOWS__
void Input::attachWindow(HWND window)
{
	// Version d'OpenGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Double Buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_Window* wPtr = SDL_CreateWindowFrom(window);
	//SDL_GetWindowPosition(wp)
	glm::ivec2 pos, size;
	SDL_GetWindowPosition(wPtr, &pos.x, &pos.y);
	SDL_GetWindowSize(wPtr, &size.x, &size.y);
	if (wPtr == 0)
		throw std::exception("SDL : Impossible de créer une fenêtre", 1);
	Uint32 wID = SDL_GetWindowID(wPtr);
	m_windowID = wID;
	auto mypair = m_windows.insert(std::make_pair<Uint32, WindowData>(std::move(wID), std::move(WindowData(wPtr))));
#ifdef TRUE
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	HDC hdc = GetDC(window);
	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	check_WinError(pixelFormat != 0, "Impossible de choisir le PixelFormat");
	check_WinError(SetPixelFormat(hdc, pixelFormat, &pfd), "Impossible de changer le PixelFormat");

	HGLRC tempContext = wglCreateContext(hdc);
	check_WinError(tempContext != 0, "Erreur lors de la création un context OpenGL 1.0");
	check_WinError(wglMakeCurrent(hdc, tempContext), "Erreur lors de la changement de context (OGL 1.0)");

	glewExperimental = true;
	glewInit();
	if (glewIsSupported("WGL_ARB_create_context") == 1)
	{
		// int attribs[] =
		// {
		// 	GL_CONTEXT_MAJOR_VERSION, 3,
		// 	GL_CONTEXT_MINOR_VERSION_ARB, 3,
		// 	GL_CONTEXT_FLAGS_ARB, 0,
		// 	0
		// };
		SDL_GLContext glcont = SDL_GL_CreateContext(wPtr);
		// mypair.first->second.glrc = glCreateContextAttribs(hdc, 0, attribs);
		check_WinError(mypair.first->second.glrc!=0, "Erreur lors de la création un context OpenGL 3.3");
		check_WinError(wglMakeCurrent(NULL, NULL),"Erreur bizarre 1");
		check_WinError(wglDeleteContext(tempContext), "Erreur lors de la destruction du contexte basique");
		check_WinError(wglMakeCurrent(hdc, mypair.first->second.glrc), "Erreur lors du changement de context (OGL 1.0->3.3)");
	}
	else
	{
		std::cerr << "warning" << "WGL create context (needed for OpenGL 3.3) not supported" << std::endl;
		mypair.first->second.glrc = tempContext;
	}
	mypair.first->second.dc = hdc;
	SDL_GL_MakeCurrent(wPtr, mypair.first->second.glrc);
#else
	std::ostringstream winStr;
	winStr << std::hex << wPtr;
	auto test = SDL_SetHint( SDL_HINT_VIDEO_WINDOW_SHARE_PIXEL_FORMAT, winStr.str().c_str() );
	SDL_GLContext glcont = SDL_GL_CreateContext(wPtr);
	if (glcont == 0)
		throw std::runtime_error("SDL : Impossible de créer le contexte OpenGL de la fenêtre");
	mypair.first->second.sdl_glContext = std::move(glcont);
#endif


#ifdef __WINDOWS__
	mypair.first->second.glrc = wglGetCurrentContext();
	mypair.first->second.dc = wglGetCurrentDC();
#endif
	mypair.first->second.reset();
	mypair.first->second.size = size;
	mypair.first->second.position = pos;
	for (auto& key : mypair.first->second.io.keys)
		key.pressing = false;
	for (auto& button : mypair.first->second.io.mouseButtons)
		button.pressing = false;
	mypair.first->second.attached = true;
}
#endif
void Input::destroyWindow()
{
	SDL_GL_DeleteContext(m_windows[m_windowID].sdl_glContext);
	SDL_DestroyWindow(m_windows[m_windowID].windowPtr);
	m_windows.erase(m_windowID);
}
void Input::Update()
{
	SDL_Event evt;
	for (auto& win : m_windows)
		win.second.reset();
	if (SDL_PollEvent(&evt))
	{
		do
		{
			switch (evt.type)
			{
			case SDL_KEYDOWN:
				m_windows[evt.key.windowID].io.keys[evt.key.keysym.scancode].pressed = true;
				m_windows[evt.key.windowID].io.keys[evt.key.keysym.scancode].pressing = true;
				m_windows[evt.key.windowID].io.anyKey.pressed = true;
				break;
			case SDL_KEYUP:
				m_windows[evt.key.windowID].io.keys[evt.key.keysym.scancode].released = true;
				m_windows[evt.key.windowID].io.keys[evt.key.keysym.scancode].pressing = false;
				m_windows[evt.key.windowID].io.anyKey.released = true;
				break;
			case SDL_TEXTINPUT:
				m_windows[evt.key.windowID].io.typing = true;
				memcpy(m_windows[evt.key.windowID].io.utf8, evt.text.text, sizeof(m_windows[evt.key.windowID].io.utf8));
				break;
			case SDL_MOUSEBUTTONDOWN:
				m_windows[evt.button.windowID].io.mouseButtons[evt.button.button].pressed = true;
				m_windows[evt.button.windowID].io.mouseButtons[evt.button.button].pressing = true;
				break;
			case SDL_MOUSEBUTTONUP:
				m_windows[evt.button.windowID].io.mouseButtons[evt.button.button].released = true;
				m_windows[evt.button.windowID].io.mouseButtons[evt.button.button].pressing = false;
				break;
			case SDL_MOUSEMOTION:
				m_windows[evt.motion.windowID].io.mouseRelPos.x += evt.motion.xrel;
				m_windows[evt.motion.windowID].io.mouseRelPos.y += evt.motion.yrel;
				m_windows[evt.motion.windowID].io.mousePos.x = evt.motion.x;
				m_windows[evt.motion.windowID].io.mousePos.y = evt.motion.y;
				break;
			case SDL_WINDOWEVENT:
				switch (evt.window.event)
				{
				case SDL_WINDOWEVENT_CLOSE:
					m_windows[evt.window.windowID].closed = true;
					break;
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					m_windows[evt.window.windowID].focus_gained = true;
					break;
				case SDL_WINDOWEVENT_FOCUS_LOST:
					m_windows[evt.window.windowID].focus_lost= true;
					break;
				case SDL_WINDOWEVENT_RESIZED:
					m_windows[evt.window.windowID].resized = true;
					m_windows[evt.window.windowID].size = glm::ivec2(evt.window.data1, evt.window.data2);
					break;
				case SDL_WINDOWEVENT_MOVED:
					m_windows[evt.window.windowID].moved = true;
					m_windows[evt.window.windowID].position = glm::ivec2(evt.window.data1, evt.window.data2);
				}
				break;
#if __WINDOWS__
			case SDL_SYSWMEVENT:
				
				if (evt.syswm.msg->msg.win.msg == WM_SYSCOMMAND && evt.syswm.msg->msg.win.wParam == SC_CLOSE || evt.syswm.msg->msg.win.msg == WM_DESTROY)// 
					m_windows[evt.window.windowID].closed = true;
				break;
#endif
			}
		} while (SDL_PollEvent(&evt));
	}
}
#if __WINDOWS__

//void Input::check_WinError(BOOL state, std::string msgPerso)

#endif
void Input::WindowData::reset()
{
	for (int i = 0; i < SDL_NUM_SCANCODES; i++)
	{
		io.keys[i].pressed = false;
		io.keys[i].released= false;
	}
	for (int i = 0; i < 6; i++)
	{
		io.mouseButtons[i].pressed = false;
		io.mouseButtons[i].released = false;
	}
	io.anyKey.pressed = false;
	io.anyKey.released = false;
	io.mouseRelPos = glm::vec2(0.f);
	resized = false;
	moved = false;
	closed = false;
	focus_gained = false;
	focus_lost = false;
	io.typing = false;
}

void Input::WindowData::makeCurrent() const
{
	SDL_GL_MakeCurrent(windowPtr, sdl_glContext);
}

void Input::WindowData::swapBuffers() const 
{
#if __WINDOWS__ 
	if (attached)
		SwapBuffers(dc);
	else
#endif
		SDL_GL_SwapWindow(windowPtr);
}



bool Input::getKey(int key) const
{
	return m_windows[m_windowID].io.keys[key].pressing;
}

bool Input::getKeyPressed(int key) const
{
	return m_windows[m_windowID].io.keys[key].pressed;
}

bool Input::getKeyReleased(int key) const
{
	return m_windows[m_windowID].io.keys[key].released;
}

Input::WindowData::IO::State Input::getKeyState(int key) const
{
	return m_windows[m_windowID].io.keys[key];
}

Input::WindowData::IO::State Input::getAnyKeyState() const
{
	return m_windows[m_windowID].io.anyKey;
}

bool Input::getMouseButton(int mb) const
{
	return m_windows[m_windowID].io.mouseButtons[mb].pressing;
}

bool Input::getMouseButtonPressed(int mb) const
{
	return m_windows[m_windowID].io.mouseButtons[mb].pressed;
}

bool Input::getMouseButtonReleased(int mb) const
{
	return m_windows[m_windowID].io.mouseButtons[mb].released;
}

Input::WindowData::IO::State Input::getMouseButtonState(int mb) const
{
	return m_windows[m_windowID].io.mouseButtons[mb];
}
glm::vec2 Input::getMousePosition()
{
	return m_windows[m_windowID].io.mousePos;
}
glm::vec2 Input::getMouseMotion()
{
	return m_windows[m_windowID].io.mouseRelPos;
}
const Input::WindowData & Input::getWindowData() const
{
	return m_windows[m_windowID];
}

void Input::closeWindow() const
{
	m_windows[m_windowID].closed = true;
}

const Input::WindowData & Input::getWindowData(Uint32 windowID)
{
	return m_windows[windowID];
}

void Input::setRelativeMode(bool enabled)
{
	SDL_SetRelativeMouseMode(static_cast<SDL_bool>(enabled));
}
bool Input::isRelativeMode()
{
	return SDL_GetRelativeMouseMode();
}

//void addAction(std::string, Data&, od::Signal<Data>&&);
//{
//	m_actions.
//}
//
//const Input::Data & Input::getAction(std::string name) const
//{
//	// TODO: insérer une instruction return ici
//}


