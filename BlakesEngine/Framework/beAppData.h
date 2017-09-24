#pragma once

class beEnvironment;
class beSystemEventManager;
class beRenderInterface;
class RenderDocManager;
class beKeyboard;
class beMouse;
class beGamepad;
struct beShaderPack;

struct beAppData
{
	beEnvironment* environment = nullptr;
	beSystemEventManager* systemEventManager = nullptr;
	beRenderInterface* renderInterface = nullptr;
	RenderDocManager* renderDocManager = nullptr;
	beKeyboard* keyboard = nullptr;
	beMouse* mouse = nullptr;
	beGamepad* gamepad = nullptr;
	beShaderPack* shaderPack = nullptr;
};

