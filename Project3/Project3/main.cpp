#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdio>
#include <iostream>
#include <math.h>
#include "vec3f.h"
#include "ppm_canvas.h"
#include <glut.h>
#include <GLU.h>
#include <glut.h>
#include <GL.h>


using namespace std;
GLUquadricObj *sphere = NULL;
Vec3f ballpos[16];
Vec3f ballvec[16];
GLint viewport[4];
int ballangx[16];
int ballangz[16];
Vec3f iniballpos[16];
GLint mode;
int playerturn = 0;   //0 = first player 1 = second player
int playerballid[2];   // 1 = red ball    2 = green ball
GLdouble modelview[16];
GLdouble projection[16];
bool pocketed[16];
bool collid[16][16];
GLdouble realx = 0.0;
GLdouble realy = 0.0;
GLdouble realz = 0.0;
GLfloat z_cursor;
bool hits = false;
bool startmove = false;
canvas_t tvskin;
canvas_t carpetskin;
canvas_t wallskin;
canvas_t ceilingskin;
canvas_t skin;
canvas_t skin2;
canvas_t skinb0,skinb1,skinb2,skinb3,skinb4,skinb5,skinb6,skinb7,skinb8,skinb9,skinb10,skinb11,skinb12,skinb13,skinb14,skinb15;
float zoomfactor = 1.0;
Vec3f temp1(0.0,0.0,0.0);
Vec3f temp2(0.0,0.0,0.0);
Vec3f temp3(0.0,0.0,0.0);
Vec3f temp4(0.0,0.0,0.0);
Vec3f sum(0.0,0.0,0.0);
bool donemoving = true;
bool viewpt = 0; // 0 = first person 1 = top view
int radi = 100;
int roangx= 0;
int roangy = 30;
float theta,phi;
GLfloat upX = 0.0;
GLfloat upY = 0.5;
GLfloat upZ = -0.5;
int wheelchange = 0;
float defx = 0;
float defy = 60.0;
float defz = 300.0;
float temppointerx = 0;
float temppointery = 0;
float width = 500;
float height = 500;
GLuint tvtexture;
GLuint carpettexture;
GLuint walltexture;
GLuint texture;
GLuint texture2;
GLuint ceilingtexture;
GLuint textureb0,textureb1,textureb2,textureb3,textureb4,textureb5,textureb6,textureb7,textureb8,textureb9,textureb10,textureb11,textureb12,textureb13,textureb14,textureb15;
GLint pointernum,pointerx,pointery;
int rightclick = 0;
int leftclick = 0;

Vec3f proj (Vec3f v1, Vec3f v2){
		//projection v1 to v2:
	// a1 = (a dot b)/(b dot b)   * b 
	return (((v1.dot(v2))/(v2.dot(v2)))* v2);
}
void animate(int value){
	if(startmove)
	{
		//prevent from adding-up vector twice
		for(int collct = 0;collct < 16;collct++){
			for(int collct2 = 0; collct2 <16;collct2++)
			{
				collid[collct][collct2] = false;
			}
		}
		for (int xc1 = 0; xc1 < 16; xc1 ++)
		{ 
			//	check for ball collision
			for(int xxc = 0; xxc < 16; xxc++)
			{
				//xxc = every other balls, xc1 = this ball
				if(xxc != xc1 && !collid[xxc][xc1]){//
					//if collision
					//circle equation :  (x-h)^2 + (y-k)^2 = r^2
					if(pow((ballpos[xc1].v[0] - ballpos[xxc].v[0]),2) + pow((ballpos[xc1].v[2] - ballpos[xxc].v[2]),2) < 16){
						//line between two centers
						collid[xc1][xxc] = true;
						Vec3f tempvec1(ballpos[xxc].v[0] -ballpos[xc1].v[0],0.0,ballpos[xxc].v[2] - ballpos[xc1].v[2]); //1st ball -> 2nd ball
						Vec3f tempvec2(ballpos[xc1].v[0] -ballpos[xxc].v[0],0.0,ballpos[xc1].v[2] - ballpos[xxc].v[2]); //2nd ball -> 1st ball
						Vec3f compvec1 = proj(ballvec[xc1],tempvec1);
						Vec3f compvec2 = proj(ballvec[xxc],tempvec2);
						ballvec[xc1] = ballvec[xc1] - compvec1 + compvec2;
						ballvec[xxc] = ballvec[xxc] - compvec2 + compvec1;
						while(pow((ballpos[xc1].v[0] + ballvec[xc1].v[0] - ballpos[xxc].v[0]-ballvec[xxc].v[0]),2) + pow((ballpos[xc1].v[2] + ballvec[xc1].v[2]- ballpos[xxc].v[2]-ballvec[xxc].v[2]),2) < 16){
							ballvec[xc1].v[0] += tempvec2.v[0];
							ballvec[xc1].v[2] += tempvec2.v[2];
							ballvec[xxc].v[0] += tempvec1.v[0];
							ballvec[xxc].v[2] += tempvec1.v[2];
						}
					}
				}
			}

		}
		//ball movement

		for (int xc = 0; xc < 16; xc ++)
		{ 

			ballpos[xc].v[0] = ballpos[xc].v[0] + ballvec[xc].v[0]/15;
			ballpos[xc].v[2] = ballpos[xc].v[2] + ballvec[xc].v[2]/15;
	
			if(ballvec[xc].magnitude() > 0.1)
			{
				// (ballvec[xc] * (-1) * 0.01) = decelaration vector
				ballvec[xc].v[0] = ballvec[xc].v[0] + (abs((ballvec[xc] * (-1) * 0.01).v[0]) > 0.003 ? (ballvec[xc] * (-1) * 0.01).v[0] :(ballvec[xc] * (-1)).v[0] );
				ballvec[xc].v[2] = ballvec[xc].v[2] +  (abs((ballvec[xc] * (-1) * 0.01).v[2]) > 0.003 ? (ballvec[xc] * (-1) * 0.01).v[2] :(ballvec[xc] * (-1)).v[2] );
			}
			else
			{
				ballvec[xc].v[0] = 0;ballvec[xc].v[2] = 0;
			}
				//reflection at edge
			if(ballpos[xc].v[0] > 48 || ballpos[xc].v[0] < -46)
			{
				ballvec[xc].v[0] = -1 * ballvec[xc].v[0];
			}
			if(ballpos[xc].v[2] > 98 || ballpos[xc].v[2] < -98)
			{
				ballvec[xc].v[2] = -1 * ballvec[xc].v[2];
			}
			//pocket or not?  pocket-d  = 6.5 pixs   take 3 pix as radius
			if(ballpos[xc].v[0] <= -45)
			{
				if((ballpos[xc].v[2] > -3 && ballpos[xc].v[2] < 3)||(ballpos[xc].v[2] > -100 && ballpos[xc].v[2] < -95)||(ballpos[xc].v[2] < 100 && ballpos[xc].v[2] > 95)  )
				{
					pocketed[xc] = true;
					if (xc == 5)
					{
						if(pocketed[1] && pocketed[2] && pocketed[6]&& pocketed[7]&& pocketed[9]&& pocketed[13]&& pocketed[15])
						{
							cout<<endl<<"Congratulations, Red player wins"<<endl;
						}
						else if(pocketed[3] && pocketed[4] && pocketed[8]&& pocketed[10]&& pocketed[11]&& pocketed[12]&& pocketed[14])
						{
							cout<<endl<<"Congratulations, Green player wins"<<endl;
						}
						else 
						{
							cout<<endl<<"player "<<playerturn+1<<" lost by pocketing black ball"<<endl;
						}
					}
					if(playerballid[playerturn] == 0)
					{
						if(xc == 1 ||xc == 2 ||xc == 6 ||xc == 7 ||xc == 9 ||xc == 13 ||xc == 15)
						{
							playerballid[playerturn] = 1;
							playerballid[(playerturn+1)%2] = 0;
						}
						else if(xc == 3 ||xc == 4 ||xc == 8 ||xc == 10 ||xc == 11 ||xc == 12 ||xc == 14)
						{
							playerballid[playerturn] = 2;
							playerballid[(playerturn+1)%2] = 1;}
					}
			
					if((xc == 1 ||xc == 2 ||xc == 6 ||xc == 7 ||xc == 9 ||xc == 13 ||xc == 15) && playerballid[playerturn] == 1)
					{
				
					}
					else if((xc == 3 ||xc == 4 ||xc == 8 ||xc == 10 ||xc == 11 ||xc == 12 ||xc == 14) && playerballid[playerturn] == 2)
					{
					
					}
					else
					{
						cout<<endl<<"Switch to next player"<<endl;
						playerturn = (playerturn +1)%2;
					}
			
			}
	}
	else if(ballpos[xc].v[0] >= 45)
	{
		if((ballpos[xc].v[2] > -3 && ballpos[xc].v[2] < 3)||(ballpos[xc].v[2] > -100 && ballpos[xc].v[2] < -95)||(ballpos[xc].v[2] < 100 && ballpos[xc].v[2] > 95)  )
		{
			pocketed[xc] = true;
			
			if (xc == 5)
			{
				if(pocketed[1] && pocketed[2] && pocketed[6]&& pocketed[7]&& pocketed[9]&& pocketed[13]&& pocketed[15])
				{
					cout<<endl<<"Congratulations, Red player wins"<<endl;
				}
				else if(pocketed[3] && pocketed[4] && pocketed[8]&& pocketed[10]&& pocketed[11]&& pocketed[12]&& pocketed[14])
				{
					cout<<endl<<"Congratulations, Green player wins"<<endl;
				}
				else 
				{
					cout<<endl<<"player "<<playerturn+1<<" lost by pocketing black ball"<<endl;
				}
			}
			//if pocket for the first time 
			if(playerballid[playerturn] == 0)
			{
				if(xc == 1 ||xc == 2 ||xc == 6 ||xc == 7 ||xc == 9 ||xc == 13 ||xc == 15)
				{
					playerballid[playerturn] = 1;
					playerballid[(playerturn+1)%2] = 0;
				}
				else if(xc == 3 ||xc == 4 ||xc == 8 ||xc == 10 ||xc == 11 ||xc == 12 ||xc == 14)
				{
					playerballid[playerturn] = 2;
					playerballid[(playerturn+1)%2] = 1;
				}
			}
			if((xc == 1 ||xc == 2 ||xc == 6 ||xc == 7 ||xc == 9 ||xc == 13 ||xc == 15) && playerballid[playerturn] == 1)
			{
				
			}
			else if((xc == 3 ||xc == 4 ||xc == 8 ||xc == 10 ||xc == 11 ||xc == 12 ||xc == 14) && playerballid[playerturn] == 2)
			{
					
			}
			else
			{
					cout<<endl<<"Switch to next player"<<endl;
					playerturn = (playerturn +1)%2;
			}
			
		}
		
	}
	

	}
	
	
		
		//set startmove = 1 if all balls are at rest
		startmove = false;
		for(int aab = 0;aab <16;aab++)
		{
			if(ballvec[aab].magnitude() != 0)
			{
				startmove = true;

			}
		}
	}
	//	glutPostRedisplay();
	//glutTimerFunc(100,animate,0);
}
void drawRoomWithTable()
{

	glPushMatrix();
		glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, walltexture);
		//backwall wall
	glBegin(GL_QUADS);

		glVertex3f(200,-50,-200);
	glTexCoord3f(1.0,0.0,0.0);
	
	glVertex3f(200,200,-200);
	 glTexCoord3f(1.0,1.0,0.0);
	
	glVertex3f(-200,200,-200);
	glTexCoord3f(0.0,1.0,0.0);

	glVertex3f(-200,-50,-200);
	glTexCoord3f(0.0,0.0,0.0);
	
	glEnd();

