#include <stdio.h>
#include <string.h>
#include "./include/common.hpp"

size_t ustrlen(const char32_t * arg)
{
    size_t i = 0;
    for(; arg[i] != 0; i++);
    return i;
}

char32_t * utf8ToUtf32(const char * stra)
{
    const unsigned char * str = (const unsigned char *)stra;
    size_t textLen = strlen(stra) +1;
    char32_t * utf32Text = new char32_t[textLen];
    for (unsigned int i = 0; i < textLen; i++) utf32Text[i] = 0;
    size_t utf32Count = 0;
    for ( unsigned int i = 0; i < textLen;)
    {
        size_t byteCount;
        byteCount = 0;
        if      ( str[i] >= 0xfe ) { fprintf(stderr, "%s :error: 不正なUTF-8シーケンス。\n", __func__); i++; continue; }
        else if ( str[i] >= 0xfc && str[i] <= 0xfd ) byteCount = 6;
        else if ( str[i] >= 0xf8 && str[i] <= 0xfb ) byteCount = 5;
        else if ( str[i] >= 0xf0 && str[i] <= 0xf7 ) byteCount = 4;
        else if ( str[i] >= 0xe0 && str[i] <= 0xef ) byteCount = 3;
        else if ( str[i] >= 0xc2 && str[i] <= 0xdf ) byteCount = 2;
        else if ( str[i] >= 0x80 && str[i] <= 0xc1 ) { fprintf(stderr, "%s :error: 不正なUTF-8シーケンス。\n", __func__); i++; continue; }
        else if ( str[i] >= 0x00 && str[i] <= 0x7F ) byteCount = 1;
        else { fprintf(stderr, "%s :error: 不正なUTF-8シーケンス。\n", __func__); i++; continue; }
        utf32Text[utf32Count] = ((const unsigned char)(str[i] << byteCount) >> byteCount) << (6 * (byteCount-1));
        i++;
        for (unsigned int j = 1; j < byteCount; j++)
        {
            utf32Text[utf32Count] += (str[i] & ~0x80) << (6 * (byteCount-j-1));
            i++;
        }
        utf32Count++;
    }
    return utf32Text;
}
