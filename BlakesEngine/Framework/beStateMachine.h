#pragma once
#include "beState.h"

class beStateMachine
{
	public:
	beStateMachine(std::shared_ptr<beState> initialState=nullptr);
	~beStateMachine();

	void ChangeState(std::shared_ptr<beState> state, std::shared_ptr<beState>* previousState=nullptr); // Can retrieve the previous state instead of destroying it
	void ChangeStateWhenLoaded(std::shared_ptr<beState> state, std::shared_ptr<beState>* previousState=nullptr);
	bool Update(float dt);
	void Render();

	private:
	std::shared_ptr<beState> m_currentState;
	std::shared_ptr<beState> m_loadingState;
};