//television
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tvtexture);
	glBegin(GL_QUADS);

	glVertex3f(75,50,-195);
	glTexCoord3f(1.0,0.0,0.0);
	
	glVertex3f(75,150,-195);
	glTexCoord3f(1.0,1.0,0.0);
	
	glVertex3f(-75,150,-195);
	glTexCoord3f(0.0,1.0,0.0);

	glVertex3f(-75,50,-195);
	glTexCoord3f(0.0,0.0,0.0);
	
	glEnd();
	glDisable(GL_TEXTURE_2D);

	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, walltexture);
	glBegin(GL_QUADS);

	glVertex3f(-200,-50,-200);
	glTexCoord3f(1.0,0.0,0.0);
	
	glVertex3f(-200,200,-200);
	 glTexCoord3f(1.0,1.0,0.0);
	
	glVertex3f(-200,200,320);
	glTexCoord3f(0.0,1.0,0.0);

	glVertex3f(-200,-50,320);
	glTexCoord3f(0.0,0.0,0.0);
	
	glEnd();


	glBegin(GL_QUADS);

	glVertex3f(200,-50,-200);
	glTexCoord3f(1.0,0.0,0.0);
	
	glVertex3f(200,200,-200);
	 glTexCoord3f(1.0,1.0,0.0);
	
	glVertex3f(200,200,320);
	glTexCoord3f(0.0,1.0,0.0);

	glVertex3f(200,-50,320);
	glTexCoord3f(0.0,0.0,0.0);
	
	glEnd();


    glBegin(GL_QUADS);

	glVertex3f(200,-50,320);
	glTexCoord3f(1.0,0.0,0.0);
	
	glVertex3f(200,200,320);
	 glTexCoord3f(1.0,1.0,0.0);
	
	glVertex3f(-200,200,320);
	glTexCoord3f(0.0,1.0,0.0);

	glVertex3f(-200,-50,320);
	glTexCoord3f(0.0,0.0,0.0);
	
	glEnd();


	
	//glPopMatrix();
	glDisable(GL_TEXTURE_2D);
		//glPushMatrix(); 


	//ball : table width(100) = 1 : 24.5614 ==> ball radius = 2 pixel
	// draw pool Table
	/*glColor3f(0.0,1.0,0.0);*/


	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, carpettexture);
	//floor
	glBegin(GL_QUADS);

	glVertex3f(-200,-50,-200);
	glTexCoord3f(0.0,1.0,0.0);
	glVertex3f(200,-50,-200);
	glTexCoord3f(1.0,1.0,0.0);
	glVertex3f(200,-50,320);
	 glTexCoord3f(1.0,0.0,0.0);

	glVertex3f(-200,-50,320);
	
 glTexCoord3f(0.0,0.0,0.0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	//glPopMatrix();

	//glPushMatrix();
	//ceiling
		glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ceilingtexture);
	glBegin(GL_QUADS);

	glVertex3f(-200,200,-200);
	glTexCoord3f(0.0,1.0,0.0);
	glVertex3f(200,200,-200);
	glTexCoord3f(1.0,1.0,0.0);
	glVertex3f(200,200,320);
	 glTexCoord3f(1.0,0.0,0.0);

	glVertex3f(-200,200,320);
	
 glTexCoord3f(0.0,0.0,0.0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	//glPopMatrix();



	//glPushMatrix(); 
	glScalef(100,5,200);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glutSolidCube(1.0);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	//head string
	glColor3f(1.0,1.0,1.0);
	glBegin(GL_LINES);
	glNormal3f(0.0,1.0,0.0);
	glVertex3f(-50,2.6,50);
	glVertex3f(50,2.6,50);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	//leftside middle
	glBegin(GL_QUADS);
	//1
	glNormal3f(0.0,1.0,0.0);
 glVertex3f(-50.0,10.0,100.0);
 glTexCoord3f(0.0,1.0,0.0);
 glVertex3f(-50.0,10.0,0.0);
 glTexCoord3f(1.0,1.0,0.0);
 glVertex3f(-60.0,10.0,0.0);
 glTexCoord3f(1.0,0.0,0.0);
 glVertex3f(-60.0,10.0,100.0);
 glTexCoord3f(0.0,0.0,0.0);
 //2
 glNormal3f(-1.0,0.0,0.0);
 glVertex3f(-60.0,10.0,0.0);
 glTexCoord3f(1.0,1.0,0.0);
 glVertex3f(-60.0,10.0,100.0);
 glTexCoord3f(1.0,0.0,0.0);
 glVertex3f(-60.0,-5.0,100.0);
 glTexCoord3f(0.0,0.0,0.0);
 glVertex3f(-60.0,-5.0,0.0);
 glTexCoord3f(0.0,1.0,0.0);
 //3
  glNormal3f(0.0,-1.0,0.0);
 glVertex3f(-60.0,-5.0,100.0);
 glTexCoord3f(0.0,0.0,0.0);
 glVertex3f(-60.0,-5.0,0.0);
 glTexCoord3f(0.0,1.0,0.0);
 glVertex3f(-50.0,-5.0,0.0);
 glTexCoord3f(1.0,1.0,0.0);
 glVertex3f(-50.0,-5.0,100.0);
 glTexCoord3f(1.0,0.0,0.0);
 //4
 glDisable(GL_TEXTURE_2D);
 glColor3f(0.0,1.0,0.0);
 glNormal3f(1.0,0.0,0.0);
 glVertex3f(-50.0,-5.0,0.0);
 glVertex3f(-50.0,-5.0,100.0);
  glVertex3f(-50.0,10.0,100.0);
 glVertex3f(-50.0,10.0,0.0);
 glColor3f(1.0,1.0,1.0);
 glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);


 //1
