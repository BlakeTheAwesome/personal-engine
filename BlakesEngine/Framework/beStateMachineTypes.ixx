module;
#include <memory>

export module beStateMachine:types;

class beStateMachine;

export class beState
{
	public:
	virtual ~beState() = default;
	beState() = default;
	beState(const beState&) = default;
	beState(beState&&) = default;
	beState& operator=(const beState&) = default;
	beState& operator=(beState&&) = default;

	virtual void Load() {}
	virtual bool UpdateLoading(float dt) { return true; }
	virtual void Unload() {}
	virtual void Enter(beStateMachine* stateMachine) {}
	virtual void Exit(beStateMachine* stateMachine) {}
	virtual void Update(beStateMachine* stateMachine, float dt) {}
	virtual void Render() {}
};

export class beStateMachine
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


