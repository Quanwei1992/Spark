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
	class Texture2D;
	class MaterialInstance;
	class Material;
	class Shader;
	struct Submesh
	{
		uint32_t BaseVertex;
		uint32_t BaseIndex;
		uint32_t MaterialIndex;
		uint32_t IndexCount;
		glm::mat4 Transform;
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

		void OnUpdate(Timestep ts);

		inline Ref<Shader> GetMeshShader() const { return m_MeshShader; }
		inline Ref<Material> GetMaterial() const { return m_BaseMaterial; }
		std::vector<Ref<MaterialInstance>>& GetMaterials() { return m_Materials; }
		const std::vector<Ref<Texture2D>>& GetTextures() const { return m_Textures; }
		inline const Path& GetFilePath() const { return m_FilePath; }

	private:
		void BoneTransform(float time);
		void ReadNodeHierarchy(float animationTime, const aiNode* pNode, const glm::mat4& parentTransform);
		void TraverseNodes(aiNode* node, const glm::mat4& parentTransform=glm::mat4(1.0f), uint32_t level = 0);

		const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const std::string& nodeName);
		uint32_t FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint32_t FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint32_t FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
		glm::vec3 InterpolateTranslation(float animationTime, const aiNodeAnim* nodeAnim);
		glm::quat InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim);
		glm::vec3 InterpolateScale(float animationTime, const aiNodeAnim* nodeAnim);

		void LoadAnimatedMesh();
		void LoadStaticMesh();
		void LoadMaterials();

		friend class Renderer;
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

		// Materials
		Ref<Shader> m_MeshShader;
		Ref<Material> m_BaseMaterial;
		std::vector<Ref<Texture2D>> m_Textures;
		std::vector<Ref<Texture2D>> m_NormalMaps;
		std::vector<Ref<MaterialInstance>> m_Materials;

		// Animation
		bool m_IsAnimated = false;
		float m_AnimationTime = 0.0f;
		float m_WorldTime = 0.0f;
		float m_TimeMultiplier = 1.0f;
		bool m_AnimationPlaying = true;
	};
}
