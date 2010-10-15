#include <GL/glew.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <cstdio>

#include "test.h"
#include "util.h"

int nextPowerOfTwo(int num);
FT_BitmapGlyph getGlyph(char letter);
void fontInit(int size,int dpi);

int Program;
int ScaleUniform;
GLuint vao,vbo[4];

FT_Face face;
FT_Library library;
FT_BitmapGlyph glyphs[92];

void testinit()
{
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

   int TextUniform = glGetUniformLocation(Program,"Texture");
   int TextScaleUniform = glGetUniformLocation(Program,"TextScale");

   int text0 = makeTexture("res/atlas.svg");
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,text0);
   glUniform1i(TextUniform,0);
   
   char log[1000];
   glGetShaderInfoLog(geom,1000,NULL,log);
   printf("The log is comming in.\n%s",log);
   
   fontInit(16,0);
   for (int i = 'A';i<='Z';i++)
      getGlyph(i);

   printf("\nThe rows are %d, the width is %d, the pitch is %d\n",glyphs['A' -32]->bitmap.rows,glyphs['A' -32]->bitmap.width,glyphs['A' -32]->bitmap.pitch);
  
   int w = nextPowerOfTwo(glyphs['A' -32]->bitmap.width + glyphs['B' -32]->bitmap.width);
   int h = nextPowerOfTwo(glyphs['A' -32]->bitmap.rows + glyphs['B' -32]->bitmap.width);
   int width = glyphs['A' -32]->bitmap.width;
   int height =glyphs['A' -32]->bitmap.rows;

   glUniform2f(TextScaleUniform,w,h);

   unsigned char *lol = static_cast<unsigned char *>(malloc(w * h));
   memset(lol,0,w * h);

   for (int i =0;i<height;i++)
      memcpy(lol + i * w,glyphs['A' -32]->bitmap.buffer + i * width ,width);

   width = glyphs['B' -32]->bitmap.width;
   height =glyphs['B' -32]->bitmap.rows;

   for (int i =0;i<height;i++)
      memcpy(lol + i * w + glyphs['A' -32]->bitmap.width + 1,glyphs['B' -32]->bitmap.buffer + i * width ,width);

   for (int i = 0; i< h;i++)
   {
      for (int l = 0; l< w;l++)
   	 printf("%4d", lol[i * h + l]);
      putchar('\n');
   }

   glActiveTexture(GL_TEXTURE1);
   GLuint texttext;;
   glGenTextures(1,&texttext);

   glBindTexture(GL_TEXTURE_2D,texttext);

   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
   glTexImage2D(GL_TEXTURE_2D,0,GL_ALPHA,w,h,0,GL_ALPHA,GL_UNSIGNED_BYTE,lol );

   glUniform1i(TextUniform,1);
}

void testdraw()
{
   GLfloat PosArray[2][2];
   GLfloat TexArray[2][2];
   GLfloat ScaArray[2][2];

   PosArray[0][0] = 0;
   PosArray[0][1] = 0; 

   TexArray[0][0] = 0;
   TexArray[0][0] = 0;

   ScaArray[0][0] = glyphs['A' -32]->bitmap.width;
   ScaArray[0][1] = glyphs['A' -32]->bitmap.rows;

   PosArray[1][0] = ((FT_Glyph *) glyphs)['A' -32]->advance.x / (65536.0 * 200.0);
   PosArray[1][1] = 0; 

   TexArray[1][0] = glyphs['A' -32]->bitmap.width + 1;
   TexArray[1][1] = 0;

   ScaArray[1][0] = glyphs['B' -32]->bitmap.width;
   ScaArray[1][1] = glyphs['B' -32]->bitmap.rows;
   
   glDisableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
   glBufferSubData(GL_ARRAY_BUFFER,0,4 * sizeof(GLfloat),PosArray);
   glVertexAttribPointer((GLuint) 0,2,GL_FLOAT,GL_FALSE,0,0);
   glEnableVertexAttribArray(0);
  
   glDisableVertexAttribArray(1);
   glBindBuffer(GL_ARRAY_BUFFER,vbo[1]);
   glBufferSubData(GL_ARRAY_BUFFER,0,4 * sizeof(GLfloat),TexArray);
   glVertexAttribPointer((GLuint) 1,2,GL_FLOAT,GL_FALSE,0,0);
   glEnableVertexAttribArray(1);

   glDisableVertexAttribArray(2);
   glBindBuffer(GL_ARRAY_BUFFER,vbo[2]);
   glBufferSubData(GL_ARRAY_BUFFER,0,4 * sizeof(GLfloat),ScaArray);
   glVertexAttribPointer((GLuint) 2,2,GL_FLOAT,GL_FALSE,0,0);
   glEnableVertexAttribArray(2);
   
   glDrawElements(GL_POINTS,2,GL_UNSIGNED_BYTE,0);
   
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

int nextPowerOfTwo(int num)
{
   int x = 1;
   while (x < num)
      x *= 2;

   return x;
}

void testmove()
{
}

void fontInit(int size,int dpi)
{
   int error = FT_Init_FreeType(&library);
   printf("\n\nThe error in initialization was %d",error);

   error = FT_New_Face(library,"res/DejaVuSerif.ttf",0,&face);
   printf("\nThe error in making the face was %d",error);

   printf("\nThe file has %d glyphs",static_cast<int>(face->num_charmaps));

   error = FT_Set_Char_Size(face,0,size *64,0,dpi);
   printf("\nThe error for size setting was %d",error);
}

FT_BitmapGlyph getGlyph(char letter)
{
   int glyph_index = FT_Get_Char_Index( face, letter);
   printf("\nThe index for the thing is %d, compared to the given %d for %c",glyph_index,letter,letter);

   int error = FT_Load_Glyph(face,glyph_index,FT_LOAD_RENDER);
   printf("\nThe error for loading the glyph was %d",error);
   
   error = FT_Render_Glyph(face->glyph,FT_RENDER_MODE_NORMAL);
   printf("\nThe error for rendering the glyph was %d\n",error);

   FT_Get_Glyph(face->glyph,(FT_Glyph *) &glyphs[letter -32]);

   return NULL;
}
