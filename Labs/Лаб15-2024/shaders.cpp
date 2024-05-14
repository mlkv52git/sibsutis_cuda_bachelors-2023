#include <GL/glew.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include "global.h"

void checkErrors(std::string desc) {
	GLenum e = glGetError();
	if (e != GL_NO_ERROR) {
		fprintf(stderr, "OpenGL error in \"%s\": %s (%d)\n", desc.c_str(), 																	     gluErrorString(e), e); 
		exit(20);
	}
}

GLuint genRenderProg() {
    GLuint progHandle = glCreateProgram();
    GLuint vp = glCreateShader(GL_VERTEX_SHADER);
    GLuint fp = glCreateShader(GL_FRAGMENT_SHADER);

	const char *vpSrc[] = {
        "#version 430\n",
        "layout (location = 0) in vec2 in_position;\
         layout (location = 1) in vec2 in_tex_coord;\
         out vec2 tex_coord;\
         void main(void){\
            gl_Position = vec4(in_position, 0.5, 1.0);\
            tex_coord = in_tex_coord;\
	 }"
	};
	const char *fpSrc[] = {
        "#version 430\n",
        "in vec2 tex_coord;\
         layout (location = 0) out vec4 color;\
         uniform sampler2D tex;\
         void main(void){\
	    color = texture(tex, tex_coord);\
         }"
        };

    glShaderSource(vp, 2, vpSrc, NULL);
    glShaderSource(fp, 2, fpSrc, NULL);

    glCompileShader(vp);
    int rvalue;
    glGetShaderiv(vp, GL_COMPILE_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in compiling vp\n");
        exit(30);
    }
    glAttachShader(progHandle, vp);

    
    glCompileShader(fp);
    glGetShaderiv(fp, GL_COMPILE_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in compiling fp\n");
        exit(31);
    }
    glAttachShader(progHandle, fp);

    
    glLinkProgram(progHandle);

    glGetProgramiv(progHandle, GL_LINK_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in linking sp\n");
        exit(32);
    }   

    checkErrors("Render shaders");

    return progHandle;
} 

GLuint genGrid2TexProg(){
    GLuint progHandle = glCreateProgram();
    GLuint cs = glCreateShader(GL_COMPUTE_SHADER);

    const char *cpSrc[] = {
	"#version 430\n",
	"layout (local_size_x = 128, local_size_y = 1, local_size_z = 1) in;\
	 layout(std430, binding = 0) buffer GridBuffer{uint Grid[];};\
	 layout(std430, binding = 1) buffer TexBuffer{float Tex[];};\
	 void main() {\
	  float color;\
	  uint index = gl_GlobalInvocationID.x;\
	  color=log2(1.0+float(Grid[index]))/6.0;\
	  Tex[index*4]=color;\
	  Tex[index*4+1]=color;\
	  Tex[index*4+2]=color;\
	  Tex[index*4+3]=1.0;\
        }"
      };

    glShaderSource(cs, 2, cpSrc, NULL);

    glCompileShader(cs);
    int rvalue;
    glGetShaderiv(cs, GL_COMPILE_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in compiling cs csGrid2Tex\n");
        exit(30);
    }
    glAttachShader(progHandle, cs);

    glLinkProgram(progHandle);

    glGetProgramiv(progHandle, GL_LINK_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in linking cs  csGrid2Tex\n");
        exit(32);
    }   

    checkErrors("Grid2Tex shader");

    return progHandle;
}    

void csGrid2Tex(GLuint* inBuf,int N){

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inBuf[GRID]);  
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, inBuf[TEXTURE]);  
  
  GLuint computeShaderID=genGrid2TexProg();
  glUseProgram(computeShaderID); // Bind compute shader
  
  glDispatchCompute(N/128, 1, 1); // Execute the compute shader with 13 workgroups

  glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT);
}

