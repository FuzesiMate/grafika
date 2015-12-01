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

   Vector operator/(const float f){
	   return Vector(x/f , y/f , z/f);
   }
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

enum State{
	PREPARE,
	INAIR,
	LAND,
	BLOWUP
};

void setMaterial(float *kd , float *ks , float shininess){
	glMaterialfv(GL_FRONT , GL_DIFFUSE , kd);
	glMaterialfv(GL_FRONT , GL_SPECULAR , ks);
	glMaterialf(GL_FRONT , GL_SHININESS , shininess);
}

Vector randomVelocity(){
	Vector velocity;
	int dir=1;
	if(rand()%2==0){
		dir = -1;
	}

	velocity.x = dir*((rand()%5+1)+dir*(rand()%3+1));
	velocity.y = 10.0f;
	velocity.z = dir*((rand()%5+1)+dir*(rand()%3+1));

	return velocity;
}

float lightpos[4]= {1,1,1,0};

class Object{
public:
	float ka[4];
	float kd[4];
	float ks[4];
	float shininess;
	float rotation;
	Vector pos;
	Vector velocity;
	float prevt;
	bool active;
	float deltau;
	float deltav;
	virtual void draw(){

	}

	Object(){
		active=true;
		deltau=1.0f;
		deltav = 1.0f;
		for(int i = 0 ; i<4 ; i++){
			ka[i]=1.0f;
		}
	}

	virtual ~Object(){

	}

	virtual void blowup(){
		float time = glutGet(GLUT_ELAPSED_TIME);
		float t = (time-prevt)/1000;
		active = true;
		pos.x+=velocity.x*t;
		velocity.y-=9.81*t;
		pos.y += velocity.y*t;
		pos.z += velocity.z*t;
		rotation+=1000.0f*t;
		prevt=time;
		if(pos.y<(-0.5f)){
			active=false;
		}
	}
};

class Body :public Object{
	public:
		float radius;
		Vector cps[4][7];
		Body(float rad=1.0f){

			radius = rad;

			float div = 1.4f;
			float r = radius;
			float y = 0;
			float z = -5.0f;

			r = radius/10.0f;

			cps[0][0]=Vector(0,-r/2+y,z);
			cps[0][1]=Vector(r/2,-r/2+y,z);
			cps[0][2]=Vector(r ,0+y,z);
			cps[0][3]=Vector(0,5*r,z);
			cps[0][4]=Vector(-(r) , 0+y , z);
			cps[0][5]=Vector(-(r/2) , -r/2+y ,z);
			cps[0][6]=Vector(0,-r/2+y,z);

			r = radius;

			r = radius;
			z = 0;
			y = 0;
			for(int i = 1 ; i<4; i++){
					cps[i][0]=Vector(0,-r/2+y,z);
					cps[i][1]=Vector(r/1.5,-r/2+y,z);
					cps[i][2]=Vector(r/2 ,0+y,z);
					cps[i][3]=Vector(0,r+y,z);
					cps[i][4]=Vector(-(r/1.5) , 0+y , z);
					cps[i][5]=Vector(-(r/2) , -r/2+y ,z);
					cps[i][6]=Vector(0,-r/2+y,z);
				z+=3.5f;
				y+=3.5f;
				r-=radius/2;
			}

		}

		void draw(){
			setMaterial(kd , ks , shininess);
			Vector point;

			glBegin(GL_POLYGON);
			for(float t = 0 ; t<=1.0f ; t+=0.08f){
				point=bezier(t,0);
				glVertex3f(point.x , point.y , point.z);
				glNormal3f(0,0,-1);
			}
			glEnd();

			for(float t = 0 ; t<=1.0f ; t+=0.08f){
				for(int i = 0 ; i<2 ; i++){
					 glBegin(GL_QUADS);
					 for(float ct = i ; ct<=(i+1) ; ct+=0.08f){
						point = catmull(ct , t , i , false);
						glVertex3f(point.x , point.y , point.z);
						point = catmull(ct , t+0.08 , i , false);
						glVertex3f(point.x , point.y , point.z);
						point = catmull(ct+0.08 , t+0.08 , i, false);
						glVertex3f(point.x , point.y , point.z);
						point = catmull(ct+0.08, t , i , false);
						glVertex3f(point.x , point.y , point.z);

						Vector normal = catmull(ct,t,i,true)%(bezier(t,i)-bezier(t+0.08 , i));
						glNormal3f(normal.x , normal.y , normal.z);
					}
					 glEnd();
				}
			}

	}

