//GameLevel.cpp
#include "GameLevel.h"
#include "ResourceManager.h"

#include <fstream>
#include <sstream>
#include <ostream>
#include <iterator>

void GameLevel::Load(const GLchar *file, GLuint levelWidth, GLuint levelHeight)
{
	//clear old data
	this->Bricks.clear();

	//load from file
	GLuint tileCode;
	GameLevel level;
	std::string line;
	std::vector<std::vector<GLuint>> tileData;

	std::vector<char> data = ResourceManager::LoadFile(file);

	if (!data.empty())
	{
    	std::stringstream ioss;
        copy(data.begin(), data.end(), std::ostream_iterator<char>(ioss));
		while (std::getline(ioss, line)) //read each line form level file
		{
			std::istringstream sstream(line);
			std::vector<GLuint> row;

			while (sstream >> tileCode)  //read each word seperated by spaces
				row.push_back(tileCode);
			tileData.push_back(row);
		}

		if (tileData.size() > 0)
			this->init(tileData, levelWidth, levelHeight);
	}
}

void GameLevel::Draw(SpriteRenderer &renderer)
{
	for (GLuint i = 0; i < this->Bricks.size(); i++)
	{
		GameObject &tile = this->Bricks[i];
		// rest of the code
		if (!tile.Destroyed)
			tile.Draw(renderer);
	}
}

GLboolean GameLevel::IsCompleted()
{
	for (GLuint i = 0; i < this->Bricks.size(); i++)
	{
		GameObject &tile = this->Bricks[i];
		if (!tile.IsSolid && !tile.Destroyed)
			return GL_FALSE;
	}
	return GL_TRUE;
}

void GameLevel::init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight)
{
	//calculate dimensions
	GLuint height = tileData.size();
	//now we can index vector at [0] since this function is only called if height > 0
	GLuint width = tileData[0].size();
	GLuint unit_width = levelWidth / static_cast<GLfloat>(width);
	GLuint unit_height = levelHeight / height;

	//Initialise levels based on tileData
	for (GLuint y = 0; y < height; ++y)
	{
		for (GLuint x = 0; x < width; ++x)
		{
			//check block type from level data (2D level array)
			if (tileData[y][x] == 1)   //solid
			{
				glm::vec2 pos(unit_width * x, unit_height * y);
				glm::vec2 size(unit_width, unit_height);
				GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
				obj.IsSolid = GL_TRUE;
				this->Bricks.push_back(obj);
			}
			else if (tileData[y][x] > 1) //Non-solid; now determine its colour based on level data
			{
				glm::vec3 colour = glm::vec3(1.0f); //original: white
				if (tileData[y][x] == 2)
					colour = glm::vec3(0.2f, 0.6f, 1.0f);
				else if (tileData[y][x] == 3)
					colour = glm::vec3(0.0f, 0.7f, 0.0f);
				else if (tileData[y][x] == 4)
					colour = glm::vec3(0.8f, 0.8f, 0.4f);
				else if (tileData[y][x] == 5)
					colour = glm::vec3(1.0f, 0.5f, 0.0f);

				glm::vec2 pos(unit_width * x, unit_height * y);
				glm::vec2 size(unit_width, unit_height);
				this->Bricks.push_back(GameObject(pos, size, ResourceManager::GetTexture("block"), colour));
			}
		}
	}
}
