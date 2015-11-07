#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <iostream>

using namespace std;

#include <GLUT\glut.h>

#define FRAMES_PER_SECOND 60
const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND;

class Vector3{
public:
	float x, y, z;
	Vector3(float _x, float _y, float _z) { x = _x, y = _y, z = _z; }
	Vector3() { x = y = z = 0; }
	Vector3 operator*(const Vector3& other_vector) { return Vector3(x*other_vector.x, y * other_vector.y, z*other_vector.z); }
	Vector3 operator*(const float f) { return Vector3(x*f, y*f, z*f); }
	Vector3 operator+(const Vector3& other_vector) { return Vector3(x + other_vector.x, y + other_vector.y, z + other_vector.z); }
};

struct Vectors{
	Vector3 v;
	bool e;
	Vectors(Vector3 _v, bool _e){ v = _v, e = _e; }
};

int windowWidth = 800;
int windowHeight = 600;
int mousePositionX;
int mousePositionY;

std::vector<Vector3> points;
std::vector<Vectors> ves;
Vector3 obj(windowWidth / 2, windowHeight / 2, 0);

float time = 0;
int stage = 0;
bool paused = true;
bool loop = false;
int state = 1;
int cycles = 0;
std::vector<Vector3> curve;




float degToRad = 3.14159f / 180.0f;
float radToDeg = 180.f / 3.14159f;

void resetCurve(){
	while (!curve.empty())
		curve.pop_back();
}

template <typename T>
T lerp(T p0, T p1, float time){
	return p0 * (1 - time) + p1 *time;
}

Vector3 interpolateLerp(std::vector<Vector3>& _vector, float time){
	std::vector<Vector3> tempVec;
	if (_vector.size() > 1){
		for (int i = 0, sv = _vector.size() - 1; i < sv; i++){
			tempVec.push_back(lerp(_vector[i], _vector[i + 1], time));
		}

		glBegin(GL_LINE_STRIP);
		glColor3f(0.5, 0.5, 0.5);
		for (int i = 0, sv = tempVec.size(); i < sv; i++){
			glVertex3f(tempVec[i].x, tempVec[i].y, 0);
		}
		glEnd();
		return interpolateLerp(tempVec, time);
	}
	else
		return _vector[0];
}