glNormal3f(0.0,1.0,0.0);
 glVertex3f(-50.0,10.0,-100.0);
 glTexCoord3f(0.0,1.0,0.0);
 glVertex3f(-50.0,10.0,0.0);
 glTexCoord3f(1.0,1.0,0.0);
 glVertex3f(-60.0,10.0,0.0);
 glTexCoord3f(1.0,0.0,0.0);
 glVertex3f(-60.0,10.0,-100.0);
 glTexCoord3f(0.0,0.0,0.0);
 //2
glNormal3f(-1.0,0.0,0.0);
 glVertex3f(-60.0,10.0,0.0);
 glTexCoord3f(1.0,1.0,0.0);
 glVertex3f(-60.0,10.0,-100.0);
 glTexCoord3f(1.0,0.0,0.0);
 glVertex3f(-60.0,-5.0,-100.0);
 glTexCoord3f(0.0,0.0,0.0);
 glVertex3f(-60.0,-5.0,0.0);
 glTexCoord3f(0.0,1.0,0.0);
 //3
  glNormal3f(0.0,-1.0,0.0);
 glVertex3f(-60.0,-5.0,-100.0);
 glTexCoord3f(0.0,0.0,0.0);
 glVertex3f(-60.0,-5.0,0.0);
 glTexCoord3f(0.0,1.0,0.0);
 glVertex3f(-50.0,-5.0,0.0);
 glTexCoord3f(1.0,1.0,0.0);
 glVertex3f(-50.0,-5.0,-100.0);
 glTexCoord3f(1.0,0.0,0.0);
 

 //4
 glDisable(GL_TEXTURE_2D);
 glColor3f(0.0,1.0,0.0);
 glNormal3f(1.0,0.0,0.0);
 glVertex3f(-50.0,-5.0,0.0);
 glVertex3f(-50.0,-5.0,-100.0);
  glVertex3f(-50.0,10.0,-100.0);
 glVertex3f(-50.0,10.0,0.0);
 glColor3f(1.0,1.0,1.0);
 glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

 //right side
 //1
 glNormal3f(0.0,1.0,0.0);
 glVertex3f(50.0,10.0,100.0);
 glTexCoord3f(0.0,1.0,0.0);
 glVertex3f(50.0,10.0,0.0);
 glTexCoord3f(1.0,1.0,0.0);
 glVertex3f(60.0,10.0,0.0);
 glTexCoord3f(1.0,0.0,0.0);
 glVertex3f(60.0,10.0,100.0);
 glTexCoord3f(0.0,0.0,0.0);
 //2
 glNormal3f(1.0,0.0,0.0);
 glVertex3f(60.0,10.0,0.0);
 glTexCoord3f(1.0,1.0,0.0);
 glVertex3f(60.0,10.0,100.0);
 glTexCoord3f(1.0,0.0,0.0);
 glVertex3f(60.0,-5.0,100.0);
 glTexCoord3f(0.0,0.0,0.0);
 glVertex3f(60.0,-5.0,0.0);
 glTexCoord3f(0.0,1.0,0.0);
 //3
  glNormal3f(0.0,-1.0,0.0);
 glVertex3f(60.0,-5.0,100.0);
 glTexCoord3f(0.0,0.0,0.0);
 glVertex3f(60.0,-5.0,0.0);
 glTexCoord3f(0.0,1.0,0.0);
 glVertex3f(50.0,-5.0,0.0);
 glTexCoord3f(1.0,1.0,0.0);
 glVertex3f(50.0,-5.0,100.0);
 glTexCoord3f(1.0,0.0,0.0);
 
 //4
 glDisable(GL_TEXTURE_2D);
 glColor3f(0.0,1.0,0.0);
 glNormal3f(-1.0,0.0,0.0);
 glVertex3f(50.0,-5.0,0.0);
  glVertex3f(50.0,10.0,0.0);
  glVertex3f(50.0,10.0,100.0);
 glVertex3f(50.0,-5.0,100.0);
 glColor3f(1.0,1.0,1.0);
 glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
 //1
 glNormal3f(0.0,1.0,0.0);
 glVertex3f(50.0,10.0,-100.0);
 glTexCoord3f(0.0,1.0,0.0);
 glVertex3f(50.0,10.0,0.0);
 glTexCoord3f(1.0,1.0,0.0);
 glVertex3f(60.0,10.0,0.0);
 glTexCoord3f(1.0,0.0,0.0);
 glVertex3f(60.0,10.0,-100.0);
 glTexCoord3f(0.0,0.0,0.0);
 //2
 glNormal3f(1.0,0.0,0.0);
 glVertex3f(60.0,10.0,0.0);
 glTexCoord3f(1.0,1.0,0.0);
 glVertex3f(60.0,10.0,-100.0);
 glTexCoord3f(1.0,0.0,0.0);
 glVertex3f(60.0,-5.0,-100.0);
 glTexCoord3f(0.0,0.0,0.0);
 glVertex3f(60.0,-5.0,0.0);
 glTexCoord3f(0.0,1.0,0.0);
 //3
  glNormal3f(0.0,-1.0,0.0);
 glVertex3f(60.0,-5.0,-100.0);
 glTexCoord3f(0.0,0.0,0.0);
 glVertex3f(60.0,-5.0,0.0);
 glTexCoord3f(0.0,1.0,0.0);
 glVertex3f(50.0,-5.0,0.0);
 glTexCoord3f(1.0,1.0,0.0);
 glVertex3f(50.0,-5.0,100.0);
 glTexCoord3f(1.0,0.0,0.0);
 
 //4
 glDisable(GL_TEXTURE_2D);
 glColor3f(0.0,1.0,0.0);
 glNormal3f(-1.0,0.0,0.0);
 glVertex3f(50.0,-5.0,0.0);
  glVertex3f(50.0,10.0,0.0);
    glVertex3f(50.0,10.0,-100.0);
 glVertex3f(50.0,-5.0,-100.0);
 glColor3f(1.0,1.0,1.0);
 glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

 //top n bottom
 //1
 glColor3f(0.0,1.0,0.0);
 glNormal3f(0.0,0.0,-1.0);
 glVertex3f(-50.0,10.0,100.0);
  glVertex3f(50.0,10.0,100.0);
   glVertex3f(50.0,-5.0,100.0);
 glVertex3f(-50.0,-5.0,100.0);
 glColor3f(1.0,1.0,1.0);
 //2
 glNormal3f(0.0,1.0,0.0);
 glVertex3f(-50.0,10.0,100.0);
 glTexCoord3f(0.0,1.0,0.0);
 glVertex3f(-50.0,10.0,110.0);
 glTexCoord3f(0.0,0.0,0.0);
 glVertex3f(50.0,10.0,110.0);
 glTexCoord3f(1.0,0.0,0.0);
 glVertex3f(50.0,10.0,100.0);
 glTexCoord3f(1.0,1.0,0.0);
 //3
  glNormal3f(0.0,0.0,1.0);
 glVertex3f(-50.0,10.0,110.0);
 glTexCoord3f(0.0,1.0,0.0);
 glVertex3f(-50.0,-5.0,110.0);
 glTexCoord3f(0.0,0.0,0.0);
 glVertex3f(50.0,-5.0,110.0);
 glTexCoord3f(1.0,0.0,0.0);
 glVertex3f(50.0,10.0,110.0);
 glTexCoord3f(1.0,1.0,0.0);
 
 //4
 glNormal3f(0.0,-1.0,0.0);
 glVertex3f(-50.0,-5.0,100.0);
 glTexCoord3f(0.0,1.0,0.0);
 glVertex3f(-50.0,-5.0,110.0);
 glTexCoord3f(0.0,0.0,0.0);
  glVertex3f(50.0,-5.0,110.0);
 glTexCoord3f(1.0,0.0,0.0);
 glVertex3f(50.0,-5.0,100.0);
 glTexCoord3f(1.0,1.0,0.0);
 //1
 glColor3f(0.0,1.0,0.0);
 glNormal3f(0.0,0.0,1.0);
 glVertex3f(-50.0,10.0,-100.0);
 glVertex3f(-50.0,-5.0,-100.0);
 glVertex3f(50.0,-5.0,-100.0);
 glVertex3f(50.0,10.0,-100.0);
 glColor3f(1.0,1.0,1.0);
 //2
 glNormal3f(0.0,1.0,0.0);
 glVertex3f(-50.0,10.0,-100.0);
 glTexCoord3f(0.0,1.0,0.0);
 glVertex3f(-50.0,10.0,-110.0);
 glTexCoord3f(0.0,0.0,0.0);
 glVertex3f(50.0,10.0,-110.0);
 glTexCoord3f(1.0,0.0,0.0);
 glVertex3f(50.0,10.0,-100.0);
 glTexCoord3f(1.0,1.0,0.0);
 //3
  glNormal3f(0.0,0.0,1.0);
 glVertex3f(-50.0,10.0,-110.0);
 glTexCoord3f(0.0,1.0,0.0);
 glVertex3f(-50.0,-5.0,-110.0);
 glTexCoord3f(0.0,0.0,0.0);
 glVertex3f(50.0,-5.0,-110.0);
 glTexCoord3f(1.0,0.0,0.0);
 glVertex3f(50.0,10.0,-110.0);
 glTexCoord3f(1.0,1.0,0.0);
 
 //4
 glNormal3f(0.0,-1.0,0.0);
 glVertex3f(-50.0,-5.0,-100.0);
 glTexCoord3f(0.0,1.0,0.0);
 glVertex3f(-50.0,-5.0,-110.0);
 glTexCoord3f(1.0,1.0,0.0);
  glVertex3f(50.0,-5.0,-110.0);
 glTexCoord3f(1.0,0.0,0.0);
 glVertex3f(50.0,-5.0,-100.0);
 glTexCoord3f(0.0,0.0,0.0);
 glEnd();

 //left top corner
 glBegin(GL_TRIANGLES);
	glVertex3f(-50.0,10.0,-110.0);
	glVertex3f(-50.0,10.0,-100.0);
	glVertex3f(-60.0,10.0,-100.0);
