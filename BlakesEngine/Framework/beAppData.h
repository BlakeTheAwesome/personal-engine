#pragma once

class beEnvironment;
class beSystemEventManager;
class beRenderInterface;
class RenderDocManager;
class beKeyboard;
class beMouse;
class beGamepad;
class beDebugWorld;
class beWindow;
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

