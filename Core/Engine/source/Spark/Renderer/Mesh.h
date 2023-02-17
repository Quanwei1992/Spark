#pragma once
#include <glm.hpp>
#include "VertexArray.h"
#include "Spark/Core/Timestep.h"

namespace Assimp
{
	class Importer;
}

struct aiNode;
struct aiAnimation;
struct aiNodeAnim;
struct aiScene;

namespace Spark
{
	class Shader;
	struct Submesh
	{
		uint32_t BaseVertex;
		uint32_t BaseIndex;
		uint32_t MaterialIndex;
		uint32_t IndexCount;
	};

	struct BoneInfo
	{
		glm::mat4 BoneOffset;
		glm::mat4 FinalTransformation;
	};

	class Mesh
	{
	public:

		Mesh(const Path& filename);
		~Mesh();

		void Render(Timestep ts,Ref<Shader>& shader);
		void OnImGuiRender();
		void DumpVertexBuffer();

		inline const Path& GetFilePath() const { return m_FilePath; }
	private:
		void BoneTransform(float time);
		void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);

		const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const std::string& nodeName);
		uint32_t FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint32_t FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint32_t FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
		glm::vec3 InterpolateTranslation(float animationTime, const aiNodeAnim* nodeAnim);
		glm::quat InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim);
		glm::vec3 InterpolateScale(float animationTime, const aiNodeAnim* nodeAnim);
	private:
		Path m_FilePath;
		std::vector<Submesh> m_Submeshes;

		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
		Ref<VertexArray> m_VertexArray;

		std::unordered_map<std::string, uint32_t> m_BoneMapping;
		std::vector<glm::mat4> m_BoneTransforms;
		uint32_t m_BoneCount = 0;
		std::vector<BoneInfo> m_BoneInfo;
		glm::mat4 m_InverseTransform;

		const aiScene* m_Scene;
		Ref<Assimp::Importer> m_Importer;
		// Animation
		float m_AnimationTime = 0.0f;
		float m_WorldTime = 0.0f;
		float m_TimeMultiplier = 1.0f;
		bool m_AnimationPlaying = true;
	};
}