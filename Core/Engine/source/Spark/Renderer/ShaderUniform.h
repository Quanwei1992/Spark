#pragma	once

namespace Spark
{
	enum class ShaderDomain
	{
		None =0,Vertex,Pixel
	};

	class ShaderUniformDeclaration
	{
	public:
		virtual  ~ShaderUniformDeclaration() = default;
		virtual const std::string& GetName() const = 0;
		virtual uint32_t GetSize() const = 0;
		virtual uint32_t GetCount() const = 0;
		virtual uint32_t GetOffset() const = 0;
		virtual ShaderDomain GetDomain() const = 0;
	protected:
		friend class ShaderStruct;
		virtual void SetOffset(uint32_t offset) = 0;
	};

	typedef std::vector<ShaderUniformDeclaration*> ShaderUniformList;

	class ShaderUniformBufferDeclaration
	{
	public:
		virtual  ~ShaderUniformBufferDeclaration() = default;
		virtual const std::string& GetName() const = 0;
		virtual uint32_t GetRegister()  const = 0;
		virtual uint32_t GetSize() const = 0;
		virtual const ShaderUniformList& GetUniformDeclarations() const = 0;
		virtual ShaderUniformDeclaration* FindUniform(const std::string& name) = 0;
	};

	typedef std::vector<ShaderUniformBufferDeclaration*> ShaderUniformBufferList;

	class ShaderStruct
	{
	public:
		ShaderStruct(const std::string& name);
		~ShaderStruct() = default;

		void AddField(ShaderUniformDeclaration* field);

		void SetOffset(uint32_t offset) { m_Offset = offset; }
		const std::string& GetName() const { return m_Name; }
		uint32_t GetSize() const { return m_Size; }
		uint32_t GetOffset() const { return m_Offset; }
		const std::vector<ShaderUniformDeclaration*>& GetFields() const { return m_Fields; }

	private:
		friend class Shader;
		std::string m_Name;
		ShaderUniformList m_Fields;
		uint32_t m_Size = 0;
		uint32_t m_Offset = 0;
	};

	typedef std::vector<ShaderStruct*> ShaderStructList;

	class ShaderResourceDeclaration
	{
	public:
		~ShaderResourceDeclaration() = default;
		virtual const std::string& GetName() const = 0;
		virtual uint32_t GetRegister() const = 0;
		virtual uint32_t GetCount() const = 0;
	};

	typedef std::vector<ShaderResourceDeclaration*> ShaderResourceList;
}
