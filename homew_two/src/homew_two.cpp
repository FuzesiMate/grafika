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
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
#include <iostream>

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
   Vector operator/(const float& f){
	   return Vector(x/f , y/f , z/f);
   }
   Vector operator+(const float& f){
	   return Vector(x+f , y+f , z+f);
   }
   float Length() { return sqrt(x * x + y * y + z * z); }

   Vector normalize(){return *this/Length();}
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
   Color operator^(const float& c){
	   return Color(powf(r,c) , powf(g,c) , powf(b,c));
   }
   Color operator-(const Color& c){
	   return Color(r-c.r , g-c.g , b-c.b);
   }
   Color operator/(const Color& c){
	   return Color(r/c.r , g/c.g , b/c.g);
   }
   Color operator/(const float& f){
	   return Color(r/f , g/f , b/f);
   }
   Color operator+(const float& f){
	   return Color(r+f , g+f , b+f);
   }
};

float lightspeed=1.0f;

class material{
public:
	bool isRefractive;
	bool isReflective;
	Color F0;
	Color kd;
	Color ks;
	float shininess;
	Color n;

	material(Color n , Color k , Color kd , Color  ks , float shininess ,  bool isReflective , bool isRefractive ){

		//F0 = (((n - Color(1.0f, 1.0f, 1.0f)) ^ 2.0f) + (k ^ 2.0f)) / (((n + Color(1.0f, 1.0f, 1.0f)) ^ 2.0f) + (k ^ 2.0f));

		F0.r = (powf((n.r-1),2.0f)+powf(k.r,2.0f))/(powf((n.r+1),2.0f)+powf(k.r , 2.0f));
		F0.g = (powf((n.g-1),2.0f)+powf(k.g,2.0f))/(powf((n.g+1),2.0f)+powf(k.g , 2.0f));
		F0.b = (powf((n.b-1),2.0f)+powf(k.b,2.0f))/(powf((n.b+1),2.0f)+powf(k.b , 2.0f));
		this->kd = kd ;
		this->ks = ks;
		this->n = n;
		this->isRefractive = isRefractive;
		this->isReflective = isReflective;
		this->shininess = shininess;
	}

	Vector reflect(Vector inDir , Vector normal){
		inDir = inDir.normalize();
		normal = normal.normalize();
		return inDir-(normal*((normal*inDir)*2.0f));
	}

	Vector refract(Vector inDir , Vector normal){

		float nt = n.r;
		float cos = (-1.0f)*(inDir*normal);
		Vector tnormal;
		if(cos<0){
			normal = normal*(-1.0f);
			//normal.x = (-1.0f)*normal.x;
			//normal.y = (-1.0f)*normal.y;
			//normal.z = (-1.0f)*normal.z;
			nt = 1.0f/nt;
			//cos = -(normal*inDir);
			cos = (-1.0f)*cos;
		}else{lightspeed = 1.0f;}

		float disc;
		disc = 1.0f - ((1.0f - cos * cos ) / (nt * nt));


		if(disc<0.0001f){
			return reflect(inDir, normal);
		}

		return (inDir / nt + normal * (cos / nt -sqrt(disc)));
	}

	Color Fresnel(Vector inDir , Vector normal){
		inDir = inDir.normalize();
		normal = normal.normalize();
		float cos = fabs(inDir*normal);

		Color temp;
		temp.r = F0.r+(1.0f-F0.r)*powf((1.0f-cos),5);
		temp.g = F0.g+(1.0f-F0.g)*powf((1.0f-cos),5);
		temp.b = F0.b+(1.0f-F0.b)*powf((1.0f-cos),5);

		return temp;
	}

	Color shade(Vector normal , Vector viewDir , Vector lightDir , Color inRad){

		Color reflrad;
		lightDir = lightDir.normalize();
		float costh = normal*lightDir;
		if(costh<0){
			return reflrad;
		}

		reflrad = inRad*kd*costh;

		Vector halfway = (viewDir + lightDir).normalize();
		float cosdt = normal*halfway;
		if(cosdt<0){
			return reflrad;
		}

		return reflrad+inRad*ks*powf(cosdt , shininess);

	}
};



