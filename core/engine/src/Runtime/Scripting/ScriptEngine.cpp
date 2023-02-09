#include "skpch.h"
#include "ScriptEngine.h"
#include "ScriptGlue.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include <fstream>

namespace Spark
{

	namespace Utils
	{
		char* ReadBytes(const std::filesystem::path filepath, uint32_t* outSize)
		{
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);
			if (!stream)
			{
				return nullptr;
			}


			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint32_t size = end - stream.tellg();
			if (size == 0)
			{
				return nullptr;
			}
			char* buffer = new char[size];
			stream.read(buffer, size);
			stream.close();
			*outSize = size;
			return buffer;
		}

		MonoAssembly* LoadMonoAssembly(const std::filesystem::path & path)
		{
			uint32_t fileSize = 0;
			char* fileData = ReadBytes(path, &fileSize);
			
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, true, &status, false);
			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				SK_CORE_ERROR("Load mono image failed - {0}", errorMessage);
				return nullptr;
			}

			MonoAssembly* assembly = mono_assembly_load_from_full(image, path.string().c_str(), &status, 0);
			mono_image_close(image);
			
			delete[] fileData;
			return assembly;
		}

		void PrintAssemblyTypes(MonoAssembly* assembly)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
			for (int32_t i =0;i<numTypes;i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				SK_CORE_INFO("{0}.{1}", nameSpace, name);
			}
		}

	}




	class ScriptClass;
	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;
		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		ScriptClass* EntityClass = nullptr;
	};

	static ScriptEngineData* s_Data = nullptr;


	class ScriptClass
	{
	public:
		ScriptClass(const std::string& classNamespace, const std::string& className)
			:m_ClassNamespace(classNamespace)
			, m_ClassName(className)
		{
			m_MonoClass = mono_class_from_name(s_Data->CoreAssemblyImage, classNamespace.c_str(), className.c_str());
		}

		MonoObject* Instantiate() const
		{
			MonoObject* instance = mono_object_new(s_Data->AppDomain, m_MonoClass);
			mono_runtime_object_init(instance);
			return instance;
		}

		MonoMethod* GetMethod(const std::string& name, int parameterCount) const
		{
			return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
		}

		MonoObject* InvokeMethod(MonoObject* instance,MonoMethod* method, void** params) const
		{
			return mono_runtime_invoke(method, instance, params, nullptr);
		}


	private:
		std::string m_ClassNamespace;
		std::string m_ClassName;
		MonoClass* m_MonoClass = nullptr;
	};



	void ScriptEngine::LoadAssembly(const std::filesystem::path& filepath)
	{
		s_Data->AppDomain = mono_domain_create_appdomain("SparkScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
		//Utils::PrintAssemblyTypes(s_Data->CoreAssembly);
	}

	void ScriptEngine::InitMono()
	{
		mono_set_assemblies_path("C:\\Program Files\\Mono\\lib");

		s_Data->RootDomain = mono_jit_init("SparkJITRumtime");
		SK_CORE_ASSERT(s_Data->RootDomain);
	}


	void ScriptEngine::ShutdownMono()
	{
		//mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;

		//mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;
	}

	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();
		InitMono();
		LoadAssembly("resources/scripts/script-core.dll");

		ScriptGlue::RegisterFunctions();

		s_Data->EntityClass = new ScriptClass("Spark", "Entity");


		MonoObject* instance = s_Data->EntityClass->Instantiate();

		// 2. call function
		MonoMethod* printMessageFunc = s_Data->EntityClass->GetMethod("PrintMessage", 0);

		s_Data->EntityClass->InvokeMethod(instance,printMessageFunc, nullptr);

		// 3. call function with param
		MonoMethod* printIntFunc = s_Data->EntityClass->GetMethod("PrintInt", 1);
		int value = 5;
		void* params = &value;
		s_Data->EntityClass->InvokeMethod(instance,printIntFunc, &params);

		MonoMethod* printCustomMessageFunc = s_Data->EntityClass->GetMethod("PrintCustomMessage", 1);
		MonoString* monoString = mono_string_new(s_Data->AppDomain, "Hello World from C++!");
		void* stringParams = monoString;
		s_Data->EntityClass->InvokeMethod(instance, printCustomMessageFunc, &stringParams);
	}

	void ScriptEngine::Shutdown()
	{
		ShutdownMono();
		delete s_Data;
		s_Data = nullptr;
	}

}
