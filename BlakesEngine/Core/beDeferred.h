#pragma once

#define defer(body) auto MACRO_CONCAT(_deferredCall, __COUNTER__) = beDeferred::MakeLambdaDefer([&](){body;})

namespace beDeferred
{
	template <typename Lambda>
	struct LambdaDefer
	{
		LambdaDefer(Lambda&& l) : m_fn(std::move(l)) {}
		LambdaDefer(LambdaDefer&& rhs) = default;
		LambdaDefer(const LambdaDefer&) = delete;
		LambdaDefer& operator=(const LambdaDefer&) = delete;

		~LambdaDefer()
		{
			if (IsValid())
			{
				Call();
			}
		}

		void Clear()
		{
			m_valid = false;
		}

		bool IsValid()
		{
			return m_valid;
		}

		void Call()
		{
			m_fn();
			Clear();
		}

		private:
		Lambda m_fn;
		bool m_valid = true;
	};

	template <typename Lambda>
	LambdaDefer<Lambda> MakeLambdaDefer(Lambda&& l)
	{
		return LambdaDefer<Lambda>(std::move(l));
	}
}
