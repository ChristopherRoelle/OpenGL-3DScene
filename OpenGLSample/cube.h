#ifndef CUBE_H
#define CUBE_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <random>
#include "stb_image.h"

using namespace std;

class Cube
{
public:
	//originOffset point will be middle of the plane
	glm::vec3 Position;

	//Dimensions (w, h, l)
	glm::vec3 Dimensions;

	//Vector of vertices
	vector<float> Vertices;

	//VAO and VBO
	unsigned int VAO, VBO;

	//Constructor
	Cube(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float length = 2.0f, float width = 2.0f, float height = 2.0f)
	{
		Position = position;

		//X-axis = width, Y-axis = height, Z-axis = length
		Dimensions.x = width;
		Dimensions.y = height;
		Dimensions.z = length;

		//Calculate the vertices
		CalculateVertices();

		//Generate the VAO/VBO
		GenerateVertexArrayAndBuffer();
	}

	//Binds the VAO associated with this object
	void BindVAO() {
		glBindVertexArray(VAO);
	}

	//Draws the object
	void Draw() {
		BindVAO();
		glDrawArrays(GL_TRIANGLES, 0, (Vertices.size() / numVertexAttributes));
	}

	//De-allocates the resources associated with the VAO/VBO
	void DeallocateVertexArrayBuffers() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

private:

	const int numVertexAttributes = 11;

	//Calculates the vertices that make up the plane
	void CalculateVertices() {
		// Determine originOffset point
		glm::vec3 originOffset;
		originOffset.x = Dimensions.x / 2; // Half the width
		originOffset.z = Dimensions.z / 2; // Half the length
		originOffset.y = Position.y + Dimensions.y;

		// Generate vertices for a rectangle (ground plane)
		glm::vec3 vertColor;

		//Normals
		glm::vec3 normals = glm::vec3(0.0);

		//vertColor = GenerateRandomVertColor();
		vertColor = glm::vec3(1.0f);

		//Bottom
		normals = glm::vec3(0.0, -1.0, 0.0); //Faces Down
		// Triangle 1		
		AddVertex(Position.x - originOffset.x, Position.y, Position.z - originOffset.z, vertColor, normals, 0.0f, 0.0f);// Bottom left
		AddVertex(Position.x - originOffset.x, Position.y, Position.z + originOffset.z, vertColor, normals, 0.0f, 1.0f);// Top left
		AddVertex(Position.x + originOffset.x, Position.y, Position.z + originOffset.z, vertColor, normals, 1.0f, 1.0f);// Top right

		// Triangle 2
		AddVertex(Position.x + originOffset.x, Position.y, Position.z + originOffset.z, vertColor, normals, 1.0f, 1.0f);// Top right
		AddVertex(Position.x + originOffset.x, Position.y, Position.z - originOffset.z, vertColor, normals, 1.0f, 0.0f);// Bottom right
		AddVertex(Position.x - originOffset.x, Position.y, Position.z - originOffset.z, vertColor, normals, 0.0f, 0.0f);// Bottom left

		//Back
		normals = glm::vec3(0.0, 0.0, 1.0); //Faces Away
		// Triangle 1 (left)	
		AddVertex(Position.x + originOffset.x, Position.y, Position.z + originOffset.z, vertColor, normals, 0.0f, 0.0f);// Bottom left
		AddVertex(Position.x + originOffset.x, originOffset.y, Position.z + originOffset.z, vertColor, normals, 0.0f, 1.0f);// Top left
		AddVertex(Position.x - originOffset.x, Position.y, Position.z + originOffset.z, vertColor, normals, 1.0f, 0.0f);// Bottom right

		// Triangle 2 (right)
		AddVertex(Position.x - originOffset.x, Position.y, Position.z + originOffset.z, vertColor, normals, 1.0f, 0.0f);// Bottom right
		AddVertex(Position.x - originOffset.x, originOffset.y, Position.z + originOffset.z, vertColor, normals, 1.0f, 1.0f);// Top right
		AddVertex(Position.x + originOffset.x, originOffset.y, Position.z + originOffset.z, vertColor, normals, 0.0f, 1.0f);// Top left

		//Right
		normals = glm::vec3(-1.0, 0.0, 0.0);
		// Triangle 1 (left)	
		AddVertex(Position.x - originOffset.x, Position.y, Position.z - originOffset.z, vertColor, normals, 0.0f, 0.0f);// Bottom left
		AddVertex(Position.x - originOffset.x, originOffset.y, Position.z - originOffset.z, vertColor, normals, 0.0f, 1.0f);// Top left
		AddVertex(Position.x - originOffset.x, Position.y, Position.z + originOffset.z, vertColor, normals, 1.0f, 0.0f);// Bottom right

		// Triangle 2 (right)
		AddVertex(Position.x - originOffset.x, Position.y, Position.z + originOffset.z, vertColor, normals, 1.0f, 0.0f);// Bottom right
		AddVertex(Position.x - originOffset.x, originOffset.y, Position.z + originOffset.z, vertColor, normals, 1.0f, 1.0f);// Top right
		AddVertex(Position.x - originOffset.x, originOffset.y, Position.z - originOffset.z, vertColor, normals, 0.0f, 1.0f);// Top left

		//Front
		normals = glm::vec3(0.0, 0.0, -1.0); //Faces towards
		// Triangle 1 (left)	
		AddVertex(Position.x + originOffset.x, Position.y, Position.z - originOffset.z, vertColor, normals, 0.0f, 0.0f);// Bottom left
		AddVertex(Position.x + originOffset.x, originOffset.y, Position.z - originOffset.z, vertColor, normals, 0.0f, 1.0f);// Top left
		AddVertex(Position.x - originOffset.x, Position.y, Position.z - originOffset.z, vertColor, normals, 1.0f, 0.0f);// Bottom right

		// Triangle 2 (right)
		AddVertex(Position.x - originOffset.x, Position.y, Position.z - originOffset.z, vertColor, normals, 1.0f, 0.0f);// Bottom right
		AddVertex(Position.x - originOffset.x, originOffset.y, Position.z - originOffset.z, vertColor, normals, 1.0f, 1.0f);// Top right
		AddVertex(Position.x + originOffset.x, originOffset.y, Position.z - originOffset.z, vertColor, normals, 0.0f, 1.0f);// Top left

		//Left
		normals = glm::vec3(1.0, 0.0, 0.0);
		// Triangle 1 (left)	
		AddVertex(Position.x + originOffset.x, Position.y, Position.z + originOffset.z, vertColor, normals, 0.0f, 0.0f);// Bottom left
		AddVertex(Position.x + originOffset.x, originOffset.y, Position.z + originOffset.z, vertColor, normals, 0.0f, 1.0f);// Top left
		AddVertex(Position.x + originOffset.x, Position.y, Position.z - originOffset.z, vertColor, normals, 1.0f, 0.0f);// Bottom right

		// Triangle 2 (right)
		AddVertex(Position.x + originOffset.x, Position.y, Position.z - originOffset.z, vertColor, normals, 1.0f, 0.0f);// Bottom right
		AddVertex(Position.x + originOffset.x, originOffset.y, Position.z - originOffset.z, vertColor, normals, 1.0f, 1.0f);// Top right
		AddVertex(Position.x + originOffset.x, originOffset.y, Position.z + originOffset.z, vertColor, normals, 0.0f, 1.0f);// Top left

		//Top
		normals = glm::vec3(0.0, 1.0, 0.0); //Faces up
		// Triangle 1		
		AddVertex(Position.x - originOffset.x, originOffset.y, Position.z - originOffset.z, vertColor, normals, 0.0f, 0.0f);// Bottom left
		AddVertex(Position.x - originOffset.x, originOffset.y, Position.z + originOffset.z, vertColor, normals, 0.0f, 1.0f);// Top left
		AddVertex(Position.x + originOffset.x, originOffset.y, Position.z + originOffset.z, vertColor, normals, 1.0f, 1.0f);// Top right

		// Triangle 2
		AddVertex(Position.x + originOffset.x, originOffset.y, Position.z + originOffset.z, vertColor, normals, 1.0f, 1.0f);// Top right
		AddVertex(Position.x + originOffset.x, originOffset.y, Position.z - originOffset.z, vertColor, normals, 1.0f, 0.0f);// Bottom right
		AddVertex(Position.x - originOffset.x, originOffset.y, Position.z - originOffset.z, vertColor, normals, 0.0f, 0.0f);// Bottom left
	}

