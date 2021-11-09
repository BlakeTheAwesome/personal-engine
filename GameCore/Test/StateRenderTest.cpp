module;
#include "BlakesEngine/bePCH.h"

#include "BlakesEngine/Core/beAssert.h"
#include "BlakesEngine/Core/beString.h"
#include "BlakesEngine/Core/bePrintf.h"
#include "BlakesEngine/Core/beMacros.h"
#include "BlakesEngine/External/Misc/StreamToDebugOutput.h"
#include "BlakesEngine/External/RenderDoc-Manager/RenderDocManager.h"

#include <shellapi.h>
#include <iomanip>
#include <d3d11.h>

module StateRenderTest;

#ifdef DEBUG
#define ENABLE_RENDERDOC
#define RENDERDOC_PATH L"d:/Dev/Renderdoc/renderDoc.dll"
#define RENDERDOC_CAPTURE_PATH "d:/temp/renderDoc/capture"
#endif


import beShaderPack;
import beRandom;
import bePackedData;
import RangeIter;
import beStateMachine;
import bePerlinNoise;
import beCameraUtils;
import beGamepad;
import beKeyboard;
import beMouse;
import beWindow;
import beRenderInterface;
import beContainerHelpers;
import beAppData;

void StateRenderTest::Enter(beStateMachine* stateMachine)
{
	auto renderInterface = m_appData->renderInterface;
	auto shaderPack = m_appData->shaderPack;

	shaderPack->UpdateFrameBuffers(renderInterface, m_camera.GetViewMatrix()); // Set ortho matrix buffer
	m_font.Init(renderInterface, shaderPack, "tutefont.txt", "tutefont.dds");
	m_models.AddNew()->Init(renderInterface, shaderPack, "boar.dds");
	//m_models.AddNew()->InitWithFilename(renderInterface, shaderPack, "zombie/obj/zombie.obj", "zombie/obj/zombie.mtl", beModel::LoadOptions::Swizzle(beModel::LoadOptions::swizZ, beModel::LoadOptions::swizX, beModel::LoadOptions::swizY));
	m_models.AddNew()->InitWithFilename(renderInterface, shaderPack, "zombie/obj/zombieSmooth.obj", "zombie/obj/zombieSmooth.mtl", beModel::LoadOptions::Swizzle(beModel::LoadOptions::swizZ, beModel::LoadOptions::swizX, beModel::LoadOptions::swizY));
	m_models.AddNew()->InitWithFilename(renderInterface, shaderPack, "cube.obj", "seafloor.dds", {});
	m_models.AddNew()->InitWithFilename(renderInterface, shaderPack, "cube2.obj",  "seafloor.dds", {});
	m_models.AddNew()->InitWithFilename(renderInterface, shaderPack, "teapot.obj", "seafloor.dds", {beRendering::Topology::TriangleList, true, 1.f,{0,2,1}});
	m_models.AddNew()->InitWithFilename(renderInterface, shaderPack, "boxes.obj", "barrels.dds", {beRendering::Topology::TriangleList, true, 0.1f,{0,2,1}});

	InitGrid(renderInterface);


	bePackedData packedModel;
	packedModel.LoadFromFile(R"===(C:\Dev\BigAnt\data\blobs\characters\player\models\player_master_head_male\player_master_head_male.modelcomponent.pc)===");
	m_models.AddNew()->InitFromPackedData(renderInterface, shaderPack, packedModel);

	beTexture::LoadOptions textureLoadOptions;
	textureLoadOptions.height = 256;
	textureLoadOptions.width = 256;
	textureLoadOptions.format = beTextureFormat::R32G32B32_FLOAT;
	m_screenGrabTexture.InitAsTarget(renderInterface, textureLoadOptions);

	m_bitmapTexQuadNorm.Init(renderInterface, shaderPack, 0.2f, 0.2f, "boar.dds");
	m_bitmapTexQuadNorm.SetPosition(0.1f, 0.1f);
	m_bitmapTexQuadPixelCoord.Init(renderInterface, shaderPack, 128, 128, "bark.dds");
	m_bitmapTexQuadPixelCoord.SetPosition(-0.8f, -0.5f);

	m_bitmapTextDynamic.SetColour({0.f, 1.f, 0.8f, 1.f});
	
	m_camera.AttachGamepad(m_appData->gamepad);
	m_camera.AttachMouse(m_appData->mouse);
	
}

