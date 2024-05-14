#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include "global.h"

void initGL();
int initBuffers();
void  genTexture(); 
GLuint genRenderProg();
GLuint progHandle;
void myCleanup();
void hMove();
void display();

GLFWwindow* window;

int main(){
  initGL();
  initBuffers();

  do{
    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	  
    	glPointSize(2);
    	
      display();
      hMove();
      
      glfwSwapBuffers(window);
    	glfwPollEvents();
  }while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != 
          GLFW_PRESS && glfwWindowShouldClose(window) == 0 );  

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);  
  
  myCleanup();
  glfwTerminate();  

  return 0;
}

void initGL(){
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, 
		       GLFW_OPENGL_COMPAT_PROFILE);
	
	window = glfwCreateWindow( window_width, window_height, 
											"Template window", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. \n" );
		getchar();
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);
	
	// Инициализация GLEW
	glewExperimental = true; 
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return;
	}
	
	return;
}

void display(){
  progHandle=genRenderProg();
  glUseProgram(progHandle); 


  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(8 * sizeof(float)));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);    
}

