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

#include "include/common/myGLFunctions.h"
#include "include/common/linear_algebra.hpp"
#include "include/common/common.hpp"
#include "include/fontcache.hpp"
int err = 0;
const int MAX_GL_CHARACTERS = 255;

fontCache * fontCache::pInstance = NULL;

const char fontCache::vShader[] = {
"#version 330 core\n\
layout(location = 0) in vec2 UV;\n\
layout(location = 1) in vec2 vert;\n\
layout(location = 2) in vec4 color;\n\
out vec2 UV_;\n\
out vec4 _color;\n\
mat4 mater = mat4(\n\
    0.125,   0.0,     0.0,    0.0,\n\
    0.0,     0.125,   0.0,    0.0,\n\
    0.0,     0.0,     1.0,    0.0,\n\
   -0.9,   -0.6,    0.0,    1.0\n\
    );\n\
void main()\n\
{\n\
    gl_Position = mater * vec4(vert, 0.0, 1.0);\n\
    UV_ = UV;\n\
    _color = color;\n\
}"
};

const char fontCache::fShader[] = {
"#version 330 core\n\
in vec2 UV_;\n\
in vec4 _color;\n\
uniform sampler2D textureSamplerText;\n\
out vec4 o_color;\n\
void main(void)\n\
{\n\
    o_color.a =  (texture2D(textureSamplerText, UV_).r) * _color.a ;\n\
    o_color.rgb = vec3(0.0, 1.0, 0.0);\n\
}\n\
"
};

fontCache::fontData_::fontData_() : UVul(), UVlr(), bitmap_left(), bitmap_top(), width(), height(), advanceX()
{}

void fontCache::writeImage(void)
{
    FILE * wfp = NULL;
    if ((wfp = fopen("hogehoge.data","wb")) != NULL)
    {
        if (fwrite(imageData, sizeof(unsigned char), IMAGE_DATA_BUFFER_SIZE, wfp) < IMAGE_DATA_BUFFER_SIZE) fprintf(stderr, "nankoka kakenakatta\n");
        fclose(wfp);
    }
    else fprintf(stderr, "kakikomierror\n");
}

void fontCache::bufferData(const char32_t * utf32string)
{
    vec2 upperleft = {};
    vec4 color = {};
    color.a = 0.5;

    size_t length = ustrlen(utf32string);
    if (length > MAX_GL_CHARACTERS) length = MAX_GL_CHARACTERS;

    this->textLength = length;
    GLfloat * buff = new GLfloat[length * 32];
    for (unsigned int i = 0; utf32string[i] != 0; i++)
    {//看板一枚
        if (utf32string[i] == U'\n')
        {
            upperleft.y += -1.0;
            upperleft.x = 0.0;
            continue;
        }
        double tempx = 0.0;
        double tempy = 0.0;
        if ( this->operator[](utf32string[i]).bitmap_left != 0 ) tempx = (1.0/(SPRITE_PIXEL/(double)this->operator[](utf32string[i]).bitmap_left));
        if ( this->operator[](utf32string[i]).bitmap_top != 0 ) tempy = (1.0/(SPRITE_PIXEL/(double)this->operator[](utf32string[i]).bitmap_top));
        double tempw = 0.0;
        double temph = 0.0;
        if ( this->operator[](utf32string[i]).width != 0 ) tempw = (1.0/(SPRITE_PIXEL/(double)this->operator[](utf32string[i]).width));
        if ( this->operator[](utf32string[i]).height != 0 ) temph = (1.0/(SPRITE_PIXEL/(double)this->operator[](utf32string[i]).height));
        buff[i*32+0]  = this->operator[](utf32string[i]).UVul.u;
        buff[i*32+1]  = this->operator[](utf32string[i]).UVul.v;
        buff[i*32+2]  = upperleft.x + tempx;
        buff[i*32+3]  = upperleft.y + tempy;
        buff[i*32+4]  = color.r;
        buff[i*32+5]  = color.g;
        buff[i*32+6]  = color.b;
        buff[i*32+7]  = color.a;
        buff[i*32+8]  = this->operator[](utf32string[i]).UVlr.u;
        buff[i*32+9]  = this->operator[](utf32string[i]).UVul.v;
        buff[i*32+10] = upperleft.x + tempx + tempw;
        buff[i*32+11] = upperleft.y + tempy;
        buff[i*32+12] = color.r;
        buff[i*32+13] = color.g;
        buff[i*32+14] = color.b;
        buff[i*32+15] = color.a;
        buff[i*32+16] = this->operator[](utf32string[i]).UVul.u;
        buff[i*32+17] = this->operator[](utf32string[i]).UVlr.v;
        buff[i*32+18] = upperleft.x + tempx;
        buff[i*32+19] = upperleft.y + tempy - temph;
        buff[i*32+20] = color.r;
        buff[i*32+21] = color.g;
        buff[i*32+22] = color.b;
        buff[i*32+23] = color.a;
        buff[i*32+24] = this->operator[](utf32string[i]).UVlr.u;
        buff[i*32+25] = this->operator[](utf32string[i]).UVlr.v;
        buff[i*32+26] = upperleft.x + tempx + tempw;
        buff[i*32+27] = upperleft.y + tempy - temph;
        buff[i*32+28] = color.r;
        buff[i*32+29] = color.g;
        buff[i*32+30] = color.b;
        buff[i*32+31] = color.a;

        if ( this->operator[](utf32string[i]).advanceX != 0 )
            upperleft.x += 1.0 / (64 / (double)this->operator[](utf32string[i]).advanceX); //XXX
    }
    if ( (err = glGetError() ) != GL_NO_ERROR ) printf("__glbufferr %d\n",err);//XXX
    glBindBuffer(GL_ARRAY_BUFFER, this->vboText);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*length*32, buff);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    delete[] buff;

    if ( (err = glGetError() ) != GL_NO_ERROR ) printf("glbufferr %d\n",err);//XXX
}

