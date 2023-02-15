#include "skpch.h"
#include "Mesh.h"
#include "RenderCommand.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

namespace Spark
{
	namespace
	{
		constexpr  uint32_t ImportFlags =
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_SortByPType |
			aiProcess_PreTransformVertices |
			aiProcess_GenNormals |
			aiProcess_GenUVCoords |
			aiProcess_OptimizeMeshes |
			aiProcess_Debone |
			aiProcess_ValidateDataStructure;

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

	struct MeshVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec3 Binormal;
		glm::vec2 Texcoord;
	};
	static_assert(sizeof(MeshVertex) == 14 * sizeof(float));

	Mesh::Mesh(const Path& filename)
		:m_FilePath(filename)
	{
		LogStream::Initialize();

		SK_CORE_TRACE("Loading mesh: {0}", filename);

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(filename.string(), ImportFlags);
		if (!scene || !scene->HasMeshes())
			SK_CORE_ERROR("Failed to load mesh file {0}", filename);

		aiMesh* mesh = scene->mMeshes[0];

		SK_CORE_ASSERT(mesh->HasPositions(), "Meshes require positions.");
		SK_CORE_ASSERT(mesh->HasNormals(), "Meshes require normals.");


		MeshVertex* vertices = new MeshVertex[mesh->mNumVertices];

		// Extract vertices from model
		for (int i = 0; i < mesh->mNumVertices; ++i)
		{
			MeshVertex& vertex = vertices[i];
			vertex.Position = { mesh->mVertices[i].x,mesh->mVertices[i].y, mesh->mVertices[i].z };
			vertex.Normal = { mesh->mNormals[i].x,mesh->mNormals[i].y, mesh->mNormals[i].z };
			if(mesh->HasTangentsAndBitangents())
			{
				vertex.Tangent = { mesh->mTangents[i].x,mesh->mTangents[i].y, mesh->mTangents[i].z };
				vertex.Binormal = { mesh->mBitangents[i].x,mesh->mBitangents[i].y, mesh->mBitangents[i].z };
			}

			if (mesh->HasTextureCoords(0))
				vertex.Texcoord = { mesh->mTextureCoords[0][i].x,mesh->mTextureCoords[0][i].y };
		}

		auto vertexBuffer = VertexBuffer::Create(vertices, mesh->mNumVertices * sizeof(MeshVertex));
		vertexBuffer->SetLayout({
			{ShaderDataType::Float3,"a_Position"},
			{ShaderDataType::Float3,"a_Normal"},
			{ShaderDataType::Float3,"a_Tangent"},
			{ShaderDataType::Float3,"a_Binormal"},
			{ShaderDataType::Float2,"a_Texcoord"}
		});

		// Extract indices from model
		auto* indices = new uint32_t[mesh->mNumFaces * 3];
		for (int i = 0; i < mesh->mNumFaces; ++i)
		{
			SK_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
			indices[i * 3] = mesh->mFaces[i].mIndices[1];
			indices[i * 3 + 1] = mesh->mFaces[i].mIndices[2];
			indices[i * 3 + 2] = mesh->mFaces[i].mIndices[3];
		}

		auto indexBuffer = IndexBuffer::Create(indices, mesh->mNumFaces * 3);

		m_VertexArray = VertexArray::Create();
		m_VertexArray->AddVertexBuffer(vertexBuffer);
		m_VertexArray->SetIndexBuffer(indexBuffer);

		delete[] vertices;
		delete[] indices;
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::Render()
	{
		RenderCommand::DrawIndexed(m_VertexArray, m_VertexArray->GetIndexBuffer()->GetCount());
	}
}