		float B(int i , float t){
			int n = 6;
			float choose = 1.0f;
			for(int j = 1 ; j<=i ; j++){
				choose*=(float)(n-j+1)/j;
			}
			return choose*powf(t,i)*powf(1-t , n-i);
		}

		Vector bezier(float t , int slice){
			Vector result(0,0,0);
			for(int i = 0 ; i<7 ; i++){
				result = result + cps[slice][i]*B(i,t);
			}
			return result;
		}

		Vector hermite(Vector p0 , Vector p1 , Vector v0 , Vector v1 , float t0 , float t1 , float t , bool derive){
			Vector a0,a1,a2,a3;

			a0 = p0;
			a1 = v0;
			a2 = (((p1-p0)*3.0f)/(pow(t1-t0,2)))-((v1+(v0*2.0f))/(t1-t0));
			a3 = (((p0-p1)*2.0f)/(pow(t1-t0,3)))+((v1+v0)/(pow(t1-t0,2)));
			float tt = t-t0;
			if(!derive){
			return a3*tt*tt*tt+a2*tt*tt+a1*tt+a0;
			}if(derive){
				return (a3*tt*tt)*3+(a2*tt)*2+a1;
			}
		}

		Vector catmull(float t , float bezier_t , int slice , bool derive){
			Vector prev , act , next , nextnext;
			float prev_t , act_t , next_t , nextnext_t;
			Vector v0 , v1;

			act = bezier(bezier_t , slice);
			next = bezier(bezier_t , slice+1);
			act_t = slice;
			next_t = slice+1;
			if(slice==0){
				v0 = Vector(0.1,0.1,0.1);
				nextnext = bezier(bezier_t,slice+2);
				nextnext_t = slice+2;
				v1 = v1 = ((nextnext-next)/(nextnext_t-next_t))+((next-act)/(next_t-act_t));
			}else if(slice==5){
				prev = bezier(bezier_t , slice-1);
				prev_t = slice-1;
				v0 = ((next-act)/(next_t-act_t))+((act-prev)/(act_t-prev_t));
				v1 = Vector(0.1,0.1,0.1);
			}else{
				prev = bezier(bezier_t , slice-1);
				nextnext = bezier(bezier_t , slice+2);
				prev_t = slice-1;
				nextnext_t = slice+2;
				v0 = ((next-act)/(next_t-slice))+((act-prev)/(slice-prev_t));
				v1 = ((nextnext-next)/(nextnext_t-next_t))+((next-act)/(next_t-slice));
			}

			return hermite(act , next , v0 , v1 , act_t , next_t , t , derive);
		}


};

class Sphere :public Object{
public:
	Vector center;
	float radius;

	Sphere(float du=0.1f , float dv=0.1f){
		deltau=du;
		deltav=dv;
	}

	void draw(){
		float x,y,z;
		Vector du;
		Vector dv;
		Vector normal;
		setMaterial(kd , ks , shininess);
		glBegin(GL_QUADS);



		for(float u = 0 ; u<M_PI ; u+=deltau){
			for(float v = 0 ; v<2*M_PI ; v+=deltav){

				x = radius*sinf(u)*cosf(v);
				y = radius*sinf(u)*sinf(v);
				z = radius*cosf(u);

				glVertex3f(x,y,z);

				x = radius*sinf(u+deltau)*cosf(v);
				y = radius*sinf(u+deltau)*sinf(v);
				z = radius*cosf(u+deltau);

				glVertex3f(x,y,z);

				x = radius*sinf(u+deltau)*cosf(v+deltav);
				y = radius*sinf(u+deltau)*sinf(v+deltav);
				z = radius*cosf(u+deltau);

				glVertex3f(x,y,z);

				x = radius*sinf(u)*cosf(v+deltav);
				y = radius*sinf(u)*sinf(v+deltav);
				z = radius*cosf(u);

				glVertex3f(x,y,z);



				du.x = radius*cosf(u)*cosf(v);
				du.y = radius*cosf(u)*sinf(v);
				du.z = radius*(-sinf(u));

				dv.x = radius*sinf(u)*(-sinf(v));
				dv.y = radius*sinf(u)*cosf(v);
				dv.z = 0;

				normal = du%dv;
				if(y==0){
					normal = Vector(0,0,1);
				}
				glNormal3f(normal.x , normal.y , normal.z);


			}
		}
		glEnd();
	}

};

