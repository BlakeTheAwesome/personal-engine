#pragma once
#include "beState.h"

class beStateMachine
{
	public:
	beStateMachine(beState* initialState=nullptr);
	~beStateMachine();

	void ChangeState(beState* state, beState** previousState=nullptr); // Can retrieve the previous state instead of destroying it
	void ChangeStateWhenLoaded(beState* state, beState** previousState=nullptr);
	bool Update(float dt);
	void Render();

	private:
	beState* m_currentState = nullptr;
	beState* m_loadingState = nullptr;
	bool m_currentStateRetrieved = false;
};


