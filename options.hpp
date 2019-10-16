#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include "valve_sdk/Misc/Color.hpp"

#define CONFIG(type, var, val) type var = val

class Options
{
public:
#pragma region Aimbot
	CONFIG(bool, Enabled, false);

	CONFIG(int, Hitchance, 25);
	CONFIG(int, Damage, 15);
#pragma endregion

#pragma region Visuals
	CONFIG(bool, Health, false);
	CONFIG(bool, Name, false);
	CONFIG(bool, Box, false);
	CONFIG(bool, Radar, false);
	CONFIG(bool, Recoil, false);

	CONFIG(int, Size, 5);
#pragma endregion

#pragma region Misc
	CONFIG(bool, AutoBunnyhop, false);
	CONFIG(bool, RemoveScope, false);
#pragma endregion
};

inline Options Feature;