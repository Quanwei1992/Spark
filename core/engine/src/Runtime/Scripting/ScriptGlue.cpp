#include "skpch.h"
#include "ScriptGlue.h"

#include <mono/metadata/loader.h>
#include <mono/metadata/object.h>

namespace Spark
{

#define SK_ADD_INTERNAL_CALL(NAME) mono_add_internal_call("Spark.InternalCalls::"#NAME, NAME)

	static void NativeLog(MonoString* text, int params)
	{
		char* cStr = mono_string_to_utf8(text);
		std::string str(cStr);
		mono_free(cStr);

		std::cout << str << ", " << params << std::endl;
	}

	static void NativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)
	{
		SK_CORE_INFO("Value: {0}", *parameter);
		*outResult = *parameter * 5.0f;
	}

	void ScriptGlue::RegisterFunctions()
	{

		SK_ADD_INTERNAL_CALL(NativeLog);
		SK_ADD_INTERNAL_CALL(NativeLog_Vector);
	}



}
