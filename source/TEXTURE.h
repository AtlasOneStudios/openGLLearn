//
// Created by Atlas on 8/10/2025.
//

#ifndef TEXTURE_H
#define TEXTURE_H
#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>
#include <string>

class Texture {
public:
    unsigned int ID;
    int height;
    int width;
    int channels;

    Texture(const std::string &filepath, unsigned int wraps, unsigned int wrapu, unsigned int minfilter, unsigned int magfilter);

    void use() const;
    void use(int textureUnit) const;
};

#endif //TEXTURE_H