#pragma once

class beStateMachine;
class beState
{
	public:
	virtual ~beState() = default;
	virtual void Load(){}
	virtual bool UpdateLoading(float dt){ return true; }
	virtual void Unload(){}
	virtual void Enter(beStateMachine* stateMachine){}
	virtual void Exit(beStateMachine* stateMachine){}
	virtual void Update(beStateMachine* stateMachine, float dt){}
	virtual void Render(){}
};

