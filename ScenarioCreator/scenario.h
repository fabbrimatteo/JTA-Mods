#ifndef SCENARIO_H
#define SCENARIO_H

#include "script.h"
#include <string>
#include "dictionary.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <Windows.h>
#include <gdiplus.h>
#include <iostream>
#include <cmath>
#include <fstream>
#include "keyboard.h"

#pragma comment (lib,"Gdiplus.lib")


class ScenarioCreator
{
public:
	ScenarioCreator();
	void update();
	~ScenarioCreator();

private:
	Player player;
	Ped playerPed;
	std::string line;								// string use the safe the fram data-line
	std::string log;
	Cam camera;										// camera
	Vector3 cam_coords;								// coordinates of the camera
	Vector3 cam_rot;
	bool SHOW_JOINT_RECT;							// bool used to switch the rectangle drawing around the joint

	int windowWidth;
	int windowHeight;
	int imageWidth;
	int imageHeight;
	int secondsBeforeSaveImages;
	int captureFreq;
	int joint_int_codes[22];

	HWND hWnd;
	HDC hWindowDC;
	HDC hCaptureDC;
	HBITMAP hCaptureBitmap;

	float recordingPeriod;
	std::clock_t lastRecordingTime;
	int nsample;
	std::ofstream coords_file;									// file used to save joints coordinates data
	std::ofstream ped_spawn_log;								// file used to save joints coordinates data
	std::ofstream log_file;

	CLSID pngClsid;

	void cameraCoords();										// function used to show the camera coordinates
	void spawn_peds(Vector3 spawnAreaCenter, int numPed);		// function used to spawn pedestrians at the beginning of the scenario
	void listen_for_keystrokes();								// function used for keyboard input
	void main_menu();											// function used to load the menu
	void resetMenuCommands();									// reset menu commands state
	void weather_menu();										// function used to load the weather menu
	void time_menu();											// function used to load the time menu
	void place_menu();											// function used to load the place menu
	void camera_menu();											// function used to load the camera menu
	void peds_menu();											// function used to load the peds menu
	void tasks_sub_menu();										// to change behaviour settings
	void stopControl();											// to fix player coords
	void walking_peds();										// to make peds walking
	void file_menu();											// function used to load or save files
	void cancelLastLog();
	void loadFile();

	void draw_text(char *text, float x, float y, float scale);

	bool keyboard_flag;
};

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

#endif // !SCENARIO_H
