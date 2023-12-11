#ifndef SPHERE_H
#define SPHERE_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <random>

//define PI
#define M_PI 3.1415926535897932384626433832795

using namespace std;

class Sphere
{
public:
	//originOffset point will be middle of the plane
	glm::vec3 Position;

	float RadiusLong;
	float RadiusLat;

	int SideCount;
	int SubDivisions;
	bool SemiCircle;

	//Vector of vertices
	vector<float> Vertices;

	//VAO and VBO
	unsigned int VAO, VBO;

	//Constructor - Uniform Sphere.
	Sphere(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float radius = 1.0f, int sides = 8, bool semiCircle = false)
	{
		Position = position;

		//X-axis = raidus, Y-axis = height
		RadiusLong = radius;
		RadiusLat = radius;

		SemiCircle = semiCircle;

		SideCount = sides;
		SubDivisions = sides;

		//Calculate the vertices
		CalculateVertices();

		//Generate the VAO/VBO
		GenerateVertexArrayAndBuffer();
	}

	//Constructor - Allows for differing longitude and latitude radius
	Sphere(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float radiusLong = 1.0f, float radiusLat = 1.0f, int sides = 8, bool semiCircle = false)
	{
		Position = position;

		//X-axis = raidus, Y-axis = height
		RadiusLong = radiusLong;
		RadiusLat = radiusLat;

		SemiCircle = semiCircle;

		SideCount = sides;
		SubDivisions = sides;

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
		// Generate vertices for a Sphere
		glm::vec3 vertColor = glm::vec3(1.0f);
		float u = 1.0f / static_cast<float>(SideCount - 1);
		float v = 1.0f / static_cast<float>(SubDivisions);

		glm::vec3 normals;

		//Limit the sides generated based on if its a semi circle or not. Doing on latitude (N/S) because my application has a semicircle facing downward.
		int latitudeLimit = (SemiCircle)?SideCount / 2:SideCount;

		//Latitude Loop (N/S)
		for (int i = 0; i < latitudeLimit; i++) {
			float phi1 = static_cast<float>(M_PI * i) / (SideCount - 1);
			float phi2 = static_cast<float>(M_PI * (i + 1)) / (SideCount - 1);

			// Longitude Loop (E/W) where the actual faces are drawn.
			for (int j = 0; j < SubDivisions; j++) {
				float theta1 = static_cast<float>(2.0f * M_PI * j) / SubDivisions;
				float theta2 = static_cast<float>(2.0f * M_PI * (j + 1)) / SubDivisions;

				// Vertex 1
				glm::vec3 vert1 = CalculateSphereVertex(Position, RadiusLong, RadiusLat, phi1, theta1);

				// Vertex 2
				glm::vec3 vert2 = CalculateSphereVertex(Position, RadiusLong, RadiusLat, phi1, theta2);

				// Vertex 3
				glm::vec3 vert3 = CalculateSphereVertex(Position, RadiusLong, RadiusLat, phi2, theta1);

				// Vertex 4
				glm::vec3 vert4 = CalculateSphereVertex(Position, RadiusLong, RadiusLat, phi2, theta2);

				glm::vec3 edge1 = vert3 - vert1;
				glm::vec3 edge2 = vert2 - vert1;
				normals = glm::normalize(glm::cross(edge1, edge2));

				// Right triangle
				AddVertex(vert1.x, vert1.y, vert1.z, vertColor, normals, 1.0f - (u * j), v * i);
				AddVertex(vert2.x, vert2.y, vert2.z, vertColor, normals, 1.0f - (u * (j + 1)), v * i);
				AddVertex(vert3.x, vert3.y, vert3.z, vertColor, normals, 1.0f - (u * j), v * (i + 1));

				edge1 = vert2 - vert4;
				edge2 = vert3 - vert4;
				normals = glm::normalize(glm::cross(edge1, edge2));

				// Left triangle
				AddVertex(vert2.x, vert2.y, vert2.z, vertColor, normals, 1.0f - (u * (j + 1)), v * i);
				AddVertex(vert4.x, vert4.y, vert4.z, vertColor, normals, 1.0f - (u * (j + 1)), v * (i + 1));
				AddVertex(vert3.x, vert3.y, vert3.z, vertColor, normals, 1.0f - (u * j), v * (i + 1));
			}
		}

		// If this is a semi-circle, we need to cap it. Full sphere doesnt require a cap.
		if (SemiCircle) {
			normals = glm::vec3(0.0f, -1.0f, 0.0);

			for (int i = 0; i <= SideCount; ++i) {

				float curTheta = (float)(2.0f * M_PI * i) / SideCount;
				float nxtTheta = (float)(2.0f * M_PI * (i + 1)) / SideCount;

				float curX = Position.x + RadiusLong * cos(curTheta);
				float curZ = Position.z + RadiusLong * sin(curTheta);

				float nxtX = Position.x + RadiusLong * cos(nxtTheta);
				float nxtZ = Position.z + RadiusLong * sin(nxtTheta);

				float textureCurThetaU = 1.0f * cos(curTheta);
				float textureCurThetaV = 1.0f * sin(curTheta);

				float textureNxtThetaU = 1.0f * cos(nxtTheta);
				float textureNxtThetaV = 1.0f * sin(nxtTheta);

				// Triangle connecting the center, current, and next vertices
				AddVertex(Position.x, Position.y, Position.z, vertColor, normals, 0.0f, 0.0f); //Center
				AddVertex(curX, Position.y, curZ, vertColor, normals, textureCurThetaU, textureCurThetaV);
				AddVertex(nxtX, Position.y, nxtZ, vertColor, normals, textureNxtThetaU, textureNxtThetaV);

			}
		}
	}

	//Helper function to calculate the vertices for the sphere.
	glm::vec3 CalculateSphereVertex(const glm::vec3& position, float radiusLong, float radiusLat, float phi, float theta) {
		float x = position.x + radiusLong * sin(phi) * cos(theta);
		float y = position.y + radiusLat * cos(phi);
		float z = position.z + radiusLong * sin(phi) * sin(theta);
		return glm::vec3(x, y, z);
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