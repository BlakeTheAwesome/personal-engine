module;
#include "BlakesEngine/bePCH.h"
#include "BlakesEngine/Core/beAssert.h"

module beByteStreams;

void beByteStreamBase::Reset()
{
	m_streamedBytes = 0;
}

void beByteStreamBase::SkipBytes(dataSize numBytes)
{
	BE_ASSERT((m_streamedBytes + numBytes) <= m_streamLength);
	m_streamedBytes += numBytes;
}

void beByteStreamBase::ReadBytes(void* data, dataSize numBytes)
{
	BE_ASSERT((m_streamedBytes + numBytes) <= m_streamLength);
	memcpy(data, m_data + m_streamedBytes, numBytes);
	m_streamedBytes += numBytes;
}

void beByteStreamBase::WriteBytes(const void* data, dataSize numBytes)
{
	BE_ASSERT((m_streamedBytes + numBytes) <= m_streamLength);
	memcpy(m_data + m_streamedBytes, data, numBytes);
	m_streamedBytes += numBytes;
}