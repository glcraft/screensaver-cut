#pragma once
#include <memory>
#include <string>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <map>
#include <array>
#ifdef __WINDOWS__
#include <Windows.h>
#endif

class Input
{
public:
	
	struct WindowData
	{
#ifdef __WINDOWS__
		typedef HGLRC GLContext;
		typedef HDC DisplayContext;
		typedef HWND OSType;

		
#else

#endif

		WindowData(SDL_Window* win = nullptr) : windowPtr(win)
		{}
		struct IO
		{
			glm::ivec2 mousePos;
			glm::ivec2 mouseRelPos;
			struct State
			{
				bool pressed;
				bool released;
				bool pressing;
			};
			std::array<State, SDL_NUM_SCANCODES> keys;
			std::array<State, 6> mouseButtons;
			State anyKey;
			bool typing;
			char utf8[5];
		}io;

		void reset();
		void makeCurrent() const;
		void swapBuffers() const;
		//void pollEvent(SDL_Event evt);
		//trigger resized
		bool resized;
		//trigger closed
		bool closed;
		bool moved;
		bool focus_gained, focus_lost;
		glm::ivec2 position;
		glm::ivec2 size;
		SDL_Window* windowPtr;
#ifdef _WIN32
		GLContext glrc;
		DisplayContext dc;
#endif
		bool attached;
#ifdef EXPERIMENTAL_MULTIWINDOWS
		static SDL_GLContext sdl_glContext;
#else
		SDL_GLContext sdl_glContext;
#endif
	};

	Input();
	Input(const std::string& title, Uint32 flags, glm::vec2 pos = glm::vec2(SDL_WINDOWPOS_CENTERED), glm::ivec2 size = glm::ivec2(-1, -1));
	~Input();

	static void Update();
	//void reset();

	void createWindow(const std::string& title, Uint32 flags = SDL_WINDOW_OPENGL, glm::ivec2 pos = glm::ivec2(SDL_WINDOWPOS_CENTERED), glm::ivec2 size = glm::ivec2(-1, -1));
#if __WINDOWS__
	void attachWindow(HWND window);
	template <typename TMessage>
	void check_WinError(BOOL state, TMessage msgPerso)
	{
		if (!state)
		{
			LPSTR  winMessage = nullptr;
			int err = GetLastError();
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | 0x40, 0, 0, 0, (LPSTR)&winMessage, 0, nullptr);

			std::ostringstream ostr;
			ostr << "SDL (Windows) : " << msgPerso << std::endl;
			ostr << "Windows API: Error " << GetLastError() << ": " << winMessage << std::endl;
			LocalFree(winMessage);
			throw std::runtime_error(ostr.str());
		}
	}
#endif
	void destroyWindow();

	bool getKey(int key) const;
	bool getKeyPressed(int key) const;
	bool getKeyReleased(int key) const;
	WindowData::IO::State getKeyState(int key) const;
	WindowData::IO::State getAnyKeyState() const;

	bool getMouseButton(int mb) const;
	bool getMouseButtonPressed(int mb) const;
	bool getMouseButtonReleased(int mb) const;
	WindowData::IO::State getMouseButtonState(int mb) const;
	glm::vec2 getMousePosition();
	//Get Relative position
	glm::vec2 getMouseMotion();

	const Input::WindowData & getWindowData() const;
	void closeWindow() const;
	static const Input::WindowData & getWindowData(Uint32 windowID);
	void setRelativeMode(bool enabled);

	bool isRelativeMode();

	
	
	/*void addAction(Data&, od::Signal<Data>&&);
	const Data& getAction(std::string)const;*/
private:
	static std::map<Uint32,WindowData> m_windows;
	Uint32 m_windowID;
	
	/*std::map<Data*, od::Signal<Data>> m_actions;*/
};