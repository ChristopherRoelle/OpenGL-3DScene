#ifndef PYRAMID_H
#define PYRAMID_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <random>
#include "stb_image.h"

#include <iostream>

using namespace std;

class Pyramid
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
	Pyramid(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float length = 2.0f, float width = 2.0f, float height = 0.5f)
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

	void CalculateVertices() {
		// Determine originOffset point
		glm::vec3 originOffset;
		originOffset.x = Dimensions.x / 2; // Half the width
		originOffset.z = Dimensions.z / 2; // Half the length
		originOffset.y = Dimensions.y / 2; // Half the height

		// Generate vertices for a rectangle (ground plane)
		glm::vec3 vertColor;
		//vertColor = GenerateRandomVertColor();
		vertColor = glm::vec3(1.0f);

		float height = Dimensions.y;

		// Base Triangle (CCW)
		glm::vec3 normals = glm::vec3(0.0f, -1.0f, 0.0f);

		//Triangle 1
		glm::vec3 vert1 = glm::vec3(Position.x - originOffset.x, Position.y, Position.z - originOffset.z);
		glm::vec3 vert2 = glm::vec3(Position.x + originOffset.x, Position.y, Position.z - originOffset.z);
		glm::vec3 vert3 = glm::vec3(Position.x - originOffset.x, Position.y, Position.z + originOffset.z);
		AddVertex(vert1, vertColor, normals, 0.0f, 0.0f); // Bottom left
		AddVertex(vert2, vertColor, normals, 1.0f, 0.0f); // Bottom right
		AddVertex(vert3, vertColor, normals, 0.0f, 1.0f); // Top left

		// Tri 2
		vert1 = glm::vec3(Position.x + originOffset.x, Position.y, Position.z - originOffset.z);
		vert2 = glm::vec3(Position.x + originOffset.x, Position.y, Position.z + originOffset.z);
		vert3 = glm::vec3(Position.x - originOffset.x, Position.y, Position.z + originOffset.z);
		AddVertex(vert1, vertColor, normals, 1.0f, 0.0f); // Bottom right
		AddVertex(vert2, vertColor, normals, 1.0f, 1.0f); // Top right
		AddVertex(vert3, vertColor, normals, 0.0f, 1.0f); // Top left

		// Create the sides
		// Right Triangle

		//Calculate the normals... This doesnt seem to work 100%, but may be due to the shader itself and the slope of the side.
		vert1 = glm::vec3(Position.x + originOffset.x, Position.y, Position.z + originOffset.z);
		vert2 = glm::vec3(Position.x, Position.y + height, Position.z);
		vert3 = glm::vec3(Position.x + originOffset.x, Position.y, Position.z - originOffset.z);

		glm::vec3 edge1 = vert3 - vert1;
		glm::vec3 edge2 = vert2 - vert1;

		normals = glm::normalize(glm::cross(edge1, edge2));
		//std::cout << "RIGHT::" << normals.x << " " << normals.y << " " << normals.z << std::endl;
		
		AddVertex(vert1, vertColor, normals, 0.0f, 0.0f); // Right-back
		AddVertex(vert2, vertColor, normals, 0.5f, 0.5f); // Tip
		AddVertex(vert3, vertColor, normals, 1.0f, 0.0f); // Top left

		
		// Back Triangle
		vert1 = glm::vec3(Position.x - originOffset.x, Position.y, Position.z - originOffset.z);
		//vert2 = glm::vec3(Position.x, Position.y + height, Position.z); //Always the tip, so commenting out.
		vert3 = glm::vec3(Position.x + originOffset.x, Position.y, Position.z - originOffset.z);

		edge1 = vert3 - vert1;
		edge2 = vert2 - vert1;

		normals = glm::normalize(glm::cross(edge2, edge1));
		//std::cout << "BACK::" << normals.x << " " << normals.y << " " << normals.z << std::endl;

		AddVertex(vert1, vertColor, normals, 0.0f, 0.0f); // Right-back
		AddVertex(vert2, vertColor, normals, 0.5f, 0.5f); // Tip
		AddVertex(vert3, vertColor, normals, 1.0f, 0.0f); // Top left
		
		
		// Left Triangle
		vert1 = glm::vec3(Position.x - originOffset.x, Position.y, Position.z - originOffset.z);
		//vert2 = glm::vec3(Position.x, Position.y + height, Position.z); //Always the tip, so commenting out.
		vert3 = glm::vec3(Position.x - originOffset.x, Position.y, Position.z + originOffset.z);

		edge1 = vert3 - vert1;
		edge2 = vert2 - vert1;

		normals = glm::normalize(glm::cross(edge1, edge2));
		//std::cout << "LEFT::" << normals.x << " " << normals.y << " " << normals.z << std::endl;

		AddVertex(vert1, vertColor, normals, 0.0f, 0.0f); // Right-back
		AddVertex(vert2, vertColor, normals, 0.5f, 0.5f); // Tip
		AddVertex(vert3, vertColor, normals, 1.0f, 0.0f); // Top left


		// Front Triangle
		vert1 = glm::vec3(Position.x + originOffset.x, Position.y, Position.z + originOffset.z);
		//vert2 = glm::vec3(Position.x, Position.y + height, Position.z); //Always the tip, so commenting out.
		vert3 = glm::vec3(Position.x - originOffset.x, Position.y, Position.z + originOffset.z);

		edge1 = vert3 - vert1;
		edge2 = vert2 - vert1;

		normals = glm::normalize(glm::cross(edge2, edge1));
		//std::cout << "FRONT::" << normals.x << " " << normals.y << " " << normals.z << std::endl;

		AddVertex(vert1, vertColor, normals, 0.0f, 0.0f); // Right-back
		AddVertex(vert2, vertColor, normals, 0.5f, 0.5f); // Tip
		AddVertex(vert3, vertColor, normals, 1.0f, 0.0f); // Top left
		
	}

	//Helper function to add the vertices
	void AddVertex(const glm::vec3& pos, const glm::vec3& color, const glm::vec3& normals, float u, float v) {
		Vertices.push_back(pos.x);
		Vertices.push_back(pos.y);
		Vertices.push_back(pos.z);
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