struct Hit{
	float t;
	Vector position;
	Vector normal;
	material *m;
	Hit(){t=-1;}
};

struct ray{
	ray(Vector e, Vector vi){
		eye = e;
		v = vi;
	}
	Vector eye;
	Vector v;
};

struct Camera{
	Vector ahead;
	Vector up;
	Vector right;
	Vector eye;
	Vector lookat;
	Camera(Vector eye, Vector lookat , Vector up){
		this->eye = eye;
		ahead = lookat-eye;
		this->lookat = lookat;
		this->up = up;
		right = up%lookat;
	}
	ray getRay(float x , float y){
		float alpha = x-300;
		float beta = y-300;
		Vector p0 = lookat+(right*alpha+(up*beta));
		Vector dir = (ahead+(right*alpha)+(up*beta)).normalize();
		return ray(p0,dir);
	}
};

struct light{
	Vector pos;
	Color color;
	light(Vector pos=Vector(0,0,0) ,Color color = Color(1.0f,1.0f,1.0f)){
		this->pos = pos;
		this->color = color;
	}
	Color weaken(float pos){
		return color/(powf(pos,2));
	}
};

class object{
public:
	material *m;
   virtual Hit intersect(ray r , float t0 , float t1)=0;
   virtual ~object(){

   }
};

class Wall : public object{
private:
	Vector normal;
	Vector p0;
	Vector p1;
	Vector p2;
	Color stripe;
	Color paint;
public:
	Wall(Vector p0 , Vector n, material *mat , Color str){
		stripe = str;
		paint = mat->kd;
		m = mat;
		normal =n;
		this->p0 = p0;
	}

	Hit intersect(ray r, float t0 , float t1){
		Hit best;
		float d = normal*r.v;
		if(d==0){
			return best;
		}

		double nx = normal.x;
		double ny = normal.y;
		double nz = normal.z;
		double Psx = p0.x;
		double Psy = p0.y;
		double Psz = p0.z;
		double dvx = r.v.x;
		double dvy = r.v.y;
		double dvz = r.v.z;
		double Pex = r.eye.x;
		double Pey = r.eye.y;
		double Pez = r.eye.z;
		double tb = -1.0 * ((nx * Pex - nx * Psx + ny * Pey - ny * Psy + nz * Pez - nz
		* Psz) / (nx * dvx + ny * dvy + nz * dvz));

		if(tb>0.0001f){
			best.t = tb ;
			best.m = this->m;
			best.position = r.eye+r.v*tb;
			best.normal = normal;

			float param = best.position.x+best.position.z+best.position.y;
			if((int)roundf(tb)%2==0){
				best.m->kd=stripe;
			}if((int)roundf(tb)%2!=0){
				best.m->kd = paint;
			}

			}

			return best;
		}
};

class Sphere :public object{
public:
	Vector o;
	float rad;

	Sphere(Vector o  , float r , material *mat){
		m=mat;
		this->o = o;
		this->rad = r;
	}

	Hit intersect(ray r , float t0 , float t1){
		Hit best;

		float tb;
		float a = r.v*r.v;
		float b = 2*((r.eye-o)*r.v);
		float c = ((r.eye-o)*(r.eye-o))-powf(rad,2.0f);
		float disc = powf(b,2.0f)-(4*a*c);

		if(disc<0.0001){

			return best;
		}
		float x1 = (-b+sqrt(disc))/(2*a);
		float x2 = (-b-sqrt(disc))/(2*a);

		if(x1<(0+0.001f)){
			tb = x2;
		}else if(x2<(0+0.001f)){
			tb = x1;
		}else if(x1<x2){
			tb = x1;
		}else if(x2<x1){
			tb = x2;
		}
		if(tb>(0+0.1f)){
		best.t = tb;
		best.m = m;
		best.position = r.eye+r.v*tb;
		best.normal = ((best.position-o)*2.0f).normalize();
		return best;
		}

		return best;
	}
};

