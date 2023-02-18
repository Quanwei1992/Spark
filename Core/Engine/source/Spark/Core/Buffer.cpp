#include "skpch.h"
#include "Buffer.h"

namespace Spark
{
	Buffer::Buffer()
		:Data(nullptr),Size(0)
	{
	}

	Buffer::Buffer(byte* data, uint32_t size)
		:Data(data), Size(size)
	{
	}

	void Buffer::Allocate(uint32_t size)
	{
		if(Data)
		{
			delete[] Data;
			Data = nullptr;
		}

		if (size == 0) return;
		Data = new byte[size];
		Size = size;
	}

	void Buffer::ZeroInitialize()
	{
		if (Data)
			memset(Data, 0, Size);
	}

	void Buffer::Write(byte* data, uint32_t size, uint32_t offset)
	{
		SK_CORE_ASSERT(offset + size <= Size, "Buffer overflow!");
		memcpy(Data + offset, data, size);
	}

	Buffer Buffer::Copy(const void* data, uint32_t size)
	{
		Buffer buffer;
		buffer.Allocate(size);
		memcpy(buffer.Data, data, size);
		return buffer;
	}
}
