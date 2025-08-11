//
// Created by Atlas on 8/10/2025.
//
#define STB_IMAGE_IMPLEMENTATION
#include "TEXTURE.h"

Texture::Texture(const std::string &filepath, unsigned int wraps, unsigned int wrapu, unsigned int minfilter, unsigned int magfilter) {
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wraps);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapu);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minfilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilter);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

void Texture::use() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ID);
}
void Texture::use(int textureUnit) const {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, ID);
}