class Paraboloid : public object{
public:
	float p1;
	float p2;
	float p3;
	int dir;
	Vector pos;

	Paraboloid(Vector pos ,float a , float b , float c , material *mat , int d){
		m=mat;
		p1 = a;
		p2 = b;
		p3 = c;
		dir = d;
		this->pos = pos;
	}
	Hit intersect(ray r , float t0 , float t1){
		Hit best;

		Vector v;
		v.x = r.v.x/(p1*p1);
		v.y = r.v.y/(p2*p2);
		v.z = -r.v.z*(p3*p3);

		Vector om;
		om.x = pos.x/(p1*p1);
		om.y = pos.y/(p2*p2);
		om.z = -pos.z*(p3*p3);

		Vector pm;
		pm.x = r.eye.x/(p1*p1);
		pm.y = r.eye.y/(p2*p2);
		pm.z = -r.eye.z*(p3*p3);

		pm = pm-om;

		float a = v.x*v.x+v.y*v.y;
		float b = 2.0f*(pm.x*v.x+pm.y*v.y)-v.z;
		float c = (pm.x*pm.x)+(pm.y*pm.y)-pm.z;

		float disc = powf(b,2.0f)-(4.0f*a*c);

		if(disc<0){
			return best;
		}

		float x1,x2;
		x1 = (-b+sqrt(disc))/(2*a);
		x2 = (-b-sqrt(disc))/(2*a);

		float tb;

		if(x1<x2){
			tb = x1;
		}else if(x2<x1){
			tb = x2;
		}
		if(x1<0.001f){
			tb = x2;
		}else if(x2<0.001f){
			tb = x1;
		}

			if(tb>0.5f){

				best.t = tb;
				best.m = m;
				best.position = r.eye+r.v*tb;

				Vector temp = best.position;

				best.normal = Vector(2.0f*(temp.x-pos.x) , 2.0f*(temp.y-pos.y) , 1.0f);
				best.normal.x = best.normal.x/(p1*p1);
				best.normal.y = best.normal.y/(p2*p2);
				best.normal.z = best.normal.z*(p3*p3);
				best.normal = best.normal.normalize();
				return best;

			}
				return best;


	}
};

class Ellipsoid : public object{
public:

	float p1,p2,p3;
	Vector o;
	Vector velocity;
	float sin;
	float cos;

	Ellipsoid(Vector center, float aa , float bb , float cc , material *mat , Vector velocity , float angle){
		angle = angle*(M_PI/180.0f);
		sin = sinf(angle);
		cos = cosf(angle);
		o = center;
		p1 = aa;
		p2 = bb;
		p3 = cc;
		m=mat;
		this->velocity = velocity;
	}
	Vector rotate(Vector v ){
		Vector temp;
		temp.x = v.x*cos-v.y*sin;
		temp.y = v.x*sin+v.y*cos;
		temp.z=v.z;

		temp.x = temp.x*cos-temp.z*sin;
		temp.z = -temp.x*sin+temp.z*cos;
		//temp.y = v.y;

		temp.y = temp.y*cos-temp.z*sin;
		temp.z = temp.y*sin+temp.z*cos;
		//temp.x = temp.x;
		//temp = v;
		return temp;
}


