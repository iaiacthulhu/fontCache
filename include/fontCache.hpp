#ifndef FONTCACHE_H
#define FONTCACHE_H

#include <iostream>
#include <map>
#include <GL/glew.h>

#include <ft2build.h>
#ifdef __clang__
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wdeprecated-register"
#endif
#include FT_FREETYPE_H
#ifdef __clang__
  #pragma clang diagnostic pop
#endif

#include "./common/linear_algebra.hpp"

#define IMAGE_SIZE 1024
#define IMAGE_DATA_BUFFER_SIZE IMAGE_SIZE*IMAGE_SIZE
#define SPRITE_PIXEL 64

class fontCache
{
    private:
    fontCache& operator=(const fontCache& r){return *this;}
    fontCache(const fontCache& r){}
    fontCache(const char *);
    ~fontCache();

    public:
    class fontData_
    {
        public:
        vec2 UVul;
        vec2 UVlr;
        unsigned int bitmap_left;
        unsigned int bitmap_top;
        unsigned int width;
        unsigned int height;
        unsigned int advanceX;
        fontData_();
    };

    private:
    static fontCache *              pInstance;

    std::map<char32_t, fontData_>   font;
    unsigned int                    fontCount;
    float                           currentU;
    float                           currentV;
    unsigned int                    currentX;
    unsigned int                    currentY;
    unsigned char                   imageData[IMAGE_DATA_BUFFER_SIZE];

    FT_Library                      ftlib;
    FT_Face                         ftface;

    GLuint programID;
    GLuint vaoText;
    GLuint vboText;
    GLuint texture;
    static const char vShader[];
    static const char fShader[];
    unsigned int textLength;

    public:
    fontData_& operator[](char32_t);
    void bufferData(const char32_t *);
    void writeImage(void);
    void draw(void);

    static void set(const char *);
    static fontCache * getInstance();
};

#endif
