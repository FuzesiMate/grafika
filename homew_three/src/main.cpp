//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2014-tol.
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk.
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization fßggvÊnyt kivÊve, a lefoglalt adat korrekt felszabadítåsa nÊlkßl
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D,
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi,
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Fuzesi Mate
// Neptun : IL67VI
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector {
   float x, y, z;

   Vector( ) {
	x = y = z = 0;
   }
   Vector(float x0, float y0, float z0 = 0) {
	x = x0; y = y0; z = z0;
   }
   Vector operator*(float a) {
	return Vector(x * a, y * a, z * a);
   }
   Vector operator+(const Vector& v) {
 	return Vector(x + v.x, y + v.y, z + v.z);
   }
   Vector operator-(const Vector& v) {
 	return Vector(x - v.x, y - v.y, z - v.z);
   }
   float operator*(const Vector& v) { 	// dot product
	return (x * v.x + y * v.y + z * v.z);
   }
   Vector operator%(const Vector& v) { 	// cross product
	return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
   }
   float Length() { return sqrt(x * x + y * y + z * z); }
};

//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color {
   float r, g, b;

   Color( ) {
	r = g = b = 0;
   }
   Color(float r0, float g0, float b0) {
	r = r0; g = g0; b = b0;
   }
   Color operator*(float a) {
	return Color(r * a, g * a, b * a);
   }
   Color operator*(const Color& c) {
	return Color(r * c.r, g * c.g, b * c.b);
   }
   Color operator+(const Color& c) {
 	return Color(r + c.r, g + c.g, b + c.b);
   }
};

void setMaterial(float *kd , float *ks , float shininess){
	glMaterialfv(GL_FRONT , GL_DIFFUSE , kd);
	glMaterialfv(GL_FRONT , GL_SPECULAR , ks);
	glMaterialf(GL_FRONT , GL_SHININESS , shininess);
}

float lightpos[4]= {1,1,1,0};

class Object{
public:
	float ka[4];
	float kd[4];
	float ks[4];
	float shininess;

	virtual void draw(){

	}
	virtual ~Object(){

	}
};

class Sphere :public Object{
public:
	Vector center;
	float radius;
	Vector pos;
	void draw(){
		float x,y,z;
		Vector du;
		Vector dv;
		Vector normal;

		glBegin(GL_TRIANGLE_FAN);

		for(float u = 0 ; u<M_PI ; u+=0.01){
			for(float v = 0 ; v<2*M_PI ; v+=0.01){
				x = radius*sinf(u)*cosf(v);
				y = radius*sinf(u)*sinf(v);
				z = radius*cosf(u);

				du.x = radius*cosf(u)*cosf(v);
				du.y = radius*cosf(u)*sinf(v);
				du.z = radius*(-sinf(u));

				dv.x = radius*sinf(u)*(-sinf(v));
				dv.y = radius*sinf(u)*cosf(v);
				dv.z = 0;

				normal = du%dv;
				glNormal3f(normal.x , normal.y , normal.z);
				glVertex3f(x,y,z);
			}
		}
		glEnd();
	}
};

class Cone :public Object{
public:

	float radius;
	float param;
	Vector pos;

	void draw(){
		float x,y,z;
		Vector du;
		Vector dv;
		Vector normal;

		glBegin(GL_TRIANGLE_FAN) ;

		for(float u = 0 ; u<radius ; u+=0.01f){
			for(float v = 0 ; v<2*M_PI ; v+=0.01f){

				x = u*cosf(v)/param;
				y = u*sinf(v)/param;
				z = (u*u+1);

				du.x = cosf(v)/param;
				du.y = sinf(v)/param;
				du.z = 2.0f*u;

				dv.x = u*(-sinf(v))/param;
				dv.y = u*cosf(v)/param;
				dv.z = 0;
				normal = du%dv;

				glNormal3f(-normal.x , -normal.y , -normal.z);
				glVertex3f(x,y,z);
			}
		}
		glEnd();
	}
};

class Cylinder :public Object{
public:
	float radius;
	float height;
	Vector pos;

	void draw(){
		float x,y,z;
		Vector du;
		Vector dv;
		Vector normal;
		glBegin(GL_TRIANGLE_FAN);
		for(float u=0 ; u<height ; u+=0.01f){
			for(float v = 0 ; v<2*M_PI ; v+=0.01f){
				x = radius*cosf(v);
				y = radius*sinf(v);
				z = u;

				dv.x = radius*(-sinf(v));
				dv.y = radius*cosf(v);
				dv.z = 0.0f;

				du.x = 0.0f;
				du.y = 0.0f;
				du.z = 1.0f;

				normal = du%dv;

				glNormal3f(-normal.x , -normal.y , -normal.z);
				glVertex3f(x,y,z);
			}
		}
		glEnd();
	}
};

