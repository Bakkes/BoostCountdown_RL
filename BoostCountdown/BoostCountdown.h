#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "version.h"


#include <thread>
#include <chrono>
#include <vector>

using namespace std;

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);



class BoostCountdown: public BakkesMod::Plugin::BakkesModPlugin, public SettingsWindowBase
{

	const int THRESHOLD = 500;
	const int PAD_SEC = 10;
	const int NEAR_COEF = 50;
	const int FAR_COEF = 6500;
	const float PI = 3.1415;
	int fov = 90;
	Vector2F screenSize;

	void onLoad() override;

	int GetClosestPad(Vector loc);
	void Countdown(int padID);

	void Render(CanvasWrapper);

	bool isOnScreen(Vector camera, Vector carPos, Vector UIPos);
	void resetTimers();

	Vector VecToVector(vector<float> vec);
	float DegToRad(float deg);

	//Locations of big pads:
	vector<vector<float>> padsLoc = { 
		{3072.0,  4096.0, 73.0},
		{-3072.0,  4096.0, 73.0},
		{3584.0, 0.0, 73.0}, 
		{-3584.0, 0.0, 73.0},
		{3072.0, -4096.0, 73.0 }, 
		{-3072.0, -4096.0, 73.0}
	};
	vector<float> time = { 0.0,0.0,0.0,0.0,0.0,0.0 };

	shared_ptr<bool> enable = make_shared<bool>(false);
	shared_ptr<float> scale_UI = make_shared<float>(5.0);

public:
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
};
