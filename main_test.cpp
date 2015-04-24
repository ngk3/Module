#include <cmath>
#include <cfloat>
#include <csignal>
#include <iostream>
#include "ModuleGame.hpp"
#include "Sound.hpp"
#include "SoundClip.hpp"
#include <OpenALInterface.hpp>
#include <SDLOpenGLInterface.hpp>
#include <PthreadsInterface.hpp>

#include "Vector3.hpp"
#include "Quaternion.hpp"

#define MATH_PI 3.14159265358979323846264

// The Chopin comes from https://archive.org/details/onclassical-quality-wav-audio-files-of-classical-music

bool halted = false;

void sigterm_handler(int signal)
{
	if (signal == SIGINT)
	{
		halted = true;
		std::cout << "Terminated!" << std::endl;
	}
}

int main(int argc, char ** argv)
{
	// Create the game
	Module::ModuleGame game;
	
	// Create the interfaces
	Module::SDLOpenGLInterface graphics;
	Module::OpenALInterface audio;
	Module::PthreadsInterface threads;
	
	// Attatch the interfaces to the game
	game.attachThreadingInterface(&threads);
	game.attachGraphicsInterface(&graphics);
	game.attachAudioInterface(&audio);
	
	// Starts the game
	game.start();
	
	// Create a game object
	Module::GameObject* gameobj = game.createGameObject();
	
	// GRAPHICS TESTS
	// std::vector<Module::Vector3> vertices(3);
	// std::vector<Module::Vector3> normals(3);
	// vertices.push_back(Module::Vector3(0,1,0));
	// vertices.push_back(Module::Vector3(-1,0,0));
	// vertices.push_back(Module::Vector3(1,0,0));
	// vertices.push_back(Module::Vector3(-1,0,0));
	// vertices.push_back(Module::Vector3(0,1,0));
	// vertices.push_back(Module::Vector3(1,0,0));
	// normals.push_back(Module::Vector3(0,0,1));
	// normals.push_back(Module::Vector3(0,0,1));
	// normals.push_back(Module::Vector3(0,0,1));
	// normals.push_back(Module::Vector3(0,0,-1));
	// normals.push_back(Module::Vector3(0,0,-1));
	// normals.push_back(Module::Vector3(0,0,-1));
	
	// Module::Mesh* myMesh = graphics.createMesh(vertices, normals, "triangle");
	
	
	// Module::Mesh* cube = graphics.loadMeshFromFile("cube", "models/cube.obj");
	// cube->setScale(0.5f);
	
	// for (unsigned int i = 0; i < cube->getNumVertices(); i++)
	// {
		// Module::Vector3 curVec = cube->getVertex(i);
		// std::cout << curVec.getX() << "," << curVec.getY() << "," << curVec.getZ() << std::endl;
	// }
	
	
	Module::Mesh* teapot = graphics.loadMeshFromFile("teapot", "models/teapot.obj", true);
	teapot->setScale(1/60.0f);
	game.setMesh(gameobj, teapot);
	
	
	// AUDIO TESTS
	Module::SoundClip* boilClip = audio.loadSoundClip("boilingWater","sounds/boiling.wav");
	Module::SoundClip* musicClip = audio.loadSoundClip("chopinScherzo","sounds/chopin_scherzo.wav");
	audio.playSound(boilClip);
	audio.playSound(musicClip);
	audio.debugAudio();
	
	std::cout << "Test" << std::endl;
	signal(SIGINT, sigterm_handler);
	
	while (game.isRunning())
	{
		unsigned long millis = game.getMilliseconds();
		gameobj->setRotation(Module::Quaternion(Module::Vector3(0,1,0), millis/1000.0f));
	}
	
	return 0;
}
