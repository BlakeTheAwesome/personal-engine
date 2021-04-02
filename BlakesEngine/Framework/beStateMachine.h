#pragma once
#include "beState.h"
#include <memory>

class beStateMachine
{
	public:
	beStateMachine(std::shared_ptr<beState> initialState=nullptr);
	beStateMachine(const beStateMachine&) = delete;
	beStateMachine(beStateMachine&&) = delete;
	~beStateMachine();
	beStateMachine& operator=(const beStateMachine&) = delete;
	beStateMachine& operator=(beStateMachine&&) = delete;

	void ChangeState(std::shared_ptr<beState> state, std::shared_ptr<beState>* previousState=nullptr); // Can retrieve the previous state instead of destroying it
	void ChangeStateWhenLoaded(std::shared_ptr<beState> state, std::shared_ptr<beState>* previousState=nullptr);
	bool Update(float dt);
	void Render();

	private:
	std::shared_ptr<beState> m_currentState;
	std::shared_ptr<beState> m_loadingState;
};


