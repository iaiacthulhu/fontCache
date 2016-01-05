//myGLFunctions.h
#ifndef MY_GL_FUNCTIONS_H
#define MY_GL_FUNCTIONS_H


int createShaderv(GLuint programID, GLenum type, const char * source);
int putProgramInfoLog(GLuint programID);
int fcreateShader(GLuint programID, GLenum type, const char * filename);
GLuint loadDDS(const char * filename);

#endif
