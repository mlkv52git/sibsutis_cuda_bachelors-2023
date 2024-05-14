#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "global.h"

void csGrid2Tex(GLuint* inBuf,int N);
void csDataInit(GLuint* inBuf,int N);
void csMove(GLuint* inBuf,int N);

GLuint bufferID[NUM_OF_BUFFERS];

void genBuffers(){
  glGenBuffers(NUM_OF_BUFFERS, bufferID);

  
  glBindBuffer( GL_ARRAY_BUFFER, bufferID[POSITIONS]);
  glBufferData( GL_ARRAY_BUFFER, 6*num_of_verticies*sizeof(float), 
                0, GL_DYNAMIC_DRAW );

  glBindBuffer( GL_ARRAY_BUFFER, bufferID[VELOCITIES]);
  glBufferData( GL_ARRAY_BUFFER, 3*num_of_verticies*sizeof(float),
                0, GL_DYNAMIC_DRAW );  
  
  
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, bufferID[TEXTURE]);  
  glBufferData(GL_PIXEL_UNPACK_BUFFER , 4*L*M*sizeof(float), 
               0, GL_DYNAMIC_DRAW);  
  
  glBindBuffer(GL_ARRAY_BUFFER, bufferID[GRID]);  
  glBufferData(GL_ARRAY_BUFFER,	L*M*sizeof(uint), 
               0, GL_DYNAMIC_DRAW);  
  
} 

void initTexBuffer(){
  csDataInit(bufferID, num_of_verticies);  
  csGrid2Tex(bufferID, num_of_verticies);
}

void initMapBuffer(){
  static const GLfloat tex_map[] = {
         0.75f, -0.75f,
        -0.75f, -0.75f,
        -0.75f,  0.75f,
         0.75f,  0.75f,

         0.0f, 0.0f,
         1.0f, 0.0f,
         1.0f, 1.0f,
         0.0f, 1.0f
  };
  glBindBuffer(GL_ARRAY_BUFFER, bufferID[MAP]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_map),tex_map, 
               GL_STATIC_DRAW);  
}
  
GLuint genTexture(){
  GLuint texHandle;  
  glGenTextures(1, &texHandle);

  glBindTexture(GL_TEXTURE_2D, texHandle);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, L, M);  
  
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, bufferID[TEXTURE]);  
  

  glTexSubImage2D(GL_TEXTURE_2D,
                  0,
                  0, 0,
                  L, M,
                  GL_RGBA, GL_FLOAT,
                  (GLvoid*)NULL);	

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  checkErrors("Gen texture");	
  return texHandle;
}; 

void initBuffers(){
  genBuffers();
  initMapBuffer();
  initTexBuffer();
  genTexture();  
}

void hMove(){
  csMove(bufferID, num_of_verticies);  
  csGrid2Tex(bufferID, num_of_verticies);
  genTexture();
}

void myCleanup(){
	glDeleteBuffers( NUM_OF_BUFFERS, bufferID);
}
