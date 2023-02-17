#pragma once

namespace Spark
{
	struct Buffer
	{
		byte* Data;
		uint32_t Size;

		Buffer();
		Buffer(byte* data, uint32_t size);

		void Allocate(uint32_t size);
		void ZeroInitialize();
		void Write(byte* data, uint32_t size, uint32_t offset = 0);

		operator bool() const { return Data; }
		byte& operator[](int index)  { return Data[index]; }
		byte operator[](int index) const { return Data[index]; }


		template<typename T>
		T* As()
		{
			return reinterpret_cast<T*>(Data);
		}

		uint32_t GetSize() const { return Size; }
	};
}
