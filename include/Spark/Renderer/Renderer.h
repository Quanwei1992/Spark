#pragma once
#include "RendererAPI.h"
#include "OrthographicCamera.h"

namespace Spark
{
	class Shader;
	class Renderer
	{
	public:

		static void Init();
		static void OnWindowResized(uint32_t width, uint32_t height);

		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader,
			const Ref<VertexArray>& vertexArray,
			const glm::mat4& transform = glm::mat4(1.0f));


		static inline RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};
		static SceneData* m_SceneData;

	};
}