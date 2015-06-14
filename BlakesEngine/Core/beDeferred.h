#pragma once
#include "beFunction.h"

class DeferredCall
{
	public:
		typedef func::function<void()> FnType;

		DeferredCall() {}
		DeferredCall(FnType fn) : m_fn(fn) {}
		~DeferredCall()
		{
			if (IsValid())
			{
				Call();
			}
		}

		void Clear()
		{
			m_fn = FnType();
		}

		bool IsValid()
		{
			return m_fn;
		}

		void Call()
		{
			m_fn();
			Clear();
		}

	private:
		DeferredCall(const DeferredCall&) = delete;
		DeferredCall& operator=(const DeferredCall&) = delete;
		FnType m_fn;
};