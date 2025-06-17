#include "pch.h"
#include "BoostCountdown.h"

void BoostCountdown::RenderSettings() {
	CVarWrapper enabled = cvarManager->getCvar("enable");
	bool enabler = enabled.getBoolValue();
	if (ImGui::Checkbox("Enable Big Boost Countdown", &enabler)) {
		enabled.setValue(enabler);
	}

	CVarWrapper color = cvarManager->getCvar("ui_color");
	LinearColor col = color.getColorValue() / 255;
	if (ImGui::ColorEdit4("Change Color", &col.R, ImGuiColorEditFlags_NoInputs)) {
		color.setValue(col*255.0);
	}

	CVarWrapper scale = cvarManager->getCvar("scale");
	float s = scale.getFloatValue();
	if (ImGui::SliderFloat("Max Text Scale", &s, 1, 10)) {
		scale.setValue(s);
	}
}