class Cone :public Object{
public:

	float radius;
	float param;

	void draw(){
		float x,y,z;
		Vector du;
		Vector dv;
		Vector normal;
		deltau = 0.1f;
		deltav = 0.1f;
		setMaterial(kd , ks , shininess);

		glBegin(GL_QUADS) ;

		for(float u = 0 ; u<radius ; u+=deltau){
			for(float v = 0 ; v<2*M_PI ; v+=deltav){

				x = u*cosf(v)/param;
				y = u*sinf(v)/param;
				z = ((u*u+1));

				glVertex3f(x,y,z);

				x = (u+deltau)*cosf(v)/param;
				y = (u+deltau)*sinf(v)/param;
				z = (((u+deltau)*(u+deltau)+1));

				glVertex3f(x,y,z);

				x = (u+deltau)*cosf(v+deltav)/param;
				y = (u+deltau)*sinf(v+deltav)/param;
				z = (((u+deltau)*(u+deltau)+1));

				glVertex3f(x,y,z);

				x = (u)*cosf(v+deltav)/param;
				y = (u)*sinf(v+deltav)/param;
				z = (((u)*(u)+1));

				glVertex3f(x,y,z);

				du.x = cosf(v);
				du.y = sinf(v);
				du.z = 2.0f*u;

				dv.x = u*(-sinf(v));
				dv.y = u*cosf(v);
				dv.z = 0;
				normal = du%dv;

				glNormal3f(-normal.x , -normal.y , -normal.z);

			}
		}
		glEnd();
	}

};

class Cylinder :public Object{
public:
	float radius;
	float height;

	void draw(){
		float x,y,z;
		Vector du;
		Vector dv;
		Vector normal;
		deltau=height/5.0f;
		deltav=M_PI/8.0f;
		setMaterial(kd , ks , shininess);

		glBegin(GL_QUADS);
		for(float u=0 ; u<height ; u+=deltau){
			for(float v = 0 ; v<2*M_PI ; v+=deltav){
				x = radius*cosf(v);
				y = radius*sinf(v);
				z = u;

				glVertex3f(x,y,z);

				x = radius*cosf(v+deltav);
				y = radius*sinf(v+deltav);
				z = u;

				glVertex3f(x,y,z);

				x = radius*cosf(v+deltav);
				y = radius*sinf(v+deltav);
				z = u+deltau;

				glVertex3f(x,y,z);

				x = radius*cosf(v);
				y = radius*sinf(v);
				z = u+deltau;

				glVertex3f(x,y,z);


				dv.x = radius*(-sinf(v));
				dv.y = radius*cosf(v);
				dv.z = 0.0f;

				du.x = 0.0f;
				du.y = 0.0f;
				du.z = 1.0f;

				normal = du%dv;

				glNormal3f(-normal.x , -normal.y , -normal.z);

			}
		}
		glEnd();
	}
};

