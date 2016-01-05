#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifndef ERROR
  #define ERROR -1
#endif
#ifndef FALSE
  #define FALSE 0
#endif
#ifndef TRUE
  #define TRUE 1
#endif
#ifndef FOURCC_DXT1
  #define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#endif
#ifndef FOURCC_DXT3
  #define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#endif
#ifndef FOURCC_DXT5
  #define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII
#endif
#include "myGLFunctions.h"


int createShaderv(GLuint programID, GLenum type, const char * source)
{
    GLuint  shaderID;
    GLint   errorStatus;
    
    shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, (const GLchar**)&source, NULL);
    glCompileShader(shaderID);
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &errorStatus);
    
    if (errorStatus == GL_FALSE)
    {
        GLchar* buffer;
        GLint   logLength;
        GLsizei readLogLength;
        
        fprintf(stderr, "シェーダのコンパイルに失敗しました。\n");
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
        buffer = (GLchar *) malloc(logLength);
            if (buffer == NULL)
            {
                fprintf(stderr, "動的メモリの確保に失敗しました。\n");
                return ERROR;
            }
            glGetShaderInfoLog(shaderID, logLength, &readLogLength, buffer);
            fprintf(stderr, "%s", (char *)buffer);
        free(buffer);
        return ERROR;
    }
    else
    {
        glAttachShader(programID, shaderID);
        glDeleteShader(shaderID);
        return 0;
    }
}

int putProgramInfoLog(GLuint programID)
{
    GLint errorStatus;
    glGetProgramiv(programID, GL_LINK_STATUS, &errorStatus);
    if (errorStatus == GL_FALSE)
    {
        GLchar* buffer;
        GLint   logLength;
        GLsizei readLogLength;
        
        fprintf(stderr, "シェーダのリンクに失敗しました。\n");
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
        buffer = (GLchar *) malloc(logLength);
            if (buffer == NULL)
            {
                fprintf(stderr, "動的メモリの確保に失敗しました。\n");
                return ERROR;
            }
            glGetProgramInfoLog(programID, logLength, &readLogLength, buffer);
            fprintf(stderr, "%s", (char *)buffer);
        free(buffer);
        return ERROR;
    }
    else
    {
        return 0;
    }
}

int fcreateShader(GLuint programID, GLenum type, const char * filename)
{
    FILE *  fp;
    int     filesize;
    GLchar* buffer;
    GLuint  shaderID;
    GLint   errorStatus;

fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "ファイルのオープンに失敗しました。\n");
        return ERROR;
    }
    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
buffer = (GLchar *) malloc(filesize);
    if (buffer == NULL)
    {
        fprintf(stderr, "動的メモリの確保に失敗しました。\n");
        return ERROR;
    }
    fseek(fp, 0, SEEK_SET);
    fread(buffer, 1, filesize, fp);
fclose(fp);
    
    shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, (const GLchar**)&buffer, &filesize);
free(buffer);
    glCompileShader(shaderID);
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &errorStatus);
    if (errorStatus == GL_FALSE)
    {
        GLint   logLength;
        GLsizei readLogLength;
        
        fprintf(stderr, "シェーダのコンパイルに失敗しました。\n");
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
buffer = (GLchar *) malloc(logLength);
        if (buffer == NULL)
        {
            fprintf(stderr, "動的メモリの確保に失敗しました。\n");
            return ERROR;
        }
        glGetShaderInfoLog(shaderID, logLength, &readLogLength, buffer);
        fprintf(stderr, "%s", (char *)buffer);
free(buffer);
        return ERROR;
    }
    else
    {
        glAttachShader(programID, shaderID);
        glDeleteShader(shaderID);
        return 0;
    }
}

GLuint loadDDS(const char * filename)
{
    FILE * fp;
    unsigned char   header[124];
    char            filecode[4];
    
    unsigned int    height;
    unsigned int    width;
    unsigned int    linearSize;
    unsigned int    mipMapCount;
    unsigned int    fourCC;
    
    unsigned char*  buffer;
    unsigned int    bufferSize;
#ifndef __clang__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif
    unsigned int    components;//謎の宣言
    unsigned int    format;
    
fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "loadDDS: DDSファイルの読み込みに失敗しました。\n");
        return 0;
    }
    
    fread(filecode, 1, 4, fp);
    if (strncmp(filecode, "DDS ", 4) != 0)
    {
        fprintf(stderr, "loadDDS: DDSファイルではありません。\n");
        fclose(fp);
        return 0;
    }
    fread(header, 124, 1, fp);
    
    height      = *(unsigned int*)&(header[8 ]);
    width       = *(unsigned int*)&(header[12]);
    linearSize  = *(unsigned int*)&(header[16]);
    mipMapCount = *(unsigned int*)&(header[24]);
    fourCC      = *(unsigned int*)&(header[80]);
    
    bufferSize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    
buffer = (unsigned char *)malloc(bufferSize * sizeof(unsigned char));
        if (buffer == NULL)
        {
            fprintf(stderr, "loadDDS: ヒープ領域の確保に失敗しました。\n");
            fclose(fp);
            return 0;
        }
        fread(buffer, 1, bufferSize, fp);
fclose(fp);
    
    components = (fourCC == FOURCC_DXT1) ? 3 : 4; //色の要素数か？

    switch (fourCC)
    {
        case FOURCC_DXT1:
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        break;
        
        case FOURCC_DXT3:
        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
        
        case FOURCC_DXT5:
        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
        
        default:
free(buffer);//下にもう一個
        return 0;
    }
    //ここからテクスチャの読み込み
    GLuint textureID;// TEXTURE
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    /* ミップマップをロードします。 */
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
    {
        unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer + offset);
                                //定数         ++     定数    /2     /2   ゼロ固定の仕様  w*h       &buffer[0] + wh ,wh/4 ,wh/4/4
        offset += size;
        width  /= 2;
        height /= 2;
    }
free(buffer); 

    return textureID;
    
}
#ifndef __clang__
  #pragma GCC diagnostic pop
#endif