	Hit intersect(ray r , float t0 , float t1){

		Hit best;
		Vector op;
		float x , y , z;
		Vector p0;
		Vector v;
		Vector orot;

		for(float p = t0 ; p<(t1*60) ; p+=0.35f){
			op = o;//-(velocity*p);

			p0.x = r.eye.x*cos-r.eye.y*sin;
			p0.y = r.eye.x*sin+r.eye.y*cos;
			p0.z = r.eye.z;

			p0.x = p0.x*cos-p0.z*sin;
			p0.z = -p0.x*sin+p0.z*cos;

			p0.y = p0.y*cos-p0.z*sin;
			p0.z = p0.y*sin+p0.z*cos;

			v.x = r.v.x*cos-r.v.y*sin;
			v.y = r.v.x*sin+r.v.y*cos;
			v.z = r.v.z;

			v.x = v.x*cos-v.z*sin;
			v.z = -v.x*sin+v.z*cos;

			v.y = v.y*cos-v.z*sin;
			v.z = v.y*sin+v.z*cos;

			orot.x = op.x*cos-op.y*sin;
			orot.y = op.x*sin+op.y*cos;
			orot.z = op.z;

			orot.x = orot.x*cos-orot.z*sin;
			orot.z = -orot.x*sin+orot.z*cos;

			orot.y = orot.y*cos-orot.z*sin;
			orot.z = orot.y*sin+orot.z*cos;

			x = (p0.x+lightspeed*v.x*p)-orot.x;
			y = (p0.y+lightspeed*v.y*p)-orot.y;
		    z = (p0.z+lightspeed*v.z*p)-orot.z;

		    x = (x*x)/(p1*p1);
		    y = (y*y)/(p2*p2);
		    z = (z*z)/(p3*p3);


		    float res = x+y+z-1.0f;
		   if(fabs(res)<0.1f){
			   best.t = p;
			   best.m = m;
			   best.position = r.eye+r.v*p;
			   best.normal = ((best.position-op)*2.0f);

			   best.normal.x = best.normal.x*cos-best.normal.y*sin;
			   best.normal.y = best.normal.x*sin+best.normal.y*cos;

			   best.normal.x = best.normal.x*cos-best.normal.z*sin;
			   best.normal.z = -best.normal.x*sin+best.normal.z*cos;

			   best.normal.y = best.normal.y*cos-best.normal.z*sin;
			   best.normal.z = best.normal.y*sin+best.normal.z*cos;

			   best.normal.x = best.normal.x/(p1);
			   best.normal.y = best.normal.y/(p2);
			   best.normal.z = best.normal.z/(p3);


			   best.normal.x = best.normal.x*cos-best.normal.y*(-sin);
			   best.normal.y = best.normal.x*(-sin)+best.normal.y*cos;

			   best.normal.x = best.normal.x*cos-best.normal.z*(-sin);
			   best.normal.z = -best.normal.x*(-sin)+best.normal.z*cos;

			   best.normal.y = best.normal.y*cos-best.normal.z*(-sin);
			   best.normal.z = best.normal.y*(-sin)+best.normal.z*cos;

			   best.normal = best.normal.normalize();
			   return best;
		   }

/*
		Vector vin = rotate(r.v,0);

		Vector v;
		v.x = vin.x/(p1);
		v.y = vin.y/(p2);
		v.z = vin.z/(p3);

		Vector orot = rotate(op,0);

		Vector om;
		om.x = orot.x/(p1);
		om.y = orot.y/(p2);
		om.z = orot.z/(p3);

		Vector pr =rotate(r.eye,0);

		Vector pm;
		pr.x = pr.x/(p1);
		pr.y = pr.y/(p2);
		pr.z = pr.z/(p3);

		pm = pr-om;

				float tb;

				float a = v*v;
				float b = 2.0f*(pm*v);
				float c = (pm*pm)-1.0f;

				float disc = powf(b,2)-(4*a*c);

				if(disc<0.001f){
					return best;
				}
				float x1 = (-b+sqrt(disc))/(2.0f*a);
				float x2 = (-b-sqrt(disc))/(2.0f*a);

				if(x1<x2){
					tb = x1;
				}else if(x2<x1){
					tb = x2;
				}
				if(x1<0.001f){
					tb = x2;
				}else if(x2<0.001f){
					tb = x2;
				}
				tb = x2;

				if(fabs(tb-(p*60)<0.1f)){

				if(tb>(1.0f)){

				best.t = tb;
				best.m = m;
				best.position = r.eye+r.v*tb;

				best.normal = (best.position-op)*2.0f;

				best.normal = rotate(best.normal,0);

				best.normal.x = best.normal.x/(p1);
				best.normal.y = best.normal.y/(p2);
				best.normal.z = best.normal.z/(p3);

				best.normal = rotate(best.normal,0);

				best.normal = best.normal.normalize();
				return best;
				}
			}
			*/
	}
	return best;
}

};

