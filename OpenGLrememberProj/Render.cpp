#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"
#include <list>
#include <time.h>
#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"
using namespace std;
bool textureMode = true;
bool lightMode = true;

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света

class Point {
public:
	double x, y, z;
	Point(double x = 0, double y = 0, double z = 0)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

class Branch {
public:
	double x, y, z;
	int deph;

	int childNum;
	double ang;
	double len;
	Branch* parent;
	Point* par;
	~Branch()
	{
		//delete parent;
	}
};
class Trunk {
public:
	double x, y, z, height, maxHeight;

};
class Tree {
private:
	double _x, _y, _z;
	list<Branch*>* _branches;


	Point* _parent;
public:
	Trunk* _trunk;
	int _deph = 0;
	int maxDeph;
	Tree(double x, double y, double z)
	{
		_branches = new list<Branch*>;
		_trunk = new Trunk();
		_x = x;
		_y = y;
		_z = z;
		_trunk->x = x;
		_trunk->y = y;
		_trunk->z = z;

		Branch* b = new Branch();
		b->deph = 4;
		b->x = x;
		b->y = y;
		b->z = z;
		b->childNum = 3;
		b->ang = 1.1;
		b->len = 10;

		_branches->push_back(b);

	}
	void addTrunk(double height)
	{
		_trunk->height = height;
		_branches->front()->z = height;
	}
	void addBranch(double x, double y, double z, int deph, int childNum, double ang, double len, Point parent)
	{
		if (deph > this->_deph)
		{
			this->maxDeph = deph;
			this->_deph = deph;
		}
		_parent = new Point(parent.x, parent.y, parent.z);
		Branch* b = new Branch();
		b->deph = deph;
		b->x = x;
		b->y = y;
		b->z = z;
		b->childNum = childNum;
		b->ang = ang;
		b->len = len;
		b->par = _parent;

		if (childNum == 1)
		{
			b->parent = _branches->back();
		}
		else
		{
			b->parent = _branches->back()->parent;
		}
		_branches->push_back(b);

	}
	void DrawTrunk() {
		double len = 5;
		double s = _trunk->height;
		glColor3d(0.61, 0.262, 0);

		glBegin(GL_POLYGON);
		glVertex3d(_x - (len / 20), _y - (len / 20), 0); //1
		glVertex3d(_x - (len / 20), _y + (len / 20), 0); //2
		glVertex3d(_x + (len / 20), _y + (len / 20), 0); //3
		glVertex3d(_x + (len / 20), _y - (len / 20), 0); //4
		glEnd();
		glBegin(GL_POLYGON);
		glVertex3d(_x - (len / 20), _y - (len / 20), s); //1'
		glVertex3d(_x - (len / 20), _y + (len / 20), s); //2'
		glVertex3d(_x + (len / 20), _y + (len / 20), s); //3'
		glVertex3d(_x + (len / 20), _y - (len / 20), s); //4'
		glEnd();
		glNormal3f(0, -1, 0);
		glBegin(GL_POLYGON);
		glVertex3d(_x - (len / 20), _y - (len / 20), 0); //1
		glVertex3d(_x - (len / 20), _y - (len / 20), s); //1'
		glVertex3d(_x + (len / 20), _y - (len / 20), s); //4'
		glVertex3d(_x + (len / 20), _y - (len / 20), 0); //4
		glEnd();
		glNormal3f(0, 1, 0);
		glBegin(GL_POLYGON);
		glVertex3d(_x - (len / 20), _y + (len / 20), 0); //2
		glVertex3d(_x - (len / 20), _y + (len / 20), s); //2'
		glVertex3d(_x + (len / 20), _y + (len / 20), s); //3'
		glVertex3d(_x + (len / 20), _y + (len / 20), 0); //3
		glEnd();
		glNormal3f(-1, 0, 0);
		glBegin(GL_POLYGON);
		glVertex3d(_x - (len / 20), _y - (len / 20), 0); //1
		glVertex3d(_x - (len / 20), _y - (len / 20), s); //1'
		glVertex3d(_x - (len / 20), _y + (len / 20), s); //2'
		glVertex3d(_x - (len / 20), _y + (len / 20), 0); //2
		glEnd();
		glNormal3f(1, 0, 0);
		glBegin(GL_POLYGON);
		glVertex3d(_x + (len / 20), _y - (len / 20), 0); //4
		glVertex3d(_x + (len / 20), _y - (len / 20), s); //4'
		glVertex3d(_x + (len / 20), _y + (len / 20), s); //3'
		glVertex3d(_x + (len / 20), _y + (len / 20), 0); //3
		glEnd();
	}
	void Draw()
	{
		DrawTrunk();
		list<Branch*>::iterator it;
		for (it = _branches->begin(); it != _branches->end(); it++)
		{

			if (it == _branches->begin())
				continue;
			glColor3d(0.61, 0.262, 0);
			if (this->_deph != -1)
			{
				if ((*it)->deph < this->_deph)
					continue;
			}
			glBegin(GL_LINES);
			glVertex3d((*it)->par->x, (*it)->par->y, (*it)->par->z);
			glVertex3d((*it)->x, (*it)->y, (*it)->z);
			glEnd();


			glBegin(GL_POLYGON);
			glVertex3d((*it)->par->x + cos((*it)->ang + PI / 2) * ((*it)->len / 20), (*it)->par->y - ((*it)->len / 20), (*it)->par->z + sin((*it)->ang + PI / 2) * ((*it)->len / 20));//1
			glVertex3d((*it)->par->x + cos((*it)->ang + PI / 2) * ((*it)->len / 20), (*it)->par->y + ((*it)->len / 20), (*it)->par->z + sin((*it)->ang + PI / 2) * ((*it)->len / 20));//2
			glVertex3d((*it)->par->x + cos((*it)->ang - PI / 2) * ((*it)->len / 20), (*it)->par->y + ((*it)->len / 20), (*it)->par->z + sin((*it)->ang - PI / 2) * ((*it)->len / 20));//3
			glVertex3d((*it)->par->x + cos((*it)->ang - PI / 2) * ((*it)->len / 20), (*it)->par->y - ((*it)->len / 20), (*it)->par->z + sin((*it)->ang - PI / 2) * ((*it)->len / 20));//4
			glEnd();
			glBegin(GL_POLYGON);
			glVertex3d((*it)->x + cos((*it)->ang + PI / 2) * ((*it)->len / 20 / 2), (*it)->y - ((*it)->len / 20 / 2), (*it)->z + sin((*it)->ang + PI / 2) * ((*it)->len / 20 / 2));//1'
			glVertex3d((*it)->x + cos((*it)->ang + PI / 2) * ((*it)->len / 20 / 2), (*it)->y + ((*it)->len / 20 / 2), (*it)->z + sin((*it)->ang + PI / 2) * ((*it)->len / 20 / 2));//2'
			glVertex3d((*it)->x + cos((*it)->ang - PI / 2) * ((*it)->len / 20 / 2), (*it)->y + ((*it)->len / 20 / 2), (*it)->z + sin((*it)->ang - PI / 2) * ((*it)->len / 20 / 2));//3'
			glVertex3d((*it)->x + cos((*it)->ang - PI / 2) * ((*it)->len / 20 / 2), (*it)->y - ((*it)->len / 20 / 2), (*it)->z + sin((*it)->ang - PI / 2) * ((*it)->len / 20 / 2));//4'
			glEnd();
			glNormal3f(0, -1, 0);
			glBegin(GL_POLYGON);
			glVertex3d((*it)->par->x + cos((*it)->ang + PI / 2) * ((*it)->len / 20), (*it)->par->y - ((*it)->len / 20), (*it)->par->z + sin((*it)->ang + PI / 2) * ((*it)->len / 20));//1
			glVertex3d((*it)->x + cos((*it)->ang + PI / 2) * ((*it)->len / 20 / 2), (*it)->y - ((*it)->len / 20 / 2), (*it)->z + sin((*it)->ang + PI / 2) * ((*it)->len / 20 / 2));//1'
			glVertex3d((*it)->x + cos((*it)->ang - PI / 2) * ((*it)->len / 20 / 2), (*it)->y - ((*it)->len / 20 / 2), (*it)->z + sin((*it)->ang - PI / 2) * ((*it)->len / 20 / 2));//4'
			glVertex3d((*it)->par->x + cos((*it)->ang - PI / 2) * ((*it)->len / 20), (*it)->par->y - ((*it)->len / 20), (*it)->par->z + sin((*it)->ang - PI / 2) * ((*it)->len / 20));//4
			glEnd();
			glNormal3f(0, 1, 0);
			glBegin(GL_POLYGON);
			glVertex3d((*it)->par->x + cos((*it)->ang + PI / 2) * ((*it)->len / 20), (*it)->par->y + ((*it)->len / 20), (*it)->par->z + sin((*it)->ang + PI / 2) * ((*it)->len / 20));//2
			glVertex3d((*it)->x + cos((*it)->ang + PI / 2) * ((*it)->len / 20 / 2), (*it)->y + ((*it)->len / 20 / 2), (*it)->z + sin((*it)->ang + PI / 2) * ((*it)->len / 20 / 2));//2'
			glVertex3d((*it)->x + cos((*it)->ang - PI / 2) * ((*it)->len / 20 / 2), (*it)->y + ((*it)->len / 20 / 2), (*it)->z + sin((*it)->ang - PI / 2) * ((*it)->len / 20 / 2));//3'
			glVertex3d((*it)->par->x + cos((*it)->ang - PI / 2) * ((*it)->len / 20), (*it)->par->y + ((*it)->len / 20), (*it)->par->z + sin((*it)->ang - PI / 2) * ((*it)->len / 20));//3
			glEnd();
			glNormal3f(-1, 0, 0);
			glBegin(GL_POLYGON);
			glVertex3d((*it)->par->x + cos((*it)->ang + PI / 2) * ((*it)->len / 20), (*it)->par->y - ((*it)->len / 20), (*it)->par->z + sin((*it)->ang + PI / 2) * ((*it)->len / 20));//1
			glVertex3d((*it)->x + cos((*it)->ang + PI / 2) * ((*it)->len / 20 / 2), (*it)->y - ((*it)->len / 20 / 2), (*it)->z + sin((*it)->ang + PI / 2) * ((*it)->len / 20 / 2));//1'
			glVertex3d((*it)->x + cos((*it)->ang + PI / 2) * ((*it)->len / 20 / 2), (*it)->y + ((*it)->len / 20 / 2), (*it)->z + sin((*it)->ang + PI / 2) * ((*it)->len / 20 / 2));//2'
			glVertex3d((*it)->par->x + cos((*it)->ang + PI / 2) * ((*it)->len / 20), (*it)->par->y + ((*it)->len / 20), (*it)->par->z + sin((*it)->ang + PI / 2) * ((*it)->len / 20));//2
			glEnd();
			glNormal3f(1, 0, 0);
			glBegin(GL_POLYGON);
			glVertex3d((*it)->par->x + cos((*it)->ang - PI / 2) * ((*it)->len / 20), (*it)->par->y - ((*it)->len / 20), (*it)->par->z + sin((*it)->ang - PI / 2) * ((*it)->len / 20));//4
			glVertex3d((*it)->x + cos((*it)->ang - PI / 2) * ((*it)->len / 20 / 2), (*it)->y - ((*it)->len / 20 / 2), (*it)->z + sin((*it)->ang - PI / 2) * ((*it)->len / 20 / 2));//4'
			glVertex3d((*it)->x + cos((*it)->ang - PI / 2) * ((*it)->len / 20 / 2), (*it)->y + ((*it)->len / 20 / 2), (*it)->z + sin((*it)->ang - PI / 2) * ((*it)->len / 20 / 2));//3'
			glVertex3d((*it)->par->x + cos((*it)->ang - PI / 2) * ((*it)->len / 20), (*it)->par->y + ((*it)->len / 20), (*it)->par->z + sin((*it)->ang - PI / 2) * ((*it)->len / 20));//3
			glEnd();

		}
	}
	~Tree()
	{
		list<Branch*>::iterator it;
		for (it = _branches->begin(); it != _branches->end(); it++)
		{
			delete* it;
		}
		delete _parent;
		delete _trunk;
	}

};


//старые координаты мыши
int mouseX = 0, mouseY = 0;
list<Tree*> trees;
void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}