void StateRenderTest::Exit(beStateMachine* stateMachine)
{
	m_camera.DetachGamepad();

	m_bitmapScreenGrab.Deinit();
	m_bitmapTextPreRendered.Deinit();
	m_bitmapTextDynamic.Deinit();
	m_bitmapTexQuadNorm.Deinit();
	m_bitmapTexQuadPixelCoord.Deinit();

	m_screenGrabTexture.FinaliseTarget();
	m_screenGrabTexture.Deinit();

	m_gridModel.Deinit();
	for (beModel& model : m_models)
	{
		model.Deinit();
	}
	m_models.Release();
	m_font.Deinit();
}

void StateRenderTest::Update(beStateMachine* stateMachine, float dt)
{
	auto gamepad = m_appData->gamepad;
	auto keyboard = m_appData->keyboard;
	auto mouse = m_appData->mouse;
	auto renderInterface = m_appData->renderInterface;

	if (gamepad->GetPressed(beGamepad::Button::A) || keyboard->IsPressed(beKeyboard::Button::W))
	{
		renderInterface->ToggleWireframe();
	}
	if (keyboard->IsPressed(beKeyboard::Button::G))
	{
		m_renderGrid = !m_renderGrid;
	}
	if (keyboard->IsPressed(beKeyboard::Button::F))
	{
		m_showGroundFilled = !m_showGroundFilled;
		m_gridModel.SetMeshVisibility("GroundFilled", m_showGroundFilled);
	}

	if (gamepad->GetPressed(beGamepad::Button::B) || keyboard->IsPressed(beKeyboard::Button::Escape))
	{
		stateMachine->ChangeState(nullptr);
		return;
	}
	if (gamepad->GetPressed(beGamepad::Button::Y))
	{
		const int shader = ((int)m_shaderToUse + 1) % (int)beRendering::ShaderType::Count;
		m_shaderToUse = (beRendering::ShaderType)shader;
	}
	if (gamepad->GetPressed(beGamepad::Button::X) || mouse->IsPressed(beMouse::Button::MiddleButton))
	{
		m_modelToUse++;
		m_modelToUse %= m_models.Count() + 1;
	}
	if (gamepad->GetPressed(beGamepad::Button::Select))
	{
		m_renderAxes = !m_renderAxes;
	}
	if (gamepad->GetPressed(beGamepad::Button::Up))
	{
		m_bitmapTexQuadNorm.SetPosition(m_bitmapTexQuadNorm.GetPosition() + Vec2(0.f, 0.1f));
		m_bitmapTexQuadPixelCoord.SetPosition(m_bitmapTexQuadPixelCoord.GetPosition() + Vec2(0.f, 20.f));
	}
	if (gamepad->GetPressed(beGamepad::Button::Down))
	{
		m_bitmapTexQuadNorm.SetPosition(m_bitmapTexQuadNorm.GetPosition() - Vec2(0.f, 0.1f));
		m_bitmapTexQuadPixelCoord.SetPosition(m_bitmapTexQuadPixelCoord.GetPosition() - Vec2(0.f, 20.f));
	}
	if (gamepad->GetPressed(beGamepad::Button::Left))
	{
		m_bitmapTexQuadNorm.SetPosition(m_bitmapTexQuadNorm.GetPosition() - Vec2(0.1f, 0.f));
		m_bitmapTexQuadPixelCoord.SetPosition(m_bitmapTexQuadPixelCoord.GetPosition() - Vec2(20.f, 0.f));
	}
	if (gamepad->GetPressed(beGamepad::Button::Right))
	{
		m_bitmapTexQuadNorm.SetPosition(m_bitmapTexQuadNorm.GetPosition() + Vec2(0.1f, 0.f));
		m_bitmapTexQuadPixelCoord.SetPosition(m_bitmapTexQuadPixelCoord.GetPosition() + Vec2(20.f, 0.f));
	}

	if (mouse->IsPressed(beMouse::Button::LeftButton))
	{
		//auto window = m_appData->window;
		const float screenX = (float)mouse->GetX();// -window->GetX();
		const float screenY = (float)mouse->GetY();// - window->GetY();
		Vec3 worldPos, worldDir;
		const bool isInBounds = beCameraUtils::GetScreeenToWorldRay(*renderInterface, m_camera.GetViewMatrix(), screenX, screenY, &worldPos, &worldDir);
		if (isInBounds)
		{
			//LOG("MOUSE CLICK: (%.2f, %.2f) POS:{%3.3f, %3.3f, %3.3f} dir:{%3.3f, %3.3f, %3.3f}\r\n", screenX, screenY, worldPos.x, worldPos.y, worldPos.z, worldDir.x, worldDir.y, worldDir.z);
			//LOG("MOUSE Deets:");
			PositionFromMatrix(m_camera.GetViewMatrix());
		}

		//if (auto realWorldPos = beCameraUtils::WorldPosFromScreenPos(*renderInterface, m_camera.GetViewMatrix(), m_camera.GetPosition(), screenX, screenY))
		//{

		//	LOG("Collision! mouse pos:{%.3f, %.3f, %.3f} World collision at:{%3.3f, %3.3f, %3.3f}\r\n", m_camera.GetPosition().x, m_camera.GetPosition().y, m_camera.GetPosition().z, realWorldPos->x, realWorldPos->y, realWorldPos->z);
		//}
	}

	m_camera.Update(dt);

}

