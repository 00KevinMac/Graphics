/**
 *  teaambient.c
 *  This program renders three lighted, shaded teapots, with
 *  different ambient values.
 */
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <vector>
#include <iostream>


# define M_PI           3.14159265358979323846  /* pi */


 //Global variables
static int elevation = 0, swing = 0;
static int mouse_x, mouse_y;
bool is_dragging = false;
bool grid_on = false;
std::vector<float> look_v = { 0, 0, 0 }, v2 = { 0, 0, 0 }, up_v = { 0, 1, 0 };
const float distance = 10;


/*  Initialize light source and lighting model.
 */
void
myinit(void)
{
	GLfloat light_ambient[] =
	{ 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] =
	{ 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] =
	{ 1.0, 1.0, 1.0, 1.0 };
	/* light_position is NOT default value */
	GLfloat light_position[] =
	{ 1.0, 1.0, 0.0, 0.0 };
	GLfloat global_ambient[] =
	{ 0.75, 0.75, 0.75, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

	glFrontFace(GL_CW);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
}


void
display(void)
{
	GLfloat low_ambient[] =
	{ 0.1, 0.1, 0.1, 1.0 };
	GLfloat more_ambient[] =
	{ 0.4, 0.4, 0.4, 1.0 };
	GLfloat most_ambient[] =
	{ 1.0, 1.0, 1.0, 1.0 };

	glEnable(GL_COLOR_MATERIAL);
	glColor3f(0.8, 0.19, 0.18); // Nice Red

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	/*  material has moderate ambient reflection */
	glMaterialfv(GL_FRONT, GL_AMBIENT, more_ambient);


	//glPushMatrix();
	glLoadIdentity();
	//gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0); //Check the last three values? Cross product?



	// Calculate the camera position using the distance and angles
	float camX = distance * -sinf(swing*(M_PI / 180)) * cosf((elevation)*(M_PI / 180));
	float camY = distance * -sinf((elevation)*(M_PI / 180));
	float camZ = -distance * cosf((swing)*(M_PI / 180)) * cosf((elevation)*(M_PI / 180));

	if (abs(elevation) >= 90 && abs(elevation) < 270)
	{
		up_v = { 0, -1, 0 };
	}
	else
		up_v = { 0, 1, 0 };

	// Set the camera position and lookat point
	gluLookAt(camX, camY, camZ,   // Camera position
		0.0, 0.0, 0.0,    // Look at point
		up_v[0], up_v[1], up_v[2]);   // Up vector



	//Elevation
	//glRotatef(elevation, 1, 0, 0);

	//Swing
	//glRotatef(swing, 0, 1, 0);

	//Show Teapot
	glutSolidTeapot(1.0);

	//Draw Gridlines
	if (grid_on)
	{
		glLineWidth(1);
		glBegin(GL_LINES);

		glColor3f(0.5, 0.5, 0.5);//grey lines

		for (float i = -2; i <= 2.2; i += 0.2f)
		{
			// X-Y Plane
			glVertex3f(i, 2, 0);
			glVertex3f(i, -2, 0);

			glVertex3f(2, i, 0);
			glVertex3f(-2, i, 0);

			// X-Z Plane
			glVertex3f(i, 0, 2);
			glVertex3f(i, 0, -2);

			glVertex3f(2, 0, i);
			glVertex3f(-2, 0, i);

			// Y-Z Plane
			glVertex3f(0, i, 2);
			glVertex3f(0, i, -2);

			glVertex3f(0, 2, i);
			glVertex3f(0, -2, i);
		}

		glEnd();
	}

	//glPopMatrix();

	glFlush();

	glutSwapBuffers();
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
		elevation = (elevation + mouse_y - y) % 360;

		// Swing
		swing = (swing + mouse_x - x) % 360;

		glutPostRedisplay();

		//update old values so the motion is smooth
		mouse_x = x;
		mouse_y = y;

		glDisable(GL_COLOR_LOGIC_OP);
	}

	glFlush();
}


// Menu system to toggle grid on and off, and to quit the program
void main_menu_select(int value)
{
	if (value == 1)
	{
		grid_on = !grid_on;
		glutPostRedisplay();
	}
	else if (value == 0)
	{
		exit(0);
	}
}


//Press escape to exit the program
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	}
}


//Reshaping the window
void
myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, (GLfloat)w / (GLfloat)h, 1, 30.0);
	glMatrixMode(GL_MODELVIEW);
}



/*  Main Loop
 *  Open window with initial window size, title bar,
 *  RGBA display mode, and handle input events.
 */
int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow(argv[0]);

	myinit();

	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);

	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutCreateMenu(main_menu_select);
	glutAddMenuEntry("Toggle Grid", 1);
	glutAddMenuEntry("Quit", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();
	return 0;             /* ANSI C requires main to return int. */
}