void interpolate(float dTime){
	std::vector<Vector3> lerps;
	if (stage + 1 < points.size() && state == 1)
	{
		obj = lerp(points[stage], points[stage + 1], time);
	}

	if (stage + 2 < points.size() && state == 2)
	{
	
		lerps.push_back(lerp(points[stage], points[stage + 1], time));
		lerps.push_back(lerp(points[stage + 1], points[stage + 2], time));
		obj = lerp(lerps[0], lerps[1], time);
		glBegin(GL_LINES);
		glColor3f(0.5, 0.5, 0.5);
		glVertex3f(lerps[0].x, lerps[0].y, lerps[0].z);
		glVertex3f(lerps[1].x, lerps[1].y, lerps[1].z);
		glEnd();

		while (!lerps.empty())
			lerps.pop_back();
	}



	if (stage + 1 < points.size() && state == 3)
	{
		//
		//float t, struct point p1, struct point p2, struct point p3, struct point p4
		float t = time;
		float t2 = time*time;
		float t3 = time*time*time;

		/* Catmull Rom spline Calculation */
		Vector3 prev(0, 0, 0);
		Vector3 last(0, 0, 0);
		if (stage == 0)
			prev = points[stage];
		else
			prev = points[stage - 1];

		if (stage + 2 == points.size())
			last = points[points.size() - 1];
		else
			last = points[stage + 2];
		obj.x = ((-t3 + 2 * t2 - t)*(prev.x) + (3 * t3 - 5 * t2 + 2)*(points[stage].x) + (-3 * t3 + 4 * t2 + t)* (points[stage + 1].x) + (t3 - t2)*(last.x)) / 2;
		obj.y = ((-t3 + 2 * t2 - t)*(prev.y) + (3 * t3 - 5 * t2 + 2)*(points[stage].y) + (-3 * t3 + 4 * t2 + t)* (points[stage + 1].y) + (t3 - t2)*(last.y)) / 2;
	}

	if (stage + 3 < points.size() && state == 4)
	{
		
		lerps.push_back(lerp(points[stage], points[stage + 1], time));
		lerps.push_back(lerp(points[stage + 1], points[stage + 2], time));
		lerps.push_back(lerp(points[stage + 2], points[stage + 3], time));

		lerps.push_back(lerp(lerps[0], lerps[1], time));
		lerps.push_back(lerp(lerps[1], lerps[2], time));

	
		obj = lerp(lerps[3], lerps[4], time);

		for (int i = 0; i < 4; i++)
		{
			if (i != 2)
			{
				glBegin(GL_LINES);
				glColor3f(0.5, 0.5, 0.5);
				glVertex3f(lerps[i].x, lerps[i].y, lerps[i].z);
				glVertex3f(lerps[i + 1].x, lerps[i + 1].y, lerps[i + 1].z);
				glEnd();
			}
		}

		while (!lerps.empty())
			lerps.pop_back();
	}


	if (state == 5)
	{
		for (int i = 0, s = points.size() - 1; i < s; i++)
			lerps.push_back(lerp(points[i], points[i + 1], time));
		glBegin(GL_LINE_STRIP);
		glColor3f(0.5, 0.5, 0.5);
		for (int i = 0, s = lerps.size(); i < s; i++)
			glVertex3f(lerps[i].x, lerps[i].y, 0);
		glEnd();
		obj = interpolateLerp(lerps, time);

		while (!lerps.empty())
			lerps.pop_back();
	}


	//Looping
	if (loop && cycles >= points.size() - 1)
	{
		stage = 0;
		time = 0;
		cycles = 0;
	}
	else if (state == 2 && loop)
	{
		if (points.size() % 2 == 0)
		{
			//last point is invalid
			if (cycles >= points.size() - 3)
			{
				stage = 0;
				time = 0;
				cycles = 0;
			}
		}
		else if (cycles >= points.size() - 2)
		{
			stage = 0;
			time = 0;
			cycles = 0;
		}
	}
	else if (state == 4 && loop)
	{
		if (cycles >= points.size() - 3)
		{
			stage = 0;
			time = 0;
			cycles = 0;
		}
	}

	if (!paused)
		time += (float)dTime / 1000;

	if (time >= 1.0f)
	{
		time = 0;
		if (state == 2)
		{
			stage += state;
			cycles += state;
		}
		else if (state == 4)
		{
			stage += 3;
			cycles += 3;
		}
		else
		{
			stage++;
			cycles++;
		}
	}

}

void DisplayCallbackFunction(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	interpolate(FRAME_DELAY);

	glBegin(GL_LINE_STRIP);
	glColor3f(0, 0, 0);
	for (unsigned int i = 0, sv = points.size() - 1; i < sv; i++)
		glVertex3f(points[i].x, points[i].y, 0);
	
	if (state == 2 && points.size() % 2 == 0)
		glColor3f(0.3, 0.3, 0.3);
	if (state == 4 && (points.size() - 4) % 3 != 0)
		glColor3f(0.3, 0.3, 0.3);
	glVertex3f(points[points.size() - 1].x, points[points.size() - 1].y, 0);
	glEnd();


	glBegin(GL_POINTS);
	glColor3f(1, 1, 1);
	for (unsigned int i = 0, sv = ves.size() - 1; i < sv; i++)
		glVertex3f(ves[i].v.x, ves[i].v.y, 0);
	glEnd();

	glBegin(GL_LINE);
	glVertex3f(0, 0, -1);
	glVertex3f(1.5, 1.1, -1);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.75f, 1.0f, 0.0f);
	glVertex3f(obj.x - 10, obj.y - 10, 0);
	glVertex3f(obj.x - 10, obj.y + 10, 0);
	glVertex3f(obj.x + 10, obj.y + 10, 0);
	glVertex3f(obj.x + 10, obj.y - 10, 0);
	glEnd();

	for (int i = 0; i < points.size(); i++)
	{
		glBegin(GL_QUADS);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(points[i].x - 5, points[i].y - 5, 0);
		glVertex3f(points[i].x - 5, points[i].y + 5, 0);
		glVertex3f(points[i].x + 5, points[i].y + 5, 0);
		glVertex3f(points[i].x + 5, points[i].y - 5, 0);
		glEnd();
	}

	curve.push_back(obj);

	glBegin(GL_LINE_STRIP);


	curve.push_back(obj);

	glBegin(GL_LINE_STRIP);
	glColor3f(0, 0, 1);
	for (int i = 0; i < curve.size(); i++)
	{
		glVertex3f(curve[i].x, curve[i].y, 0);
		if (i > 32000)
		{
			resetCurve();
			break;
		}
	}
	glEnd();

	glutSwapBuffers();
}

