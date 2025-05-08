#pragma once

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>


struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};


GLuint setup_texture(const char* filename)
{
	// Enable textures
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	GLuint texObject;
	glGenTextures(1, &texObject);
	glBindTexture(GL_TEXTURE_2D, texObject);

	// Handle how we want the image to fit the screen.
	// GL_Repeat repeats the image
	// S T and R correspond to the x, y and z axis respectively.
	// Can mix and match depending on what you want to happen on each axis.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Linear for blurrier smoother image
	// Nearest for pixelated image
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	int w, h, chan;
	// Load image at filename, x pixels wide and y pixels high
	stbi_set_flip_vertically_on_load(true);
	unsigned char* pxls = stbi_load(filename, &w, &h, &chan, 0);

	// Check pxls colour data ia loaded correctly
	if (pxls) {
		// Decide which image format to use.
		GLenum format;
		if (chan == 1) format = GL_RED;
		else if (chan == 3) format = GL_RGB;
		else if (chan == 4) format = GL_RGBA;
		else {
			std::cerr << "Unsupported number of channels in texture: " << filename << std::endl;
			stbi_image_free(pxls);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
			return 0; 
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, pxls);
		// Free the image data after uploading
		stbi_image_free(pxls); 
	}
	else {
		std::cerr << "Failed to load texture: " << filename << std::endl;
	}
	glGenerateMipmap(GL_TEXTURE_2D);
	
	printf("Successfully loaded texture %s.\n", filename);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	return texObject;
}

GLuint setup_texture_pbr(const char* filename, bool is_srgb)
{
	// Enable textures
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	GLuint texObject;
	glGenTextures(1, &texObject);
	glBindTexture(GL_TEXTURE_2D, texObject);

	// Handle how we want the image to fit the screen.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Use mipmapping for better quality
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	int w, h, chan;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* pxls = stbi_load(filename, &w, &h, &chan, 0);

	if (pxls) {
		// Decide which image format to use.
		GLenum format;
		GLenum internalFormat;

		if (chan == 1) {
			format = GL_RED;
			internalFormat = GL_R8;
		}
		else if (chan == 3) {
			format = GL_RGB;
			// Use sRGB for color textures 
			internalFormat = is_srgb ? GL_SRGB8 : GL_RGB8;
		}
		else if (chan == 4) {
			format = GL_RGBA;
			internalFormat = is_srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
		}
		else {
			std::cerr << "Unsupported number of channels in texture: " << filename << std::endl;
			stbi_image_free(pxls);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
			return 0;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, pxls);
		stbi_image_free(pxls);

		// Generate mipmaps for better quality
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cerr << "Failed to load texture: " << filename << std::endl;
	}
	printf("Successfully loaded PBR texture %s.\n", filename);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	return texObject;
}

GLuint setup_mipmaps(const char* filename[], int n)
{
	// Enable textures
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	GLuint texObject;
	glGenTextures(1, &texObject);
	glBindTexture(GL_TEXTURE_2D, texObject);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	int w[16], h[16], chan[16];
	unsigned char* pxls[16];
	// Load image at filename, x pixels wide and y pixels high
	stbi_set_flip_vertically_on_load(true);
	for (int c = 0; c < n; c++) {
		pxls[c] = stbi_load(filename[c], &w[c], &h[c], &chan[c], 0);
		if (pxls[c]) {
			GLenum format = (chan[c] == 4) ? GL_RGBA : GL_RGB;
			glTexImage2D(GL_TEXTURE_2D, c, format, w[c], h[c], 0, format, GL_UNSIGNED_BYTE, pxls[c]);
		}

		stbi_image_free(pxls[c]);
	}

	// Check pxls colour data ia loaded correctly
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	printf("Successfully loaded mipmap texture %s to %s.\n", filename[0], filename[n-1]);
	return texObject;
}

GLuint setup_cubemap(std::vector<std::string> faces)
{
	// Set up a texture with a cubemap
	if (faces.size() != 6) {
		std::cerr << "Error: Cubemap requires exactly 6 faces." << std::endl;
		return 0;
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			GLenum format = GL_RGB;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 4)
				format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cerr << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	// Configure texture properties
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Generate mipmaps for the cubemap texture
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	printf("Successfully applied cubemap.\n");

	return textureID;
}
