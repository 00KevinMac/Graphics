

#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

#define X 0.525731112119133696
#define Z 0.850650808352039932

//Global variables
static int elevation = 0, swing = 0;
static int mouse_x, mouse_y;
bool is_dragging = false;
bool grid_on = false;
bool blue_light = true, green_light = true;




/* vertex data array */
static GLfloat vdata[12][3] = {
  {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
  {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
  {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
};

/* triangle indices */
static int tindices[20][3] = {
  {1,4,0}, {4,9,0}, {4,5,9}, {8,5,4}, {1,8,4},
  {1,10,8}, {10,3,8}, {8,3,5}, {3,2,5}, {3,7,2},
  {3,10,7}, {10,6,7}, {6,11,7}, {6,0,11}, {6,1,0},
  {10,1,6}, {11,0,9}, {2,11,9}, {5,2,9}, {11,2,7}
};

GLfloat mat_specular[] = { 0.6, 0.6, 0.6, 1.0 };
GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_ambient[] = { 0.98, 0.01, 0.01, 1.0 };
GLfloat mat_shininess = 100.0;	/* unused if specular is 0 */

GLfloat light_ambient[] = { 0.0, 0.5, 0.0, 1.0 };
GLfloat light_diffuse[] = { 0.0, 1.0, 0.0, 1.0 };
GLfloat light_specular[] = { 0.8, 0.8, 0.8, 1.0 };
GLfloat light_ambient_1[] = { 0.0, 0.0, 0.5, 1.0 };
GLfloat light_diffuse_1[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat light_specular_1[] = { 0.8, 0.8, 0.8, 1.0 };

GLfloat light_position[] = { 5.0, 2.0, 2.0, 0.0 }; /* directional */
GLfloat light_position_1[] = { -5.0, -1.0, 1.0, 0.0 }; /* directional */

int flat = 1;			/* 0 = smooth shading, 1 = flat shading */
int subdiv = 0;			/* number of subdivisions */

/* normalize a vector of non-zero length */
void normalize(GLfloat v[3])
{
	GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	/* omit explict check for division by zero */

	v[0] /= d; v[1] /= d; v[2] /= d;
}

/* normalized cross product of non-parallel vectors */
void normCrossProd(GLfloat u[3], GLfloat v[3], GLfloat n[3])
{
	n[0] = u[1] * v[2] - u[2] * v[1];
	n[1] = u[2] * v[0] - u[0] * v[2];
	n[2] = u[0] * v[1] - u[1] * v[0];
	normalize(n);
}

void normFace(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3])
{
	GLfloat d1[3], d2[3], n[3];
	int k;
	for (k = 0; k < 3; k++) {
		d1[k] = v1[k] - v2[k];
		d2[k] = v2[k] - v3[k];
	}
	normCrossProd(d1, d2, n);
	glNormal3fv(n);
}

/* draw triangle using face normals */
void drawTriangleFlat(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3])
{
	glBegin(GL_TRIANGLES);
	normFace(v1, v2, v3);
	glVertex3fv(v1);
	glVertex3fv(v2);
	glVertex3fv(v3);
	glEnd();
}

/* draw triangle using sphere normals */
void drawTriangleSmooth(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3])
{
	glBegin(GL_TRIANGLES);
	glNormal3fv(v1);
	glVertex3fv(v1);
	glNormal3fv(v2);
	glVertex3fv(v2);
	glNormal3fv(v3);
	glVertex3fv(v3);
	glEnd();
}

/* recursively subdivide face `depth' times */
/* and draw the resulting triangles */
void subdivide(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], int depth)
{
	GLfloat v12[3], v23[3], v31[3];
	int i;

	if (depth == 0) {
		if (flat == 1)
			drawTriangleFlat(v1, v2, v3);
		else
			drawTriangleSmooth(v1, v2, v3);
		return;
	}

	/* calculate midpoints of each side */
	for (i = 0; i < 3; i++) {
		v12[i] = (v1[i] + v2[i]) / 2.0;
		v23[i] = (v2[i] + v3[i]) / 2.0;
		v31[i] = (v3[i] + v1[i]) / 2.0;
	}
	/* extrude midpoints to lie on unit sphere */
	normalize(v12);
	normalize(v23);
	normalize(v31);

	/* recursively subdivide new triangles */
	subdivide(v1, v12, v31, depth - 1);
	subdivide(v2, v23, v12, depth - 1);
	subdivide(v3, v31, v23, depth - 1);
	subdivide(v12, v23, v31, depth - 1);
}