void reset()
{
	stage = 0;
	time = 0;
	obj = points[0];
	cycles = 0;

	resetCurve();
}

void connect()
{
	while (!points.empty())
		points.pop_back();

	for (int i = 0, s = ves.size(); i < s; i++)
		if (ves[i].v.x > mousePositionX - 5 && ves[i].v.x < mousePositionX + 5)
			if (ves[i].v.y >windowHeight - mousePositionY - 5 && ves[i].v.y < windowHeight - mousePositionY + 5)
				ves[i].e = !ves[i].e;

	for (int i = 0, s = ves.size(); i < s; i++)
		if (ves[i].e == true)
			points.push_back(ves[i].v);

	if (points.size() == 0)
		points.push_back(obj);

	resetCurve();
}

void KeyboardCallbackFunction(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 32: // the space bar
		paused = !paused;
		break;
	case 'w':
		reset();
		break;
	case 'e':
		while (!points.empty())
			points.pop_back();
		while (!ves.empty())
			ves.pop_back();
		points.push_back(obj);
		ves.push_back(Vectors(obj, true));
		stage = 0;
		time = 0;
		break;
	case 'r':
		loop = !loop;
		break;
	case '1':
		state = 1;
		reset();
		break;
	case 'f':
		connect();
		break;
	case '2':
		state = 2;
		reset();
		break;
	case '3':
		state = 3;
		reset();
		break;
	case '4':
		state = 4;
		reset();
		break;
	case '5':
		state = 5;
		reset();
		break;
	case 27: // the escape key
	case 'q': // the 'q' key
		exit(0);
		break;
	}
}

void TimerCallbackFunction(int value)
{
	glutPostRedisplay();
	glutTimerFunc(FRAME_DELAY, TimerCallbackFunction, 0);
}

void WindowReshapeCallbackFunction(int w, int h)
{
	float asp = (float)w / (float)h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(0, w, 0, h);

	windowWidth = w;
	windowHeight = h;
	glViewport(0, 0, windowWidth, windowHeight);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void MouseMoved(int x, int y)
{
	mousePositionX = x;
	mousePositionY = y;
}

void MouseClickCallbackFunction(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		points.push_back(Vector3(x, windowHeight - y, 0));
		ves.push_back(Vectors(Vector3(x, windowHeight - y, 0), true));
	}
}

void init()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);
}

int main(int argc, char **argv)
{
	/* initialize the window and OpenGL properly */
	glutInit(&argc, argv);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("INFR1350U - Example");

	/* set up our function callbacks */
	glutDisplayFunc(DisplayCallbackFunction);
	glutKeyboardFunc(KeyboardCallbackFunction);
	glutReshapeFunc(WindowReshapeCallbackFunction);
	glutMouseFunc(MouseClickCallbackFunction);
	glutTimerFunc(1, TimerCallbackFunction, 0);
	glutPassiveMotionFunc(MouseMoved);

	init(); //Setup OpenGL States

	//fill vectors with obj to give first point
	points.push_back(obj);
	ves.push_back(Vectors(obj, true));

	/* start the event handler */
	glutMainLoop();
	return 0;
}



