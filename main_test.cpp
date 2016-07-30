#include <cmath>
#include <cfloat>
#include <csignal>
#include <iostream>
#include <iomanip>

// Module includes
#include <ModuleDefines.hpp>
#include <ModuleGame.hpp>
#include <OpenALInterface.hpp>
#include <SDLOpenGLInterface.hpp>
#include <PthreadsInterface.hpp>
#include "Vector3.hpp"
#include "Quaternion.hpp"
#include "Sound.hpp"
#include "SoundClip.hpp"
#include "InputCallback.hpp"
#include "GraphicsCallback.hpp"
#include "GraphicsContext.hpp"
#include "NetworkIO.cpp"
#define MATH_PI 3.14159265358979323846264
// The Chopin comes from https://archive.org/details/onclassical-quality-wav-audio-files-of-classical-music

bool net_as_input = false;

class TestInputCallback : public Module::InputCallback
{
private:
	bool leftPressed;
	bool rightPressed;
public:
	int rotX;
	int rotY;
	int posX;
	int posY;
	
	TestInputCallback() : Module::InputCallback(),
		leftPressed(false), rightPressed(false), rotX(0), rotY(0), posX(0), posY(0) {}
	
	void onMouseDown(uint8_t button, uint16_t x, uint16_t y)
	{
		switch (button)
		{
			case 0:
				leftPressed = true;
				break;
			case 1:
				rightPressed = true;
				break;
		}
	}
	void onMouseUp(uint8_t button, uint16_t x, uint16_t y)
	{
		switch (button)
		{
			case 0:
				leftPressed = false;
				break;
			case 1:
				rightPressed = false;
				break;
		}
	}
	void onMouseMove(uint16_t x, uint16_t y, int16_t dx, int16_t dy)
	{
		if (leftPressed)
		{
			rotX += dx;
			rotX %= 360;
			rotY += dy;
			rotY %= 360;
		}
		if (rightPressed)
		{
			posX += dx;
			posY += dy;
		}
	}
	void onKeyDown(Module::KeyCode::Code code, char keyChar)
	{
		std::cout << "Key press: code = " << code << " char = " << keyChar << std::endl;
	}
};

bool halted = false;

void sigterm_handler(int signal)
{
	if (signal == SIGINT)
	{
		halted = true;
		std::cout << "Terminated!" << std::endl;
	}
}

string serializeDataSheet(vector<float> datasheet){
	string result;
	for (int i = 0; i < datasheet.size(); i++){
		std::stringstream ss;
		ss << std::fixed << std::setprecision(10) << datasheet[i];
		result += ss.str();
		if (i != datasheet.size() - 1){
			result += ",";
		}
	}
	return result;
}

vector<float> deserializeDataSheet(string datasheet){
	istringstream ss(datasheet);
	std::string token;
	vector<float> result;
	while (std::getline(ss, token, ',')){
		result.push_back(atof(token.c_str()));
	}
	return result;
}

