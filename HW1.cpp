//Kevin Macfarlane
//Student ID: 108458203



#include <GL/glut.h>
#include <stdlib.h>

class Point
{
	int xPos, yPos;

	public:
	void setCoords(int x, int y)
	{
		xPos = x;
		yPos = y;
	}

	int getX() { return xPos; }
	int getY() { return yPos; }
};


//GLOBAL VARIABLES
Point points[50];
Point currentPoint;
bool leftClick = false;
bool rightClick = false;
bool firstClick = true;
int count = 0;


void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(0.0f, 0.0f, 0.0f);
	glPointSize(5);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(0.0, 500.0, 0.0, 500.0);
	
	glShadeModel(GL_SMOOTH);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);

	//Drawing Points and Lines

	for (int i = 0; i < count; ++i)
	{
		int x = points[i].getX();
		int y = points[i].getY();

		//drawing points
		glBegin(GL_POINTS);
			glColor3f(0.3, 0.3, 0.3);
			glVertex2f(x, glutGet(GLUT_WINDOW_HEIGHT) - y);
		glEnd();

		//drawing lines
		if (i < count - 1)
		{
			glBegin(GL_LINES);
				glColor3f(0.3, 0.3, 0.3);
				glVertex2f(points[i].getX(), glutGet(GLUT_WINDOW_HEIGHT) - points[i].getY());
				glVertex2f(points[i + 1].getX(), glutGet(GLUT_WINDOW_HEIGHT) - points[i + 1].getY());
			glEnd();
		}

		//draw the last line
		if (rightClick)
		{
			glBegin(GL_LINES);
				glColor3f(0.3, 0.3, 0.3);
				glVertex2f(points[count - 1].getX(), glutGet(GLUT_WINDOW_HEIGHT) - points[count - 1].getY());
				glVertex2f(points[count].getX(), glutGet(GLUT_WINDOW_HEIGHT) - points[count].getY());
			glEnd();
		}
	}

	//color the polygon
	if (rightClick && count > 2)
	{
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glBegin(GL_POLYGON);
		for (int x = 0; x < count; ++x)
		{
			float col = float(x) / float(count);
			glColor3f( col, 1 - col, col);
			glVertex2f(points[x].getX(), glutGet(GLUT_WINDOW_HEIGHT) - points[x].getY());
			//glVertex2f(points[x + 1].getX(), glutGet(GLUT_WINDOW_HEIGHT) - points[x + 1].getY());
		}
		glEnd();
	}

	glFlush();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	}
}

void mouse(GLint button, GLint state, int x, int y)
{
	//if there are no points, add one and then have the "rubber band line"
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		glLogicOp(GL_XOR); //need to disable all the lines for the rubber banding

		rightClick = false; // for filling the poly

		//draw the very first point
		if (firstClick)
		{
			firstClick = false;
			count = 0;
			points[count].setCoords(x, y);
			currentPoint.setCoords(x, y); // for the rubber band line
			count++;
		}

		//for the rubber band line
		leftClick = true;
	}

	//add a new point and set the line from previous point to this one
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		glDisable(GL_COLOR_LOGIC_OP);

		//add the point
		points[count].setCoords(x, y);
		currentPoint.setCoords(x, y); // for the rubber band line
		count++;

		//for the rubber band line
		leftClick = false;
	}

	//fill the shape with a color
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		rightClick = true; // for filling the poly

		firstClick = true; // reset so we can reset the count

		//make the last point equal the first one
		points[count].setCoords(points[0].getX(), points[0].getY());
	}

	glutPostRedisplay();
}

void mouseMotion(int x, int y)
{
	if (count > 0 && leftClick)
	{
		glEnable(GL_COLOR_LOGIC_OP);
		//glLogicOp(GL_XOR);

		glBegin(GL_LINES);
			glVertex2f(points[count-1].getX(), glutGet(GLUT_WINDOW_HEIGHT) - points[count - 1].getY());
			glVertex2f(currentPoint.getX(), glutGet(GLUT_WINDOW_HEIGHT) - currentPoint.getY());
			glVertex2f(points[count - 1].getX(), glutGet(GLUT_WINDOW_HEIGHT) - points[count - 1].getY());
			glVertex2f(x, glutGet(GLUT_WINDOW_HEIGHT) - y);
		glEnd();
	}
	
	glFlush();
	glDisable(GL_COLOR_LOGIC_OP);

	currentPoint.setCoords(x, y);

}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);

	glutMainLoop();
	return 0;
}
