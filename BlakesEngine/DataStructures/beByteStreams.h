#pragma once

#include "beDataBuffer.h"


class beByteStreamBase
{
	public:
		using dataSize = u32;

		void Reset();
		void SkipBytes(dataSize numBytes);
	
	protected:
		void ReadBytes(void* data, dataSize numBytes);
		void WriteBytes(const void* data, dataSize numBytes);
		u8*    m_data = nullptr;
		dataSize m_streamLength = 0;
		dataSize m_streamedBytes = 0;
		bool     m_hasFailed = false;
		
		// protected constructors
		beByteStreamBase() = default;
		beByteStreamBase(void* data, dataSize length) : m_data((u8*)data), m_streamLength(length) {}
};

class beReadStream : public beByteStreamBase
{
	public:
		beReadStream( void const* data, dataSize length ) : beByteStreamBase(const_cast<void*>(data), length){};
		beReadStream(beDataBuffer const& buffer) : beReadStream(buffer.GetBuffer(), buffer.GetSize()) {}
		
		template<typename T> void Stream(T& data) { ReadBytes(&data, sizeof(data)); }
		template<typename T> beReadStream& operator>>(T& data) { ReadBytes(&data, sizeof(data)); return *this; }

	private:
		beReadStream& operator= (const beReadStream&) = delete;
		beReadStream(const beReadStream&) = delete;
};

class beWriteStream : public beByteStreamBase
{
	public:
		beWriteStream( void* data, dataSize length ) : beByteStreamBase(data, length){};
		beWriteStream(beDataBuffer* buffer) : beWriteStream(buffer->ModifyBuffer(), buffer->GetSize()) {}

		template<typename T> void Stream(const T& data) { WriteBytes(&data, sizeof(data)); }
		template<typename T> beWriteStream& operator<<(const T& data) { WriteBytes(&data, sizeof(data)); return *this; }
	private:
		beWriteStream& operator= (const beWriteStream&) = delete;
		beWriteStream(const beWriteStream&) = delete;
};

template< int streamSize >
class beFixedWriteStream : beWriteStream
{
	public:
		beFixedWriteStream() : beWriteStream(m_internalData, streamSize){};

		template<typename T> void Stream(const T& data) { WriteBytes(&data, sizeof(data)); }
		template<typename T> beFixedWriteStream& operator<<(const T& data) { WriteBytes(&data, sizeof(data)); return *this; }
	private:
		u8 m_internalData[streamSize];
};