	//Helper function to add the vertices
	void AddVertex(float x, float y, float z, const glm::vec3& color, const glm::vec3& normals, float u, float v) {
		Vertices.push_back(x);
		Vertices.push_back(y);
		Vertices.push_back(z);
		Vertices.push_back(color.r);
		Vertices.push_back(color.g);
		Vertices.push_back(color.b);
		Vertices.push_back(normals.x);
		Vertices.push_back(normals.y);
		Vertices.push_back(normals.z);
		Vertices.push_back(u);
		Vertices.push_back(v);
	}

	//Generates the VAO and VBO for the object
	void GenerateVertexArrayAndBuffer() {

		//Gen the vertex array
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		//Gen and bind the buffer
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(float), &Vertices[0], GL_STATIC_DRAW);

		//Configure the Buffer Attributes

		//Position (x, y, z)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, numVertexAttributes * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute (r, g, b)
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, numVertexAttributes * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// normals attribute (x, y, z)
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, numVertexAttributes * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		// texture attribute (U, V)
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, numVertexAttributes * sizeof(float), (void*)(9 * sizeof(float)));
		glEnableVertexAttribArray(3);
		
	}

	//Generates a random color for the object's vertices
	glm::vec3 GenerateRandomVertColor() {
		//default colors for now, going to randomize since no shading.
		random_device rd;
		mt19937 gen(rd());
		uniform_real_distribution<float> dis(0.0f, 1.0f);
		glm::vec3 vertColor;
		vertColor.r = dis(gen);
		vertColor.g = dis(gen);
		vertColor.b = dis(gen);

		return vertColor;
	}
};

#endif