module;
#include "BlakesEngine/External/RenderDoc-Manager/RenderDocManager.h"
export module beAppData;


import beKeyboard;
import beGamepad;
import beSystemEventManager;
import beMouse;
import beEnvironment;
import beWindow;
import beShaderPack;
import beRenderInterface;
import beDebugWorld;

export struct beAppData
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

