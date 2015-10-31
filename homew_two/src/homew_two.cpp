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
	   return Color(r-f , g-f , b-f);
   }
};

Vector transform(Vector p){
	Vector temp;
	temp.x = ((p.x)/600)*10;
	temp.y = ((p.y)/600)*10;
	temp.z = p.z;
	return temp;
}

int texture=0;

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
		return inDir-normal*((normal*inDir)*2.0f);
	}

	Vector refract(Vector inDir , Vector normal){

		Vector tnormal=normal;
		float nt = n.r;
		float cos = -(normal*inDir);

		if(cos<0){
			tnormal = normal*(-1.0f);
			nt = 1/nt;
			//cos = -(normal*inDir);
			cos = (-1.0f)*cos;
		}

		float disc;
		disc = 1.0f - ((1.0f - cos * cos ) / (nt * nt));


		if(disc<0.01f){
			return reflect(inDir, tnormal);
		}

		Vector res;

		res = inDir / nt + tnormal * (cos / nt -sqrt(disc));
/*
		res.x=inDir.x/nt+tnormal.x*(cos/nt-sqrtf(disc));
		res.y=inDir.y/nt+tnormal.y*(cos/nt-sqrtf(disc));
		res.z=inDir.z/nt+tnormal.z*(cos/nt-sqrtf(disc));
		//std::cout<<res.x<<" "<<res.y<<" "<<res.z<<std::endl;
		 *
		 */
		return res;
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
	ray(Vector eye , Vector dir){
		this->eye = eye;
		v = dir;
	}
	Vector eye;
	Vector v;
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
   virtual Hit intersect(ray r , bool shadow){

   }
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

	Hit intersect(ray r , bool shadow){
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
		double t = -1.0 * ((nx * Pex - nx * Psx + ny * Pey - ny * Psy + nz * Pez - nz
		* Psz) / (nx * dvx + ny * dvy + nz * dvz));

		if(t>0.0001f){
			best.t = t ;
			best.m = this->m;
			best.position = r.eye+r.v*t;
			best.normal = normal;

			if((int)roundf(best.t)%10==0){
				best.m->kd=stripe;
			}else if((int)roundf(best.t)%10!=0){
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

	Hit intersect(ray r , bool shadow){
		Hit best;

		float tb;
		float a = r.v*r.v;
		float b = 2*((r.eye-o)*r.v);
		float c = ((r.eye-o)*(r.eye-o))-powf(rad,2.0f);
		float disc = powf(b,2)-(4*a*c);

		if(disc<0){

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
		if(tb>(0+0.001f)){
		best.t = tb;
		best.m = m;
		best.position = r.eye+r.v*tb;
		best.normal = (best.position-o).normalize();
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
	Hit intersect(ray r , bool shadow){
		Hit best;

		Vector v;
		v.x = r.v.x/p1;
		v.y = r.v.y/p2;
		v.z = r.v.z/p3;

		Vector om;
		om.x = pos.x/p1;
		om.y = pos.y/p2;
		om.z = pos.z/p3;

		Vector pm;
		pm.x = r.eye.x/p1;
		pm.y = r.eye.y/p2;
		pm.z = r.eye.z/p3;

		pm = pm-om;

		float a = v.x*v.x+v.y*v.y;
		float b = 2.0f*(pm.x*v.x+pm.y*v.y)-v.z;
		float c = pm.x*pm.x+pm.y*pm.y-pm.z;

		float disc = powf(b,2.0f)-(4.0f*a*c);

		if(disc<0){
			return best;
		}

		float x1,x2;
		x1 = (-b+sqrt(disc))/(2*a);
		x2 = (-b-sqrt(disc))/(2*a);

		float tb;
				if(x1<0.01f){
					tb = x2;
				}else if(x2<0.01f){
					tb = x1;
				}else if(x1<x2){
					tb = x1;
				}else if(x2<x1){
					tb = x2;
				}

				best.t = tb;
				best.m = m;
				best.position = r.eye+r.v*tb;


				Vector temp;
				temp.x = best.position.x/p1;
				temp.y = best.position.y/p2;
				temp.z = best.position.z/p3;

				best.normal = Vector(temp.x*2.0f , temp.y*2.0f , 1.0f/p3*dir).normalize();
				return best;


				return best;


	}
};

class Ellipsoid : public object{
public:

	float p1,p2,p3;
	Vector o;

	Ellipsoid(Vector center, float aa , float bb , float cc , material *mat){
		o = center;
		p1 = aa;
		p2 = bb;
		p3 = cc;
		m=mat;
	}
	Vector rotate(Vector v){
		Vector temp;
		temp.x = v.x*cosf(20)-v.y*sinf(20);
		temp.y = v.x*sinf(20)-v.y*cosf(20);
		temp.z=v.z;
	}


	Hit intersect(ray r ,bool shadow){

		Vector v;
		v.x = r.v.x/p1;
		v.y = r.v.y/p2;
		v.z = r.v.z/p3;

		Vector om;
		om.x = o.x/p1;
		om.y = o.y/p2;
		om.z = o.z/p3;

		Vector pm;
		pm.x = r.eye.x/p1;
		pm.y = r.eye.y/p2;
		pm.z = r.eye.z/p3;

		pm = pm-om;

		Hit best;

				float tb;
				float a = v*v;
				float b = 2*((pm)*v);
				float c = ((pm)*(pm))-powf(1.0f,2.0f);
				float disc = powf(b,2)-(4*a*c);

				if(disc<0){

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
				if(tb>(0+0.001f)){
				best.t = tb;
				best.m = m;
				best.position = r.eye+r.v*tb;

				//best.position.z = best.position.x*cosf(90.0f)-best.position.y*sinf(90.0f);
				//best.position.y = best.position.x*sinf(90.0f)+best.position.y*cosf(90.0f);

				Vector temp;
				temp.x = best.position.x/(p1);
				temp.y = best.position.y/(p2);
				temp.z = best.position.z/(p3);

				best.normal = (((temp-om)*2.0f).normalize());

				return best;
				}

				return best;
			}

};

object *obs[100];
int objnum = 0;
int maxdepth = 7;
float eps = 0.001f;
light l(Vector(0.0f , 180.0f , 0.0f) , Color(1.0f, 1.0f , 1.0f));
Hit firstIntersect(ray r , bool shadow){

	Hit best;
	for(int i=0 ; i<objnum ; i++){
		Hit hit = obs[i]->intersect(r , shadow);
		if(hit.t>0 && (best.t < 0 || hit.t < best.t)){
			best = hit;
		}
	}

	return best;

}

Color trace(ray r , int depth){
	Color outRad(0,0,0);

	bool recurse=false;

	//std::cout<<depth;
	if(depth>maxdepth) return outRad;

	Hit hit = firstIntersect(r , recurse);

	if(hit.t<0)return outRad;
	if(!hit.m->isReflective && !hit.m->isRefractive){

	ray shadowray(hit.position+hit.normal*0.0001f , (l.pos-hit.position).normalize() );

	//Hit shadowHit = firstIntersect(shadowray ,true);
	//if(shadowHit.t<0 || shadowHit.t > (l.pos-hit.position).Length()){

		float dist = (hit.position-l.pos).Length();
		dist = fabs(dist/300);
		Color cinrad = l.weaken(dist);
		outRad = outRad + hit.m->shade(hit.normal , r.v , l.pos-hit.position , cinrad);
	//}

	}

	if(hit.m->isReflective){
		Vector refldir = hit.m->reflect(r.v , hit.normal);
		ray reflray(hit.position+hit.normal*0.0001f , refldir);
		outRad = outRad+trace(reflray , depth+1)*hit.m->Fresnel(r.v , hit.normal);
	}

	if(hit.m->isRefractive){

		Vector refractDir = hit.m->refract(r.v , hit.normal);
		refractDir.normalize();
		ray refractRay(hit.position-hit.normal*0.0001f , refractDir);
		outRad = outRad+trace(refractRay , depth+1)*(Color(1.0f, 1.0f , 1.0f)-hit.m->Fresnel(r.v , hit.normal));
		/*
		Vector dir;
		Vector tnormal=hit.normal;
		float nt = hit.m->n.r;
		float cos = -(hit.normal*r.v);
		if(cos<0){
			cos = (-1.0f)*cos;
			tnormal.x = -tnormal.x;
			tnormal.y = -tnormal.y;
			nt = 1.0f/nt;
		}

		float disc;
		disc = 1.0f-(1.0f-cos*cos)/(nt*nt);

		if(disc>0){
			r.v.normalize();
			ray refractRay(hit.position-tnormal*0.0001*(-1.0f), r.v/nt+tnormal*(cos/nt-sqrtf(disc)));
			//refractRay.eye= hit.position-tnormal*0.01*(-1.0f);
			//refractRay.v = refractRay.v/nt+tnormal*(cos/nt-sqrtf(disc));
			refractRay.v.normalize();
			outRad = outRad+trace(refractRay , depth+1)*(Color(1.0f, 1.0f , 1.0f)-hit.m->Fresnel(r.v , hit.normal));
		}else if(disc<0){
			Vector refldir = hit.m->reflect(r.v , tnormal);
			ray reflectRay( hit.position-tnormal*0.0001 , refldir);
			outRad = outRad+trace(reflectRay , depth+1)*(Color(1.0f, 1.0f , 1.0f)-hit.m->Fresnel(r.v , hit.normal));
		}

		*/


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


// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) {
	glViewport(0, 0, screenWidth, screenHeight);
	Vector v;
	Vector eye(0.0f,0.0f,-1.0f);


	material gold(Color(0.17f , 0.35f , 1.5f) , Color(3.1f, 2.7f, 1.9f) , Color(1,1,1) , Color(1,1,1), 100.0f , true , false);
	material glass(Color(1.5f , 1.5f , 1.5f), Color(0,0,0) , Color(0,0,0) , Color(0,0,0) , 100.0f , false, true);
	material c(Color(0.17f , 0.35f , 1.5f) , Color(3.1f, 2.7f, 1.9f) , Color(1,1,1) , Color(1,1,1), 30.0f , false, false);
	material f(Color(0.17f , 0.35f , 1.5f) , Color(3.1f, 2.7f, 1.9f) , Color(1,1,1) , Color(1,1,1), 30.0f , false, false);
	material d(Color(0.17f , 0.35f , 1.5f) , Color(3.1f, 2.7f, 1.9f) , Color(1,1,1) , Color(1,1,1), 30.0f , false, false);
	material a(Color(0.17f , 0.35f , 1.5f) , Color(3.1f, 2.7f, 1.9f) , Color(1,1,1) , Color(1,1,1), 300.0f , false, false);
	material z(Color(0.17f , 0.35f , 1.5f) , Color(3.1f, 2.7f, 1.9f) , Color(1,1,1) , Color(1,1,1), 30.0f , false, false);
	material q(Color(0.17f , 0.35f , 1.5f) , Color(3.1f, 2.7f, 1.9f) , Color(1,1,1) , Color(1,1,1), 3000.0f , false, false);
	material s(Color(0.17f , 0.35f , 1.5f) , Color(3.1f, 2.7f, 1.9f) , Color(1,1,1) , Color(1,1,1), 50.0f , false, false);

	obs[objnum]= new Sphere(Vector(0.0f, 0.0f ,300.0f) , 167.0f , &gold);
	objnum++;
	//obs[objnum]=new Paraboloid(Vector(0,0,200) , 20.0f , 20.0f , 1.0f ,&gold , 1);
	//objnum++;
	//obs[objnum]=new Sphere(Vector(-30.0f , -20.0f , 100.0f) , 30.0f , &glass);
	//objnum++;

	obs[objnum]=new Ellipsoid(Vector(0,-30,80) , 60 , 15 , 15 , &glass);
	objnum++;

	obs[objnum]=new Wall(Vector(200,0,0),Vector(-1, 0 , 0), &c, Color(0.3,0.6,0.8));
	objnum++;
	obs[objnum]=new Wall(Vector(0,200,0) , Vector(0,-1,0) , &f , Color(0.7,0.6,0.7));
	objnum++;
	obs[objnum]= new Wall(Vector(-200 , 0,0) , Vector(1,0,0), &a, Color(0.5, 0.9 , 0.8));
	objnum++;

	obs[objnum]= new Wall(Vector(0,-200,0) , Vector(0,1,0) , &z, Color(0.5, 0.7, 0.9));
	objnum++;

	obs[objnum]=new Wall(Vector(0,0,300) , Vector(0,0,-1) , &d, Color(0.5,0.8,0.5));
	objnum++;
	obs[objnum]=new Wall(Vector(0,0,-100) , Vector(0,0,1),&q, Color(0.4, 0.8 , 0.8));
	objnum++;

	for (int i = 0; i < screenHeight; i++) {
	        for (int j = 0; j < screenWidth; j++) {
	            traced[i][j] = trace(ray(eye, (Vector((i / 300.0f - 1.0f), (j / 300.0f - 1.0f), 0.0f) - eye).normalize()), 0);
	        }
	    }

	int db = 0;
	    for (int i = 0; i < screenHeight; i++) {
	        for (int j = 0; j < screenWidth; j++) {
	            image[db++] = traced[j][i];
	        }
	    }


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
    if (key == 'd') glutPostRedisplay( ); 		// d beture rajzold ujra a kepet

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
     long time = glutGet(GLUT_ELAPSED_TIME);		// program inditasa ota eltelt ido

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