class Csirguru :public Object{
	float angle;
	Vector pos;
	Vector velocity;
	Sphere head;
	Sphere lefteye;
	Sphere righteye;
	Cone beak;
	Cone rowel[3];
	Cylinder leg[3];
	float prevt;
	float upordown;
	bool impulse;
	float accel;
	float anglevelocity;
	float verticalvelocity;
public:
	Csirguru(Vector pos , Vector dir){
		verticalvelocity = dir.y;
		anglevelocity = 35.0f;
		accel = 0;
		impulse = true;
		upordown=1.0f;
		angle = 15;
		velocity=dir;
		prevt=0;
		head.radius = 3.0f;
		lefteye.radius = 0.8f;
		righteye.radius = 0.8f;
		beak.radius = 2.0f;
		beak.param = 1.2f;
		for(int i=0 ; i<3 ; i++){
			rowel[i].param=1.3f;
			rowel[i].radius=2.0f;
			rowel[i].shininess=10;

			rowel[i].kd[0]=2.0f;
			rowel[i].kd[1]=0.0f;
			rowel[i].kd[2]=0.0f;
			rowel[i].kd[3]=1.0f;

			rowel[i].ks[0]=1.0f;
			rowel[i].ks[1]=0.0f;
			rowel[i].ks[2]=0.0f;
			rowel[i].ks[3]=1.0f;
		}

		for(int i=0 ; i<3 ; i++){
			leg[i].radius=0.3f;
			leg[i].height=3;
			if(i==2){
				leg[i].height=2.5;
			}
			if(i==0){
				leg[i].height=2;
			}
			leg[i].kd[0]=1.0f;
			leg[i].kd[1]=1.0f;
			leg[i].kd[2]=0.0f;
			leg[i].kd[3]=1.0f;

			leg[i].ks[0]=0.5f;
			leg[i].ks[1]=0.5f;
			leg[i].ks[2]=0.0f;
			leg[i].ks[3]=1.0f;

			leg[i].shininess=20;
		}

		this->pos = pos;

		head.kd[0]=1.0f;
		head.kd[1]=1.0f;
		head.kd[2]=1.0f;
		head.kd[3]=1.0f;

		head.ks[0]=0.5f;
		head.ks[1]=0.5f;
		head.ks[2]=0.5f;
		head.ks[3]=1.0f;
		head.shininess = 50;
		lefteye.kd[0]=0.1f;
		lefteye.kd[1]=0.1f;
		lefteye.kd[2]=0.1f;
		lefteye.kd[3]=1.0f;

		lefteye.ks[0]=1.0f;
		lefteye.ks[1]=1.0f;
		lefteye.ks[2]=1.0f;
		lefteye.ks[3]=1.0f;
		lefteye.shininess = 40;
		righteye.kd[0]=0.1f;
		righteye.kd[1]=0.1f;
		righteye.kd[2]=0.1f;
		righteye.kd[3]=1.0f;

		righteye.ks[0]=1.0f;
		righteye.ks[1]=1.0f;
		righteye.ks[2]=1.0f;
		righteye.ks[3]=1.0f;
		righteye.shininess=40;
		beak.kd[0]=3.5f;
		beak.kd[1]=1.5f;
		beak.kd[2]=0.0f;
		beak.kd[3]=1.0f;

		beak.ks[0]=1.5f;
		beak.ks[1]=0.5f;
		beak.ks[2]=0.0f;
		beak.ks[3]=1.0f;
		beak.shininess = 50;
	}

	void draw(){

		setMaterial(head.kd , head.ks , head.shininess);

		glPushMatrix();

		glTranslatef(pos.x , pos.y , pos.z);

		float rotate = atanf(velocity.x/velocity.z);
		rotate*=57.2957795f;
		glRotatef(rotate,0,1,0);

		setMaterial(leg[1].kd , leg[1].ks , leg[1].shininess);

		glPushMatrix();
			glTranslatef(0, leg[2].radius ,0);
			leg[2].draw();
		glPopMatrix();

		glPushMatrix();
			glRotatef(angle,1,0,0);
			glTranslatef(0,leg[1].height ,0);
			glRotatef(90,1,0,0);

			leg[1].draw();
		glPopMatrix();

		glPushMatrix();
			float sina = sinf(angle*(M_PI/180.0f));
			float cosa = cosf(angle*(M_PI/180.0f));
			glTranslatef(0,(leg[0].height+leg[1].height)-(leg[1].height-(cosa*leg[1].height)), sina*leg[1].height);
			glRotatef(90 ,1,0,0);
			leg[0].draw();
			glRotatef(-90,1,0,0);

			setMaterial(head.kd, head.ks , head.shininess);
			glTranslatef(0,head.radius,0);
			head.draw();

			setMaterial(beak.kd , beak.ks , beak.shininess);

			glPushMatrix();
				glTranslatef( 0 , -head.radius/2 , head.radius+2.5);
				glRotatef(-170 ,1,0,0);
				beak.draw();
			glPopMatrix();

		setMaterial(lefteye.kd , lefteye.ks , lefteye.shininess);

			glPushMatrix();
				glTranslatef(-head.radius/4 , +head.radius/4 , head.radius-lefteye.radius);
				lefteye.draw();
			glPopMatrix();

		setMaterial(righteye.kd , righteye.ks , righteye.shininess);

			glPushMatrix();
				glTranslatef(+head.radius/4 , +head.radius/4, head.radius-righteye.radius);
				righteye.draw();
				glPopMatrix();

		setMaterial(rowel[0].kd , rowel[0].ks , rowel[0].shininess);

		for(int i=0 ; i<3 ; i++){
			glPushMatrix();
			glTranslatef(0 , head.radius+rowel[i].radius , head.radius-(i)-head.radius/1.9);
			glRotatef(90 , 1,0,0);
			rowel[i].draw();
			glPopMatrix();

		}

		glPopMatrix();
		glPopMatrix();
	}

