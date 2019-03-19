#include "BlakesEngine/bePCH.h"
#include "beByteStreams.h"

void beByteStreamBase::Reset()
{
	m_streamedBytes = 0;
}

void beByteStreamBase::SkipBytes(dataSize numBytes)
{
	BE_ASSERT((m_streamedBytes + numBytes) <= m_streamedBytes);
	m_streamedBytes += numBytes;
}

void beByteStreamBase::ReadBytes(void* data, dataSize numBytes)
{
	BE_ASSERT((m_streamedBytes + numBytes) <= m_streamedBytes);
	memcpy(data, m_data + m_streamedBytes, numBytes);
	m_streamedBytes += numBytes;
}

void beByteStreamBase::WriteBytes(const void* data, dataSize numBytes)
{
	BE_ASSERT((m_streamedBytes + numBytes) <= m_streamedBytes);
	memcpy(m_data + m_streamedBytes, data, numBytes);
	m_streamedBytes += numBytes;
}