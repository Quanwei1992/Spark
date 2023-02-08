#include "skpch.h"
#include "ScriptEngine.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include <fstream>

namespace Spark
{

	namespace Utils
	{
		char* ReadBytes(const std::string& filepath, uint32_t* outSize)
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

		MonoAssembly* LoadCSharpAssembly(const std::string& path)
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

			MonoAssembly* assembly = mono_assembly_load_from_full(image, path.c_str(), &status, 0);
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


	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;
		MonoAssembly* CoreAssembly = nullptr;

	};

	static ScriptEngineData* s_Data = nullptr;

	void ScriptEngine::InitMono()
	{
		mono_set_assemblies_path("C:\\Program Files\\Mono\\lib");

		// Create Root Domain

		s_Data->RootDomain = mono_jit_init("SparkJITRumtime");
		SK_CORE_ASSERT(s_Data->RootDomain);

		// Create Application Domain
		s_Data->AppDomain = mono_domain_create_appdomain("SparkScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		// Load Core Assembly

		s_Data->CoreAssembly = Utils::LoadCSharpAssembly("resources/scripts/script-core.dll");
		Utils::PrintAssemblyTypes(s_Data->CoreAssembly);

		MonoImage* assemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
		MonoClass* monoClass = mono_class_from_name(assemblyImage, "Spark", "Main");


		// 1. create an Object

		MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(instance);

		// 2. call function
		MonoMethod* printMessageFunc = mono_class_get_method_from_name(monoClass, "PrintMessage", 0);
		mono_runtime_invoke(printMessageFunc, instance, nullptr, nullptr);

		// 3. call function with param
		MonoMethod* printIntFunc = mono_class_get_method_from_name(monoClass, "PrintInt", 1);
		int value = 5;
		void* params[1] =
		{
			&value
		};
		mono_runtime_invoke(printIntFunc, instance, params, nullptr);

		MonoMethod* printCustomMessageFunc = mono_class_get_method_from_name(monoClass, "PrintCustomMessage", 1);
		MonoString* monoString = mono_string_new(s_Data->AppDomain, "Hello World from C++!");
		void* stringParams = monoString;
		mono_runtime_invoke(printCustomMessageFunc, instance, &stringParams, nullptr);
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
	}

	void ScriptEngine::Shutdown()
	{
		ShutdownMono();
		delete s_Data;
		s_Data = nullptr;
	}



}
