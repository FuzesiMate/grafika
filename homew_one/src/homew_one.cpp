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
// Nev    : <VEZETEKNEV(EK)> <KERESZTNEV(EK)>
// Neptun : <NEPTUN KOD>
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
#include <iostream>

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
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
#include<vector>

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

   Vector operator/(const float s){
	   return Vector(x/s , y/s , z/s);
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



class CatmullRom{
private:
	std::vector<Vector> controls;
	std::vector<float> time;
	std::vector<Vector> velocity;
	Vector v_first;
	Vector v_last;
	float intersect;
	Vector tangent_v;
	Vector p0;

	Vector Hermite (Vector p0 , Vector v0 , float t0 , Vector p1 , Vector v1 , float t1 , float t , bool derive){
			Vector a0,a1,a2,a3;
			a0 = p0;
			a1 = v0;
			a2 = (((p1-p0)*3.0f)/(pow(t1-t0,2)))-((v1+(v0*2.0f))/(t1-t0));
			a3 = (((p0-p1)*2.0f)/(pow(t1-t0,3)))+((v1+v0)/(pow(t1-t0,2)));
			float tt = t-t0;
		if(!derive){
			return a3*tt*tt*tt+a2*tt*tt+a1*tt+a0;
		}
		if(derive){
			return (a3*tt*tt)*3+(a2*tt)*2+a1;
		}
	}

public:
	void addPoint(Vector point , float t){
		controls.push_back(point);
		time.push_back(t);
	}

	Vector r(float t){

		float last_diff = (time[time.size()-1]+500)-time[time.size()-1];
		v_last = (((controls[0]-controls[controls.size()-1])/last_diff)+((controls[controls.size()-1]-controls[controls.size()-2])/(time[time.size()-1]-time[time.size()-2])))/2.0f;
		v_first = Vector(0.2f , 0.2f , 0);
		Vector v0 , v1;
		Vector p_prev , p_next;
		float  t_prev , t_next;

		for(size_t i = 0 ; i<controls.size() ; i++){



			if(i<controls.size()-1){
				if(time[i]<=t && t<=time[i+1]){
					p_next = controls[i+1];
					t_next = time[i+1];
					p_prev = controls[i-1];
					t_prev = time[i-1];

					if(i==0){
						v0 = v_first;
						v1 = (((controls[i+2]-p_next)/(time[i+2]-t_next))+((p_next-controls[i])/(t_next-time[i])))/2.0f;
						//return Hermite(controls[i] , v0 , time[i] , p_next , v1 , t_next , t , false);

					}else if(i==controls.size()-2){

						v0 = (((p_next-controls[i])/(t_next-time[i]))+((controls[i]-p_prev)/(time[i]-t_prev)))/2.0f;
						v1 = v_last;
						//return Hermite(controls[i] , v0 , time[i] , p_next , v1 , t_next , t,false);
					}else{

						v0 = (((p_next-controls[i])/(t_next-time[i]))+((controls[i]-p_prev)/(time[i]-t_prev)))/2.0f;
						v1 = (((controls[i+2]-p_next)/(time[i+2]-t_next))+((p_next-controls[i])/(t_next-time[i])))/2.0f;
						//call hermite interpolation function and return the point
						//return Hermite(controls[i] , v0 , time[i] , p_next , v1 , t_next , t , false);
					}
					if(abs(t-intersect)<=0.1f){
						tangent_v = Hermite (controls[i] , v0 , time[i] , p_next , v1 , t_next , t , true);
						tangent_v = tangent_v/tangent_v.Length();
					}
					return Hermite(controls[i] , v0 , time[i] , p_next , v1 , t_next , t , false);
				}

			}

			else{
				v0 = v_last;
				v1 = v_first;
				p_next = controls[0];
				t_next = time[time.size()-1]+500;
				if(abs(t-intersect)<=0.1f){
					tangent_v = Hermite(controls[i] , v_last , time[i] , controls[0] , v_first , time[time.size()-1]+500 , t , true);
					tangent_v = tangent_v/tangent_v.Length();
				}
				return Hermite(controls[i] , v_last , time[i] , controls[0] , v_first , time[time.size()-1]+500 , t , false);
			}

	}

}

	Vector tangent_r(float t){
		return p0+tangent_v*t;
	}

	std::vector<Vector> getControls(){
		return controls;
	}

	float getBegin(){
		return time[0];
	}

	float getEnd(){
		return time[time.size()-1]+500;
	}

	void addIntersect(Vector p , float i){
		intersect = i;
		p0 = p;
	}


};

class parabola{

private:
	Vector lp[2];
	Vector sp;
	Vector p0;
	Vector n;
	Vector v;
	Vector intersect;
	int count;

public:
	parabola(){
		count = 0;
	}

	void addPoint(Vector p){
		if(count<2){
			lp[count]=p;
		}else if(count==2){
			sp=p;
			v = lp[1]-lp[0];
			n.x = v.y;
			n.y = -v.x;
			n = n/(n.Length());
			p0 = lp[0];
		}else{return;}
		count++;
	}

	bool isOnParabola(Vector point){
		return abs((abs(line_distance(point))-abs(focus_distance(point))))<0.1f;
	}

	float line_distance(Vector r){

		return n*(p0-r);
	}

	float focus_distance(Vector r){
		Vector temp = r-sp;
		return temp.Length();
	}

	int getCount(){
		return count;
	}


	bool isInside(Vector point){
		if(abs(focus_distance(point))<abs(line_distance(point))){
			return true;
		}else {return false;}
	}

	void addIntersect(Vector in){
		intersect = in;
	}

	Vector tangent_r( float t ){
		Vector v;
		Vector n;
		n= gradient(intersect);
		n = n/n.Length();
		v.x = n.y;
		v.y = -n.x;

		return intersect+v*t;

	}

	Vector gradient(Vector p){
		Vector grad;
		grad.x = 2*(n*(p-p0))*n.x-2*(p.x-sp.x);
		grad.y = 2*(n*(p-p0))*n.y-2*(p.y-sp.y);
		return grad;
	}



};

const int screenWidth = 600;	// alkalmazås ablak felbontåsa
const int screenHeight = 600;

Vector transform(Vector v){
	v.x = (v.x-screenWidth/2)/(screenWidth/2);
	v.y = (screenHeight/2-v.y)/(screenHeight/2);
	return v;
}

long time;
CatmullRom catmull;
parabola para;
Vector intersect;
bool found_intersect = false;
Color image[screenWidth*screenHeight];	// egy alkalmazås ablaknyi kÊp
Color bgr[screenWidth*screenHeight];

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) {
	glViewport(0, 0, screenWidth, screenHeight);
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {

    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

    // ..

    // Peldakent atmasoljuk a kepet a rasztertarba
    glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, bgr);

    if(para.getCount()==3){
//drawing background point by point
    	glBegin(GL_POINTS);
    		for(int x = 0 ; x<600 ; x++){
    			for(int y=0 ; y<600 ; y++){
    				Vector temp(x,y,0);
    				if(para.isInside(temp)){
    					temp=transform(temp);
    					glColor3f(1,1,0);
    					glVertex2f(temp.x , temp.y);

    				}else{
    					temp=transform(temp);
    					glColor3f(0,1,1);
    					glVertex2f(temp.x , temp.y);
    				}
    			}
    		}
    		glEnd();
    	}

//drawing the spline
    glColor3f(1,1,1);
	if(catmull.getControls().size()>1){
		glBegin(GL_LINE_STRIP);
			for(float i = catmull.getBegin(); i<=catmull.getEnd(); i+=0.1f){
				Vector temp;
				temp=catmull.r(i);
				//if the point is on the parabola it is an intersection
				if(para.isOnParabola(temp) && !found_intersect){
					//only need the first
					found_intersect = true;
					//save the point
					intersect = temp;
					para.addIntersect(temp);
					catmull.addIntersect(temp, i);
				}
				temp=transform(temp);
				glVertex2f(temp.x , temp.y);
			}
		glEnd();
	}

	//draw the tangent to the parabola
		glColor3f(0,1,0);

		glBegin(GL_LINE_STRIP);

			for(float i = -100 ; i<100 ; i+=1){
				Vector temp;
				temp = para.tangent_r(i);
				temp = transform(temp);
				glVertex2f(temp.x , temp.y);
			}
		glEnd();

		glBegin(GL_LINE_STRIP);
			for(float i = -100 ; i<100 ; i+=1){
				Vector temp;
				temp = catmull.tangent_r(i);
				temp = transform(temp);
				glVertex2f(temp.x , temp.y);
			}
		glEnd();

	//radius of the point
		float radius = 0.01f;

		//drawing the control points
		for(size_t i=0 ; i<catmull.getControls().size() ; i++){
			Vector temp;
			temp = catmull.getControls()[i];
			temp=transform(temp);
			//draw a polygon (circle)
			glColor3f(1,0,0);
			glBegin(GL_POLYGON);
				//calculate the vertexes
				// X = x0+cos(angle) , Y=y0+sin(angle)
				for(float j = 0 ; j<M_PI*2 ; j+=M_PI/12){
					glVertex2f(temp.x+cos(j)*radius , temp.y+sin(j)*radius);
				}

			glEnd();

			//draw the contour of control points
			glColor3f(1.0f , 1.0f , 1.0f);
			glBegin(GL_LINE_LOOP);
			for(float j = 0 ; j<M_PI*2 ; j+=M_PI/12){
					glVertex2f(temp.x+cos(j)*radius , temp.y+sin(j)*radius);
				}
			glEnd();

		}

	found_intersect = false;
    glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'd') glutPostRedisplay( ); 		// d beture rajzold ujra a kepet

}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {

}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
	// A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
    	Vector temp;
    	temp.x = x;
    	temp.y = y;
    	catmull.addPoint(temp , (float)time);
    	para.addPoint(temp);

		glutPostRedisplay( ); 						 // Ilyenkor rajzold ujra a kepet
    }
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y)
{

}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( ) {
     time = glutGet(GLUT_ELAPSED_TIME);		// program inditasa ota eltelt ido

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