	void upanddown(){
			float time = glutGet(GLUT_ELAPSED_TIME);
			float t = time-prevt;
			anglevelocity+= accel*t;
			angle+=(t/1000)*upordown*anglevelocity;
			if(angle>60){upordown = -upordown; accel=1.0f; angle =60;}
			if(angle<15){upordown=-upordown; impulse = false; accel=0.0f ,anglevelocity = 35.0f; angle=15;}
			prevt=time;
	}

	void jump(){

		if(impulse){
			upanddown();
		}else{

			float time=glutGet(GLUT_ELAPSED_TIME);
			float t = (time-prevt)/1000.0f;


		pos.x+= velocity.x*t;
		verticalvelocity-= 9.81f*t;
		pos.y+= (verticalvelocity*t);
		pos.z+= velocity.z*t;

		if(pos.y<0){
			verticalvelocity=velocity.y;
			pos.y = 0;
			impulse=true;
		}

		prevt=time;
		}
		glutPostRedisplay();

	}

	void blowup(){
	   float time = glutGet(GLUT_ELAPSED_TIME);
	   float t = (time-prevt)/1000;



	   prevt=time;
	}


};

const int screenWidth = 600;	// alkalmazås ablak felbontåsa
const int screenHeight = 600;


Color image[screenWidth*screenHeight];	// egy alkalmazås ablaknyi kÊp

Csirguru csirg(Vector(-20,0.0,-20) , Vector(2.0f, 10.0f,0.0f));


// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) {
	glViewport(0, 0, screenWidth, screenHeight);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	/*
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float ka[4] = {0.5f, 0.5f, 0.5f ,1};
	float kd[4] = {0.8 , 0.8 , 0.8 , 1};
	float ks[4] = {1.5,1.5,1.5,1};
	glLightfv(GL_LIGHT0 , GL_AMBIENT , ka);
	glLightfv(GL_LIGHT0 , GL_DIFFUSE , kd);
	glLightfv(GL_LIGHT0 , GL_SPECULAR , ks);
	glLightfv(GL_LIGHT0 , GL_POSITION , lightpos);
	glEnable(GL_LIGHT0);

	gluLookAt(0, 0 , 0,   0, 0, -1,   0, 1 , 0);
	*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60 , 1 , 0.1 , 100);

	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);


}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {

    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0, 10 , 50 ,   0, 0, -1,   0, 1 , 0);

    float ka[4] = {0.5f, 0.5f, 0.5f ,1};
    float kd[4] = {0.8 , 0.8 , 0.8 , 1};
    float ks[4] = {1.5,1.5,1.5,1};
    glLightfv(GL_LIGHT0 , GL_AMBIENT , ka);
    glLightfv(GL_LIGHT0 , GL_DIFFUSE , kd);
    glLightfv(GL_LIGHT0 , GL_SPECULAR , ks);
    glLightfv(GL_LIGHT0 , GL_POSITION , lightpos);
    glEnable(GL_LIGHT0);

    float kdc[4]={0.0 , 1.0 , 0.0 , 1.0};
    float ksc[4]={0.0 , 1.0 , 0.0 , 1.0};

    setMaterial(kdc , ksc , 20);

    glBegin(GL_QUADS);
    glNormal3f(0,1,0);
    glVertex3f(-1000,0,-1000);
    glVertex3f(-1000,0,1000);
    glVertex3f(1000,0,1000);
    glVertex3f(1000,0,-1000);
    glEnd();

    csirg.draw();
    glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'd'){

    	glutPostRedisplay( ); 		// d beture rajzold ujra a kepet
    }

}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {

}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)   // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
		glutPostRedisplay( ); 						 // Ilyenkor rajzold ujra a kepet
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y)
{

}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( ) {
	//csirg.upanddown();
	csirg.jump();
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv); 				// GLUT inicializalasa
    glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel
    glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer

    glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon

    glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();

    onInitialization();					// Az altalad irt inicializalast lefuttatjuk

    glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
    glutMouseFunc(onMouse);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutMotionFunc(onMouseMotion);

    glutMainLoop();					// Esemenykezelo hurok

    return 0;
}
