#include "skpch.h"
#include "Mesh.h"
#include "RenderCommand.h"
#include "Shader.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>
#include <gtx/quaternion.hpp>
#include <glad/glad.h>

#include "imgui.h"
#include "Material.h"
#include "RenderCommandQueue.h"
#include "Renderer.h"

namespace Spark
{
	namespace
	{
		constexpr  uint32_t s_MeshImportFlags =
			aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
			aiProcess_Triangulate |             // Make sure we're triangles
			aiProcess_SortByPType |             // Split meshes by primitive type
			aiProcess_GenNormals |              // Make sure we have legit normals
			aiProcess_GenUVCoords |             // Convert UVs if required 
			aiProcess_OptimizeMeshes |          // Batch draws where possible
			aiProcess_ValidateDataStructure;    // Validation

		 struct LogStream : public Assimp::LogStream
		{
			static void Initialize()
			{
				if (!Assimp::DefaultLogger::isNullLogger()) return;
				Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
				Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err);
			}
			void write(const char* message) override
			{
				SK_CORE_ERROR("Assimp error: {0}", message);
			}
		};

	}

	struct StaticVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec3 Binormal;
		glm::vec2 Texcoord;
	};

	struct AnimatedVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec3 Binormal;
		glm::vec2 Texcoord;
		uint32_t IDs[4] = { 0,0,0,0 };
		float Weights[4] = { 0,0,0,0 };
		void AddBoneData(uint32_t BoneID, float Weight)
		{
			for (size_t i = 0; i < 4; i++)
			{
				if (Weights[i] == 0.0)
				{
					IDs[i] = BoneID;
					Weights[i] = Weight;
					return;
				}
			}
			// TODO: Keep top weights
			SK_CORE_WARN("Vertex has more than four bones/weights affecting it, extra data will be discarded (BoneID={0}, Weight={1})", BoneID, Weight);
		}
	};
	static const int NumAttributes = 5;

	struct Index
	{
		uint32_t V1, V2, V3;
	};

	static_assert(sizeof(Index) == 3 * sizeof(uint32_t));

	namespace Utils
	{
		static glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from)
		{
			glm::mat4 to;
			//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
			to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
			to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
			to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
			to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
			return to;
		}
	}
	void Mesh::LoadAnimatedMesh()
	{
		uint32_t vertexCount = 0;
		uint32_t indexCount = 0;

		std::vector<AnimatedVertex> vertices;
		std::vector<Index> indices;

		// Submeshes
		m_Submeshes.reserve(m_Scene->mNumMeshes);
		for (int m = 0; m < m_Scene->mNumMeshes; ++m)
		{
			aiMesh* mesh = m_Scene->mMeshes[m];
			Submesh submesh;
			submesh.BaseVertex = vertexCount;
			submesh.BaseIndex = indexCount;
			submesh.MaterialIndex = mesh->mMaterialIndex;
			submesh.IndexCount = mesh->mNumFaces * 3;
			m_Submeshes.push_back(submesh);

			vertexCount += mesh->mNumVertices;
			indexCount += submesh.IndexCount;

			SK_CORE_ASSERT(mesh->HasPositions(), "Meshes require positions.");
			SK_CORE_ASSERT(mesh->HasNormals(), "Meshes require normals.");

			// Vertices
			for (size_t i = 0; i < mesh->mNumVertices; i++)
			{
				AnimatedVertex vertex;
				vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
				vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

				if (mesh->HasTangentsAndBitangents())
				{
					vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
					vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
				}

				if (mesh->HasTextureCoords(0))
					vertex.Texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
				vertices.push_back(vertex);
			}

			// Indices
			for (size_t i = 0; i < mesh->mNumFaces; i++)
			{
				SK_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
				indices.push_back({ mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2] });
			}
		}

		TraverseNodes(m_Scene->mRootNode);

		// Bones
		for (int m = 0; m < m_Scene->mNumMeshes; ++m)
		{
			aiMesh* mesh = m_Scene->mMeshes[m];
			Submesh& submesh = m_Submeshes[m];
			for (int i = 0; i < mesh->mNumBones; ++i)
			{
				aiBone* bone = mesh->mBones[i];
				std::string boneName(bone->mName.data);
				int boneIndex = 0;

				if (m_BoneMapping.find(boneName) == m_BoneMapping.end())
				{
					boneIndex = m_BoneCount;
					m_BoneCount++;
					BoneInfo bi;
					bi.BoneOffset = Utils::aiMatrix4x4ToGlm(bone->mOffsetMatrix);
					m_BoneInfo.push_back(bi);
					m_BoneMapping[boneName] = boneIndex;
				}
				else
				{
					SK_CORE_TRACE("Found existing bone in map");
					boneIndex = m_BoneMapping[boneName];
				}
				for (size_t j = 0; j < bone->mNumWeights; j++)
				{
					int VertexID = submesh.BaseVertex + bone->mWeights[j].mVertexId;
					float Weight = bone->mWeights[j].mWeight;
					vertices[VertexID].AddBoneData(boneIndex, Weight);
				}
			}
		}

		m_VertexBuffer = VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(AnimatedVertex));
		m_VertexBuffer->SetLayout({
			{ShaderDataType::Float3,"a_Position"},
			{ShaderDataType::Float3,"a_Normal"},
			{ShaderDataType::Float3,"a_Tangent"},
			{ShaderDataType::Float3,"a_Binormal"},
			{ShaderDataType::Float2,"a_Texcoord"},
			{ShaderDataType::Int4,"a_IDs"},
			{ShaderDataType::Float4,"a_Weights"},
			});
		m_IndexBuffer = IndexBuffer::Create((uint32_t*)indices.data(), indices.size() * sizeof(Index));

		m_VertexArray = VertexArray::Create();
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	void Mesh::LoadStaticMesh()
	{
		uint32_t vertexCount = 0;
		uint32_t indexCount = 0;

		std::vector<StaticVertex> vertices;
		std::vector<Index> indices;

		// Submeshes
		m_Submeshes.reserve(m_Scene->mNumMeshes);
		for (int m = 0; m < m_Scene->mNumMeshes; ++m)
		{
			aiMesh* mesh = m_Scene->mMeshes[m];
			Submesh submesh;
			submesh.BaseVertex = vertexCount;
			submesh.BaseIndex = indexCount;
			submesh.MaterialIndex = mesh->mMaterialIndex;
			submesh.IndexCount = mesh->mNumFaces * 3;
			m_Submeshes.push_back(submesh);

			vertexCount += mesh->mNumVertices;
			indexCount += submesh.IndexCount;

			SK_CORE_ASSERT(mesh->HasPositions(), "Meshes require positions.");
			SK_CORE_ASSERT(mesh->HasNormals(), "Meshes require normals.");

			// Vertices
			for (size_t i = 0; i < mesh->mNumVertices; i++)
			{
				StaticVertex vertex;
				vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
				vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

				if (mesh->HasTangentsAndBitangents())
				{
					vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
					vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
				}

				if (mesh->HasTextureCoords(0))
					vertex.Texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
				vertices.push_back(vertex);
			}

			// Indices
			for (size_t i = 0; i < mesh->mNumFaces; i++)
			{
				SK_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
				indices.push_back({ mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2] });
			}
		}

		TraverseNodes(m_Scene->mRootNode);

		m_VertexBuffer = VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(StaticVertex));
		m_VertexBuffer->SetLayout({
			{ShaderDataType::Float3,"a_Position"},
			{ShaderDataType::Float3,"a_Normal"},
			{ShaderDataType::Float3,"a_Tangent"},
			{ShaderDataType::Float3,"a_Binormal"},
			{ShaderDataType::Float2,"a_Texcoord"}
			});
		m_IndexBuffer = IndexBuffer::Create((uint32_t*)indices.data(), indices.size() * sizeof(Index));

		m_VertexArray = VertexArray::Create();
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}


	Mesh::Mesh(const Path& filename)
		:m_FilePath(filename)
	{
		LogStream::Initialize();
		SK_CORE_INFO("Loading mesh: {0}", filename.string().c_str());

		m_Importer = CreateRef<Assimp::Importer>();

		const aiScene* scene = m_Importer->ReadFile(filename.string(), s_MeshImportFlags);
		if (!scene || !scene->HasMeshes())
			SK_CORE_ERROR("Failed to load mesh file: {0}", filename);

		m_IsAnimated = scene->mAnimations != nullptr;
		std::string shaderName = "PBR_Static.glsl";
		if (m_IsAnimated) shaderName = "PBR_Anim.glsl";

		m_MeshShader = Renderer::GetShaderLibrary()->Get(shaderName);
		m_Material = Material::Create(m_MeshShader);
		m_Scene = scene;

		m_InverseTransform = glm::inverse(Utils::aiMatrix4x4ToGlm(scene->mRootNode->mTransformation));

		if (m_IsAnimated) LoadAnimatedMesh();
		else LoadStaticMesh();
	}

	Mesh::~Mesh()
	{

	}



	uint32_t Mesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		for (uint32_t i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
		{
			if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
				return i;
		}

		return 0;
	}


	uint32_t Mesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		SK_CORE_ASSERT(pNodeAnim->mNumRotationKeys > 0);

		for (uint32_t i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
		{
			if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
				return i;
		}

		return 0;
	}


	uint32_t Mesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		SK_CORE_ASSERT(pNodeAnim->mNumScalingKeys > 0);

		for (uint32_t i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
		{
			if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
				return i;
		}

		return 0;
	}

	glm::vec3 Mesh::InterpolateTranslation(float animationTime, const aiNodeAnim* nodeAnim)
	{
		if (nodeAnim->mNumPositionKeys == 1)
		{
			// No interpolation necessary for single value
			auto v = nodeAnim->mPositionKeys[0].mValue;
			return { v.x, v.y, v.z };
		}

		uint32_t PositionIndex = FindPosition(animationTime, nodeAnim);
		uint32_t NextPositionIndex = (PositionIndex + 1);
		SK_CORE_ASSERT(NextPositionIndex < nodeAnim->mNumPositionKeys);
		float DeltaTime = (float)(nodeAnim->mPositionKeys[NextPositionIndex].mTime - nodeAnim->mPositionKeys[PositionIndex].mTime);
		float Factor = (animationTime - (float)nodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
		if (Factor < 0.0f)
			Factor = 0.0f;
		SK_CORE_ASSERT(Factor <= 1.0f, "Factor must be below 1.0f");
		const aiVector3D& Start = nodeAnim->mPositionKeys[PositionIndex].mValue;
		const aiVector3D& End = nodeAnim->mPositionKeys[NextPositionIndex].mValue;
		aiVector3D Delta = End - Start;
		auto aiVec = Start + Factor * Delta;
		return { aiVec.x, aiVec.y, aiVec.z };
	}

	glm::vec3 Mesh::InterpolateScale(float animationTime, const aiNodeAnim* nodeAnim)
	{
		if (nodeAnim->mNumScalingKeys == 1)
		{
			// No interpolation necessary for single value
			auto v = nodeAnim->mScalingKeys[0].mValue;
			return { v.x, v.y, v.z };
		}

		uint32_t index = FindScaling(animationTime, nodeAnim);
		uint32_t nextIndex = (index + 1);
		SK_CORE_ASSERT(nextIndex < nodeAnim->mNumScalingKeys);
		float deltaTime = (float)(nodeAnim->mScalingKeys[nextIndex].mTime - nodeAnim->mScalingKeys[index].mTime);
		float factor = (animationTime - (float)nodeAnim->mScalingKeys[index].mTime) / deltaTime;
		if (factor < 0.0f)
			factor = 0.0f;
		SK_CORE_ASSERT(factor <= 1.0f, "Factor must be below 1.0f");
		const auto& start = nodeAnim->mScalingKeys[index].mValue;
		const auto& end = nodeAnim->mScalingKeys[nextIndex].mValue;
		auto delta = end - start;
		auto aiVec = start + factor * delta;
		return { aiVec.x, aiVec.y, aiVec.z };
	}



	glm::quat Mesh::InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim)
	{
		if (nodeAnim->mNumRotationKeys == 1)
		{
			// No interpolation necessary for single value
			auto v = nodeAnim->mRotationKeys[0].mValue;
			return glm::quat(v.w, v.x, v.y, v.z);
		}

		uint32_t RotationIndex = FindRotation(animationTime, nodeAnim);
		uint32_t NextRotationIndex = (RotationIndex + 1);
		SK_CORE_ASSERT(NextRotationIndex < nodeAnim->mNumRotationKeys);
		float DeltaTime = (float)(nodeAnim->mRotationKeys[NextRotationIndex].mTime - nodeAnim->mRotationKeys[RotationIndex].mTime);
		float Factor = (animationTime - (float)nodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
		if (Factor < 0.0f)
			Factor = 0.0f;
		SK_CORE_ASSERT(Factor <= 1.0f, "Factor must be below 1.0f");
		const aiQuaternion& StartRotationQ = nodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = nodeAnim->mRotationKeys[NextRotationIndex].mValue;
		auto q = aiQuaternion();
		aiQuaternion::Interpolate(q, StartRotationQ, EndRotationQ, Factor);
		q = q.Normalize();
		return glm::quat(q.w, q.x, q.y, q.z);
	}

	const aiNodeAnim* Mesh::FindNodeAnim(const aiAnimation* animation, const std::string& nodeName)
	{
		for (uint32_t i = 0; i < animation->mNumChannels; i++)
		{
			const aiNodeAnim* nodeAnim = animation->mChannels[i];
			if (std::string(nodeAnim->mNodeName.data) == nodeName)
				return nodeAnim;
		}
		return nullptr;
	}

	void Mesh::ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform)
	{
		std::string name(pNode->mName.data);
		const aiAnimation* animation = m_Scene->mAnimations[0];
		glm::mat4 nodeTransform(Utils::aiMatrix4x4ToGlm(pNode->mTransformation));
		const aiNodeAnim* nodeAnim = FindNodeAnim(animation, name);

		if (nodeAnim)
		{
			glm::vec3 translation = InterpolateTranslation(AnimationTime, nodeAnim);
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(translation.x, translation.y, translation.z));

			glm::quat rotation = InterpolateRotation(AnimationTime, nodeAnim);
			glm::mat4 rotationMatrix = glm::toMat4(rotation);

			glm::vec3 scale = InterpolateScale(AnimationTime, nodeAnim);
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));

			nodeTransform = translationMatrix * rotationMatrix * scaleMatrix;
		}

		glm::mat4 transform = ParentTransform * nodeTransform;

		if (m_BoneMapping.find(name) != m_BoneMapping.end())
		{
			uint32_t BoneIndex = m_BoneMapping[name];
			m_BoneInfo[BoneIndex].FinalTransformation = m_InverseTransform * transform * m_BoneInfo[BoneIndex].BoneOffset;
		}

		for (uint32_t i = 0; i < pNode->mNumChildren; i++)
			ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], transform);
	}

	void Mesh::TraverseNodes(aiNode* node)
	{
		for (int i = 0; i < node->mNumMeshes; ++i)
		{
			uint32_t mesh = node->mMeshes[i];
			m_Submeshes[mesh].Transform = Utils::aiMatrix4x4ToGlm(node->mTransformation);
		}
		for (int i = 0; i < node->mNumChildren; ++i)
		{
			aiNode* child = node->mChildren[i];
			TraverseNodes(child);
		}
	}

	void Mesh::BoneTransform(float time)
	{
		ReadNodeHierarchy(time, m_Scene->mRootNode, glm::mat4(1.0f));
		m_BoneTransforms.resize(m_BoneCount);
		for (size_t i = 0; i < m_BoneCount; i++)
			m_BoneTransforms[i] = m_BoneInfo[i].FinalTransformation;
	}
	void Mesh::Render(Timestep ts, Ref<MaterialInstance> materialInstance)
	{
		Render(ts, glm::mat4(1.0f), materialInstance);
	}

	void Mesh::Render(Timestep ts, const glm::mat4& transform, Ref<MaterialInstance> materialInstance)
	{
		if (m_IsAnimated)
		{
			if(m_AnimationPlaying)
			{
				m_WorldTime += ts;
				float ticksPerSecond = (float)(m_Scene->mAnimations[0]->mTicksPerSecond != 0 ? m_Scene->mAnimations[0]->mTicksPerSecond : 25.0f) * m_TimeMultiplier;
				m_AnimationTime += ts * ticksPerSecond;
				m_AnimationTime = fmod(m_AnimationTime, (float)m_Scene->mAnimations[0]->mDuration);
			}
			BoneTransform(m_AnimationTime);
		}

		if(materialInstance)
		{
			materialInstance->Bind();
		}

		bool materialOverride = !!materialInstance;

		m_VertexArray->Bind();

		for (Submesh& submesh : m_Submeshes)
		{

			if (m_Scene->mAnimations)
			{
				for (size_t i = 0; i < m_BoneTransforms.size(); i++)
				{
					std::string uniformName = std::string("u_BoneTransforms[") + std::to_string(i) + std::string("]");
					m_MeshShader->SetMat4(uniformName, m_BoneTransforms[i]);
				}
			}
			if(!materialOverride)
			{
				m_MeshShader->SetMat4("u_ModelMatrix", transform * submesh.Transform);
			}
			RenderCommandQueue::Submit([this,submesh]()
			{
				glDrawElementsBaseVertex(GL_TRIANGLES, submesh.IndexCount, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * submesh.BaseIndex), submesh.BaseVertex);
			});
			
		}
		m_VertexArray->Unbind();
	}

	void Mesh::OnImGuiRender()
	{
		ImGui::Begin("Mesh Debug");
		if (ImGui::CollapsingHeader(m_FilePath.string().c_str()))
		{
			if (ImGui::CollapsingHeader("Animation"))
			{
				if (ImGui::Button(m_AnimationPlaying ? "Pause" : "Play"))
					m_AnimationPlaying = !m_AnimationPlaying;

				ImGui::SliderFloat("##AnimationTime", &m_AnimationTime, 0.0f, (float)m_Scene->mAnimations[0]->mDuration);
				ImGui::DragFloat("Time Scale", &m_TimeMultiplier, 0.05f, 0.0f, 10.0f);
			}
		}

		ImGui::End();
	}

	void Mesh::DumpVertexBuffer()
	{

	}
}
