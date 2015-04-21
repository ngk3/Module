#ifndef __SDLOPENGLINTERFACE_HPP__
#define __SDLOPENGLINTERFACE_HPP__

// C++ standard library includes
#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <fstream>

// Module includes
#include <ModuleGame.hpp>
#include <GraphicsInterface.hpp>
#include <Book.hpp>

// Library includes
#define GLM_FORCE_RADIANS
#define SDL_MAIN_HANDLED
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Module
{
	class SDLOpenGLInterface : public GraphicsInterface
	{
		protected:
			Book<GLuint> vertexBuffers;
			
			SDL_Window *window;
			SDL_GLContext context;
			unsigned int vShader;
			unsigned int fShader;
			//unsigned int gShader;
			unsigned int program;
			bool running;
			bool terminated;
	
			unsigned int initShader(const char * path, unsigned int shaderType);
			std::string readSource(const char * path);
			void setupShaders();
			void terminate();
	
			void createWindow(int width, int height, int fps);
			void createVNBuffers(Mesh* mesh);
			
			// OVERRIDES
			void createWindow() { createWindow(800, 600, 60); }
			void renderFrame();
			unsigned long getMilliseconds() { return SDL_GetTicks(); }
			bool isRunning() { return running; }
			
			
		public:
			SDLOpenGLInterface();
			~SDLOpenGLInterface();
			
	};
}

#endif