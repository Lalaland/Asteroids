#include <GL/glew.h>

#include <cstdio>

#include "asteroids.h"
#include "util.h"

int Program;
int TextScaleUniform;
int TextUniform;
int ScaleUniform;
int RotateUniform;
GLuint vao,vbo[4];

//Class: t_asteroids
//Purpose: container for other classes
//Interface: moveall and drawall

t_asteroids::t_asteroids()
{
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   GLuint frag = makeShader("frag",GL_FRAGMENT_SHADER);
   GLuint vert = makeShader("vert",GL_VERTEX_SHADER);
   GLuint geom = makeShader("geom",GL_GEOMETRY_SHADER);

   const GLubyte index[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};

   Program = glCreateProgram();
   glAttachShader(Program,frag);
   glAttachShader(Program,vert);
   glAttachShader(Program,geom);

   glGenVertexArrays(1,&vao);
   glBindVertexArray(vao);

   glGenBuffers(4,vbo);

   glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
   glBufferData(GL_ARRAY_BUFFER,20 * 2 * sizeof(GLfloat),NULL,GL_STREAM_DRAW);
   glVertexAttribPointer((GLuint) 0,2,GL_FLOAT,GL_FALSE,0,0);
   glEnableVertexAttribArray(0);

   glBindBuffer(GL_ARRAY_BUFFER,vbo[1]);
   glBufferData(GL_ARRAY_BUFFER,20 * 2 * sizeof(GLfloat),NULL,GL_STREAM_DRAW);
   glVertexAttribPointer((GLuint) 1,2,GL_FLOAT,GL_FALSE,0,0);
   glEnableVertexAttribArray(1);

   glBindBuffer(GL_ARRAY_BUFFER,vbo[2]);
   glBufferData(GL_ARRAY_BUFFER,20 * 2 * sizeof(GLfloat),NULL,GL_STREAM_DRAW);
   glVertexAttribPointer((GLuint) 2,2,GL_FLOAT,GL_FALSE,0,0);
   glEnableVertexAttribArray(2);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vbo[3]);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER,20 * sizeof(GLbyte),index,GL_STATIC_DRAW);

   glBindAttribLocation(Program, 0, "in_Position");
   glBindAttribLocation(Program, 1, "in_Textcord");
   glBindAttribLocation(Program, 2, "in_PositionScale");

   glLinkProgram(Program);
   glUseProgram(Program);

   TextUniform = glGetUniformLocation(Program,"Texture");
   TextScaleUniform = glGetUniformLocation(Program,"TextScale");
   ScaleUniform = glGetUniformLocation(Program,"Scale");
   RotateUniform = glGetUniformLocation(Program,"Rotate");

   glUniform1f(RotateUniform,0);

   int text0 = makeTexture("res/space3.svg");
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,text0);
   glUniform1i(TextUniform,0);
   
   char log[1000];
   glGetShaderInfoLog(geom,1000,NULL,log);
   printf("The log is comming in.\n%s",log);
   
   loadText();
}

void t_asteroids::drawall() const
{
   if (menu.valid())
      menu.drawall();
   
   else
      game.drawall();
      

   int GLERROR;

   while ((GLERROR = glGetError()))
   {
      if (GLERROR == GL_INVALID_ENUM)
	 printf("Invalid enum error");
      else if(GLERROR == GL_INVALID_VALUE)
	 printf("Invalid value");
      else if (GLERROR == GL_INVALID_OPERATION)
	 printf("Invalid op");
      else if (GLERROR == GL_OUT_OF_MEMORY)
	 printf("Out of mem");
      else
	 printf("Other error");
   }							    
}

void t_asteroids::moveall()
{
   if (menu.valid())
      menu.moveall();
   
   else
      if(game.moveall())
	 menu.open();
}
