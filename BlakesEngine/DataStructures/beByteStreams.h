#ifndef _ByteStreams_h_
#define _ByteStreams_h_

#include "BlakesEngine/DataStructures/DataStructuresConsts.h"


class beByteStreamBase
{
	typedef u32 dataSize;
	
	public:
		void Reset();
		void Continue();
		void SkipBytes( dataSize numBytes );
	
	protected:
		void StreamBytes( void* srcData, dataSize srcDataLength );
		template<T> void StreamBytes( const T& data )
			{	StreamBytes( &data, sizeof(data) ); }
			
		void*    m_streamData;
		dataSize m_streamLength;
		dataSize m_streamedBytes;
		bool     m_isWriteStream;
		bool     m_hasFailed;
		
		// protected constructors
		beByteStreamBase();
		beByteStreamBase( void* data, dataSize length, bool isWrite );
		~beByteStreamBase();
}

class beReadStream : public beByteStreamBase
{
	public:
		beReadStream( void* data, dataSize length )
			:	beByteStreamBase( data, length, false ){};
			
	private:
		beReadStream& operator= (const beVector&){};
		beReadStream(const beVector&){};
}

class beWriteStream : public beByteStreamBase
{
	public:
		beWriteStream( void* data, dataSize length )
			:	beByteStreamBase( data, length, true ){};
			
	private:
		beWriteStream& operator= (const beVector&){};
		beWriteStream(const beVector&){};
}

template< int streamSize >
class beFixedWriteStream : beWriteStream
{
	public:
		beFixedWriteStream()
			:	beWriteStream( m_internalData, streamSize ){};
	
	private:
		u8 m_internalData[streamSize];
}


#endif