GLuint texId;

//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	

	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE *texarray;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("grass.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//генерируем ИД для текстуры
	glGenTextures(1, &texId);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//отчистка памяти
	free(texCharArray);
	free(texarray);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}
//=================================================================================================================================================================================================================
//const double PI = 3.1415926535897;

double trunk(double x, double y) {
	double len = 5;
	int size = rand() % 100 + 20;
	double s = size / 10./2;

	//glColor3d(0.61, 0.262, 0);
	//glBegin(GL_POLYGON);
	//glVertex3d(x - (len / 20), y - (len / 20), 0); //1
	//glVertex3d(x - (len / 20), y + (len / 20), 0); //2
	//glVertex3d(x + (len / 20), y + (len / 20), 0); //3
	//glVertex3d(x + (len / 20), y - (len / 20), 0); //4
	//glEnd();
	//glBegin(GL_POLYGON);
	//glVertex3d(x - (len / 20), y - (len / 20), s); //1'
	//glVertex3d(x - (len / 20), y + (len / 20), s); //2'
	//glVertex3d(x + (len / 20), y + (len / 20), s); //3'
	//glVertex3d(x + (len / 20), y - (len / 20), s); //4'
	//glEnd();
	//glBegin(GL_POLYGON);
	//glVertex3d(x - (len / 20), y - (len / 20), 0); //1
	//glVertex3d(x - (len / 20), y - (len / 20), s); //1'
	//glVertex3d(x + (len / 20), y - (len / 20), s); //4'
	//glVertex3d(x + (len / 20), y - (len / 20), 0); //4
	//glEnd();
	//glBegin(GL_POLYGON);
	//glVertex3d(x - (len / 20), y + (len / 20), 0); //2
	//glVertex3d(x - (len / 20), y + (len / 20), s); //2'
	//glVertex3d(x + (len / 20), y + (len / 20), s); //3'
	//glVertex3d(x + (len / 20), y + (len / 20), 0); //3
	//glEnd();
	//glBegin(GL_POLYGON);
	//glVertex3d(x - (len / 20), y - (len / 20), 0); //1
	//glVertex3d(x - (len / 20), y - (len / 20), s); //1'
	//glVertex3d(x - (len / 20), y + (len / 20), s); //2'
	//glVertex3d(x - (len / 20), y + (len / 20), 0); //2
	//glEnd();
	//glBegin(GL_POLYGON);
	//glVertex3d(x + (len / 20), y - (len / 20), 0); //4
	//glVertex3d(x + (len / 20), y - (len / 20), s); //4'
	//glVertex3d(x + (len / 20), y + (len / 20), s); //3'
	//glVertex3d(x + (len / 20), y + (len / 20), 0); //3
	//glEnd();

	return s;
}
void branch(double x, double y, double z, double angle, double len, int deph, Tree* t) {


	double angp = 0.5;
	int count = 0;
	for (int i = -1; i < 5; i += 2) {
		count++;
		int d = rand() % 2 + 1;

		if (deph > 0)
		{
			if (d == 1)
				branch(x + cos(angle + i * angp) * len / 2, y, z + sin(angle + i * angp) * len / 2, angle + i * angp, len / 2, deph - 1, t);
			else
				branch(x, y + cos(angle + i * angp) * len / 2, z + sin(angle + i * angp) * len / 2, angle + i * angp, len / 2, deph - 1, t);

		}


		//glBegin(GL_LINES);
		//glVertex3d(x, y, z);
		double nx, ny, nz;
		if (d == 1)
		{
			nx = x + cos(angle + i * angp) * len / 2;
			ny = y;
			nz = z + sin(angle + i * angp) * len / 2;



		}
		else
		{
			nx = x;
			ny = y + cos(angle + i * angp) * len / 2;
			nz = z + sin(angle + i * angp) * len / 2;
		}

		//glVertex3d(nx, ny, nz);
		//glEnd();
		Point p(x, y, z);
		t->addBranch(nx, ny, nz, deph, count, angle + i * angp, len, p);

		//glColor3d(0.61, 0.262, 0);
		//glBegin(GL_POLYGON);
		//glVertex3d(x + cos(angle + i * angp + PI / 2) * (len / 10), y - (len / 20), z + sin(angle + i * angp + PI / 2) * (len / 20)); //1
		//glVertex3d(x + cos(angle + i * angp + PI / 2) * (len / 20), y + (len / 20), z + sin(angle + i * angp + PI / 2) * (len / 20));//2
		//glVertex3d(x + cos(angle + i * angp - PI / 2) * (len / 20), y + (len / 20), z + sin(angle + i * angp - PI / 2) * (len / 20));//3
		//glVertex3d(x + cos(angle + i * angp - PI / 2) * (len / 20), y - (len / 20), z + sin(angle + i * angp - PI / 2) * (len / 20));//4
		//glEnd(); //низ
		//glBegin(GL_POLYGON);
		//glVertex3d(nx + cos(angle + i * angp + PI / 2) * (len / 20), ny - (len / 20), nz + sin(angle + i * angp + PI / 2) * (len / 20));//1'
		//glVertex3d(nx + cos(angle + i * angp + PI / 2) * (len / 20), ny + (len / 20), nz + sin(angle + i * angp + PI / 2) * (len / 20));//2'
		//glVertex3d(nx + cos(angle + i * angp - PI / 2) * (len / 20), ny + (len / 20), nz + sin(angle + i * angp - PI / 2) * (len / 20));//3'
		//glVertex3d(nx + cos(angle + i * angp - PI / 2) * (len / 20), ny - (len / 20), nz + sin(angle + i * angp - PI / 2) * (len / 20));//4'
		//glEnd();//верх
		//glBegin(GL_POLYGON);
		//glVertex3d(x + cos(angle + i * angp + PI / 2) * (len / 20), y - (len / 20), z + sin(angle + i * angp + PI / 2) * (len / 20)); //1
		//glVertex3d(nx + cos(angle + i * angp + PI / 2) * (len / 20), ny - (len / 20), nz + sin(angle + i * angp + PI / 2) * (len / 20));//1'
		//glVertex3d(nx + cos(angle + i * angp - PI / 2) * (len / 20), ny - (len / 20), nz + sin(angle + i * angp - PI / 2) * (len / 20));//4'
		//glVertex3d(x + cos(angle + i * angp - PI / 2) * (len / 20), y - (len / 20), z + sin(angle + i * angp - PI / 2) * (len / 20));//4
		//glEnd();//перед
		//glBegin(GL_POLYGON);
		//glVertex3d(x + cos(angle + i * angp + PI / 2) * (len / 20), y + (len / 20), z + sin(angle + i * angp + PI / 2) * (len / 20));//2
		//glVertex3d(nx + cos(angle + i * angp + PI / 2) * (len / 20), ny + (len / 20), nz + sin(angle + i * angp + PI / 2) * (len / 20));//2'
		//glVertex3d(nx + cos(angle + i * angp - PI / 2) * (len / 20), ny + (len / 20), nz + sin(angle + i * angp - PI / 2) * (len / 20));//3'
		//glVertex3d(x + cos(angle + i * angp - PI / 2) * (len / 20), y + (len / 20), z + sin(angle + i * angp - PI / 2) * (len / 20));//3
		//glEnd();//зад
		//glBegin(GL_POLYGON);
		//glVertex3d(x + cos(angle + i * angp + PI / 2) * (len / 20), y - (len / 20), z + sin(angle + i * angp + PI / 2) * (len / 20)); //1
		//glVertex3d(nx + cos(angle + i * angp + PI / 2) * (len / 20), ny - (len / 20), nz + sin(angle + i * angp + PI / 2) * (len / 20));//1'
		//glVertex3d(nx + cos(angle + i * angp + PI / 2) * (len / 20), ny + (len / 20), nz + sin(angle + i * angp + PI / 2) * (len / 20));//2'
		//glVertex3d(x + cos(angle + i * angp + PI / 2) * (len / 20), y + (len / 20), z + sin(angle + i * angp + PI / 2) * (len / 20));//2
		//glEnd();//левый
		//glBegin(GL_POLYGON);
		//glVertex3d(x + cos(angle + i * angp - PI / 2) * (len / 20), y - (len / 20), z + sin(angle + i * angp - PI / 2) * (len / 20));//4
		//glVertex3d(nx + cos(angle + i * angp - PI / 2) * (len / 20), ny - (len / 20), nz + sin(angle + i * angp - PI / 2) * (len / 20));//4'
		//glVertex3d(nx + cos(angle + i * angp - PI / 2) * (len / 20), ny + (len / 20), nz + sin(angle + i * angp - PI / 2) * (len / 20));//3'
		//glVertex3d(x + cos(angle + i * angp - PI / 2) * (len / 20), y + (len / 20), z + sin(angle + i * angp - PI / 2) * (len / 20));//3
		//glEnd();//правый
	}
}
void genTree(double x, double y, Tree* t,int &ddeph)
{
	double zt = trunk(x, y);
	t->addTrunk(zt);
	int deph = rand() % 3 + 3;
	ddeph = deph;
	branch(x, y, zt, 1.1, 5, deph, t);
	/*for (int i = deph; i >= 0; i--)
	{
		t->Draw(i, -1);
		Sleep(1000);
	}*/
}
void drawPlane()
{
	glBindTexture(GL_TEXTURE_2D, texId);
	glColor3d(0.058, 0.876, 0.039);
	glNormal3f(0, 0, 1);
	/*glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3d(-25, -25, 0);
	glTexCoord2d(1, 0);
	glVertex3d(-25, 25, 0);
	glTexCoord2d(1, 1);
	glVertex3d(25, 25, 0);
	glTexCoord2d(0, 1);
	glVertex3d(25, -25, 0);
	glEnd();

*/
	glBegin(GL_QUADS);

	int i, j;
	int size = 3;
	for (i = -25; i < 25; i=i+size) {

		for (j = -25; j < 25; j=j+size)

		{
			glTexCoord2d(0, 0);
			glVertex3f(i, j, 0);
			glTexCoord2d(1, 0);
			glVertex3f(i + size, j, 0);
			glTexCoord2d(1, 1);
			glVertex3f(i + size, j + size, 0);
			glTexCoord2d(0, 1);
			glVertex3f(i, j + size, 0);

		}
	}
	glEnd();



}
bool isFirst = 0;
int debug = 0;






