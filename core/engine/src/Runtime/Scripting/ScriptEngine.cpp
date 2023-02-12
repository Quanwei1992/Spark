#include "skpch.h"
#include "Spark/Scripting/ScriptEngine.h"
#include "Spark/Scene/Entity.h"
#include "ScriptGlue.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include <fstream>

#include "Spark/Scene/Components.h"

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

		Ref<ScriptClass> EntityClass;

		std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
		std::unordered_map<UUID, Ref<ScriptInstance>> EntityInstances;
		Scene* SceneContext = nullptr;
	};

	static ScriptEngineData* s_Data = nullptr;

	void ScriptEngine::LoadAssembly(const std::filesystem::path& filepath)
	{
		s_Data->AppDomain = mono_domain_create_appdomain("SparkScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
	}

	const std::unordered_map<std::string, Ref<ScriptClass>>& ScriptEngine::GetEntityClasses()
	{
		return s_Data->EntityClasses;
	}

	bool ScriptEngine::ClassExists(const std::string& name)
	{
		return s_Data->EntityClasses.find(name) != s_Data->EntityClasses.end();
	}

	void ScriptEngine::OnRuntimeStart(Scene* scene)
	{
		s_Data->SceneContext = scene;
	}

	void ScriptEngine::OnRuntimeStop()
	{
		s_Data->EntityInstances.clear();
	}

	void ScriptEngine::OnCreateEntity(Entity e)
	{
		const auto& sc = e.GetComponent<ScriptComponent>();
		if(!ClassExists(sc.ClassName))
		{
			return;
		}
		auto instance = CreateRef<ScriptInstance>(s_Data->EntityClasses[sc.ClassName],e);
		s_Data->EntityInstances[e.GetUUID()] = instance;
		instance->InvokeOnCreate();
	}

	void ScriptEngine::OnUpdateEntity(Entity entity, float ts)
	{
		UUID entityUUID = entity.GetUUID();
		SK_CORE_ASSERT(s_Data->EntityInstances.find(entity.GetUUID()) != s_Data->EntityInstances.end());
		auto instance = s_Data->EntityInstances[entityUUID];
		instance->InvokeOnUpdate(ts);
	}

	Scene* ScriptEngine::GetSceneContext()
	{
		return s_Data->SceneContext;
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

	void ScriptEngine::LoadAssemblyClasses(MonoAssembly* assembly, MonoImage* image)
	{
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* entityClass = mono_class_from_name(image, "Spark", "Entity");
		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
			std::string fullName;
			if(strlen(nameSpace) != 0)
			{
				fullName = fmt::format("{}.{}", nameSpace, name);
			}else
			{
				fullName = name;
			}

			MonoClass* monoClass = mono_class_from_name(image,nameSpace, name);
			
			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
			if(isEntity && monoClass != entityClass)
			{
				s_Data->EntityClasses[fullName] = CreateRef<ScriptClass>(nameSpace, name);
			}


			SK_CORE_INFO("{0}.{1}", nameSpace, name);
		}
	}

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
		:m_ClassNamespace(classNamespace)
		, m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(s_Data->CoreAssemblyImage, classNamespace.c_str(), className.c_str());
	}

	MonoObject* ScriptClass::Instantiate() const
	{
		MonoObject* instance = mono_object_new(s_Data->AppDomain, m_MonoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount) const
	{
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params) const
	{
		return mono_runtime_invoke(method, instance, params, nullptr);
	}


	ScriptInstance::ScriptInstance(Spark::Ref<ScriptClass> scriptClass,Entity entity)
		:m_ScriptClass(scriptClass)
	{
		m_Instance = scriptClass->Instantiate();
		m_ConstructorMethod = s_Data->EntityClass->GetMethod(".ctor", 1);
		m_OnCreateMethod = scriptClass->GetMethod("OnCreate", 0);
		m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);

		uint32_t entityID = entity;
		void* params = &entityID;
		m_ScriptClass->InvokeMethod(m_Instance, m_ConstructorMethod, &params);

	}

	void ScriptInstance::InvokeOnCreate()
	{
		m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod, nullptr);
	}

	void ScriptInstance::InvokeOnUpdate(float ts)
	{
		void* params = &ts;
		m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &params);
	}

	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();
		InitMono();
		LoadAssembly("resources/scripts/script-core.dll");
		LoadAssemblyClasses(s_Data->CoreAssembly, s_Data->CoreAssemblyImage);

		s_Data->EntityClass = CreateRef<ScriptClass>("Spark", "Entity");

		ScriptGlue::RegisterFunctions();
#if 0

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
#endif
	}

	void ScriptEngine::Shutdown()
	{
		ShutdownMono();
		delete s_Data;
		s_Data = nullptr;
	}

}
