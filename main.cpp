#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))
#include <iostream>
#include <time.h>
using namespace std;

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
double sf;
double oldSF;
int flag[5];

struct point
{
	double x,y,z;

	point operator+(point p)
	{
	    return {x+p.x, y+p.y, z+p.z};
    }
    point operator-(point p)
	{
	    return {x-p.x, y-p.y, z-p.z};
    }
    point operator*(double n)
	{
	    return {x*n, y*n, z*n};
    }
    point operator/(double n)
    {
        return {x/n, y/n, z/n};
    }
};
point bubbles[5];
point speed[5];

int counter;

double dotMul(point a, point b)
{
    double result;
    result = a.x * b.x + a.y * b.y + a.z * b.z;
    return  result;
}

double magnitude(point p)
{
    return sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
}

double distance2d(point p1, point p2)
{
    double dist;
    dist = sqrt(pow(p1.x-p2.x, 2) + pow(p1.y-p2.y, 2));
    return dist;
}


void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}


void drawLine()
{
    glBegin(GL_LINES);{
        glVertex3f(10,20,0);
        glVertex3f(20,10,0);
    }glEnd();
}


void drawSquare(double a)
{
    //glColor3f(1.0,1.0,0.0);
	/*glBegin(GL_QUADS);{
		glVertex3f( a, a,2);
		glVertex3f( a,-a,2);
		glVertex3f(-a,-a,2);
		glVertex3f(-a, a,2);
	}glEnd(); */

	glBegin(GL_LINES);{
		glVertex3f( a, a,2);
		glVertex3f( a,-a,2);

		glVertex3f( a, a,2);
		glVertex3f(-a,a,2);

		glVertex3f( a,-a,2);
		glVertex3f(-a,-a,2);

		glVertex3f(-a,-a,2);
		glVertex3f(-a, a,2);
	}glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    //glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}


void drawBubble(double x, double y, double radius)
{
    int i;
    struct point points[100];
    //glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=36;i++)
    {
        points[i].x = x + radius*cos(((double)i/36)*2*pi);
        points[i].y = y + radius*sin(((double)i/36)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<36;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}


void drawBubbles()
{
    for (int i = 0; i < 5; i++)
    {
        if (counter >= i*500 )
            drawBubble(bubbles[i].x, bubbles[i].y, 15);
    }
}


void newDraw()
{
    glRotatef(angle, 0, 0, 1);
    glTranslatef(85, 0, 0);
    //glTranslatef(20, 0, 0);

    drawSquare(10);
    glColor3f(1,1,1);
    glRotatef(angle, 0,0,1);
    drawSquare(5);
    glPushMatrix();
    {
        glRotatef(60, 0, 0, 0);
        drawSquare(40);
    }
    glPopMatrix();
    drawSquare(5);
}


void drawCone(double radius,double height,int segments)
{
    int i;
    double shade;
    struct point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
    {
        //create shading effect
        if(i<segments/2)shade=2*(double)i/(double)segments;
        else shade=2*(1.0-(double)i/(double)segments);
        glColor3f(shade,shade,shade);

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0,0,height);
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}


void drawSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}


void drawSS()
{
    glColor3f(1,0,0);
    drawSquare(20);

    glRotatef(angle,0,0,1);
    glTranslatef(110,0,0);
    glRotatef(2*angle,0,0,1);
    glColor3f(0,1,0);
    drawSquare(15);

    glPushMatrix();
    {
        glRotatef(angle,0,0,1);
        glTranslatef(60,0,0);
        glRotatef(2*angle,0,0,1);
        glColor3f(0,0,1);
        drawSquare(10);
    }
    glPopMatrix();

    glRotatef(3*angle,0,0,1);
    glTranslatef(40,0,0);
    glRotatef(4*angle,0,0,1);
    glColor3f(1,1,0);
    drawSquare(5);
}


void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			drawgrid=1-drawgrid;
			break;
        case 'p':
            if (sf > 0)
            {
                oldSF = sf;
                sf = 0;
            }
            else
            {
                sf = oldSF;
            }

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			cameraHeight -= 3.0;
			if(sf > 1)
                sf -= 0.1;
			break;
		case GLUT_KEY_UP:		// up arrow key
			cameraHeight += 3.0;
			if(sf <= 9.9)
                sf += 0.1;
			break;

		case GLUT_KEY_RIGHT:
			cameraAngle += 0.03;
			break;
		case GLUT_KEY_LEFT:
			cameraAngle -= 0.03;
			break;

		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}


void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(0,0,200,	0,0,0,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	//drawAxes();
	//drawGrid();

	//drawLine();
    glColor3f(0,1,0);
    drawSquare(120);

    glColor3f(1,0,0);
    drawCircle(85, 36);
    //drawBubble(15, 36);
    drawBubbles();

    //newDraw();

    //drawCone(20,50,24);

	//drawSphere(30,24,20);


	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}

void speedController()
{
    for (int i = 0; i < 5; i++)
    {
        if (counter > i*500)
        {
            point o = {0,0,0};
            if ( distance2d(bubbles[i], o) <= 85-15) // marking the bubbles that entered the circle.
                flag[i] = 1;

            //Circle - Bubble reflection
            if (flag[i] == 1 && distance2d(bubbles[i], o) >= 85-15)
            {
                point d, n, r; //
                d.x = speed[i].x;
                d.y = speed[i].y;
                d.z = 0;

                n.x = 0 - bubbles[i].x;
                n.y = 0 - bubbles[i].y;
                n.z = 0;

                n = n / magnitude(n); // normalizing

                double temp = dotMul(d, n);
                if (temp < 0)
                {
                    r = d - n * temp * 2;

                    speed[i].x = r.x;
                    speed[i].y = r.y;
                }

            }

            //Bubble-Bubble reflection
            if (flag[i] == 1)
            {
                for (int j = 0; j < 5; j++)
                {
                    if ( flag[j] == 1 && j != i && distance2d(bubbles[i], bubbles[j]) <= 30)
                    {
                        point dn, nn, rn; //
                        dn.x = speed[i].x;
                        dn.y = speed[i].y;
                        dn.z = 0;

                        nn.x = bubbles[j].x - bubbles[i].x;
                        nn.y = bubbles[j].y - bubbles[i].y;
                        nn.z = 0;

                        nn = nn / magnitude(nn); // normalizing

                        double temp = dotMul(dn, nn);
                        if (temp > 0)
                        {
                            rn = dn - nn * temp * 2;

                            speed[i].x = rn.x;
                            speed[i].y = rn.y;
                        }

                    }
                }
            }


            //Square - Bubble reflection
            if (bubbles[i].y >= 120-15)
            {
                speed[i].y *= -1;
            }
            else if (bubbles[i].y < -120+15 )
            {
                speed[i].y *= -1;
            }
            if (bubbles[i].x >= 120-15)
            {
                speed[i].x *= -1;
            }
            else if (bubbles[i].x < -120+15 )
            {
                speed[i].x *= -1;
            }

            bubbles[i].x = bubbles[i].x + sf*speed[i].x;
            bubbles[i].y = bubbles[i].y + sf*speed[i].y;
            //cout << speed[i].x <<"  " <<speed[i].y << endl;

        }

    }
}

void animate(){
    counter++;
	angle+=0.02;
	speedController();
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;
	counter = 0;
	sf = 1;

	for(int i=0; i<5; i++)
    {
        bubbles[i].x = -105;
        bubbles[i].y = -105;
        bubbles[i].z = 0;
        speed[i].x = (double) rand()/(100*RAND_MAX);
        speed[i].y = sqrt(0.0005 - speed[i].x*speed[i].x);
        speed[i].z = 0;

        flag[i] = 0;
    }

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Task 2");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