//=================================================================================================================================================================================================================


int nndeph=-1;

void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.2, 1. };
	GLfloat dif[] = { 0.058, 0.876, 0.039, 0.1 };
	GLfloat spec[] = { 0.1, 0.1, 0.1, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); 
		//размер блика
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  
	srand(time(NULL));

	if (!isFirst)
	{
		isFirst = 1;
		//tree = new Tree(5, 5, 0);
		//genTree(5, 5, tree);

	}
	drawPlane();

	//настройка материала
	GLfloat amb2[] = { 0.5, 0.5, 0.5, 0.7 };
	GLfloat dif2[] = { 0.61, 0.262, 0, 0.2 };
	GLfloat spec2[] = { 0.1, 0.1, 0.1, 1. };
	GLfloat sh2 = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb2);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif2);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec2);
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh2);


	if (!trees.empty())
	{
		list<Tree*>::iterator it;
		for (it = trees.begin(); it != trees.end(); it++)
		{
			if ((*it)->_deph != -1)
			{
				
					(*it)->Draw();
					(*it)->_deph--;
				
				Sleep(100);
			}
			else
			{
				(*it)->Draw();
			}
			
		}
	}
	
	if (GetAsyncKeyState(VK_SPACE))
	{
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//delete tree;
		Tree* tree;
		int x, y;
		x = rand() % 30 - 15;
		y = rand() % 30 - 15;
		tree = new Tree(x, y, 0);
		genTree(x, y, tree, nndeph);
		trees.push_back(tree);
	}
	if (GetAsyncKeyState(VK_DELETE))
	{
		if (!trees.empty())
		{
			trees.clear();
		}
	}

	////Начало рисования квадратика станкина
	//double A[2] = { -4, -4 };
	//double B[2] = { 4, -4 };
	//double C[2] = { 4, 4 };
	//double D[2] = { -4, 4 };

	//glBindTexture(GL_TEXTURE_2D, texId);

	//glColor3d(0.6, 0.6, 0.6);
	//glBegin(GL_QUADS);

	//glNormal3d(0, 0, 1);
	//glTexCoord2d(0, 0);
	//glVertex2dv(A);
	//glTexCoord2d(1, 0);
	//glVertex2dv(B);
	//glTexCoord2d(1, 1);
	//glVertex2dv(C);
	//glTexCoord2d(0, 1);
	//glVertex2dv(D);

	//glEnd();
	////конец рисования квадратика станкина


   //Сообщение вверху экрана

	
	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
	                                //(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции

	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(300, 200);
	rec.setPosition(10, ogl->getHeight() - 150 - 50);


	std::stringstream ss;
	ss << "T - вкл/выкл текстур" << std::endl;
	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "F - Свет из камеры" << std::endl;
	ss << "G - двигать свет по горизонтали" << std::endl;
	ss << "G+ЛКМ двигать свет по вертекали" << std::endl << std::endl;
	ss << "DEL - Удалить все деревья" << std::endl;
	ss << "SPACE - Создать дерево в случайном месте " << std::endl << std::endl;
	ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "Параметры камеры: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}