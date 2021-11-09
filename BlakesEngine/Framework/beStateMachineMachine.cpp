module;
#include "BlakesEngine/Core/beAssert.h"
#include <memory>
module beStateMachine;

import :types;

// This is set up to allow you to get back the current state when you change.
// Potential issue if you retrieve a state and stash it on the next state who then deletes it.
// Might want to change to a ref counting model or a serialise/deserialise for storing state data.

beStateMachine::beStateMachine(std::shared_ptr<beState> initialState)
{
	if (initialState)
	{
		initialState->Enter(this);
		m_currentState = move(initialState);
	}
}

beStateMachine::~beStateMachine()
{
	if (m_loadingState)
	{
		m_loadingState->Unload();
	}
	if (m_currentState)
	{
		m_currentState->Exit(this);
	}
}

bool beStateMachine::Update(float dt)
{
	if (m_loadingState && m_loadingState->UpdateLoading(dt))
	{
		if (m_currentState)
		{
			m_currentState->Exit(this);
			m_currentState.reset();
		}
		m_currentState = move(m_loadingState);
		BE_ASSERT(!m_loadingState);
		m_currentState->Enter(this);
	}
	if (m_currentState)
	{
		m_currentState->Update(this, dt);
		return true;
	}
	else
	{
		return false;
	}
}

void beStateMachine::Render()
{
	if (m_currentState)
	{
		m_currentState->Render();
	}
}


void beStateMachine::ChangeState(std::shared_ptr<beState> state, std::shared_ptr<beState>* out_prevState)
{
	if (m_loadingState)
	{
		BE_ASSERT((bool)m_currentState);
		m_currentState->Exit(this);
		m_loadingState->Unload();
		
		if (out_prevState)
		{
			*out_prevState = move(m_loadingState);
		}
		else
		{
			m_loadingState.reset();
		}
		m_currentState = move(state);
	}
	else
	{
		if (m_currentState)
		{
			m_currentState->Exit(this);
			if (out_prevState)
			{
				*out_prevState = move(m_currentState);
			}
		}
		else if (out_prevState)
		{
			out_prevState->reset();
		}
		m_currentState = move(state);
	}

	if (m_currentState)
	{
		m_currentState->Enter(this);
	}
}


void beStateMachine::ChangeStateWhenLoaded(std::shared_ptr<beState> state, std::shared_ptr<beState>* out_prevState)
{
	BE_ASSERT((bool)m_currentState);
	BE_ASSERT((bool)state);

	if (m_loadingState)
	{
		if (out_prevState)
		{
			m_loadingState->Unload();
			*out_prevState = move(m_loadingState);
		}

		// Leave current state, set new state being loaded
		m_loadingState = state;
	}
	else
	{
		if (out_prevState)
		{
			*out_prevState = move(m_currentState);
		}
		m_loadingState = state;
	}

	state->Load();
}

