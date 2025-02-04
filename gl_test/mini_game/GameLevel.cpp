#include "GameLevel.h"
#include <fstream>
#include <sstream>

#include "ResourceManage.h"

void GameLevel::Load(const std::string& file, GLuint LvWidth, GLuint LvHeight)
{
	this->Bricks.clear();

	GLuint tileCode;
	std::string line;
	std::ifstream fstream(file);
	std::vector<std::vector<GLuint>> tileData;

	if (fstream)
	{
		while (std::getline(fstream, line))
		{
			std::istringstream sstream(line);
			std::vector<GLuint> row;
			while (sstream >> tileCode)
			{
				row.push_back(tileCode);
			}
			tileData.push_back(row);
		}

		if (tileData.size() > 0)
		{
			this->Init(tileData, LvWidth, LvHeight);
		}
	}
}

void GameLevel::Draw(SpriteRender& render)
{
	for (GameObject& tile : this->Bricks)
	{
		if (!tile.Destroyed)
		{
			tile.Draw(render);
		}
	}
}


GLboolean GameLevel::IsCompleted()
{
	for (GameObject& tile : this->Bricks)
	{
		if (!tile.IsSolid && !tile.Destroyed)
		{
			return GL_FALSE;
		}
	}
	return GL_TRUE;
}

void GameLevel::Init(std::vector<std::vector<GLuint>> tileData, GLuint LvWidth, GLuint LvHeight)
{
	GLuint h = tileData.size();
	GLuint w = tileData[0].size();
	GLfloat unit_w = LvWidth / static_cast<GLfloat>(w);
	GLfloat unit_h = LvHeight / static_cast<GLfloat>(h);

	for (GLuint y = 0; y < h; ++y)
	{
		for (GLuint x = 0; x < w; ++x)
		{
			GLuint t_type = tileData[y][x];
			if (t_type == 1)
			{
				glm::vec2 pos(unit_w * x, unit_h * y);
				glm::vec2 size(unit_w, unit_h);
				GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
				obj.IsSolid = GL_TRUE;
				this->Bricks.push_back(obj);
			} 
			else if (t_type > 1)
			{
				glm::vec3 color = glm::vec3(1.f);
				if (t_type == 2)
					color = glm::vec3(0.2f, 0.6f, 1.0f);
				else if (t_type == 3)
					color = glm::vec3(0.f, 0.7f, 0.f);
				else if (t_type == 4)
					color = glm::vec3(0.8f, 0.8f, 0.4f);
				else if (t_type == 5)
					color = glm::vec3(1.0f, 0.5f, 0.f);

				glm::vec2 pos(unit_w * x, unit_h * y);
				glm::vec2 size(unit_w, unit_h);
				this->Bricks.push_back(GameObject(pos, size, ResourceManager::GetTexture("block"), color));
			}
		}
	}
}