object *obs[100];
int objnum = 0;
int maxdepth = 5;
float eps = 0.001f;
light l(Vector(280.0f , -280.0f , 200.0f) , Color(1.0f, 1.0f , 1.0f));

Vector lampspeed(0.0,0.7f,0);
Vector elipsspeed(0.4f,0.3f,0);
float end;

Hit firstIntersect(ray r , float t0 , float t1){

	Hit best;
		for(int i=0 ; i<objnum ; i++){
			Hit hit = obs[i]->intersect(r,t0 , t1);
			if(hit.t>0 && (best.t < 0 || hit.t < best.t)){
				best = hit;
			}
		}
	return best;
}

Color trace(ray r , int depth , float t0 , float t1){
	Color outRad(0,0,0);

	if(depth>maxdepth) return outRad;

	Hit hit = firstIntersect(r , t0 , t1);

	if(hit.t<0)return outRad;
	if(!hit.m->isReflective && !hit.m->isRefractive){

	ray shadowray(hit.position+hit.normal*0.001f , (l.pos-hit.position).normalize() );

	Hit shadowHit = firstIntersect(shadowray , hit.t , t1);
	if(!hit.m->isReflective && !hit.m->isRefractive){
	if((shadowHit.t<0 || shadowHit.t > (l.pos-hit.position).Length())){

		float dist =hit.t;

		Vector pos;
		if(dist<end*60){
			pos = l.pos-lampspeed*dist;
		}else{
			pos = l.pos-lampspeed*(end*60);
		}

		Color cinrad = l.weaken((pos-hit.position).Length()/500);

		outRad = outRad + hit.m->shade(hit.normal , r.v , pos-hit.position , cinrad);

		}
		}
	}
	if(hit.m->isReflective){
		Vector refldir = hit.m->reflect(r.v , hit.normal);
		ray reflray(hit.position+hit.normal*0.0001f , refldir);
		outRad = outRad+trace(reflray , depth+1 , hit.t , t1)*hit.m->Fresnel(r.v , hit.normal);
	}

	if(hit.m->isRefractive){
		Vector refractDir = hit.m->refract(r.v , hit.normal);
		refractDir.normalize();
		ray refractRay(hit.position-hit.normal*0.0001f , refractDir);
		outRad = outRad+trace(refractRay , depth+1 , hit.t , t1)*(Color(1.0f, 1.0f , 1.0f)-hit.m->Fresnel(r.v , hit.normal));
	}

	if(outRad.r>1){
		outRad.r=1;
	}
	if(outRad.g>1){
		outRad.g=1;
	}
	if(outRad.b>1){
		outRad.b=1;
	}

	return outRad;
}



const int screenWidth = 600;	// alkalmazås ablak felbontåsa
const int screenHeight = 600;


Color image[screenWidth*screenHeight];	// egy alkalmazås ablaknyi kÊp
Color traced[600][600];
Camera cam(Vector(0,0,-1000) , Vector(0,0,1) , Vector(0,1,0));
Ellipsoid *elips;