class Csirguru :public Object{
	float ankle_angle;
	float knee_angle;
	Sphere head;
	Sphere lefteye;
	Sphere righteye;
	Cone beak;
	Cone rowel[3];
	Cylinder leg[4];
	Body body;
	float upordown;
	bool impulse;
	float accel;
	float anglevelocity;
	float verticalvelocity;
	Vector knee_velocity;
	State state;


public:
	bool blownup;
	Csirguru(Vector pos=Vector(0,0,0) , Vector dir=Vector(0,0,0)){
		state = PREPARE;
		blownup = false;
		active = true;
		verticalvelocity = dir.y;
		anglevelocity = 85.0f;
		accel = 0;
		impulse = true;
		upordown=1.0f;
		knee_angle= 40;
		ankle_angle = -(knee_angle/2.0f);
		velocity=dir;
		head.radius = 2.5f;
		lefteye.radius = 0.5f;
		righteye.radius = 0.5f;
		beak.radius = 1.25f;
		beak.param = 1.3f;
		body = Body(8.0f);
		for(int i=0 ; i<3 ; i++){
			rowel[i].param=1.0f;
			rowel[i].radius=1.0f;
			rowel[i].shininess=10;

			rowel[i].kd[0]=1.25f;
			rowel[i].kd[1]=0.0f;
			rowel[i].kd[2]=0.0f;
			rowel[i].kd[3]=1.0f;

			rowel[i].ks[0]=1.0f;
			rowel[i].ks[1]=0.0f;
			rowel[i].ks[2]=0.0f;
			rowel[i].ks[3]=1.0f;
		}

		for(int i=0 ; i<4 ; i++){

			leg[i].radius=0.3f;
			if(i==3){
				leg[i].height=2.0f;
			}if(i==2){
				leg[i].height=1.5f;
			}if(i==1){
				leg[i].height=3.0f;
			}if(i==0){
				leg[i].height=1.0f;
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

		head.shininess = 20;

		body.kd[0]=1.0f;
		body.kd[1]=1.0f;
		body.kd[2]=1.0f;
		body.kd[3]=1.0f;

		body.ks[0]=0.5f;
		body.ks[1]=0.5f;
		body.ks[2]=0.5f;
		body.ks[3]=1.0f;

		body.shininess = 500;

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

	if(!blownup){
		glPushMatrix();
		glTranslatef(pos.x , pos.y , pos.z);

		rotation = atanf(velocity.x/velocity.z);
		rotation*=57.2957795f;
		if(velocity.z<0){rotation = rotation-180;}
		glRotatef(rotation,0,1,0);


		glPushMatrix();
			glTranslatef(0, leg[3].radius ,0);
			leg[3].draw();


			glRotatef(ankle_angle,1,0,0);
			glTranslatef(0,leg[2].height,0);
			glRotatef(90,1,0,0);

			leg[2].draw();

			glRotatef(-90 ,1,0,0);

			glRotatef(knee_angle , 1,0,0);
			glTranslatef(0,leg[1].height ,0);
			glRotatef(90,1,0,0);

			leg[1].draw();

			glRotatef(-90 ,1,0,0);

			glRotatef(-(ankle_angle+knee_angle),1,0,0);
			glTranslatef(0,leg[0].height,0);
			glRotatef(90,1,0,0);

			leg[0].draw();

			glRotatef(-90,1,0,0) ;

			glTranslatef(0,body.radius/2-leg[0].height,0);
			body.draw();

			glTranslatef(0.0f, head.radius+head.radius-leg[0].height, 3.5f);

			head.draw();

			glPushMatrix();
				glTranslatef( 0 , -head.radius/2.0f , head.radius+head.radius/1.5f);
				glRotatef(-170 ,1,0,0);
				beak.draw();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(-head.radius/4.0f , +head.radius/4.0f , head.radius-lefteye.radius/1.5f);
				lefteye.draw();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(+head.radius/4.0f , +head.radius/4.0f, head.radius-righteye.radius/1.5f);
				righteye.draw();
				glPopMatrix();

		for(int i=0 ; i<3 ; i++){
			glPushMatrix();
			glTranslatef(0 , head.radius+rowel[i].radius*1.3 , -i+head.radius/2.5);
			glRotatef(90.0f , 1,0,0);
			rowel[i].draw();
			glPopMatrix();
		}
			glPopMatrix();
			glPopMatrix();
		}
		if(blownup){
		if(head.active){
			for(int i =0 ; i<3 ; i++){
				leg[i].blowup();
				glPushMatrix();
				glTranslatef(leg[i].pos.x , leg[i].pos.y , leg[i].pos.z);
				glRotatef(90 , 1,0,0);
				glRotatef(leg[i].rotation , 1,0,0);
				leg[i].draw();
				glPopMatrix();
			}

			head.blowup();
			glPushMatrix();
			glTranslatef(head.pos.x , head.pos.y , head.pos.z);
			glRotatef(head.rotation , 1,0,0);
			head.draw();
			glPopMatrix();

			body.blowup();
			glPushMatrix();
			glTranslatef(body.pos.x , body.pos.y , body.pos.z);
			glRotatef(body.rotation , 1,1,1);
			body.draw();
			glPopMatrix();

			beak.blowup();
			glPushMatrix();
			glTranslatef(beak.pos.x , beak.pos.y , beak.pos.z);
			glRotatef(beak.rotation , 1,0,0);
			glRotatef(90,1,0,0);
			beak.draw();
			glPopMatrix();

			lefteye.blowup();
			glPushMatrix();
			glTranslatef(lefteye.pos.x , lefteye.pos.y , lefteye.pos.z);
			glRotatef(lefteye.rotation , 1,0,0);
			lefteye.draw();
			glPopMatrix();

			righteye.blowup();
			glPushMatrix();
			glTranslatef(righteye.pos.x , righteye.pos.y , righteye.pos.z);
			glRotatef(righteye.rotation , 1,0,0);
			righteye.draw();
			glPopMatrix();

			for(int i=0 ; i<3 ; i++){
				rowel[i].blowup();
				glPushMatrix();
				glTranslatef(rowel[i].pos.x , rowel[i].pos.y , rowel[i].pos.z);
				glRotatef(90 ,1,0,0);
				glRotatef(rowel[i].rotation , 1,0,1);
				rowel[i].draw();
				glPopMatrix();
			}
			}else{
				active =false;
			}
		}
}

	void prepare(){
		float time = glutGet(GLUT_ELAPSED_TIME);
					float t = time-prevt;
					anglevelocity+= accel*t;
					knee_angle+=(t/1000)*upordown*anglevelocity;

					if(knee_angle>110){
						upordown = -upordown;
						accel=5.0f;
						knee_angle =110;
					}
					if(knee_angle<40.0f){
						ankle_angle+=(t/1000)*(-upordown)*anglevelocity;

					}if(ankle_angle>20.0f){
						knee_angle = 0.0f;
						ankle_angle = 20.0f;
						state = INAIR;
						accel = -5.0f;
						upordown = -upordown;
						return;
					}
		prevt=glutGet(GLUT_ELAPSED_TIME);
	}

	void land(){
		float time = glutGet(GLUT_ELAPSED_TIME);
		float t = time-prevt;
		anglevelocity+= accel*t;
		knee_angle+=(t/1000)*upordown*anglevelocity;

		if(anglevelocity<85.0f){
			accel=0.0f;
			anglevelocity=85;
		}

		if(knee_angle>130.0f){
			accel=0.0f;
			anglevelocity = 85.0f;
			upordown = -upordown;
			knee_angle = 90.0f;

		}if(knee_angle<40.0f){
			knee_angle = 40.0f;
			ankle_angle = 0.0f;
			upordown = -upordown;
			accel = 0.0f;
			anglevelocity = 85.0f;
			velocity=randomVelocity();
			verticalvelocity=velocity.y;
			state=PREPARE;
		}
		prevt = time;

	}

	void jump(){

		float time=glutGet(GLUT_ELAPSED_TIME);
		float t = (time-prevt)/1000.0f;
		pos.x+= velocity.x*t;
		verticalvelocity-= 9.81f*t;
		pos.y+= (verticalvelocity*t);
		pos.z+= velocity.z*t;

			knee_angle = 0.0f;
			ankle_angle-=40.0f*t;
			if(ankle_angle<-20.0f){
				ankle_angle = -20.0f;
			}

		if(pos.y<-0.5f){
			pos.y = 0;
			state=LAND;
			knee_angle = 40.0f;
		}
		prevt=time;
	}

	void move(){
		switch(state){
		case PREPARE:
			prepare();
			break;
		case INAIR:
			jump();
			break;
		case LAND:
			land();
			break;
		case BLOWUP:
			blownup=true;
			break;
		}
	}

	void blowup(){
		float t = glutGet(GLUT_ELAPSED_TIME);

		blownup = true;
		Vector initpos = pos;
		initpos.y+=leg[2].radius;
		leg[2].pos = initpos;
		leg[2].velocity = randomVelocity();
		leg[2].rotation = rotation;

		initpos.y+=leg[1].height;
		leg[1].pos = initpos;
		leg[1].velocity = randomVelocity();
		leg[1].prevt = t;

		initpos.y+=leg[2].height;
		leg[0].pos = initpos;
		leg[0].velocity = randomVelocity();
		leg[0].prevt = t;

		body.pos = initpos;
		body.velocity = randomVelocity();
		body.prevt = t;

		initpos.y+=head.radius;
		head.pos = initpos;
		head.velocity = randomVelocity();
		head.prevt = t;

		Vector temp(initpos.x-head.radius/4 , initpos.y+head.radius/4 , initpos.z+head.radius-lefteye.radius);
		lefteye.pos = temp;
		lefteye.velocity=randomVelocity();
		lefteye.prevt = t;

		temp = Vector(initpos.x+head.radius/4 , initpos.y+head.radius/4 , initpos.z+head.radius-lefteye.radius);
		righteye.pos = temp;
		righteye.velocity=randomVelocity();
		righteye.prevt = t;

		temp.x = initpos.x;
		temp.y = initpos.y;
		temp.z = initpos.z+head.radius+2.5;
		beak.pos = temp;
		beak.velocity = randomVelocity();
		beak.prevt =t;

		for(int i = 0 ; i<3 ; i++){
			temp.x = initpos.x;
			temp.y = initpos.y+head.radius+rowel[i].radius;
			temp.z = initpos.z-i-head.radius/1.9f;
			rowel[i].pos = temp;
			rowel[i].velocity = randomVelocity();
			rowel[i].prevt = t;
			rowel[i].radius = 1.5f;
		}
	}

};

class Bomb :public Object{

private:
	Sphere body;
	Cylinder wick;
	bool isfall;
	bool land;

public:
	Bomb(){
		prevt = 0;
		isfall = false;
		land = false;
		velocity.y = 5.0f;
		pos = Vector(0,25,0);
		body.radius = 2.0f;
		body.kd[0]=0.0f;
		body.kd[1]=0.0f;
		body.kd[2]=0.0f;
		body.kd[3]=1.0f;

		body.ks[0]=1.0f;
		body.ks[1]=1.0f;
		body.ks[2]=1.0f;
		body.ks[3]=1.0f;

		body.shininess = 100.0f;

		wick.height = 1.0f;
		wick.radius = 0.5f;

		wick.kd[0]=1.0f;
		wick.kd[1]=0.5f;
		wick.kd[2]=0.2f;
		wick.kd[3]=1.0f;

		wick.ks[0]=1.0f;
		wick.ks[1]=0.5f;
		wick.ks[2]=0.2f;
		wick.ks[3]=1.0f;
	}

	void fall(){
		float time = glutGet(GLUT_ELAPSED_TIME);
		float t = (time-prevt)/1000.0f;
		pos.y-=velocity.y*t;
	}

	void draw(){
		if(isfall){
			fall();
			if(pos.y<0){
				pos.y=25.0f;
				velocity.y=5.0f;
				isfall=false;
				land = true;
			}
		}

		glPushMatrix();
		glTranslatef(pos.x , pos.y , pos.z);
		body.draw();
		glTranslatef(0,body.radius+wick.height-body.radius/8 , 0);
		glRotatef(90,1,0,0);
		wick.draw();
		glPopMatrix();
	}

	void move(char dir){
		switch(dir){
		case 'w':
			pos.z-=1.0f;
			break;
		case 'a':
			pos.x-=1.0f;
			break;
		case 'd':
			pos.x+=1.0f;
			break;
		case 'y':
			pos.z+=1;
			break;
		case 0x20:
			isfall=true;
			prevt = glutGet(GLUT_ELAPSED_TIME);
			break;
		default:
			break;
		}

	}

	bool isLand(){
		return land;
	}

	void setLand(bool l){
		land = l;
	}

};

class Game{
private:
	Csirguru csirbox[7];
	Bomb bomb;
	int count;

public:
	float prevt;
	Game(){
		count=0;
		prevt=-1;
		csirbox[count]=Csirguru(Vector(bomb.pos.x , 0 , bomb.pos.z),randomVelocity());
		count++;
	}

	void step(){

		float distance;
				for(int i =0 ; i<count ; i++){
					if(bomb.isLand()){
						if(i==count-1){
							bomb.setLand(false);
						}

						Vector temp(bomb.pos.x , 0, bomb.pos.z);
						distance=(temp-csirbox[i].pos).Length();

						if(distance<10.0f && !csirbox[i].blownup && csirbox[i].active){
							csirbox[i].blowup();
						}
					}
					if(csirbox[i].active){
						csirbox[i].move();
					}
				}

		float time = glutGet(GLUT_ELAPSED_TIME);

		float diff = time-prevt;

		int deltat = (int)(diff/1000);

		for(int i = 0 ; i<deltat ; i++){
			if(count<5){
			csirbox[count]=Csirguru(Vector(bomb.pos.x , 0 , bomb.pos.z) , randomVelocity());
			csirbox[count].prevt = prevt;
			count++;
			}
			if(count==5){
				for(int i = 0 ; i<count ; i++){
					if(!csirbox[i].active){
						csirbox[i]=Csirguru(Vector(bomb.pos.x , 0 , bomb.pos.z) , randomVelocity());
						csirbox[i].prevt = prevt;
						break;
					}
				}
			}
		}

		if(deltat>=1){
			prevt = time;
		}

	}

	void moveBomb(char dir){
		bomb.move(dir);
	}

	void draw(){
		for(int i = 0 ; i<count ; i++){
			if(csirbox[i].active){
				csirbox[i].draw();
			}
		}
		bomb.draw();
	}
};

const int screenWidth = 600;	// alkalmazås ablak felbontåsa
const int screenHeight = 600;


Color image[screenWidth*screenHeight];	// egy alkalmazås ablaknyi kÊp

Game game;
Csirguru csirg(Vector(0,0,0) , Vector(1,10,0));
Body body(3.0f);



// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) {
	glViewport(0, 0, screenWidth, screenHeight);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);