GLuint genInitProg(){
    GLuint progHandle = glCreateProgram();
    GLuint cs = glCreateShader(GL_COMPUTE_SHADER);

    const char *cpSrc[] = {
	"#version 430\n",
	"layout (local_size_x = 128, local_size_y = 1, local_size_z = 1) in; \
	 layout(std430, binding = 0) buffer PositionBuffer{float Pos[];};\
	 layout(std430, binding = 1) buffer VelocityBuffer{float Vel[];};\
	 layout(std430, binding = 2) buffer GridBuffer{int Grid[];};\
	 uniform int L,M;\
	 float lmap(in uint i){\
	  uint count;\
	  float x=0.78;\
	  for(count=0;count<i;count++)\
	    x=3.99*x*(1-x);\
	  return x;\
	 }\
	 void main() {\
	  uint index = gl_GlobalInvocationID.x;\
	  float hx=1.5/L;\
	  float hy=1.5/M;\
	  int i,j;\
	  Grid[index]=0;\
	  Pos[index*6]=-0.5+1.0*lmap(index);\
	  Pos[index*6+1]=-0.5+1.0*lmap(index*10);\
	  Pos[index*6+2]=0.0;\
	  Pos[index*6+3]=0.0;\
	  Pos[index*6+4]=0.0;\
	  Pos[index*6+5]=0.0;\
	  Vel[3*index]=0.0;/*-0.5+1.0*lmap(index);*/\
	  Vel[3*index+1]=0.0;/*-0.5+1.0*lmap(index*10);*/\
	  Vel[3*index+2]=0.0;\
	  barrier();\
	  i=int((Pos[index*6]+0.75)/hx);\
	  j=int((Pos[index*6+1]+0.75)/hy);\
	  atomicAdd(Grid[j+i*M],1);\
    }"
      };

    glShaderSource(cs, 2, cpSrc, NULL);

    glCompileShader(cs);
    int rvalue;
    glGetShaderiv(cs, GL_COMPILE_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in compiling cs csDataInit\n");
        exit(30);
    }
    glAttachShader(progHandle, cs);

    glLinkProgram(progHandle);

    glGetProgramiv(progHandle, GL_LINK_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in linking cs  csDataInit\n");
        exit(32);
    }   

    checkErrors("DataInit shader");

    return progHandle;
}

void csDataInit(GLuint* inBuf,int N){

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inBuf[POSITIONS]);  
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, inBuf[VELOCITIES]);  
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, inBuf[GRID]); 
  
  GLuint computeShaderID=genInitProg();
  glUseProgram(computeShaderID); // Bind compute shader
  
  GLuint loc = glGetUniformLocation(computeShaderID,"L");
  glUniform1i(loc, L);
  loc = glGetUniformLocation(computeShaderID,"M");
  glUniform1i(loc, M);
  
  glDispatchCompute(N/128, 1, 1); // Execute the compute shader with 13 workgroups

  glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT);
}

GLuint genMoveProg(){
    GLuint progHandle = glCreateProgram();
    GLuint cs = glCreateShader(GL_COMPUTE_SHADER);

    const char *cpSrc[] = {
	"#version 430\n",
	"layout (local_size_x = 128, local_size_y = 1, local_size_z = 1) in; \
	 layout(std430, binding = 0) buffer PositionBuffer{float Pos[];};\
	 layout(std430, binding = 1) buffer VelocityBuffer{float Vel[];};\
	 layout(std430, binding = 2) buffer GridBuffer{int Grid[];};\
	 uniform int L,M;\
	 void main() {\
	  float x,y,vx,vy;\
	  float tau=0.01;\
	  float eps=0.5;\
	  float hx=1.5/L;\
	  float hy=1.5/M;\
	  int i,j;\
	  uint index = gl_GlobalInvocationID.x;\
	  x=Pos[index*6];\
	  y=Pos[index*6+1];\
	  vx=Vel[3*index];\
	  vy=Vel[3*index+1];\
	  \
	  vx=vx+tau*10.0*(-x-eps*(2*x*y));\
	  vy=vy+tau*10.0*(-y-eps*(x*x-y*y));\
	  \
	  x=x+tau*vx;\
	  y=y+tau*vy;\
	  \
	  Pos[index*6]=x;\
	  Pos[index*6+1]=y;\
	  Vel[index*3]=vx;\
	  Vel[index*3+1]=vy;\
	  Grid[index]=0;\
	  barrier();\
	  i=int((Pos[index*6]+0.75)/hx);\
	  j=int((Pos[index*6+1]+0.75)/hy);\
	  atomicAdd(Grid[j+i*M],1);\
         }"
	};

    glShaderSource(cs, 2, cpSrc, NULL);

    glCompileShader(cs);
    int rvalue;
    glGetShaderiv(cs, GL_COMPILE_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in compiling cs csMove\n");
        exit(30);
    }
    glAttachShader(progHandle, cs);

    glLinkProgram(progHandle);

    glGetProgramiv(progHandle, GL_LINK_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in linking cs  csMove\n");
        exit(32);
    }   

    checkErrors("Move shader");

    return progHandle;
}    
    
void csMove(GLuint* inBuf,int N){
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inBuf[POSITIONS]);  
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, inBuf[VELOCITIES]);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, inBuf[GRID]);   
  
  GLuint computeShaderID=genMoveProg();
  glUseProgram(computeShaderID); // Bind compute shader
  
  GLuint loc = glGetUniformLocation(computeShaderID,"L");
  glUniform1i(loc, L);
  loc = glGetUniformLocation(computeShaderID,"M");
  glUniform1i(loc, M);
  
  glDispatchCompute(N/128, 1, 1); 

  glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT);
}
