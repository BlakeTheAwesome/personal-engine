#pragma once

#include "Core/beTypes.h"

class beTimeValue
{
	public:
		beTimeValue();
		beTimeValue(s64 microseconds);

		void Set(s64 microseconds);

		static beTimeValue Now();

		float ToSeconds() const;
		s64 ToMicroSeconds() const;

		beTimeValue& operator+=(const beTimeValue& that);
		beTimeValue operator+(const beTimeValue& that) const;
		beTimeValue operator-(const beTimeValue& that) const;

		bool operator>(const beTimeValue& that) const;
		bool operator<(const beTimeValue& that) const;
		bool operator>=(const beTimeValue& that) const;
		bool operator<=(const beTimeValue& that) const;
		bool operator==(const beTimeValue& that) const;
		bool operator!=(const beTimeValue& that) const;
	private:
		s64 m_microseconds;
};