material gold(Color(0.17f , 0.35f , 1.5f) , Color(3.1f, 2.7f, 1.9f) , Color(1,1,1) , Color(1,1,1), 100.0f , true , false);
material glass(Color(1.5f , 1.5f , 1.5f), Color(0,0,0) , Color(0,0,0) , Color(0,0,0) , 100.0f , false, true);
material left(Color(0.0f , 0.0f , 0.0f) , Color(1.0f, 0.0f, 1.0f) , Color(1,1,1) , Color(1,1,1), 3000.0f , false, false);
material right(Color(0.0f , 0.0f , 0.0f) , Color(1.0f, 1.0f, 0.0f) , Color(1,1,1) , Color(1,1,1), 3000.0f , false, false);
material up(Color(0.0f , 0.0f , 0.0f) , Color(0.0f, 0.0f, 0.0f) , Color(1,1,1) , Color(1,1,1), 3000.0f , false, false);
material down(Color(0.0f , 0.0f , 0.0f) , Color(0.0f, 0.0f, 0.0f) , Color(1,1,1) , Color(1,1,1), 3000.0f , false, false);
material forw(Color(0.0f , 0.0f , 0.0f) , Color(0.0f, 0.0f, 0.0f) , Color(1,1,1) , Color(1,1,1), 3000.0f , false, false);
material back(Color(0.0f , 0.0f , 0.0f) , Color(0.0f, 0.0f, 0.0f) , Color(1,1,1) , Color(1,1,1), 3000.0f , false, false);
material a(Color(0.0f , 0.0f , 0.0f) , Color(0.0f, 0.0f, 0.0f) , Color(1,0,0) , Color(1,1,1), 3000.0f , false, false);

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) {
	glViewport(0, 0, screenWidth, screenHeight);
	obs[objnum]=new Paraboloid(Vector(0,0,828) , 1.0f , 1.0f , 28.0f ,&gold , 1);
	objnum++;
	//obs[objnum]= new Sphere(Vector(0.0f, 0.0f ,300.0f) , 50.0f , &glass);
	//objnum++;

	//obs[objnum]=new Sphere(Vector(0.0f , 0.0f , 200.0f) , 150.0f , &gold);
	//objnum++;
	elips = new Ellipsoid(Vector(-250,-250,100) , 60.0f , 30.0f , 15.0f , &glass ,elipsspeed , 20.0f);
	obs[objnum]= elips;
	objnum++;

	obs[objnum]=new Wall(Vector(300,0,0),Vector(-1, 0 , 0), &right, Color(0,1,1));
	objnum++;
	obs[objnum]=new Wall(Vector(0,300,0) , Vector(0,-1,0) , &up , Color(0.7,0.6,0.7));
	objnum++;
	obs[objnum]= new Wall(Vector(-300 , 0,0) , Vector(1,0,0), &left, Color(1, 0 , 1));
	objnum++;
	obs[objnum]= new Wall(Vector(0,-300,0) , Vector(0,1,0) , &down, Color(0.5, 0.7, 0.9));
	objnum++;
	//obs[objnum]=new Wall(Vector(0,0,600) , Vector(0,0,-1) , &forw, Color(1,1,0));
	//objnum++;
	obs[objnum]=new Wall(Vector(0,0,0) , Vector(0,0,1),&back, Color(0.7, 0.7, 0.7));
	objnum++;


/*
	int db = 0;
	    for (int i = 0; i < screenHeight; i++) {
	        for (int j = 0; j < screenWidth; j++) {
	            image[db++] = traced[j][i];
	        }
	    }

*/
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

    // ..

    // Peldakent atmasoljuk a kepet a rasztertarba
    glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, image);


    glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
    if (key == 0x20){
    	end = glutGet(GLUT_ELAPSED_TIME);
    	end = end/1000;
    	l.pos = l.pos+(lampspeed*(end*60));
    	elips->o = elips->o+(elipsspeed*(end*60));

    	for (int i = 0; i < screenHeight; i++) {
    		for (int j = 0; j < screenWidth; j++) {
    		//traced[i][j] = trace(ray(eye, (Vector((i / 300.0f - 1.0f), (j / 300.0f - 1.0f), 0.0f)-eye ).normalize()), 0);
    		//traced[i][j]=trace(cam.getRay(i,j) , 0);
    		image[i*600+j]=trace(cam.getRay(j,i), 0 , 0 , end);
    		}
    	}
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

   	// program inditasa ota eltelt ido
    // anim.animate(glutGet(GLUT_ELAPSED_TIME));
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
