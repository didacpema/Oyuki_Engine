#include "Texture.h"

void TextureData::bind() const {
    glBindTexture(GL_TEXTURE_2D, id);
}

void TextureData::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}