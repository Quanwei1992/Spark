#pragma once
#include <glm.hpp>
#include "VertexArray.h"

namespace Spark
{
	class Mesh
	{
	public:

		Mesh(const Path& filename);
		~Mesh();

		void Render();

		inline const Path& GetFilePath() const { return m_FilePath; }

	private:
		Path m_FilePath;
		Ref<VertexArray> m_VertexArray;
	};
}
