
#include "BlakesEngine/bePCH.h"
#include "StateRenderTest.h"

#include "BlakesEngine/Core/beString.h"
#include "BlakesEngine/Core/bePrintf.h"
#include "BlakesEngine/Core/beMacros.h"
#include "BlakesEngine/Core/beTypeTests.h"
#include "BlakesEngine/Math/beRandom.h"
#include "BlakesEngine/Math/bePerlinNoise.h"
#include "BlakesEngine/Input/beGamepad.h"
#include "BlakesEngine/Input/beKeyboard.h"
#include "BlakesEngine/Input/beMouse.h"
#include "BlakesEngine/Window/beWindow.h"
#include "BlakesEngine/Rendering/beRenderInterface.h"
#include "BlakesEngine/Rendering/beDebugWorld.h"
#include "BlakesEngine/Camera/beCameraUtils.h"
#include "BlakesEngine/Shaders/beShaderPack.h"
#include "BlakesEngine/External/RenderDoc-Manager/RenderDocManager.h"
#include "BlakesEngine/Framework/beStateMachine.h"
#include "BlakesEngine/Framework/beAppData.h"

#include "BlakesEngine/External/Misc/StreamToDebugOutput.h"
#include <shellapi.h>
#include <iomanip>

#ifdef DEBUG
#define ENABLE_RENDERDOC
#define RENDERDOC_PATH L"d:/Dev/Renderdoc/renderDoc.dll"
#define RENDERDOC_CAPTURE_PATH "d:/temp/renderDoc/capture"
#endif

void StateRenderTest::Enter(beStateMachine* stateMachine)
{
	auto renderInterface = m_appData->renderInterface;

	m_font.Init(renderInterface, "tutefont.txt", beWString(L"tutefont.dds"));
	m_model1.Init(renderInterface, beWString(L"boar.dds"));
	m_model2.InitWithFilename(renderInterface, "cube.obj", beWString(L"seafloor.dds"), {});
	m_model3.InitWithFilename(renderInterface, "cube2.obj", beWString(L"seafloor.dds"), {});
	m_model4.InitWithFilename(renderInterface, "teapot.obj", beWString(L"seafloor.dds"), {});
	m_model5.InitWithFilename(renderInterface, "boxes.obj", beWString(L"barrels.dds"), {0.001f});

	InitGrid(renderInterface);

	m_screenGrabTexture.InitAsTarget(renderInterface, 256, 256);

	m_bitmapTexQuad.Init(renderInterface, 128, 128, beWString(L"boar.dds"));
	m_bitmapTexQuad.SetPosition(1024/2-128, 768/2-128);
	m_bitmapTextDynamic.SetColour(Vec4(0.f, 1.f, 0.8f, 1.f));
	
	m_camera.AttachGamepad(m_appData->gamepad);
	m_camera.AttachMouse(m_appData->mouse);
	
}

void StateRenderTest::Exit(beStateMachine* stateMachine)
{
	m_camera.DetachGamepad();

	m_bitmapScreenGrab.Deinit();
	m_bitmapTextPreRendered.Deinit();
	m_bitmapTextDynamic.Deinit();
	m_bitmapTexQuad.Deinit();

	m_screenGrabTexture.FinaliseTarget();
	m_screenGrabTexture.Deinit();

	m_gridModelLinesIndexBuffer.Release();
	m_gridModel.Deinit();
	m_model5.Deinit();
	m_model4.Deinit();
	m_model3.Deinit();
	m_model2.Deinit();
	m_model1.Deinit();
	m_font.Deinit();
}