glEnd();
 glBegin(GL_TRIANGLES);
	glVertex3f(-50.0,-5.0,-110.0);
	glVertex3f(-50.0,-5.0,-100.0);
	glVertex3f(-60.0,-5.0,-100.0);
glEnd();

 glBegin(GL_QUADS);
	glVertex3f(-50.0,10.0,-110.0);
	glVertex3f(-50.0,-5.0,-110.0);
	glVertex3f(-60.0,-5.0,-100.0);
	glVertex3f(-60.0,10.0,-100.0);
glEnd();


 //Left bottom corner
 glBegin(GL_TRIANGLES);
	glVertex3f(-50.0,10.0,110.0);
	glVertex3f(-50.0,10.0,100.0);
	glVertex3f(-60.0,10.0,100.0);
glEnd();
 glBegin(GL_TRIANGLES);
	glVertex3f(-50.0,-5.0,110.0);
	glVertex3f(-50.0,-5.0,100.0);
	glVertex3f(-60.0,-5.0,100.0);
glEnd();

 glBegin(GL_QUADS);
	glVertex3f(-50.0,10.0,110.0);
	glVertex3f(-50.0,-5.0,110.0);
	glVertex3f(-60.0,-5.0,100.0);
	glVertex3f(-60.0,10.0,100.0);
glEnd();


 //Right Top corner
 glBegin(GL_TRIANGLES);
	glVertex3f(50.0,10.0,-110.0);
	glVertex3f(50.0,10.0,-100.0);
	glVertex3f(60.0,10.0,-100.0);
glEnd();

glBegin(GL_TRIANGLES);
	glVertex3f(50.0,-5.0,-110.0);
	glVertex3f(50.0,-5.0,-100.0);
	glVertex3f(60.0,-5.0,-100.0);
glEnd();

 glBegin(GL_QUADS);
 	glVertex3f(50.0,10.0,-110.0);
	glVertex3f(50.0,-5.0,-110.0);
	glVertex3f(60.0,-5.0,-100.0);
	glVertex3f(60.0,10.0,-100.0);
glEnd();

 //Right Bottom corner
 glBegin(GL_TRIANGLES);
	glVertex3f(50.0,10.0,110.0);
	glVertex3f(50.0,10.0,100.0);
	glVertex3f(60.0,10.0,100.0);
glEnd();

glBegin(GL_TRIANGLES);
	glVertex3f(50.0,-5.0,110.0);
	glVertex3f(50.0,-5.0,100.0);
	glVertex3f(60.0,-5.0,100.0);
glEnd();

 glBegin(GL_QUADS);
	glVertex3f(50.0,10.0,110.0);
	glVertex3f(50.0,-5.0,110.0);
	glVertex3f(60.0,-5.0,100.0);
	glVertex3f(60.0,10.0,100.0);
glEnd();




 glDisable(GL_TEXTURE_2D);
 //draw 6 pockets
 glColor3f(0.0,0.0,0.0);
