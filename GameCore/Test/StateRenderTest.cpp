
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
#include "BlakesEngine/Shaders/beShaderPack.h"
#include "BlakesEngine/External/RenderDoc-Manager/RenderDocManager.h"
#include "BlakesEngine/Framework/beStateMachine.h"
#include "BlakesEngine/Framework/beAppData.h"

#include "BlakesEngine/External/Misc/StreamToDebugOutput.h"
#include <shellapi.h>

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
	m_model5.InitWithFilename(renderInterface, "boxes.obj", beWString(L"barrels.dds"), {});

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

	if (gamepad->GetButtonReleased(beGamepad::A) || keyboard->IsPressed(beKeyboard::Button::W))
	{
		renderInterface->ToggleWireframe();
	}
	if (keyboard->IsPressed(beKeyboard::Button::G))
	{
		m_renderGrid = !m_renderGrid;
	}
	if (gamepad->GetButtonReleased(beGamepad::B) || keyboard->IsPressed(beKeyboard::Button::Escape))
	{
		stateMachine->ChangeState(nullptr);
		return;
	}
	if (gamepad->GetButtonReleased(beGamepad::Y))
	{
		m_shaderToUse++;
		m_shaderToUse %= numShaders;
	}
	if (gamepad->GetButtonReleased(beGamepad::X) || mouse->IsPressed(beMouse::Button::MiddleButton))
	{
		m_modelToUse++;
		m_modelToUse %= numModels;
	}
	if (gamepad->GetButtonReleased(beGamepad::Select))
	{
		m_renderAxes = !m_renderAxes;
	}
	if (gamepad->GetButtonReleased(beGamepad::Up))
	{
		Vec2 bitmapPosition = m_bitmapTexQuad.GetPosition();
		m_bitmapTexQuad.SetPosition(bitmapPosition.x, bitmapPosition.y + 10.f);
	}
	if (gamepad->GetButtonReleased(beGamepad::Down))
	{
		Vec2 bitmapPosition = m_bitmapTexQuad.GetPosition();
		m_bitmapTexQuad.SetPosition(bitmapPosition.x, bitmapPosition.y - 10.f);
	}
	if (gamepad->GetButtonReleased(beGamepad::Left))
	{
		Vec2 bitmapPosition = m_bitmapTexQuad.GetPosition();
		m_bitmapTexQuad.SetPosition(bitmapPosition.x - 10.f, bitmapPosition.y);
	}
	if (gamepad->GetButtonReleased(beGamepad::Right))
	{
		Vec2 bitmapPosition = m_bitmapTexQuad.GetPosition();
		m_bitmapTexQuad.SetPosition(bitmapPosition.x + 10.f, bitmapPosition.y);
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

	if (!m_haveWrittenToTexture)
	{
		beTexture writeTexture;
		writeTexture.InitAsTarget(renderInterface, 512, 512);
		defer(
			writeTexture.Deinit();
		);
		m_haveWrittenToTexture = true;
		writeTexture.SetAsTarget(renderInterface);
		writeTexture.Clear(renderInterface, Vec4(0.f, 0.f, 0.f, 0.0f));
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
					shaderPack->shaderLitTexture.SetShaderParameters(renderInterface, m_camera.GetViewMatrix(), m_camera.GetPosition());
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
			u32 offsets[] ={ 0 };

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
		beStringBuilder sb;
		sb << "Dynamic Text\nMouseX:"<<mouse->GetX()<<"\nMouseY:"<<mouse->GetY();
		m_bitmapTextDynamic.InitText(renderInterface, &m_font, sb, 1.f, 512.f, 0, false, beFont::WrapMode::Default);
		m_bitmapTextDynamic.Render(renderInterface);
		shaderPack->shaderTexture2d.Render(renderInterface, m_bitmapTextDynamic.GetIndexCount(), m_bitmapTextDynamic.GetTexture(), beShaderTexture2d::TextureMode::Clamped);
			
		//bitmapTextPreRendered.Render(renderInterface);
		//textureShader2d.Render(renderInterface, bitmapTextPreRendered.GetIndexCount(), bitmapTextPreRendered.GetTexture());
		renderInterface->DisableAlpha();
			
		renderInterface->EnableZBuffer();
	};
	
	bool doScreenGrab = keyboard->IsPressed(beKeyboard::Button::Space);
	if (doScreenGrab)
	{
		m_screenGrabTexture.SetAsTarget(renderInterface);
		m_screenGrabTexture.Clear(renderInterface, Vec4(0.f, 0.f, 0.f, 0.f));
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

	for (int i = 0; i < lineIndices.Count(); i++)
	{
		lineIndices[i] = i;
	}

	for (int i = 0; i < quadCount; i++)
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
			float zPos0 = z + xzOffset;
			float zPos1 = zPos0+gridSize;

			float yPos0 = noiseHeight * noise.GetOctave(xPos0/noiseScale, zPos0/noiseScale, 4);
			float yPos1 = noiseHeight * noise.GetOctave(xPos0/noiseScale, zPos1/noiseScale, 4);
			float yPos2 = noiseHeight * noise.GetOctave(xPos1/noiseScale, zPos1/noiseScale, 4);
			float yPos3 = noiseHeight * noise.GetOctave(xPos1/noiseScale, zPos0/noiseScale, 4);
			Vec4 pos0(xPos0, yPos0, zPos0, 1.f);
			Vec4 pos1(xPos0, yPos1, zPos1, 1.f);
			Vec4 pos2(xPos1, yPos2, zPos1, 1.f);
			Vec4 pos3(xPos1, yPos3, zPos0, 1.f);

			vertices[vertexIndex+0].position = pos0;
			vertices[vertexIndex+1].position = pos1;
			vertices[vertexIndex+2].position = pos1;
			vertices[vertexIndex+3].position = pos2;
			vertices[vertexIndex+4].position = pos2;
			vertices[vertexIndex+5].position = pos3;
			vertices[vertexIndex+6].position = pos3;
			vertices[vertexIndex+7].position = pos0;
			vertices[vertexIndex+0].colour = Vec4(yPos0 / noiseHeight, yPos0 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+1].colour = Vec4(yPos1 / noiseHeight, yPos1 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+2].colour = Vec4(yPos1 / noiseHeight, yPos1 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+3].colour = Vec4(yPos2 / noiseHeight, yPos2 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+4].colour = Vec4(yPos2 / noiseHeight, yPos2 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+5].colour = Vec4(yPos3 / noiseHeight, yPos3 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+6].colour = Vec4(yPos3 / noiseHeight, yPos3 / noiseHeight, 1.f, 1.f);
			vertices[vertexIndex+7].colour = Vec4(yPos0 / noiseHeight, yPos0 / noiseHeight, 1.f, 1.f);
			vertexIndex += 8;
		}
	}

	beRenderBuffer vertexBuffer, indexBuffer;

	vertexBuffer.Allocate(renderInterface, decltype(vertices)::element_size, vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0, vertices.begin());
	indexBuffer.Allocate(renderInterface, decltype(triIndices)::element_size, triIndices.Count(), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 0, 0, triIndices.begin());
	bool success = m_gridModel.InitFromBuffers(&vertexBuffer, &indexBuffer);
	if (!success) { BE_ASSERT(false); return; }

	success = m_gridModelLinesIndexBuffer.Allocate(renderInterface, decltype(lineIndices)::element_size, vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, D3D11_PRIMITIVE_TOPOLOGY_LINELIST, 0, 0, lineIndices.begin());
	if (!success) { BE_ASSERT(false); return; }
}