int main(int argc, char ** argv)
{
	if (argc != 2){
		cout << "required: ./main_test <net_as_input? t : f>" << endl;
		return -1;
	}else{
		if (strcmp( argv[1], "t") == 0 ){
			net_as_input = true;
		}else if (strcmp( argv[1], "f") == 0){
			net_as_input = false;
		}else{
			cout << argv[1] << std::endl;
			return -1;
		}
	}
	// initialize networking
	InputQueue* queue = new InputQueue();
	int rc;
	pthread_t t;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	std::cout << "Starting threads" << std::endl;
	if (net_as_input){
		rc = pthread_create(&t, NULL, start_inchan, (void *) queue);
	}else{
		rc = pthread_create(&t, NULL, start_outchan, (void *) queue);
	}
	if (rc){
		std::cout << "Error: Unable to create thread" << std::endl;
		exit(-1);
	}
	
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
	
	// Add input callback object to record mouse events
	TestInputCallback input;
	graphics.addInputCallback(&input);
	
	// Starts the game
	game.start();
		
	// GRAPHICS TESTS
	unsigned int numObjects = 10;
		
	// Load cube mesh and texture from files
	Module::Mesh* cubeMesh = graphics.loadMeshFromFile("cube", "models/cube.obj", false);	
	Module::Texture* cubeTexture = graphics.loadTexture("textures/test_tex.png");
	
	// Scale mesh
	cubeMesh->setScale(1/1.0f);
	
	// Create and initialize vector of objects with the cube mesh
	eastl::vector<Module::GameObject*> objects;
		
	for (unsigned int i = 0; i < numObjects; i++)
	{
		float theta = 2 * MATH_PI * i / numObjects;
		Module::GameObject* gameobj = game.createGameObject();
		game.setMesh(gameobj, cubeMesh);
		game.setTexture(gameobj, cubeTexture);
		gameobj->setPosition(Module::Vector3(6 * std::sin(theta), 0.0f, 6 * std::cos(theta)));
		objects.push_back(gameobj);
	}
	
	// Create a game object to be controlled by the player
	Module::GameObject* playerObject = game.createGameObject();
	game.setMesh(playerObject, cubeMesh);
	game.setTexture(playerObject, cubeTexture);
	playerObject->setPosition(Module::Vector3(0,0,0));
	
	// Create the camera object
	Module::GameObject* camera = game.createGameObject();
	
	// Set camera position
	camera->setPosition(Module::Vector3(0,15.0f, 0.0f));
	camera->setRotation(Module::Quaternion(Module::Vector3(1,0,0),	-MATH_PI/2.0f));
	
	graphics.setCamera(camera);
	
	
	// AUDIO TESTS
	Module::SoundClip* boilClip = game.loadSoundClip("boilingWater","sounds/boiling.wav");
	Module::SoundClip* musicClip = game.loadSoundClip("chopinScherzo","sounds/chopin_scherzo.wav");
	Module::GameObject* thing = game.createGameObject();
	thing->setPosition(Module::Vector3(0,0,0));
	thing->playSound(boilClip);
	game.playSound(musicClip);
	
	signal(SIGINT, sigterm_handler);
	unsigned long millisStart = game.getMilliseconds();
	uint16_t counter = 0;
	while (game.isRunning() and !halted)
	{
		unsigned long millis = game.getMilliseconds() - millisStart;
		for (unsigned int i = 0; i < objects.size(); i++)
		{
			thing->setPosition(Module::Vector3(2*std::sin(0.25f * MATH_PI * millis / 1000.0f),0,1));
			objects[i]->setRotation(Module::Quaternion(Module::Vector3(0,1,0), std::sin(2 * MATH_PI * millis / 1000.0f) + (2 * MATH_PI * i / numObjects)));
		}
		
		counter += 1;
		counter = counter % 1000;
		// server simple example, server send the message, no usage of pthread
		// serialize current rotation and current position
		if (net_as_input){
			if (counter == 0){
				// receive internet 
				string input_string = queue->pop();
				if (input_string.empty()){
					continue;
				}
				std::cout << "received: " << input_string << endl;
				vector<float> v = deserializeDataSheet(input_string);
				Module::Quaternion new_rot(v[0], v[1], v[2], v[3]);
				Module::Vector3 new_pos(v[4],v[5],v[6]);
				playerObject->setPosition(new_pos);
				playerObject->setRotation(new_rot);
			}
		}else{
			// receive keyboard/mouse input to control this
			Module::Quaternion cubeRot = playerObject->getRotation();
			cubeRot = Module::Quaternion(Module::Vector3(1,0,0), MATH_PI * input.rotY / 180.0f) * cubeRot;
			cubeRot = Module::Quaternion(Module::Vector3(0,0,1), MATH_PI * -input.rotX / 180.0f) * cubeRot;
			playerObject->setRotation(cubeRot);			
			Module::Vector3 cubeForward = Module::Vector3(1,0,0).rotate(cubeRot);
			Module::Vector3 cubeUp = Module::Vector3(0,1,0).rotate(cubeRot);
			Module::Vector3 cubeRight = Module::Vector3(0,0,1).rotate(cubeRot);
			input.rotX = 0;
			input.rotY = 0;
			Module::Vector3 cubPos = Module::Vector3(input.posX / 30.0f, 0.0f, input.posY / 30.0f) +
				cubeForward * -0.5f + cubeUp * -0.5f + cubeRight * -0.5f;
			playerObject->setPosition(cubPos);
			if (counter == 0){
				// send the datasheet every 1000 frame
				float datasheet[7];
				datasheet[0] = cubeRot.getX();
				datasheet[1] = cubeRot.getY();
				datasheet[2] = cubeRot.getZ();
				datasheet[3] = cubeRot.getW();
				datasheet[4] = cubPos.getX();
				datasheet[5] = cubPos.getY();
				datasheet[6] = cubPos.getZ();
				std::vector<float> v(datasheet, datasheet + 7);
				string s = serializeDataSheet(v);
				queue->push(s);
				cout << "sent to queue: " << s << endl;

			}
		}
	}
	pthread_cancel(t);
	delete queue;
	return 0;
}
