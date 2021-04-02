#pragma once

import beKeyboard;
import beGamepad;
import beSystemEventManager;
import beMouse;
import beEnvironment;
import beWindow;

class beRenderInterface;
class RenderDocManager;
class beDebugWorld;
class beShaderPack;

struct beAppData
{
	beEnvironment* environment = nullptr;
	beSystemEventManager* systemEventManager = nullptr;
	beRenderInterface* renderInterface = nullptr;
	RenderDocManager* renderDocManager = nullptr;
	beDebugWorld* debugWorld = nullptr;
	beKeyboard* keyboard = nullptr;
	beMouse* mouse = nullptr;
	beGamepad* gamepad = nullptr;
	beShaderPack* shaderPack = nullptr;
	beWindow* window = nullptr;
};

