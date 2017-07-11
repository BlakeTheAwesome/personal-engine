#include "BlakesEngine/bePCH.h"
#include "beStateMachine.h"

// This is set up to allow you to get back the current state when you change.
// Potential issue if you retrieve a state and stash it on the next state who then deletes it.
// Might want to change to a ref counting model or a serialise/deserialise for storing state data.

beStateMachine::beStateMachine(beState* initialState)
{
	if (initialState)
	{
		initialState->Enter(this);
		m_currentState = initialState;
	}
}

beStateMachine::~beStateMachine()
{
	if (m_loadingState)
	{
		m_loadingState->Unload();
		BE_DELETE(m_loadingState);
	}
	if (m_currentState)
	{
		m_currentState->Exit(this);
		BE_DELETE(m_currentState);
	}
}

bool beStateMachine::Update(float dt)
{
	if (m_loadingState)
	{
		if (m_loadingState->UpdateLoading(dt))
		{
			if (m_currentState)
			{
				m_currentState->Exit(this);
				if (!m_currentStateRetrieved)
				{
					BE_DELETE(m_currentState);
				}
			}
			else
			{
				BE_ASSERT(!m_currentStateRetrieved);
			}
			m_currentState = m_loadingState;
			m_loadingState = nullptr;
			m_currentStateRetrieved = false;
			m_currentState->Enter(this);
		}
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


void beStateMachine::ChangeState(beState* state, beState** out_prevState)
{
	if (m_loadingState)
	{
		BE_ASSERT(m_currentState);
		m_currentState->Exit(this);
		m_loadingState->Unload();
		
		if (out_prevState)
		{
			if (m_currentStateRetrieved) // If current state is already owned elsewhere, then we're now passing ownership of currently loading state.
			{
				*out_prevState = m_loadingState;
			}
			else
			{
				*out_prevState = m_currentState;
				BE_DELETE(m_loadingState); // Current state owned by someone, nobody to own this loading state
			}
		}
		else
		{
			BE_DELETE(m_currentState);
			BE_DELETE(m_loadingState);
		}
		m_loadingState = nullptr;
		m_currentStateRetrieved = false;
		m_currentState = state;
	}
	else
	{
		BE_ASSERT(!m_currentStateRetrieved);
		if (m_currentState)
		{
			m_currentState->Exit(this);
			if (out_prevState)
			{
				*out_prevState = m_currentState;
			}
			else
			{
				BE_DELETE(m_currentState);
			}
		}
		else if (out_prevState)
		{
			*out_prevState = nullptr;
		}
		m_currentState = state;
	}

	if (m_currentState)
	{
		m_currentState->Enter(this);
	}
}


void beStateMachine::ChangeStateWhenLoaded(beState* state, beState** out_prevState)
{
	BE_ASSERT(m_currentState);
	BE_ASSERT(state);

	if (m_loadingState)
	{
		if (out_prevState)
		{
			m_loadingState->Unload();
			*out_prevState = m_loadingState;
		}
		else
		{
			BE_DELETE(m_loadingState);
		}
		// Leave current state, set new state being loaded
		m_loadingState = state;
	}
	else
	{
		BE_ASSERT(!m_currentStateRetrieved);
		if (out_prevState)
		{
			*out_prevState = m_currentState;
			m_currentStateRetrieved = true;
		}
		m_loadingState = state;
	}

	state->Load();
}