void StateRenderTest::Render()
{
	auto renderInterface = m_appData->renderInterface;
	auto keyboard = m_appData->keyboard;
	auto mouse = m_appData->mouse;
	auto shaderPack = m_appData->shaderPack;
	auto debugWorld = m_appData->debugWorld;

	shaderPack->UpdateFrameBuffers(renderInterface, m_camera.GetViewMatrix());

	//if (!m_haveWrittenToTexture)
	//{
	//	beTexture writeTexture;
	//	beTexture::LoadOptions textureLoadOptions;
	//	textureLoadOptions.height = 512;
	//	textureLoadOptions.width = 512;
	//	textureLoadOptions.format = beTextureFormat::R32G32B32_FLOAT;
	//	writeTexture.InitAsTarget(renderInterface, textureLoadOptions);
	//	defer(
	//		writeTexture.Deinit();
	//	);
	//	m_haveWrittenToTexture = true;
	//	writeTexture.SetAsTarget(renderInterface);
	//	writeTexture.Clear(renderInterface, V40());
	//	m_models[3].Render(renderInterface, shaderPack, beRendering::ShaderType::Colour);
	//	
	//	renderInterface->DisableZBuffer();
	//	shaderPack->shaderTexture2d.SetShaderParameters(renderInterface, m_camera.GetViewMatrix());
	//		
	//	//m_bitmapTexQuad.Render(renderInterface);
	//	//textureShader2d.Render(renderInterface, m_bitmapTexQuad.GetIndexCount(), m_bitmapTexQuad.GetTexture());
	//	
	//	renderInterface->EnableAlpha();
	//	m_bitmapTextDynamic.InitText(renderInterface, &m_font, "initial string", 1.f, 640.f, 0, false, beFont::WrapMode::Default);
	//	m_bitmapTextDynamic.SetPosition((float)(-writeTexture.GetWidth() / 2), (float)(writeTexture.GetHeight() / 2));
	//	m_bitmapTextDynamic.Render(renderInterface);
	//	shaderPack->shaderTexture2d.Render(renderInterface, m_bitmapTextDynamic.GetIndexCount(), m_bitmapTextDynamic.GetTexture(), true);
	//	renderInterface->DisableAlpha();
	//	renderInterface->EnableZBuffer();
	//	renderInterface->RestoreRenderTarget();
	//	writeTexture.FinaliseTarget();
	//			
	//	m_bitmapTextPreRendered.Init(renderInterface, writeTexture);
	//	m_bitmapTextPreRendered.SetPosition(-400, -400);
	//}
	
	auto renderFrame = [&](bool writingToScreenGrabTexture)
	{
		beModel* modelToRender = m_modelToUse < m_models.Count() ? &m_models[m_modelToUse] : nullptr;
			
		if (modelToRender)
		{
			modelToRender->Render(renderInterface, shaderPack, m_shaderToUse);
		}

		if (m_renderGrid)
		{
			m_gridModel.Render(renderInterface, shaderPack);
		}

		debugWorld->SetRenderAxes(m_renderAxes);
		debugWorld->Render(renderInterface, &shaderPack->shaderColour, m_camera.GetViewMatrix(), m_camera.GetPosition());

		renderInterface->DisableZBuffer();
		shaderPack->shaderTexture2d.SetShaderParameters(renderInterface, m_camera.GetViewMatrix());
			
		if (m_bitmapScreenGrab.GetTexture() && !writingToScreenGrabTexture)
		{
			m_bitmapScreenGrab.Render(renderInterface);
			shaderPack->shaderTexture2d.Render(renderInterface, m_bitmapScreenGrab.GetIndexCount(), m_bitmapScreenGrab.GetTexture(), true);
		}
			
		m_bitmapTexQuadNorm.Render(renderInterface);
		shaderPack->shaderTexture2d.Render(renderInterface, m_bitmapTexQuadNorm.GetIndexCount(), m_bitmapTexQuadNorm.GetTexture(), true);
	
		m_bitmapTexQuadPixelCoord.Render(renderInterface);
		shaderPack->shaderTexture2d.Render(renderInterface, m_bitmapTexQuadPixelCoord.GetIndexCount(), m_bitmapTexQuadPixelCoord.GetTexture(), false);

		renderInterface->EnableAlpha();

		{
			const Vec3 cameraPos = m_camera.GetPosition();
			const Matrix viewMatrix = m_camera.GetViewMatrix();

			const float screenX = (float)mouse->GetX();
			const float screenY = (float)mouse->GetY();
			Vec3 worldPos, worldDir;
			const bool isInBounds = beCameraUtils::GetScreeenToWorldRay(*renderInterface, m_camera.GetViewMatrix(), screenX, screenY, &worldPos, &worldDir);
			auto maybeRealWorldPos = beCameraUtils::WorldPosFromScreenPos(*renderInterface, m_camera.GetViewMatrix(), (int)screenX, (int)screenY);

			if (!isInBounds)
			{
				worldPos = V3N1();
				worldDir = V3N1();
			}
			if (!maybeRealWorldPos)
			{
				maybeRealWorldPos = V3N1();
			}


			fmt::memory_buffer message;
			fmt::format_to(std::back_inserter(message), "Dynamic Text\n");
			fmt::format_to(std::back_inserter(message), "Mouse [X, Y]: [{}, {}]\n", mouse->GetX(), mouse->GetY());
			fmt::format_to(std::back_inserter(message), "Camera [X, Y, Z]: [{:.2f}, {:.2f}, {:.2f}]\n", cameraPos.x, cameraPos.y, cameraPos.z);
			fmt::format_to(std::back_inserter(message), "Mat Right [X, Y, Z, W]: [{:.2f}, {:.2f}, {:.2f}, {:.2f}]\n", viewMatrix.m[0][0], viewMatrix.m[0][1], viewMatrix.m[0][2], viewMatrix.m[0][3]);
			fmt::format_to(std::back_inserter(message), "Mat Up    [X, Y, Z, W]: [{:.2f}, {:.2f}, {:.2f}, {:.2f}]\n", viewMatrix.m[1][0], viewMatrix.m[1][1], viewMatrix.m[1][2], viewMatrix.m[1][3]);
			fmt::format_to(std::back_inserter(message), "-Mat Forw [X, Y, Z, W]: [{:.2f}, {:.2f}, {:.2f}, {:.2f}]\n", viewMatrix.m[2][0], viewMatrix.m[2][1], viewMatrix.m[2][2], viewMatrix.m[2][3]);
			fmt::format_to(std::back_inserter(message), "_Mat Pos  [X, Y, Z, W]: [{:.2f}, {:.2f}, {:.2f}, {:.2f}]\n", viewMatrix.m[3][0], viewMatrix.m[3][1], viewMatrix.m[3][2], viewMatrix.m[3][3]);
			;
			if (isInBounds)
			{
				fmt::format_to(std::back_inserter(message), "ClickPos [X, Y, Z]: [{:.2f}, {:.2f}, {:.2f}]\n", worldPos.x, worldPos.y, worldPos.z);
				fmt::format_to(std::back_inserter(message), "ClickDir [X, Y, Z]: [{:.2f}, {:.2f}, {:.2f}]\n", worldDir.x, worldDir.y, worldDir.z);
			}
			if (maybeRealWorldPos)
			{
				fmt::format_to(std::back_inserter(message), "Collision [X, Y, Z]: [{:.2f}, {:.2f}, {:.2f}]\n", maybeRealWorldPos->x, maybeRealWorldPos->y, maybeRealWorldPos->z);
			}

			beString messageStr = fmt::to_string(message);
			m_bitmapTextDynamic.InitText(renderInterface, &m_font, messageStr.c_str(), 1.5f, 512.f, 0, false, beFont::WrapMode::Default);
			m_bitmapTextDynamic.Render(renderInterface);
			shaderPack->shaderTexture2d.Render(renderInterface, m_bitmapTextDynamic.GetIndexCount(), m_bitmapTextDynamic.GetTexture(), false, beShaderTexture2d::TextureMode::Clamped);
		}
			
		renderInterface->DisableAlpha();
			
		renderInterface->EnableZBuffer();
	};
	
	const bool doScreenGrab = keyboard->IsPressed(beKeyboard::Button::Space);
	if (doScreenGrab)
	{
		m_screenGrabTexture.SetAsTarget(renderInterface);
		m_screenGrabTexture.Clear(renderInterface, V40());
		renderFrame(true);
		renderInterface->RestoreRenderTarget();
		m_bitmapScreenGrab.Init(renderInterface, m_screenGrabTexture);
		renderInterface->ClearDepth();
	}
	renderFrame(false);
}

