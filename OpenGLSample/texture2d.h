#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <random>
#include "stb_image.h"
#include <iostream>

using namespace std;

class Texture2D
{
public:

	//The texture
	unsigned int Texture;

	//Default Constructor: Path to file, is there an Alpha channel. Will default repeat on U and V, generates MipMaps and Flips Vertical Load
	Texture2D(const char* path, bool hasAlpha) {
		GenerateTexture(path, hasAlpha, true, true, true, true);
	}

	//Constructor: Path to file, is there an Alpha channel, repeat on U/V, Generate MipMaps, flipVerticalOnLoad
	Texture2D(const char* path, bool hasAlpha, bool repeatTextureU, bool repeatTextureV, bool genMipMaps, bool flipVerticalOnLoad) {
		GenerateTexture(path, hasAlpha, repeatTextureU, repeatTextureV, genMipMaps, flipVerticalOnLoad);
	}

private:

	int width, height, numChannels;
	unsigned char* data;

	//Generate the Texture and store in Texture
	void GenerateTexture(const char* path, bool hasAlpha, bool repeatU, bool repeatV, bool genMipMaps, bool flip) {

		//Flip y-axis during load so images arent flipped upside down
		stbi_set_flip_vertically_on_load(flip);

		//Generate and bind the texture
		glGenTextures(1, &Texture);
		glBindTexture(GL_TEXTURE_2D, Texture);

		//Set repeat/wrap settings
		if (repeatU) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		}
		if (repeatV) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		//Mip map settings
		if (genMipMaps) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		data = stbi_load(path, &width, &height, &numChannels, 0);

		//Generate texture/mipmaps if data is available
		if (data) {

			//If the type has support for alpha channel, set this to true
			if (!hasAlpha) {
				//           target         miplvl storeFormat  self explanatory   legacy, always 0  format/datatype of source  actual image data
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else {
				//           target         miplvl storeFormat  self explanatory   legacy, always 0  format/datatype of source  actual image data
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}

			glGenerateMipmap(GL_TEXTURE_2D); //Gens all required mipmaps for currently bound texture
		}
		else {
			cout << "FAILURE::LOAD::TEXTURE" << endl;
		}

		//free image memory
		stbi_image_free(data);

	}

};

#endif