void StateRenderTest::Update(beStateMachine* stateMachine, float dt)
{
	const int numShaders = 3;
	const int numModels = 6;
	
	auto gamepad = m_appData->gamepad;
	auto keyboard = m_appData->keyboard;
	auto mouse = m_appData->mouse;
	auto renderInterface = m_appData->renderInterface;

	if (gamepad->GetPressed(beGamepad::A) || keyboard->IsPressed(beKeyboard::Button::W))
	{
		renderInterface->ToggleWireframe();
	}
	if (keyboard->IsPressed(beKeyboard::Button::G))
	{
		m_renderGrid = !m_renderGrid;
	}
	if (gamepad->GetPressed(beGamepad::B) || keyboard->IsPressed(beKeyboard::Button::Escape))
	{
		stateMachine->ChangeState(nullptr);
		return;
	}
	if (gamepad->GetPressed(beGamepad::Y))
	{
		m_shaderToUse++;
		m_shaderToUse %= numShaders;
	}
	if (gamepad->GetPressed(beGamepad::X) || mouse->IsPressed(beMouse::Button::MiddleButton))
	{
		m_modelToUse++;
		m_modelToUse %= numModels;
	}
	if (gamepad->GetPressed(beGamepad::Select))
	{
		m_renderAxes = !m_renderAxes;
	}
	if (gamepad->GetPressed(beGamepad::Up))
	{
		Vec2 bitmapPosition = m_bitmapTexQuad.GetPosition();
		m_bitmapTexQuad.SetPosition(bitmapPosition.x, bitmapPosition.y + 10.f);
	}
	if (gamepad->GetPressed(beGamepad::Down))
	{
		Vec2 bitmapPosition = m_bitmapTexQuad.GetPosition();
		m_bitmapTexQuad.SetPosition(bitmapPosition.x, bitmapPosition.y - 10.f);
	}
	if (gamepad->GetPressed(beGamepad::Left))
	{
		Vec2 bitmapPosition = m_bitmapTexQuad.GetPosition();
		m_bitmapTexQuad.SetPosition(bitmapPosition.x - 10.f, bitmapPosition.y);
	}
	if (gamepad->GetPressed(beGamepad::Right))
	{
		Vec2 bitmapPosition = m_bitmapTexQuad.GetPosition();
		m_bitmapTexQuad.SetPosition(bitmapPosition.x + 10.f, bitmapPosition.y);
	}

	if (mouse->IsPressed(beMouse::LeftButton))
	{
		//auto window = m_appData->window;
		float screenX = (float)mouse->GetX();// -window->GetX();
		float screenY = (float)mouse->GetY();// - window->GetY();
		Vec3 worldPos, worldDir;
		bool isInBounds = beCameraUtils::GetScreeenToWorldRay(*renderInterface, m_camera.GetViewMatrix(), screenX, screenY, &worldPos, &worldDir);
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

	if (!m_haveWrittenToTexture)
	{
		beTexture writeTexture;
		writeTexture.InitAsTarget(renderInterface, 512, 512);
		defer(
			writeTexture.Deinit();
		);
		m_haveWrittenToTexture = true;
		writeTexture.SetAsTarget(renderInterface);
		writeTexture.Clear(renderInterface, V40());
		m_model4.Render(renderInterface);
		shaderPack->shaderColour.SetShaderParameters(renderInterface, m_camera.GetViewMatrix());
		shaderPack->shaderColour.Render(renderInterface, m_model4.GetIndexCount(), 0);
				
		renderInterface->DisableZBuffer();
		shaderPack->shaderTexture2d.SetShaderParameters(renderInterface, m_camera.GetViewMatrix());
			
		//m_bitmapTexQuad.Render(renderInterface);
		//textureShader2d.Render(renderInterface, m_bitmapTexQuad.GetIndexCount(), m_bitmapTexQuad.GetTexture());
		
		renderInterface->EnableAlpha();
		m_bitmapTextDynamic.InitText(renderInterface, &m_font, "initial string", 1.f, 640.f, 0, false, beFont::WrapMode::Default);
		m_bitmapTextDynamic.SetPosition((float)(-writeTexture.GetWidth() / 2), (float)(writeTexture.GetHeight() / 2));
		m_bitmapTextDynamic.Render(renderInterface);
		shaderPack->shaderTexture2d.Render(renderInterface, m_bitmapTextDynamic.GetIndexCount(), m_bitmapTextDynamic.GetTexture());
		renderInterface->DisableAlpha();
		renderInterface->EnableZBuffer();
		renderInterface->RestoreRenderTarget();
		writeTexture.FinaliseTarget();
				
		m_bitmapTextPreRendered.Init(renderInterface, writeTexture);
		m_bitmapTextPreRendered.SetPosition(-400, -400);
	}
	
	auto renderFrame = [&](bool writingToScreenGrabTexture)
	{
		beModel* modelToRender = nullptr;
		switch (m_modelToUse)
		{
			case 0: modelToRender = nullptr; break;
			case 1: modelToRender = &m_model1; break;
			case 2: modelToRender = &m_model2; break;
			case 3: modelToRender = &m_model3; break;
			case 4: modelToRender = &m_model4; break;
			case 5: modelToRender = &m_model5; break;
		}
			
		if (modelToRender)
		{
			modelToRender->Render(renderInterface);

			switch (m_shaderToUse)
			{
				case 0:
					shaderPack->shaderLitTexture.SetShaderParameters(renderInterface, m_camera.GetViewMatrix());
					shaderPack->shaderLitTexture.Render(renderInterface, modelToRender->GetIndexCount(), modelToRender->GetTexture());
				break;
				case 1:
					shaderPack->shaderTexture.SetShaderParameters(renderInterface, m_camera.GetViewMatrix());
					shaderPack->shaderTexture.Render(renderInterface, modelToRender->GetIndexCount(), modelToRender->GetTexture());
				break;
				case 2:
					shaderPack->shaderColour.SetShaderParameters(renderInterface, m_camera.GetViewMatrix());
					shaderPack->shaderColour.Render(renderInterface, modelToRender->GetIndexCount(), 0);
				break;
			}
		}

		if (m_renderGrid)
		{
			shaderPack->shaderColour.SetShaderParameters(renderInterface, m_camera.GetViewMatrix());
			//m_gridModel.Render(renderInterface);
			//shaderPack->shaderColour.Render(renderInterface, modelToRender->GetIndexCount(), 0);


			ID3D11DeviceContext* deviceContext = renderInterface->GetDeviceContext();
			ID3D11Buffer* vertexBuffers[] = { m_gridModel.GetVertexBuffer().GetBuffer() };
			u32 strides[] = {(u32)m_gridModel.GetVertexBuffer().ElementSize() };
			u32 offsets[] = { 0 };

			deviceContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
			
			//deviceContext->IASetIndexBuffer(m_gridModel.GetIndexBuffer().GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
			//deviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_gridModel.GetIndexBuffer().D3DIndexTopology());
			//BE_ASSERT(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST == (D3D11_PRIMITIVE_TOPOLOGY)m_gridModel.GetIndexBuffer().D3DIndexTopology());
			//shaderPack->shaderColour.Render(renderInterface, modelToRender->GetIndexCount(), 0);

			deviceContext->IASetIndexBuffer(m_gridModelLinesIndexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
			deviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_gridModelLinesIndexBuffer.D3DIndexTopology());
			shaderPack->shaderColour.Render(renderInterface, m_gridModelLinesIndexBuffer.NumElements(), 0);
			BE_ASSERT(D3D11_PRIMITIVE_TOPOLOGY_LINELIST == (D3D11_PRIMITIVE_TOPOLOGY)m_gridModelLinesIndexBuffer.D3DIndexTopology());
		}

		debugWorld->SetRenderAxes(m_renderAxes);
		debugWorld->Render(renderInterface, &shaderPack->shaderColour, m_camera.GetViewMatrix(), m_camera.GetPosition());

		renderInterface->DisableZBuffer();
		shaderPack->shaderTexture2d.SetShaderParameters(renderInterface, m_camera.GetViewMatrix());
			
		if (m_bitmapScreenGrab.GetTexture() && !writingToScreenGrabTexture)
		{
			m_bitmapScreenGrab.Render(renderInterface);
			shaderPack->shaderTexture2d.Render(renderInterface, m_bitmapScreenGrab.GetIndexCount(), m_bitmapScreenGrab.GetTexture());
		}
			
		m_bitmapTexQuad.Render(renderInterface);
		shaderPack->shaderTexture2d.Render(renderInterface, m_bitmapTexQuad.GetIndexCount(), m_bitmapTexQuad.GetTexture());
	
		renderInterface->EnableAlpha();

		{
			Vec3 cameraPos = m_camera.GetPosition();
			Matrix viewMatrix = m_camera.GetViewMatrix();

			float screenX = (float)mouse->GetX();
			float screenY = (float)mouse->GetY();
			Vec3 worldPos, worldDir;
			bool isInBounds = beCameraUtils::GetScreeenToWorldRay(*renderInterface, m_camera.GetViewMatrix(), screenX, screenY, &worldPos, &worldDir);
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
			fmt::format_to(message, "Dynamic Text\n");
			fmt::format_to(message, "Mouse [X, Y]: [{}, {}]\n", mouse->GetX(), mouse->GetY());
			fmt::format_to(message, "Camera [X, Y, Z]: [{:.2f}, {:.2f}, {:.2f}]\n", cameraPos.x, cameraPos.y, cameraPos.z);
			fmt::format_to(message, "Mat Right [X, Y, Z, W]: [{:.2f}, {:.2f}, {:.2f}, {:.2f}]\n", viewMatrix.m[0][0], viewMatrix.m[0][1], viewMatrix.m[0][2], viewMatrix.m[0][3]);
			fmt::format_to(message, "Mat Up    [X, Y, Z, W]: [{:.2f}, {:.2f}, {:.2f}, {:.2f}]\n", viewMatrix.m[1][0], viewMatrix.m[1][1], viewMatrix.m[1][2], viewMatrix.m[1][3]);
			fmt::format_to(message, "-Mat Forw [X, Y, Z, W]: [{:.2f}, {:.2f}, {:.2f}, {:.2f}]\n", viewMatrix.m[2][0], viewMatrix.m[2][1], viewMatrix.m[2][2], viewMatrix.m[2][3]);
			fmt::format_to(message, "_Mat Pos  [X, Y, Z, W]: [{:.2f}, {:.2f}, {:.2f}, {:.2f}]\n", viewMatrix.m[3][0], viewMatrix.m[3][1], viewMatrix.m[3][2], viewMatrix.m[3][3]);
			;
			if (isInBounds)
			{
				fmt::format_to(message, "ClickPos [X, Y, Z]: [{:.2f}, {:.2f}, {:.2f}]\n", worldPos.x, worldPos.y, worldPos.z);
				fmt::format_to(message, "ClickDir [X, Y, Z]: [{:.2f}, {:.2f}, {:.2f}]\n", worldDir.x, worldDir.y, worldDir.z);
			}
			if (maybeRealWorldPos)
			{
				fmt::format_to(message, "Collision [X, Y, Z]: [{:.2f}, {:.2f}, {:.2f}]\n", maybeRealWorldPos->x, maybeRealWorldPos->y, maybeRealWorldPos->z);
			}

			beString messageStr = fmt::to_string(message);
			m_bitmapTextDynamic.InitText(renderInterface, &m_font, messageStr.c_str(), 1.5f, 512.f, 0, false, beFont::WrapMode::Default);
			m_bitmapTextDynamic.Render(renderInterface);
			shaderPack->shaderTexture2d.Render(renderInterface, m_bitmapTextDynamic.GetIndexCount(), m_bitmapTextDynamic.GetTexture(), beShaderTexture2d::TextureMode::Clamped);
		}
			
		//bitmapTextPreRendered.Render(renderInterface);
		//textureShader2d.Render(renderInterface, bitmapTextPreRendered.GetIndexCount(), bitmapTextPreRendered.GetTexture());
		renderInterface->DisableAlpha();
			
		renderInterface->EnableZBuffer();
	};
	
	bool doScreenGrab = keyboard->IsPressed(beKeyboard::Button::Space);
	if (doScreenGrab)
	{
		m_screenGrabTexture.SetAsTarget(renderInterface);
		m_screenGrabTexture.Clear(renderInterface, V40());
		renderFrame(true);
		renderInterface->RestoreRenderTarget();
		m_bitmapScreenGrab.Init(renderInterface, m_screenGrabTexture);
	}
	renderFrame(false);

}

void StateRenderTest::InitGrid(beRenderInterface* renderInterface)
{
	int gridRadius = 100;
	float gridSize = 1.f;
	float xzOffset = gridSize / 2.f; // Don't draw on same spot as renderAxes
	float gridOffset = ((float)gridRadius / 2.f);

	int quadCount = gridRadius * gridRadius;
	int vertexCount = quadCount * 8;
	int triCount = quadCount * 2;
	int triIndexCount = triCount * 3;

	float noiseScale = 5.f;
	float noiseHeight = 8.f;
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
		int triListIndex = i * 6;
		int lineListIndex = i * 8;
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
		float xPos0 = x + xzOffset;
		float xPos1 = xPos0+gridSize;
		for (float z = -gridOffset; z < gridOffset; z += gridSize)
		{
			float yPos0 = z + xzOffset;
			float yPos1 = yPos0+gridSize;

			float zPos0 = noiseHeight * noise.GetOctave(xPos0/noiseScale, yPos0/noiseScale, 4);
			float zPos1 = noiseHeight * noise.GetOctave(xPos0/noiseScale, yPos1/noiseScale, 4);
			float zPos2 = noiseHeight * noise.GetOctave(xPos1/noiseScale, yPos1/noiseScale, 4);
			float zPos3 = noiseHeight * noise.GetOctave(xPos1/noiseScale, yPos0/noiseScale, 4);
			Vec4 pos0(xPos0, yPos0, zPos0, 1.f);
			Vec4 pos1(xPos0, yPos1, zPos1, 1.f);
			Vec4 pos2(xPos1, yPos1, zPos2, 1.f);
			Vec4 pos3(xPos1, yPos0, zPos3, 1.f);

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

	beRenderBuffer vertexBuffer, indexBuffer;

	vertexBuffer.Allocate(renderInterface, ElementSize(vertices), vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0, vertices.begin());
	indexBuffer.Allocate(renderInterface, decltype(triIndices)::element_size, triIndices.Count(), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 0, 0, triIndices.begin());
	bool success = m_gridModel.InitFromBuffers(&vertexBuffer, &indexBuffer);
	if (!success) { BE_ASSERT(false); return; }

	success = m_gridModelLinesIndexBuffer.Allocate(renderInterface, decltype(lineIndices)::element_size, vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, D3D11_PRIMITIVE_TOPOLOGY_LINELIST, 0, 0, lineIndices.begin());
	if (!success) { BE_ASSERT(false); return; }
}

