#pragma once
#include "Spark/Scene/Entity.h"
#include "Spark/Scene/Scene.h"

namespace Spark
{
	class Texture2D;

	class SceneRenderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void SetViewportSize(uint32_t width, uint32_t height);

		static void BeginScene(Scene* scene, Camera camera);
		static void EndScene();

		static void SubmitEntity(Entity entity);

		static Environment CreateEnvironmentMap(const std::string& filepath);

		static Ref<Texture2D> GetFinalColorBuffer();
		static uint32_t GetFinalColorBufferRendererID();
	private:
		static void FlushDrawList();
		static void GeometryPass();
		static void CompositePass();
	};
}