void display(void)
{
	int i;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, -10, /* eye */
		0.0, 0.0, 0.0,  /* at */
		0.0, 1.0, 0.0); /* up */


	//Elevation
	glRotatef(elevation, 1, 0, 0);

	//Swing
	glRotatef(swing, 0, -1, 0);

	/* drawIco(); */
	/* drawSphere(); */
	glPushMatrix();

	for (i = 0; i < 20; i++) {
		subdivide(&vdata[tindices[i][0]][0],
			&vdata[tindices[i][1]][0],
			&vdata[tindices[i][2]][0],
			subdiv);
	}

	glPopMatrix();

	glFlush();
}


// Get the x and y coordinates of initial click and set dragging to true/false
void mouse(GLint button, GLint state, int x, int y)
{
	// if left button is down, set dragging to true. We also want the x and y coordinates
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		is_dragging = true;
		mouse_x = x;
		mouse_y = y;
	}

	// if we release the left button, set dragging to false
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		is_dragging = false;
	}
}


// To calculate the swing and elevation
void motion(int x, int y)
{
	if (is_dragging)
	{
		glEnable(GL_COLOR_LOGIC_OP);

		// Elevation
		elevation += mouse_y - y;

		// Swing
		swing += mouse_x - x;

		glutPostRedisplay();

		//update old values so the motion is smooth
		mouse_x = x;
		mouse_y = y;

		glDisable(GL_COLOR_LOGIC_OP);
	}

	glFlush();
}



void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, (GLfloat)w / (GLfloat)h, 1, 30.0);
	glMatrixMode(GL_MODELVIEW);
}


void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);


	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient_1);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position_1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse_1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular_1);


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glEnable(GL_DEPTH_TEST);
}

void main_menu_select(int value)
{	
	switch (value)
	{
	case 0: // quit
		exit(0);
		break;
	case 1: // 20 triangles
		subdiv = 0;
		break;
	case 2: // 80 triangles
		subdiv = 1;
		break;
	case 3: // 320 triangles
		subdiv = 2;
		break;
	case 4: // 1280 triangles
		subdiv = 3;
		break;
	case 5: // flat shading
		flat = 1;
		break;
	case 6: // smooth shading
		flat = 0;
		break;
	case 7: // blue light
		blue_light = !blue_light;

		if (blue_light == true)
			glEnable(GL_LIGHT1);
		else
			glDisable(GL_LIGHT1);

		break;
	case 8:	// green light
		green_light = !green_light;

		if (green_light)
			glEnable(GL_LIGHT0);
		else
			glDisable(GL_LIGHT0);

		break;

	default:
		break;
	}
	
	glutPostRedisplay();
}

int main(int argc, char **argv)
{

	int subdivision_menu, shading_menu, lighting_menu;

	/* create window */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Subdivide");

	/* set callbacks */
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

#define SUBDIVISION_MENU_ENTRIES()\
	glutAddMenuEntry("20 Triangles", 1);\
	glutAddMenuEntry("80 Triangles", 2);\
	glutAddMenuEntry("320 Triangles", 3);\
	glutAddMenuEntry("1280 Triangles", 4);

#define SHADING_MENU_ENTRIES()\
	glutAddMenuEntry("Flat", 5);\
	glutAddMenuEntry("Smooth", 6);

#define LIGHTING_MENU_ENTRIES()\
	glutAddMenuEntry("Blue", 7);\
	glutAddMenuEntry("Green", 8);

	subdivision_menu = glutCreateMenu(main_menu_select);
	SUBDIVISION_MENU_ENTRIES();
	shading_menu = glutCreateMenu(main_menu_select);
	SHADING_MENU_ENTRIES();
	lighting_menu = glutCreateMenu(main_menu_select);
	LIGHTING_MENU_ENTRIES();

	glutCreateMenu(main_menu_select);
	glutAddSubMenu("Subdivisions", subdivision_menu);
	glutAddSubMenu("Shading", shading_menu);
	glutAddSubMenu("Toggle Lighting", lighting_menu);
	glutAddMenuEntry("Quit", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	/* initialize GL */
	init();

	glutMainLoop();
	return(0);
}
