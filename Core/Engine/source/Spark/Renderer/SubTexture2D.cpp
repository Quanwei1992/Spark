#include "skpch.h"
#include "Spark/Renderer/SubTexture2D.h"

namespace Spark
{
	SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max)
		:m_Texture(texture)
	{
		m_TexCoords[0] = { min.x, min.y};
		m_TexCoords[1] = { max.x, min.y};
		m_TexCoords[2] = { max.x, max.y};
		m_TexCoords[3] = { min.x, max.y};
	}


	Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize)
	{
		float sheetWidth  = (float)texture->GetWidth();
		float sheetHeight = (float)texture->GeHeight();

		glm::vec2 min = { coords.x * cellSize.x / sheetWidth ,coords.y * cellSize.y / sheetHeight };
		glm::vec2 max = { (coords.x + spriteSize.x) * cellSize.x / sheetWidth ,(coords.y+ spriteSize.y) * cellSize.y / sheetHeight };

		return CreateRef<SubTexture2D>(texture, min, max);
	}
}