/* glGenTextures(1, &texture2);
			glBindTexture(GL_TEXTURE_2D, texture2);
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, skin2.width, skin2.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skin2.pixels);
 glEnable(GL_TEXTURE_2D);
 */
 //left lower
 GLUquadricObj * quadObj = gluNewQuadric();
 glPushMatrix();
 glTranslatef(-50,2.6,100.0);
 glRotatef(-90.0,1.0,0.0,0.0);
 gluDisk(quadObj,0,5,20,20);
  glRotatef(-90.0,0.0,1.0,0.0);
 gluDisk(quadObj,0,5,20,20);
   glRotatef(-90.0,0.0,0.0,1.0);
 gluDisk(quadObj,0,5,20,20);

 //left leg
 glEnable(GL_TEXTURE_2D);
 glBindTexture(GL_TEXTURE_2D, texture);
 glTranslatef(-5.0,-45,-5.0);
 glRotatef(-90.0,1.0,0.0,0.0);
 gluCylinder(quadObj,5,10,40,20,20);
 glDisable(GL_TEXTURE_2D);

 //gluCylinder(quadObj,5,5,10,20,20);
 //gluDisk(quadObj,4,5,20,20);
glPopMatrix();
 //right lower
 glPushMatrix();
 glTranslatef(50,2.6,100.0);
 glRotatef(-90.0,1.0,0.0,0.0);

  gluDisk(quadObj,0,5,20,20);
  glRotatef(-90.0,0.0,1.0,0.0);
 gluDisk(quadObj,0,5,20,20);
   glRotatef(-90.0,0.0,0.0,1.0);
 gluDisk(quadObj,0,5,20,20);


  //right bottom leg
 glEnable(GL_TEXTURE_2D);
 glBindTexture(GL_TEXTURE_2D, texture);
 glTranslatef(-5.0,-45,5.0);
 glRotatef(-90.0,1.0,0.0,0.0);
 gluCylinder(quadObj,5,10,40,20,20);
 glDisable(GL_TEXTURE_2D);
//gluCylinder(quadObj,5,5,10,20,20);
glPopMatrix();
// left top
 glPushMatrix();
 glTranslatef(-50,2.6,-100.0);
 glRotatef(-90.0,1.0,0.0,0.0);
 
  gluDisk(quadObj,0,5,20,20);
  glRotatef(-90.0,0.0,1.0,0.0);
 gluDisk(quadObj,0,5,20,20);
   glRotatef(-90.0,0.0,0.0,1.0);
 gluDisk(quadObj,0,5,20,20);

    //left top leg
 glEnable(GL_TEXTURE_2D);
 glBindTexture(GL_TEXTURE_2D, texture);
 glTranslatef(5.0,-45,-5.0);
 glRotatef(-90.0,1.0,0.0,0.0);
 gluCylinder(quadObj,5,10,40,20,20);
 glDisable(GL_TEXTURE_2D);

//gluCylinder(quadObj,5,5,10,20,20);
glPopMatrix();
// right top
 glPushMatrix();
 glTranslatef(50,2.6,-100.0);
 glRotatef(-90.0,1.0,0.0,0.0);
   gluDisk(quadObj,0,5,20,20);
  glRotatef(-90.0,0.0,1.0,0.0);
 gluDisk(quadObj,0,5,20,20);
   glRotatef(-90.0,0.0,0.0,1.0);
 gluDisk(quadObj,0,5,20,20);

   //right top leg
 glEnable(GL_TEXTURE_2D);
 glBindTexture(GL_TEXTURE_2D, texture);
 glTranslatef(-5.0,-45,-5.0);
 glRotatef(-90.0,1.0,0.0,0.0);
 gluCylinder(quadObj,5,10,40,20,20);
 glDisable(GL_TEXTURE_2D);
//gluCylinder(quadObj,5,5,10,20,20);
glPopMatrix();
// middle left
 glPushMatrix();
 glTranslatef(-50.0,2.6,0.0);
 glRotatef(-90.0,1.0,0.0,0.0);
   gluDisk(quadObj,0,5,20,20);
  glRotatef(-90.0,0.0,1.0,0.0);
 gluDisk(quadObj,0,5,20,20);
   glRotatef(-90.0,0.0,0.0,1.0);
 gluDisk(quadObj,0,5,20,20);
//gluCylinder(quadObj,5,5,10,20,20);
glPopMatrix();
// middle left
 glPushMatrix();
 glTranslatef(50.0,2.6,0.0);
 glRotatef(-90.0,1.0,0.0,0.0);

   gluDisk(quadObj,0,5,20,20);
  glRotatef(-90.0,0.0,1.0,0.0);
 gluDisk(quadObj,0,5,20,20);
   glRotatef(-90.0,0.0,0.0,1.0);
 gluDisk(quadObj,0,5,20,20);
//gluCylinder(quadObj,5,5,10,20,20);
glPopMatrix();
// glDisable(GL_TEXTURE_2D);


 
}


void drawBall(){
	/*glColor3f(1.0,1.0,1.0);*/
	glPushMatrix();
	glTranslatef(ballpos[0].v[0],ballpos[0].v[1],ballpos[0].v[2]);
	//pocket white foul
	if(pocketed[0]){ballpos[0].v[0] = 0.0;ballpos[0].v[1] = 4.6;ballpos[0].v[2] = 50.0;cout<<endl<<"Foul!!"<<endl<<endl;
	pocketed[0]=false;donemoving = false;
	ballvec[0].v[0] = 0.0;ballvec[0].v[2] = 0.0;
	}
	else
	{
		//Cue ball
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureb0);
		gluSphere(sphere,2.0,10,10);
		glDisable(GL_TEXTURE_2D);
	}
	
	glPopMatrix();
	for(int temcount = 1; temcount <16; temcount++){
		if(temcount == 1 ||temcount == 2 ||temcount == 6 ||temcount == 7 ||temcount == 9 ||temcount == 13 ||temcount == 15){
						
		/*glColor3f(1.0,0.0,0.0);*/glPushMatrix();
		ballangx[temcount] = ((ballpos[temcount].v[0]-iniballpos[temcount].v[0])/(3.141592654*4))*360;
		ballangz[temcount] = ((ballpos[temcount].v[2]-iniballpos[temcount].v[2])/(3.141592654*4))*360;

		}
		else if (temcount == 3 ||temcount == 4 ||temcount == 8 ||temcount == 10 ||temcount == 11 ||temcount == 12 ||temcount == 14){
		/*glColor3f(0.0,1.0,0.0);*/glPushMatrix();}
		else if (temcount == 5){
		/*glColor3f(0.0,0.0,0.0);*/glPushMatrix();}
		
		glTranslatef(ballpos[temcount].v[0],ballpos[temcount].v[1],ballpos[temcount].v[2]);
				glRotatef(-ballangx[temcount],0.0,0.0,1.0);
		glRotatef(ballangz[temcount],1.0,0.0,0.0);
		if(!pocketed[temcount]){
				if(temcount == 1 )
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, textureb1);
						gluSphere(sphere,2.0,10,10);
					glDisable(GL_TEXTURE_2D);
				}
				else if(temcount == 2 )
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, textureb2);
						gluSphere(sphere,2.0,10,10);
					glDisable(GL_TEXTURE_2D);
				}
				else if(temcount == 3 )
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, textureb3);
						gluSphere(sphere,2.0,10,10);
					glDisable(GL_TEXTURE_2D);
				}
				else if(temcount == 4 )
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, textureb4);
						gluSphere(sphere,2.0,10,10);
					glDisable(GL_TEXTURE_2D);
				}
				else if(temcount == 5 )
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, textureb5);
						gluSphere(sphere,2.0,10,10);
					glDisable(GL_TEXTURE_2D);
				}
				else if(temcount == 6 )
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, textureb6);
						gluSphere(sphere,2.0,10,10);
					glDisable(GL_TEXTURE_2D);
				}
				else if(temcount == 7 )
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, textureb7);
						gluSphere(sphere,2.0,10,10);
					glDisable(GL_TEXTURE_2D);
				}
				else if(temcount == 8 )
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, textureb8);
						gluSphere(sphere,2.0,10,10);
					glDisable(GL_TEXTURE_2D);
				}
				else if(temcount == 9 )
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, textureb9);
						gluSphere(sphere,2.0,10,10);
					glDisable(GL_TEXTURE_2D);
				}
				else if(temcount == 10 )
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, textureb10);
						gluSphere(sphere,2.0,10,10);
					glDisable(GL_TEXTURE_2D);
				}
				else if(temcount == 11 )
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, textureb11);
						gluSphere(sphere,2.0,10,10);
					glDisable(GL_TEXTURE_2D);
				}
				else if(temcount == 12 )
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, textureb12);
						gluSphere(sphere,2.0,10,10);
					glDisable(GL_TEXTURE_2D);
				}
				else if(temcount == 13 )
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, textureb13);
						gluSphere(sphere,2.0,10,10);
					glEnable(GL_TEXTURE_2D);
				}
				else if(temcount == 14 )
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, textureb14);
						gluSphere(sphere,2.0,10,10);
					glDisable(GL_TEXTURE_2D);
				}
				else if(temcount == 15 )
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, textureb15);
						gluSphere(sphere,2.0,10,10);
					glDisable(GL_TEXTURE_2D);
				}
				


			}
		/*glDisable(GL_TEXTURE_2D);*/
		glPopMatrix();
	}
	
	/*glColor3f(1.0,1.0,1.0);*/
}

