#include "pch.h"
#include "BoostCountdown.h"


BAKKESMOD_PLUGIN(BoostCountdown, "Timer until big boosts respawn", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void BoostCountdown::onLoad()
{
	_globalCvarManager = cvarManager;

	string res = gameWrapper->GetSettings().GetVideoSettings().Resolution;
	size_t x_pos = res.find('x');
	screenSize.X = std::stoi(res.substr(0, x_pos));
	screenSize.Y = std::stoi(res.substr(x_pos + 1));

	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {
		Render(canvas);
		});

	cvarManager->registerCvar("boostcountdown_enable", "0", "Enables the plugin", true, true, 0, true, 1, false).bindTo(enable);
	cvarManager->registerCvar("boostcountdown_ui_color", "#FFFF00FF", "color of background", true, false, 0, false, 0, true);
	cvarManager->registerCvar("boostcountdown_scale", "5", "scale of ui", true, true, 1, true, 10, true).bindTo(scale_UI);

	gameWrapper->HookEventWithCaller<CarWrapper>("Function TAGame.VehiclePickup_TA.OnPickUp", [this](CarWrapper caller, void* params, std::string eventName) {
		if (!*enable) {return;}

		Vector loc = caller.GetLocation();
		int close = GetClosestPad(loc);

		if (close != -1) {
			ServerWrapper server = gameWrapper->GetCurrentGameState();
			if (!server) { return; }
			time.at(close) = server.GetSecondsElapsed();
		}
	});

	//Hooks to reset timers based off game events:
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState", [this](string eventName) {
		resetTimers();
		});
	gameWrapper->HookEvent("Function TAGame.GameEvent_TA.ResetPlayers", [this](string eventName) {
		resetTimers();
		});
	
	
}

void BoostCountdown::Countdown(int padID) {
	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server) { return; }
	float start = server.GetSecondsElapsed();
	float end = start + 10.0;
	int seconds = PAD_SEC;

	while (seconds > 0) {
		time.at(padID) = seconds;

		start = server.GetSecondsElapsed();
		seconds = end - start;
	}
	
}

int BoostCountdown::GetClosestPad(Vector loc) {
	for (int i = 0; i < padsLoc.size(); ++i) {
		float dist = sqrt(pow(padsLoc[i][0] - loc.X, 2) + pow(padsLoc[i][1] - loc.Y, 2) + pow(padsLoc[i][3] - loc.Z, 2));
		if (dist <= THRESHOLD) {
			return i;
		}
	}
	return -1;
}

void BoostCountdown::Render(CanvasWrapper canvas)
{
	if (gameWrapper->IsInOnlineGame())
	{
		return;
	}
	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server) { return; }
	CameraWrapper camera = gameWrapper->GetCamera();

	Vector view = RotatorToVector(camera.GetRotation()).getNormalized();
	Vector camPos = camera.GetLocation();
	fov = DegToRad(camera.GetFOV());

	for (int i = 0; i < padsLoc.size(); ++i) {
		if (time.at(i) != 0) {

			//Tracks the current time in relation to getting boost
			float cur_time = 10.9999 - (server.GetSecondsElapsed() - time.at(i));

			if (cur_time < 1) {
				time.at(i) = 0;
			}
			else {
				//Ensures the UI is only displayed while onscren:
				Vector pos = VecToVector(padsLoc.at(i));
				//Updates the scale to give a sort of perspective :
				float dist = abs(Vector{ camPos - pos }.magnitude());
				float distScale = (dist > 6000) ? 0 : ((6000 - dist) / 6000);
				float scale = *scale_UI * distScale;

				if (scale != 0 && isOnScreen(view, camPos, pos)) {
					string count = to_string((int)cur_time);

					float scaleX = *scale_UI / (*scale_UI - distScale);


					Vector2F pos = canvas.ProjectF(VecToVector(padsLoc.at(i)));
					Vector2F textSize = canvas.GetStringSize(count, scale, scale);

					//adjust to center text :
					pos.X -= textSize.X / 2;
					pos.Y -= textSize.Y / 2;

					CVarWrapper col = cvarManager->getCvar("ui_color");
					canvas.SetColor(col.getColorValue());

					canvas.SetPosition(pos);
					canvas.DrawString(count, scale, scale);
				}

			}

		}
	}
}


bool BoostCountdown::isOnScreen(Vector camera, Vector camPos, Vector UIPos) {
	if (gameWrapper->IsInOnlineGame())
	{
		return;
	}
	Vector vecToUI = Vector{ camPos - UIPos }.getNormalized();
	float viewToUIAng = acos(Vector::dot(camera, vecToUI) / camera.magnitude() * vecToUI.magnitude());
	//LOG(to_string(viewToUIAng));
	if (3.1415 - viewToUIAng <= fov) {
		return true;
	}
	return false;
}

void BoostCountdown::resetTimers() {
	for (int i = 0; i < time.size(); ++i) {
		time.at(i) = 0.0f;
	}
}

Vector BoostCountdown::VecToVector(vector<float> vec) {
	return { vec.at(0),vec.at(1),vec.at(2) };
}

float BoostCountdown::DegToRad(float deg) {
	return deg * 3.1415 / 180;
}
