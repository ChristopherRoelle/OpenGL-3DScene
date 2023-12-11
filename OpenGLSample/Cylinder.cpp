#include "Cylinder.h"
#include <iostream>

using namespace std;

//constants
const int MIN_SECTOR_CNT = 3; //Cannot have a cylinder with less than 3 sides
const int MIN_STACK_CNT = 1;  //If less than 1, we have a circle (no longer 3D)

//Constructor
Cylinder::Cylinder(float baseRadius, float topRadius, float height, int sectors,
	int stacks, bool smooth, int up) : interleavedStride(32) {
	Set(baseRadius, topRadius, height, sectors, stacks, smooth, up);
}

//Setters
void Cylinder::Set(float baseRadius, float topRadius, float height, int sectors,
	int stacks, bool smooth, int up) {

	//Need a radius > 0
	if (baseRadius > 0) {
		this->baseRadius = baseRadius;
	}
	if (topRadius > 0) {
		this->topRadius = topRadius;
	}

	//Need a height greater than 0 (otherwise we are 2D)
	if (height > 0) {
		this->height = height;
	}

	//Determine Sector count
	this->sectorCount = sectors;
	if (sectors < MIN_SECTOR_CNT) {
		this->sectorCount = MIN_SECTOR_CNT;
	}

	//Determine Stack Count
	this->stackCount = stacks;
	if (stacks < MIN_STACK_CNT) {
		this->stackCount = MIN_STACK_CNT;
	}

	this->smooth = smooth;

	//Determine Up Axis
	this->upAxis = up;
	if (up < 1 || up > 3) {
		this->upAxis = 3;
	}
}

void Cylinder::SetBaseRadius(float radius) {

	//Only make the change if the value changed
	if (this->baseRadius != radius) {
		Set(radius, topRadius, height, sectorCount, stackCount, smooth, upAxis);
	}
}

void Cylinder::SetTopRadius(float radius) {

	//Only make the change if the value changed
	if (this->topRadius != radius) {
		Set(baseRadius, radius, height, sectorCount, stackCount, smooth, upAxis);
	}
}

void Cylinder::SetHeight(float height) {

	//Only make the change if the value changed
	if (this->height != height) {
		Set(baseRadius, topRadius, height, sectorCount, stackCount, smooth, upAxis);
	}
}

void Cylinder::SetSectorCount(int sectors) {

	//Only make the change if the value changed
	if (this->sectorCount != sectors) {
		Set(baseRadius, topRadius, height, sectors, stackCount, smooth, upAxis);
	}
}

void Cylinder::SetStackCount(int stacks) {

	//Only make the change if the value changed
	if (this->stackCount != stacks) {
		Set(baseRadius, topRadius, height, sectorCount, stacks, smooth, upAxis);
	}
}

void Cylinder::SetSmooth(bool smooth) {

	//Only change if smoothing is different
	if (this->smooth == smooth) {
		return;
	}

	this->smooth = smooth;
	if (smooth) {
		BuildVerticesSmooth();
	}
	else {
		BuildVerticesFlat();
	}
}

void Cylinder::SetUpAxis(int up) {
	if (this->upAxis == up || up < 1 || up > 3)
		return;

	ChangeUpAxis(this->upAxis, up);
	this->upAxis = up;
}

//Debug
void Cylinder::Print() const {
	cout << "Cylinder\n"
		 << "Base Radius: " << baseRadius << "\n"
		 << "Top Radius: " << topRadius << "\n"
		 << "Height: " << height << "\n"
		 << "Sector Cnt: " << sectorCount << "\n"
		 << "Stack Cnt: " << stackCount << "\n"
		 << "Smoothing: " << (smooth ? "TRUE" : "FALSE") << "\n"
		 << "Up Axis: " << (upAxis == 1 ? "X" : (upAxis == 2 ? "Y" : "Z")) << "\n"
		 << "Tris Cnt: " << GetTrisCount() << "\n"
		 << "Index Cnt: " << GetIndexCount() << "\n"
		 << "Vertex Cnt: " << GetVertexCount() << "\n"
		 << "Normal Cnt: " << GetNormalCount() << "\n"
		 << "TexCoord Cnt: " << GetTexCoordCount() << "\n" << endl;
}

void Cylinder::Draw() const {
	//interleaved Array
	glEnableClientState(GL_VERTEX_ARRAY);
}