void callback_display() 
{
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity(  );
	glViewport( 0, 0, width, height );
	gluPerspective( 60*zoomfactor, ( float )width/height, 1, 1000 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity(  );	  
    //  clear stuff on the canvas
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClearColor(1.0f,1.0f,1.0f,1.0);
	glPushMatrix();
	// glTranslatef(0, 0, -35);
	//  if(changeid == 1){
	//  glPushMatrix();
	if(!viewpt)
	{
		gluLookAt(defx,defy,defz,ballpos[0].v[0],ballpos[0].v[1], ballpos[0].v[2], upX,upY,upZ);
	}//deflookatx,deflookaty, deflookatz,
	else
	{
		gluLookAt(0,200,0,0,0, 0, 0,0,-1.0);
	}
//	  glPopMatrix();
	glLineWidth(1.5);
	if(hits)
	{
					glBegin(GL_LINES);
					glColor3f(1.0,0.0,0.0);
					glVertex3f(ballpos[0].v[0],ballpos[0].v[1],ballpos[0].v[2]);
					glVertex3f(realx, 4.6, realy);
					Vec3f oppopt(ballpos[0].v[0]-realx,0.0,ballpos[0].v[2] - realy);
					Vec3f originpt(ballpos[0].v[0],ballpos[0].v[1],ballpos[0].v[2]);
					/*glColor3f(1.0,1.0,0.0);*/
					glVertex3f(ballpos[0].v[0],ballpos[0].v[1],ballpos[0].v[2]);
					ballvec[0].v[0] = oppopt.v[0];
					if(ballvec[0].v[0] > 50)
					{
						ballvec[0].v[0] = 50;
					}
					else if(ballvec[0].v[0] < -50)
					{
						ballvec[0].v[0] = -50;
					}
					ballvec[0].v[2] = oppopt.v[2];
					if(ballvec[0].v[2] > 50)
					{
						ballvec[0].v[2] = 50;
					}
					else if(ballvec[0].v[2] < -50)
					{
						ballvec[0].v[2] = -50;
					}
					//draw dash lines
					int fk = 0;
					bool shotlinestop = false;
					oppopt = oppopt/(oppopt.magnitude())*3;
					while(!shotlinestop)
					{
						glVertex3f(originpt.v[0]+fk*oppopt.v[0],4.6,originpt.v[2]+fk*oppopt.v[2]);
						for(int shotlineind = 1; shotlineind < 16;shotlineind ++)
						{
							if(pow((originpt.v[0]+fk*oppopt.v[0] - ballpos[shotlineind].v[0]),2) + pow((originpt.v[2]+fk*oppopt.v[2] - ballpos[shotlineind].v[2]),2) < 4)
							{
								shotlinestop = true;
							}
						
						}
						if (originpt.v[2]+fk*oppopt.v[2] > 100 || originpt.v[2]+fk*oppopt.v[2] < -100 || originpt.v[0]+fk*oppopt.v[0] < -50 ||originpt.v[0]+fk*oppopt.v[0] > 50)
						{
							shotlinestop = true;
						}
						fk++;
					}
					glEnd();
		
	}
	drawRoomWithTable();
	drawBall();
	glPopMatrix();
	glutSwapBuffers();
}

void cb_idle() 
{
	if(startmove){glutTimerFunc(100,animate,0);}
	glutPostRedisplay();
	
}

void callback_mouse(int button, int state, int x, int y) 
{	
   if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
   {
	   if(!startmove && donemoving){
		   leftclick = 1;
	 temppointerx = x;
	 temppointery = y;
	 viewpt = 1;}
 //glutPostRedisplay();
	}

    if(button == GLUT_LEFT_BUTTON && state == GLUT_UP) {	
			leftclick = 0;
		if(hits){
		hits = false;
		startmove = true;
		//glutTimerFunc(100,animate,0);
		}
	}

	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
	 rightclick = 1;
    pointerx = x;
    pointery = y;
	//glutPostRedisplay();
	}

    if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
		rightclick = 0;
		
	}
   //wheel
	if(button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN){
		wheelchange = 1;
		pointerx = x;
		pointery = y;
		//glutPostRedisplay();
	}
	if(button == GLUT_MIDDLE_BUTTON && state == GLUT_UP){
		wheelchange = 0;
	}
		
}

