#include "BlakesEngine/bePCH.h"
//#include "beMouse.h"
//#include "BlakesEngine/Core/beAssert.h"
//#include "BlakesEngine/Platform/beSystemEventManager.h"
//
//#include <mutex>
//
//DirectX::Keyboard s_keyboard;
//static int s_instanceCount = 0;
//static beSystemEventManager::CallbackId s_systemCallbackId;
//static std::mutex s_mutex;
//
//beMouse::~beMouse()
//{
//	BE_ASSERT(!m_systemEventManager); 
//}
//
//void beMouse::Init(beSystemEventManager* systemEventManager)
//{
//	m_systemEventManager = systemEventManager;
//
//	std::lock_guard<decltype(s_mutex)> lock(s_mutex);
//	if (++s_instanceCount == 1)
//	{
//		s_systemCallbackId = systemEventManager->RegisterCallbackWinProc(nullptr, [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, void* userdata, LRESULT* result) -> bool {
//			s_keyboard.ProcessMessage(message, wParam, lParam);
//			return false;
//		});
//	}
//}
//
//void beMouse::Deinit()
//{
//	std::lock_guard<decltype(s_mutex)> lock(s_mutex);
//	if (--s_instanceCount == 0)
//	{
//		m_systemEventManager->DeregisterCallbackWinProc(s_systemCallbackId);
//	}
//	m_systemEventManager = nullptr;
//}
//
//void beMouse::Update(float dt)
//{
//	m_tracker.Update(s_keyboard.GetState());
//}
//
//bool beMouse::IsPressed(Button button) const
//{
//	return m_tracker.IsKeyPressed(button);
//}
//
//bool beMouse::IsReleased(Button button) const
//{
//	return m_tracker.IsKeyReleased(button);
//}
//
//bool beMouse::IsDown(Button button) const
//{
//	return m_tracker.GetLastState().IsKeyDown(button);
//}
//
