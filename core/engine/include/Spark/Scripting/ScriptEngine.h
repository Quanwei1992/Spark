#pragma once
#include <filesystem>
#include <mono/metadata/object-forward.h>
#include <mono/metadata/object.h>


extern "C"
{
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
}

namespace Spark
{
	class Entity;
	class Scene;
	class ScriptClass
	{
	public:
		ScriptClass(const std::string& classNamespace, const std::string& className);

		MonoObject* Instantiate() const;

		MonoMethod* GetMethod(const std::string& name, int parameterCount) const;

		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params) const;

	private:
		std::string m_ClassNamespace;
		std::string m_ClassName;
		MonoClass* m_MonoClass = nullptr;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(Spark::Ref<ScriptClass> scriptClass,Entity entity);
		void InvokeOnCreate();
		void InvokeOnUpdate(float ts);
	private:
		Ref<ScriptClass> m_ScriptClass;
		MonoObject* m_Instance = nullptr;
		MonoMethod* m_ConstructorMethod = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;
	};



	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();
		static void LoadAssembly(const std::filesystem::path& filepath);
		static const std::unordered_map<std::string, Ref<ScriptClass>>& GetEntityClasses();
		static bool ClassExists(const std::string& name);
		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();

		static void OnCreateEntity(Entity entity);
		static void OnUpdateEntity(Entity entity,float ts);

		static Scene* GetSceneContext();

	private:
		static void InitMono();
		static void ShutdownMono();

		static void LoadAssemblyClasses(MonoAssembly* assembly, MonoImage* image);
	};
}