void callback_motion(int x, int y) {
	if(rightclick == 1 ||wheelchange == 1){
		if(rightclick == 1){
			if(defy < 90){
				if(pointery > y){roangy-=2;}}
			if(pointerx > x){roangx-=2;}
			if(pointerx < x){roangx+=2;}
			if(defy > 20){
				if(pointery < y){roangy+=2;}}
		}
		else if (wheelchange == 1){
			if(zoomfactor >= 0.1){
				if(y <pointery){zoomfactor -= 0.1;
				}
			}
			if(zoomfactor<= 2.5){
				if(y > pointery){zoomfactor += 0.1;
				}
			}
		}
			//mouse point to angle
			theta = (360.0/height*(roangy)*2.0);
			phi = (360.0/width*(roangx)*2.0);
			//restrict the angle within 360 deg
			if(theta>360)theta = fmod((double)theta,360.0);
			if(phi>360)phi = fmod((double)phi,360.0);
			
			//degress to radian  radius = 100 now
			defx = radi*sin(theta*0.0174532)*sin(phi*0.0174532) + ballpos[0].v[0];
			defy = radi*cos(theta*0.0174532) + ballpos[0].v[1];
			defz = radi*sin(theta*0.0174532)*cos(phi*0.0174532)+ ballpos[0].v[2];
			//reduce theta slightly to obtain another point the the same logitude line on the sphere
			GLfloat dt = 1.0;
			GLfloat eyeXtemp = radi*sin(theta*0.0174532-dt)*sin(phi*0.0174532)+ballpos[0].v[0];
			GLfloat eyeYtemp = radi*cos(theta*0.0174532-dt)+ballpos[0].v[1];
			GLfloat eyeZtemp = radi*sin(theta*0.0174532-dt)*cos(phi*0.0174532)+ballpos[0].v[2];
			//connect 2 points to obtain up vector
			upX=eyeXtemp-defx;
			upY=eyeYtemp-defy;
			upZ=eyeZtemp-defz;
			
			 
			pointery = y;
			pointerx = x;
			wheelchange = 0;
	}
	else if(leftclick == 1){

			glGetIntegerv(GL_VIEWPORT,viewport);
//new
			glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
			glGetDoublev(GL_PROJECTION_MATRIX, projection);

			glReadPixels((float)x, (float)y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z_cursor);
			gluUnProject((float)x, (float)y, z_cursor, modelview, projection, viewport, &realx, &realy, &realz);


			if(realx<(ballpos[0].v[0]+6) && realx > (ballpos[0].v[0] - 6))
			{
				if(realy<(ballpos[0].v[2]+6) && realy > (ballpos[0].v[2] - 6))
				{
					hits = true;
				}
			}
	
	}
	
}
void callback_keyboard(unsigned char key, int x, int y) {
   switch(key) {
		case 'c':
			if(viewpt == false){
				viewpt = true;}
			else if (viewpt){
			viewpt = false;}
			break;
		case 'x':
			//restart the game, reset position dx=r dz=2r
			//y = 4.6 always
			for(int temp = 0; temp <= 15;temp++){
				ballpos[temp].v[1] = 4.6;
				ballvec[temp].v[0] = 0.0;
				ballvec[temp].v[1] = 0.0;
				ballvec[temp].v[2] = 0.0;
				pocketed[temp] = false;
			}
			//z
			int accu = 1;
			for(int temp2 = 1; temp2 <= 5; temp2++){
				for(int intemp = 0;intemp <temp2;intemp++){
					ballpos[accu].v[2] = -50.0 - (3.46411*(temp2-1));
					accu++;
				}
			}
			//x
			ballpos[0].v[2] = 50.0;
	ballpos[0].v[0] = 0.0;

	ballpos[1].v[0] = 0.0;

	ballpos[2].v[0] = -2.0;
	ballpos[3].v[0] = 2.0;

	ballpos[4].v[0] = -4.0;
	//balck
	ballpos[5].v[0] = 0.0 ;
	ballpos[6].v[0] = 4.0;

	ballpos[7].v[0] = -6.0;
	ballpos[8].v[0] = -2.0;
	ballpos[9].v[0] = 2.0;
	ballpos[10].v[0] = 6.0 ;

	ballpos[11].v[0] = -8.0;
	ballpos[12].v[0] = -4.0;
	ballpos[13].v[0] = 0.0;
	ballpos[14].v[0] = 4.0 ;
	ballpos[15].v[0] = 8.0;
	playerballid[0] = 0;
	playerballid[1] = 0;
//	pickingable = true;
			break;

/*		case 'b':
			if(enabledl){enabledl = false;}
			else if (enabledl == false){enabledl = true;}
			break;
*/
	}
   
}
void callback_special(int key, int x, int y){
	switch(key)
	{
	case GLUT_KEY_UP:
		if(!donemoving){
			if(ballpos[0].v[2] > - 98){
				ballpos[0].v[2] -=1;}
		}
		break;
	case GLUT_KEY_DOWN:
		if(!donemoving){
			if(ballpos[0].v[2] < 98){
				ballpos[0].v[2] +=1;}
		}
		break;
	case GLUT_KEY_LEFT:
		if(!donemoving){
			if(ballpos[0].v[0] > - 48){
				ballpos[0].v[0] -=1;}
		}
		break;
	case GLUT_KEY_RIGHT:
		if(!donemoving){
			if(ballpos[0].v[0] < 48){
				ballpos[0].v[0] +=1;}
		}
		break;
	case GLUT_KEY_F1:
		if(!donemoving){
			donemoving = true;
		}
		break;
	}
}

void callback_reshape(int w, int h) {
	  glMatrixMode( GL_PROJECTION );
  glLoadIdentity(  );
	width=w; height=h;
  glViewport( 0, 0, width, height );
  gluPerspective( 60*zoomfactor, ( float )width/height, 1, 360. );
  glMatrixMode( GL_MODELVIEW );

}

int main(int argc, char* argv[]) {
   glutInit(&argc, argv); /* initialize GLUT and let it grab arguments */
      glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
  glutInitWindowSize(width, height);
   glutCreateWindow("Pool Animation");
   	        //texture initialization
	ppmLoadCanvas("wood.ppm",&skin);
	
//	ppmLoadCanvas("metal.ppm",&skin2);
	glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, skin.width, skin.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skin.pixels);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	 glEnable( GL_DEPTH_TEST );	
	 glDepthFunc(GL_LEQUAL);

	 ppmLoadCanvas("pool_table.ppm",&skin2);
	 	glGenTextures(1, &texture2);
			glBindTexture(GL_TEXTURE_2D, texture2);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, skin2.width, skin2.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skin2.pixels);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	 glEnable( GL_DEPTH_TEST );	
	 glDepthFunc(GL_LEQUAL);

	 	 	 ppmLoadCanvas("tv.ppm",&tvskin);
	 	glGenTextures(1, &tvtexture);
			glBindTexture(GL_TEXTURE_2D, tvtexture);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, tvskin.width, tvskin.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tvskin.pixels);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	 glEnable( GL_DEPTH_TEST );	
	 glDepthFunc(GL_LEQUAL);

	 	 ppmLoadCanvas("carpet.ppm",&carpetskin);
	 	glGenTextures(1, &carpettexture);
			glBindTexture(GL_TEXTURE_2D, carpettexture);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, carpetskin.width, carpetskin.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, carpetskin.pixels);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	 glEnable( GL_DEPTH_TEST );	
	 glDepthFunc(GL_LEQUAL);

	 	 	 ppmLoadCanvas("wall.ppm",&wallskin);
	 	glGenTextures(1, &walltexture);
			glBindTexture(GL_TEXTURE_2D, walltexture);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, wallskin.width, wallskin.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, wallskin.pixels);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	 glEnable( GL_DEPTH_TEST );	
	 glDepthFunc(GL_LEQUAL);

	 	 	 	 ppmLoadCanvas("ceiling.ppm",&ceilingskin);
	 	glGenTextures(1, &ceilingtexture);
			glBindTexture(GL_TEXTURE_2D, ceilingtexture);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, ceilingskin.width, ceilingskin.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ceilingskin.pixels);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	 glEnable( GL_DEPTH_TEST );	
	 glDepthFunc(GL_LEQUAL);
  
  
	 for (int i = 0; i < 16; i++)
	 {
		 if(i==0)
		 {
			ppmLoadCanvas("BallCue.ppm",&skinb0);
	 		glGenTextures(1, &textureb0);
		 
			glBindTexture(GL_TEXTURE_2D, textureb0);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, skinb0.width, skinb0.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skinb0.pixels);
		}

		 if(i==1)
		 {
			ppmLoadCanvas("Ball1.ppm",&skinb1);
	 		glGenTextures(1, &textureb1);
		 
			glBindTexture(GL_TEXTURE_2D, textureb1);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, skinb1.width, skinb1.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skinb1.pixels);
		}
		else if(i==2)
		{
			ppmLoadCanvas("Ball2.ppm",&skinb2);
	 		glGenTextures(1, &textureb2);
		 
			glBindTexture(GL_TEXTURE_2D, textureb2);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, skinb2.width, skinb2.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skinb2.pixels);
		}
		else if(i==3)
		{
			ppmLoadCanvas("Ball3.ppm",&skinb3);
	 		glGenTextures(1, &textureb3);
		 
			glBindTexture(GL_TEXTURE_2D, textureb3);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, skinb3.width, skinb3.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skinb3.pixels);
		}
		else if(i==4)
		{
			ppmLoadCanvas("Ball4.ppm",&skinb4);
	 		glGenTextures(1, &textureb4);
		 
			glBindTexture(GL_TEXTURE_2D, textureb4);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, skinb4.width, skinb4.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skinb4.pixels);
		}
		else if(i==5)
		{
			ppmLoadCanvas("Ball5.ppm",&skinb5);
	 		glGenTextures(1, &textureb5);
		 
			glBindTexture(GL_TEXTURE_2D, textureb5);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, skinb5.width, skinb5.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skinb5.pixels);
		}
		else if(i==6)
		{
			ppmLoadCanvas("Ball6.ppm",&skinb6);
	 		glGenTextures(1, &textureb6);
		 
			glBindTexture(GL_TEXTURE_2D, textureb6);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, skinb6.width, skinb6.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skinb6.pixels);
		}
		else if(i==7)
		{
			ppmLoadCanvas("Ball7.ppm",&skinb7);
	 		glGenTextures(1, &textureb7);
		 
			glBindTexture(GL_TEXTURE_2D, textureb7);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, skinb7.width, skinb7.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skinb7.pixels);
		}
		else if(i==8)
		{
			ppmLoadCanvas("Ball8.ppm",&skinb8);
	 		glGenTextures(1, &textureb8);
		 
			glBindTexture(GL_TEXTURE_2D, textureb8);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, skinb8.width, skinb8.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skinb8.pixels);
		}
		else if(i==9)
		{
			ppmLoadCanvas("Ball9.ppm",&skinb9);
	 		glGenTextures(1, &textureb9);
		 
			glBindTexture(GL_TEXTURE_2D, textureb9);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, skinb9.width, skinb9.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skinb9.pixels);
		}
		else if(i==10)
		{
			ppmLoadCanvas("Ball10.ppm",&skinb10);
	 		glGenTextures(1, &textureb10);
		 
			glBindTexture(GL_TEXTURE_2D, textureb10);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, skinb10.width, skinb10.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skinb10.pixels);

		}
		else if(i==11)
		{

			ppmLoadCanvas("Ball11.ppm",&skinb11);
	 		glGenTextures(1, &textureb11);
		 
			glBindTexture(GL_TEXTURE_2D, textureb11);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, skinb11.width, skinb11.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skinb11.pixels);

		}
		else if(i==12)
		{
			ppmLoadCanvas("Ball12.ppm",&skinb12);
	 		glGenTextures(1, &textureb12);
		 
			glBindTexture(GL_TEXTURE_2D, textureb12);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, skinb12.width, skinb12.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skinb12.pixels);
		}
		else if(i==13)
		{
			ppmLoadCanvas("Ball13.ppm",&skinb13);
	 		glGenTextures(1, &textureb13);
		 
			glBindTexture(GL_TEXTURE_2D, textureb13);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, skinb13.width, skinb13.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skinb13.pixels);
		}
		else if(i==14)
		{
			ppmLoadCanvas("Ball14.ppm",&skinb14);
	 		glGenTextures(1, &textureb14);
		 
			glBindTexture(GL_TEXTURE_2D, textureb14);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, skinb14.width, skinb14.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skinb14.pixels);
		}
		else if(i==15)
		{

			ppmLoadCanvas("Ball15.ppm",&skinb15);
	 		glGenTextures(1, &textureb15);
		 
			glBindTexture(GL_TEXTURE_2D, textureb15);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//GL_MODULATE
			glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA8, skinb15.width, skinb15.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skinb15.pixels);
		}

		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	 glEnable( GL_DEPTH_TEST );	
	 glDepthFunc(GL_LEQUAL);
	 }
  //sphere
  sphere = gluNewQuadric();
   gluQuadricDrawStyle( sphere, GLU_FILL);
   gluQuadricNormals( sphere, GLU_SMOOTH);
