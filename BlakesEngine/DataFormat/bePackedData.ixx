module;

#include "BlakesEngine/Core/beString.h"

export module bePackedData;
import beSpan;
import beDataBuffer;
import beVector;
import beHashMap;
import beTypes;

export class bePackedData
{
	public:

	static constexpr int MAX_STREAMS = 8;
	struct DataStream
	{
		beString name;
		beDataBuffer data;
		u32 dataLen = 0;
		u32 fileOffset = 0;
	};

	void LoadFromFile(beStringView filePath);

	void ReadHeaders(std::FILE* file);

	beDataBuffer GetStream(beStringView name) const;
	std::span<const DataStream> GetStreams() const { return to_span(m_streams); }

	private:
		void ReadStreams(std::FILE* file);
		void ReadStream(std::FILE* file, DataStream* header);
		u32 GetDataOffset() const;
		beFixedVector<DataStream, MAX_STREAMS> m_streams;
};