	glEnable(GL_NORMALIZE);
	float ka[4] = {0.5f, 0.5f, 0.5f ,1};
	float kd[4] = {0.8 , 0.8 , 0.8 , 1};
	float ks[4] = {1.5,1.5,1.5,1};
	glLightfv(GL_LIGHT0 , GL_AMBIENT , ka);
	glLightfv(GL_LIGHT0 , GL_DIFFUSE , kd);
	glLightfv(GL_LIGHT0 , GL_SPECULAR , ks);
	glEnable(GL_LIGHT0);



}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {

    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60 , 1 , 0.1 , 200);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0, 40 , 50 ,   0, 0, -1,   0, 1 , 0);

    glLightfv(GL_LIGHT0 , GL_POSITION , lightpos);

    glEnable(GL_LIGHTING);

    glBegin(GL_QUADS);
    for(float u = -100 ; u<100 ; u+=10){
    	float kdc[4]={0.0f , fabs(u/100.0f) , 0.0f , 1.0f};
    	    		 float ksc[4]={1.0 , 1.0 , 1.0 , 1.0};
    	    		setMaterial(kdc ,ksc , 20);
    	for(float v = -100 ; v<100 ; v+=10){

    		 glNormal3f(0,1,0);
    		 glVertex3f(u,0,v);
    		 glVertex3f(u+10,0,v);
    		 glVertex3f(u+10,0,v+10);
    		 glVertex3f(u,0,v+10);
    	}
    }

    glEnd();
    game.draw();

    float shadow[4][4] = {1,0,0,0,
    					  -lightpos[0]/lightpos[1] , 0 , -lightpos[2]/lightpos[1],0,
						  0,0,1,0,
						  0,0.5f , 0 , 1
    };
    glMultMatrixf(&shadow[0][0]);
    glDisable(GL_LIGHTING);
    glColor3f(0,0,0);
    game.draw();

    glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
    	game.moveBomb(key);
    	glutPostRedisplay();
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
	if(game.prevt<0.0f){
		game.prevt = glutGet(GLUT_ELAPSED_TIME);
	}
	game.step();
	glutPostRedisplay();
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
