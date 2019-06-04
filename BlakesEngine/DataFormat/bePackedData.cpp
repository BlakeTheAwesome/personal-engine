#include "BlakesEngine/bePCH.h"
#include "bePackedData.h"
#include "BlakesEngine/DataStructures/beDataBuffer.h"
#include <filesystem>

static int ReadString(char* buffer, int bufferLen, std::FILE* file)
{
	int stringLength = std::fgetc(file);
	if (stringLength == EOF)
	{
		BE_ASSERT(false);
		return -1;
	}

	BE_ASSERT(stringLength < bufferLen);

	int bytesRead = (int)std::fread(buffer, sizeof(u8), stringLength, file);
	if (bytesRead < stringLength)
	{
		BE_ASSERT(false);
		return -1;
	}
	buffer[stringLength] = '\0';
	return stringLength;
}

struct ScopedFile
{
	std::FILE* file = nullptr;
	~ScopedFile()
	{
		fclose(file);
	}
};

void bePackedData::LoadFromFile(beStringView filePath)
{
	std::FILE* file = std::fopen(filePath.c_str(), "r");
	if (!file)
	{
		BE_ASSERT(false);
		return;
	}
	ScopedFile scope{file};
	ReadHeaders(file);
	ReadStreams(file);
}

void bePackedData::ReadHeaders(std::FILE* file)
{
	int numStreams = std::fgetc(file);
	if (numStreams == EOF)
	{
		BE_ASSERT(false);
		return;
	}
	BE_ASSERT(numStreams < 256);

	for (int i : RangeIter(numStreams))
	{
		Unused(i);
		u32 dataLen{};
		auto bytesRead = std::fread(&dataLen, sizeof(u8), sizeof(dataLen), file);
		if (bytesRead != sizeof(dataLen))
		{
			BE_ASSERT(false);
			return;
		}
		char stringBuf[256]{};
		int stringLength = ReadString(stringBuf, sizeof(stringBuf), file);
		if (stringLength < 1)
		{
			BE_ASSERT(false);
			return;
		}
		DataStream* header = m_streams.AddNew();
		header->dataLen = dataLen;
		header->name.assign(stringBuf, stringLength);
	}

	u32 dataOffset = (u32)ftell(file);
	for (DataStream& dataStream : m_streams)
	{
		dataStream.fileOffset = dataOffset;
		dataOffset += dataStream.dataLen;
	}
}

beDataBuffer bePackedData::GetStream(beStringView name) const
{
	DataStream const* stream = m_streams.AddressOf([name](auto&& iter) { return iter.name == name; });
	if (!stream)
	{
		BE_ASSERT(false);
		return {};
	}
	return {stream->data.GetBuffer(), stream->data.GetSize()};
}

void bePackedData::ReadStreams(std::FILE* file)
{
	for (DataStream& dataStream : m_streams)
	{
		ReadStream(file, &dataStream);
	}

	// Verify we're at end of file
	BE_ASSERT_CODE(u32 filePos = (u32)std::ftell(file);)
	BE_ASSERT_CODE(std::fseek(file, 0, SEEK_END);)
	BE_ASSERT(filePos == (u32)std::ftell(file));
}

void bePackedData::ReadStream(std::FILE* file, DataStream* dataStream)
{
	beDataBuffer* dataBuffer = &dataStream->data;
	dataBuffer->ResizeBuffer(dataStream->dataLen);

	[[maybe_unused]] int ret = std::fseek(file, dataStream->fileOffset, SEEK_SET);
	BE_ASSERT(ret == 0);
	[[maybe_unused]] auto bytesRead = std::fread(dataBuffer->ModifyBuffer(), sizeof(u8), dataStream->dataLen, file);
	BE_ASSERT(bytesRead == dataStream->dataLen);
}