void StateRenderTest::InitGrid(beRenderInterface* renderInterface)
{
	const int gridRadius = 100;
	const float gridSize = 1.f;
	const float xyOffset = gridSize / 2.f; // Don't draw on same spot as renderAxes
	const float gridOffset = ((float)gridRadius / 2.f);

	const int quadCount = gridRadius * gridRadius;
	const int vertexCount = quadCount * 8;
	const int triCount = quadCount * 2;
	const int triIndexCount = triCount * 3;

	const float noiseScale = 5.f;
	const float noiseHeight = 8.f;
	beRandom rng;
	rng.InitFromSystemTime();
	bePerlinNoise2D noise;
	noise.Initialise(rng.Next());

	beVector<beShaderColour::VertexType> vertices(vertexCount, vertexCount, 0);
	beVector<u32> lineIndices(vertexCount, vertexCount, 0);
	beVector<u32> triIndices(triIndexCount, triIndexCount, 0);

	for (int i : RangeIter(lineIndices.Count()))
	{
		lineIndices[i] = i;
	}

	for (int i : RangeIter(quadCount))
	{
		const int triListIndex = i * 6;
		const int lineListIndex = i * 8;
		triIndices[triListIndex+0] = lineListIndex+0;
		triIndices[triListIndex+1] = lineListIndex+2;
		triIndices[triListIndex+2] = lineListIndex+4;

		triIndices[triListIndex+3] = lineListIndex+4;
		triIndices[triListIndex+4] = lineListIndex+6;
		triIndices[triListIndex+5] = lineListIndex+0;
	}

	int vertexIndex = 0;
	for (float x = -gridOffset; x < gridOffset; x += gridSize)
	{
		const float xPos0 = x + xyOffset;
		const float xPos1 = xPos0+gridSize;
		for (float y = -gridOffset; y < gridOffset; y += gridSize)
		{
			const float yPos0 = y + xyOffset;
			const float yPos1 = yPos0+gridSize;

			const float zPos0 = noiseHeight * noise.GetOctave(xPos0/noiseScale, yPos0/noiseScale, 4);
			const float zPos1 = noiseHeight * noise.GetOctave(xPos0/noiseScale, yPos1/noiseScale, 4);
			const float zPos2 = noiseHeight * noise.GetOctave(xPos1/noiseScale, yPos1/noiseScale, 4);
			const float zPos3 = noiseHeight * noise.GetOctave(xPos1/noiseScale, yPos0/noiseScale, 4);

			const Vec4 pos0(xPos0, yPos0, zPos0, 1.f);
			const Vec4 pos1(xPos0, yPos1, zPos1, 1.f);
			const Vec4 pos2(xPos1, yPos1, zPos2, 1.f);
			const Vec4 pos3(xPos1, yPos0, zPos3, 1.f);

			//LOG("[{},{}] 0- {}", x, y, pos0);
			//LOG("[{},{}] 1- {}", x, y, pos1);
			//LOG("[{},{}] 2- {}", x, y, pos2);
			//LOG("[{},{}] 3- {}", x, y, pos3);

			vertices[vertexIndex+0].position = pos0;
			vertices[vertexIndex+1].position = pos1;
			vertices[vertexIndex+2].position = pos1;
			vertices[vertexIndex+3].position = pos2;  
			vertices[vertexIndex+4].position = pos2;
			vertices[vertexIndex+5].position = pos3;
			vertices[vertexIndex+6].position = pos3;
			vertices[vertexIndex+7].position = pos0;
			vertices[vertexIndex+0].colour = Vec4(zPos0 / noiseHeight, zPos0 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+1].colour = Vec4(zPos1 / noiseHeight, zPos1 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+2].colour = Vec4(zPos1 / noiseHeight, zPos1 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+3].colour = Vec4(zPos2 / noiseHeight, zPos2 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+4].colour = Vec4(zPos2 / noiseHeight, zPos2 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+5].colour = Vec4(zPos3 / noiseHeight, zPos3 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+6].colour = Vec4(zPos3 / noiseHeight, zPos3 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+7].colour = Vec4(zPos0 / noiseHeight, zPos0 / noiseHeight, 1.f, 1.f);
			vertexIndex += 8; 
		}
	}

	beRenderBuffer vertexBuffer, indexBuffer, linesIndexB;

	vertexBuffer.Allocate(renderInterface, ElementSize(vertices), vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0, vertices.begin());

	beModel::Material materials[1];
	materials[0].m_shader = beRendering::ShaderType::Colour;

	beModel::Mesh meshes[2];
	meshes[0].m_name = beString("GroundFilled");
	meshes[0].m_indexBuffer.Allocate(renderInterface, decltype(triIndices)::element_size, triIndices.Count(), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 0, 0, triIndices.begin());
	meshes[0].m_materialIndex = 0;
	meshes[0].m_enabled = false;

	meshes[1].m_name = beString("GroundLines");
	meshes[1].m_indexBuffer.Allocate(renderInterface, decltype(lineIndices)::element_size, vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, D3D11_PRIMITIVE_TOPOLOGY_LINELIST, 0, 0, lineIndices.begin());
	meshes[1].m_materialIndex = 0;

	const bool success = m_gridModel.InitFromBuffers(&vertexBuffer, meshes, materials);
	if (!success) { BE_ASSERT(false); return; }
}

