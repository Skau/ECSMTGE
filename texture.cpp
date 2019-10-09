#include "innpch.h"
#include <QImage>
#include <QBuffer>
#include <QByteArray>
#include <cstring>

#include "texture.h"

Texture::Texture(GLuint textureUnit) : QOpenGLFunctions_4_1_Core()
{
    initializeOpenGLFunctions();
    //small dummy texture
    for (int i=0; i<16; i++)
        pixels[i] = 0;
    pixels[0] = 255;
    pixels[4] = 255;
    pixels[8] = 255;
    pixels[9] = 255;
    pixels[10] = 255;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    mType = GL_TEXTURE_2D;

    glGenTextures(1, &mId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mId);
    qDebug() << "Texture::Texture() id = " << mId;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 reinterpret_cast<const GLvoid*>(pixels));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    setTexture(textureUnit);
}

Texture::Texture(const std::string &filename, GLenum type, GLuint textureUnit)
{
    initializeOpenGLFunctions();

    mType = type;

    readBitmap(filename);

    switch (type)
    {
        case GL_TEXTURE_1D:
            // Idk how to do that yet
            break;
        case GL_TEXTURE_2D:
            setTexture(textureUnit);
            break;
        case GL_TEXTURE_CUBE_MAP:
            initCubeMap(textureUnit);
            break;
    }
}

/**
    \brief Texture::id() Return the id of a previously generated texture object
    \return The id of a previously generated texture object
 */
GLuint Texture::id() const
{
    return mId;
}

void Texture::readBitmap(const std::string &filename)
{
    OBITMAPFILEHEADER bmFileHeader;
    OBITMAPINFOHEADER bmInfoHeader;

    std::string fileWithPath =  gsl::assetFilePath + "Textures/" + filename;

    std::ifstream file;
    file.open (fileWithPath.c_str(), std::ifstream::in | std::ifstream::binary);
    if (file.is_open())
    {
        file.read((char *) &bmFileHeader, 14);

        file.read((char *) &bmInfoHeader, sizeof(OBITMAPINFOHEADER));
        mColumns = bmInfoHeader.biWidth;
        mRows = bmInfoHeader.biHeight;
        mnByte = bmInfoHeader.biBitCount / 8;

        mBitmap = new unsigned char[mColumns * mRows * mnByte];
        file.read((char *) mBitmap, mColumns * mRows * mnByte);
        file.close();
    }
    else
    {
        qDebug() << "Can not read " << QString(fileWithPath.c_str());
    }
    unsigned char tmp;
    // switch red and blue
    for (int k = 0; k < mColumns * mRows * mnByte; k += mnByte) {
        tmp = mBitmap[k];
        mBitmap[k] = mBitmap[k + 2];
        mBitmap[k + 2] = tmp;
    }
    qDebug() << "Texture read: " << QString(fileWithPath.c_str());
}

void Texture::setTexture(GLuint textureUnit)
{
    glGenTextures(1, &mId);
    // activate the texture unit first before binding texture
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, mId);
    qDebug() << "Texture::Texture() id = " << mId;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGB,
                mColumns,
                mRows,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                mBitmap);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::initCubeMap(GLuint textureUnit)
{
    glGenTextures(1, &mId);
    // activate the texture unit first before binding texture
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mId);
    qDebug() << "Texture::Texture(Cube_Map_Texture) id = " << mId;
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


    const gsl::ivec2 skyboxSampleOrder[]
    {
        // Right, left, top, bottom, back, front
        {0, 1}, {2, 1}, {1, 0}, {1, 2}, {3, 1}, {1, 1}
    };

    // Cubemap file should be 4 in width and 3 in height
    if (mRows < 1 || mColumns < 1 || mnByte < 1)
        return;
    unsigned int faceWidth{static_cast<unsigned int>(mColumns / 4)},
    faceHeight{static_cast<unsigned int>(mRows / 3)},
    byteSize{static_cast<unsigned int>(mnByte)};

    // Create a buffer to copy subimage to.
    unsigned char *buffer = new unsigned char[faceWidth * faceHeight * byteSize]{};

    for (unsigned int i{0}; i < 6; ++i)
    {
        auto coords = skyboxSampleOrder[i];

        // Copy each line of the subimage into the buffer
        for (unsigned int j{0}; j < faceHeight; ++j)
        {
            std::memcpy(
                    (buffer + j * faceWidth * byteSize),
                    (mBitmap // Start
                     + j * static_cast<unsigned int>(mColumns) * byteSize // entire line offset
                     + static_cast<unsigned int>(coords.x) * faceWidth * byteSize // x offset
                     + static_cast<unsigned int>(coords.y) * faceHeight * static_cast<unsigned int>(mColumns) * byteSize), // y offset
                    faceWidth * byteSize
                );
        }

        // Copy subimage from buffer to texture
        glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0,
                    GL_RGB,
                    static_cast<int>(faceWidth),
                    static_cast<int>(faceHeight),
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    buffer);
    }

    // glGenerateMipmap(GL_TEXTURE_2D);
}
