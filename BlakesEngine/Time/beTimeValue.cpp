#include "BlakesEngine/bePCH.h"
#include "beTimeValue.h"

#include "beClock.h"

beTimeValue::beTimeValue()
	: m_microseconds(0)
{
}

beTimeValue::beTimeValue(s64 microseconds)
	: m_microseconds(microseconds)
{
}

void beTimeValue::Set(s64 microseconds)
{
	m_microseconds = microseconds;
}

beTimeValue beTimeValue::Now()
{
	double millisSinceStart = beClock::GetMillisecondsSinceStart();
	s64 microsSinceStart = (s64)(millisSinceStart * 1000.0);
	return beTimeValue(microsSinceStart);
}

float beTimeValue::ToSeconds() const
{
	return (float)(m_microseconds / 1000000.0);
}

s64 beTimeValue::ToMicroSeconds() const
{
	return m_microseconds;
}

beTimeValue& beTimeValue::operator+=(const beTimeValue& that)
{
	m_microseconds += that.m_microseconds;
	return *this;
}

beTimeValue beTimeValue::operator+(const beTimeValue& that) const
{
	return beTimeValue(m_microseconds + that.m_microseconds);
}

beTimeValue beTimeValue::operator-(const beTimeValue& that) const
{
	return beTimeValue(m_microseconds - that.m_microseconds);

}

bool beTimeValue::operator>(const beTimeValue& that) const
{
	return m_microseconds > that.m_microseconds;
}

bool beTimeValue::operator<(const beTimeValue& that) const
{
	return m_microseconds < that.m_microseconds;
}

bool beTimeValue::operator>=(const beTimeValue& that) const
{
	return m_microseconds >= that.m_microseconds;
}

bool beTimeValue::operator<=(const beTimeValue& that) const
{
	return m_microseconds <= that.m_microseconds;
}

bool beTimeValue::operator==(const beTimeValue& that) const
{
	return m_microseconds == that.m_microseconds;
}

bool beTimeValue::operator!=(const beTimeValue& that) const
{
	return m_microseconds != that.m_microseconds;
}