void fontCache::draw(void)
{
    static int cnt;
    static int wait;
    if (wait < 5)
    {
        wait++;
    }
    else if (cnt < this->textLength)
    {
        cnt++;
        wait = 0;
    }
    if (this->textLength == 0) return;

    if (glIsEnabled(GL_DEPTH_TEST) == GL_TRUE ) glDisable(GL_DEPTH_TEST);
    if (glIsEnabled(GL_CULL_FACE) == GL_TRUE ) glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//XXX
    glUseProgram(this->programID);
    glUniform1i(glGetUniformLocation(this->programID, "textureSamplerText"), 0);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glBindVertexArray(this->vaoText);
    for (unsigned int i = 0; i < cnt; i++)
    {
        glDrawArrays(GL_TRIANGLE_STRIP, 4*i, 4);
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    if ((err = glGetError()) != GL_NO_ERROR) printf("gldrawerr %d\n",err);//XXX
}

fontCache::fontData_& fontCache::operator[](char32_t u)
{
    if ( font.find(u) == font.end() )
    {
    puts("debug2");
        if ( FT_Error error = FT_Load_Glyph(this->ftface, FT_Get_Char_Index(this->ftface, u), 0) ) printf("%d", error);
        if ( FT_Error error = FT_Render_Glyph(this->ftface->glyph, FT_RENDER_MODE_NORMAL) ) printf("%d", error);

        auto fontalloc = [&]()
        {
            this->font[u].UVul.u        = this->currentU;
            this->font[u].UVul.v        = this->currentV;
 //           if ( this->ftface->glyph->bitmap.width == 0 )
 //               this->font[u].UVlr.u = this->currentU;
 //           else this->font[u].UVlr.u        = this->currentU + 1.0 / (IMAGE_SIZE / this->ftface->glyph->bitmap.width);
            if ( (this->currentX == 0) && (this->ftface->glyph->bitmap.width == 0) )
                this->font[u].UVlr.u = 0.0f;
            else
                this->font[u].UVlr.u        = (float)(1.0/((double)IMAGE_SIZE / (double)(this->currentX + this->ftface->glyph->bitmap.width)));

            if ( this->ftface->glyph->bitmap.rows == 0 )
                this->font[u].UVlr.v = this->currentV;
            else
                this->font[u].UVlr.v        = this->currentV + 1.0 / ((double)IMAGE_SIZE / (double)this->ftface->glyph->bitmap.rows);
            this->font[u].bitmap_left = this->ftface->glyph->bitmap_left ;
            this->font[u].bitmap_top  = this->ftface->glyph->bitmap_top ;
            this->font[u].width       = this->ftface->glyph->bitmap.width ;
            this->font[u].height      = this->ftface->glyph->bitmap.rows ;
            this->font[u].advanceX    = this->ftface->glyph->advance.x >> 6 ;

            if ( this->ftface->glyph->bitmap.width != 0 )
            {
                glBindTexture(GL_TEXTURE_2D, this->texture);
                glTexSubImage2D(GL_TEXTURE_2D, 0, this->currentX, this->currentY, this->ftface->glyph->bitmap.width, this->ftface->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, this->ftface->glyph->bitmap.buffer);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            this->currentX += this->ftface->glyph->bitmap.width;
            this->currentU = this->font[u].UVlr.u;
            this->fontCount++;
        };

        if (this->currentX + this->ftface->glyph->bitmap.width > IMAGE_SIZE)
        {
            if (this->currentY + SPRITE_PIXEL > IMAGE_SIZE) throw("sprite sheet mou onaka ippai\n");//error manpai
            else
            {
                this->currentU = 0.0; this->currentX = 0;
                this->currentV += 1.0 / ((double)IMAGE_SIZE/(double)SPRITE_PIXEL); this->currentY += SPRITE_PIXEL; //高さは固定
                fontalloc();
            }
        }
        else
            fontalloc();
        return font[u];
    }
    else return font[u];
}

fontCache::fontCache(const char * fontname) : fontCount(), currentU(), currentV(), currentX(), currentY(), imageData(), ftlib(), ftface(), programID(), vaoText(), vboText(), texture(), textLength()
{
    if ( FT_Error error = FT_Init_FreeType(&(this->ftlib)) ) printf("布都%d", error);
    if ( FT_Error error = FT_New_Face(this->ftlib, fontname, 0, &this->ftface) ) printf("布都%d", error);
    if ( FT_Error error = FT_Set_Char_Size(this->ftface, 0, 64*64, 300, 300) ) printf("布都%d", error);
    if ( FT_Error error = FT_Set_Pixel_Sizes(this->ftface, 0, 64) ) printf("布都%d", error);

    this->programID = glCreateProgram();
    if (createShaderv(this->programID, GL_VERTEX_SHADER, this->vShader))
        puts("createShaderv vs");
    if (createShaderv(this->programID, GL_FRAGMENT_SHADER, this->fShader))
        puts("createShaderv fs");
    glLinkProgram(this->programID);
    if (putProgramInfoLog(this->programID))
        puts("putProgramInfoLog");

    glGenVertexArrays(1, &(this->vaoText));
    glBindVertexArray(this->vaoText);

    glGenBuffers(1, &(this->vboText));
    glBindBuffer(GL_ARRAY_BUFFER, this->vboText);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*8*4*MAX_GL_CHARACTERS, NULL, GL_DYNAMIC_DRAW);// 32は看板一個あたりの要素数

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLubyte *)0);//UV
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLubyte *)8);//XY
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLubyte *)16);//COL

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, IMAGE_SIZE, IMAGE_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, this->imageData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glUseProgram(this->programID);
//    glUniform1i(glGetUniformLocation(this->programID, "textureSamplerText"), 0);

    glBindVertexArray(0);

}

fontCache::~fontCache()
{
    glUseProgram(0);
    glDeleteBuffers(1, &this->vboText);
    glDeleteTextures(1, &this->texture);
    glDeleteVertexArrays(1, &this->vaoText);
    glDeleteProgram(this->programID);

    FT_Done_Face ( this->ftface );
    FT_Done_FreeType( this->ftlib );
}

void fontCache::set(const char * arg)
{
    if (fontCache::pInstance)
    {
        // 生成に失敗した場合に元に戻せると良さそう。TODO
        delete fontCache::pInstance;
        fontCache::pInstance = new fontCache(arg);
    }
    else
        fontCache::pInstance = new fontCache(arg);
}

fontCache * fontCache::getInstance()
{
    if (fontCache::pInstance == NULL)
        throw ("fontCache: error: no instance");
    else
        return fontCache::pInstance;
}