//   gluQuadricOrientation( sphere, GLU_OUTSIDE);
   gluQuadricTexture( sphere, GL_TRUE);

  glLoadIdentity();
  //second part
  	mode = GL_RENDER;
	glGetIntegerv(GL_RENDER_MODE, &mode);
	glMatrixMode (GL_MODELVIEW);
	
	GLfloat mat_specular[] = {0.3, 0.3, 0.3, 1.0};
	GLfloat mat_shininess[] = { 10.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

//	glEnable(GL_DEPTH_TEST);

	glShadeModel(GL_SMOOTH);
		glEnable(GL_LIGHTING);

	//Light 0
	 GLfloat lightColor0[] = {1.0f, 1.0f, 0.7f, 1.0f};
	GLfloat lightPos0[] = {0.0, 200.0, -100.0, 1.0f};
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	glEnable(GL_LIGHT0);
	//ballpos init
	
	for(int temp = 0; temp <= 15;temp++){
		ballpos[temp].v[1] = 4.6;
		iniballpos[temp].v[1] = 4.6;
		ballvec[temp].v[0] = 0.0;
		ballvec[temp].v[1] = 0.0;
		ballvec[temp].v[2] = 0.0;
		pocketed[temp] = false;
		ballangx[temp] = 0;
		ballangz[temp] = 0;
	}
	
	
	
	int accu = 1;
	for(int temp2 = 1; temp2 <= 5; temp2++){
		for(int intemp = 0;intemp <temp2;intemp++){
		ballpos[accu].v[2] = -50.0 - (3.46411*(temp2-1));
		iniballpos[accu].v[2] = -50.0 - (3.46411*(temp2-1));
		accu++;
		}
	}
	//white
	ballpos[0].v[2] = 50.0;
	ballpos[0].v[0] = 0.0;

	ballpos[1].v[0] = 0.0;

	ballpos[2].v[0] = -2.0;
	ballpos[3].v[0] = 2.0;

	ballpos[4].v[0] = -4.0;
	//balck
	ballpos[5].v[0] = 0.0 ;
	ballpos[6].v[0] = 4.0;

	ballpos[7].v[0] = -6.0;
	ballpos[8].v[0] = -2.0;
	ballpos[9].v[0] = 2.0;
	ballpos[10].v[0] = 6.0 ;

	ballpos[11].v[0] = -8.0;
	ballpos[12].v[0] = -4.0;
	ballpos[13].v[0] = 0.0;
	ballpos[14].v[0] = 4.0 ;
	ballpos[15].v[0] = 8.0;
	//ini
	//white
	iniballpos[0].v[2] = 50.0;
	iniballpos[0].v[0] = 0.0;

	iniballpos[1].v[0] = 0.0;

	iniballpos[2].v[0] = -2.0;
	iniballpos[3].v[0] = 2.0;

	iniballpos[4].v[0] = -4.0;
	//balck
	iniballpos[5].v[0] = 0.0 ;
	iniballpos[6].v[0] = 4.0;

	iniballpos[7].v[0] = -6.0;
	iniballpos[8].v[0] = -2.0;
	iniballpos[9].v[0] = 2.0;
	iniballpos[10].v[0] = 6.0 ;

	iniballpos[11].v[0] = -8.0;
	iniballpos[12].v[0] = -4.0;
	iniballpos[13].v[0] = 0.0;
	iniballpos[14].v[0] = 4.0 ;
	iniballpos[15].v[0] = 8.0;
	playerballid[0] = 0;
	playerballid[1] = 0;
   // Print out some useful example information...
   
  
   printf("User Menu:\n");
   printf("Press 'x' to start a new game or resttart game\n");
   printf("Press 'c' to switch between 'top view mode' or 'first person view mode'\n");
   printf("Hold the mouse right button and drag to rotate the camera\n");
   printf("Hold the mouse middle button and drag up or down to zoom in/out \n");
   printf("In case of foul, use the UP,DOWN,LEFT,RIGHT arrow keys to adjust the position of white ball, and press F1 key to confirm \n");
   
   



   glClearColor(0, 0, 0, 0);
   
   // Register Callbacks:
   glutDisplayFunc(callback_display);
   glutMouseFunc(callback_mouse);
   glutMotionFunc(callback_motion);
   glutKeyboardFunc(callback_keyboard);
   glutSpecialFunc(callback_special);
   glutReshapeFunc(callback_reshape);
  // glutTimerFunc(100,animate,0);
   	glutIdleFunc(cb_idle);
	
	
   // Hand control off to GLUT!
   glutMainLoop();
   
   return 0;
}