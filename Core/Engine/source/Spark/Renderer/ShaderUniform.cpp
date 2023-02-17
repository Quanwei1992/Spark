#include "skpch.h"
#include "ShaderUniform.h"

namespace Spark
{
	ShaderStruct::ShaderStruct(const std::string& name)
		:m_Name(name)
	{
	}

	void ShaderStruct::AddField(ShaderUniformDeclaration* field)
	{
		m_Size += field->GetSize();
		uint32_t offset = 0;
		if(m_Fields.size())
		{
			ShaderUniformDeclaration* previous = m_Fields.back();
			offset = previous->GetOffset() + previous->GetOffset();
		}
		field->SetOffset(offset);
		m_Fields.push_back(field);
	}
}
