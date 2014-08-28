#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <string>
#include <cmath>
#include <sstream>

using std::string;

#define PI 3.1415926535897932384626433832795028841968

class RGBColor;
struct Vertex2f;
struct Vector2f;
struct Vertex3f;
struct Vector3f;
struct Feature;
struct Triangle3f;

inline string intToString(int number) { 
  std::stringstream ss;
  ss << number;
  return ss.str();
}

inline float degreesToRad(float degree) { 
  return (float)(degree*PI/180.0); 
}

inline int sign(float f) {
  return f >= 0 ? 1 : -1;
}

inline double random_double(double min, double max) {
  double range = max - min;
  return min + (double(rand()) / double(RAND_MAX)) * range;
}

inline float random_float(float min, float max) {
  float range = max - min;
  return min + (float(rand()) / float(RAND_MAX)) * range;
}

inline int random_int(int min, int max) {
  int range = max - min;
  return min + int((double(rand())/double(RAND_MAX)) * range);
}

inline RGBColor random_rgbcolor(float scale);

inline void get_random_color(float &r, float &g, float &b,
    float r_min = 0., float r_max = 1.,
    float g_min = 0., float g_max = 1.,
    float b_min = 0., float b_max = 1.);

inline RGBColor linColor(RGBColor c1, RGBColor c2, float t);

inline float length(Vector2f a);
inline float length(Vector3f a);

inline Vector2f normalize(Vector2f a);
inline Vector3f normalize(Vector3f a);

inline float dotProduct(Vector2f a, Vector2f b);
inline float dotProduct(Vector3f a, Vector3f b);
inline Vector3f crossProduct(Vector3f a, Vector3f b);
inline Vector3f calculateNormal(Vertex3f a, Vertex3f b, Vertex3f c);

inline float Distance(Vertex2f a, Vertex2f b);
inline float Distance(Vertex3f a, Vertex3f b);

inline Vertex2f VecToVert(Vector2f v);
inline Vertex3f VecToVert(Vector3f v);
inline Vector2f VertToVec(Vertex2f v);
inline Vector3f VertToVec(Vertex3f v);

inline Vector2f FeatureVec(Feature f);
inline float length(Feature f);
inline Feature linFeature(Feature src, Feature dest, float t);

class RGBColor
{
public:

    float r, g, b;
    
  RGBColor(float red, float green, float blue):
      r(red),
      g(green),
      b(blue)
      {}
    
  RGBColor():
      r(0),
      g(0),
      b(0)
      {}
    
  RGBColor operator+(RGBColor const o) {
    return RGBColor(r+o.r, g+o.g, b+o.b);
  }
  RGBColor operator-(RGBColor const o) {
    return RGBColor(r-o.r,g-o.g, b-o.b);
  }
  RGBColor operator*(float s) {
    return RGBColor(r*s, g*s, b*s);
  }
};

struct Vector2f {
  float x, y;

  Vector2f(float _x = 0., float _y = 0.) {
    x = _x;
    y = _y;
  }

  Vector2f operator*(float s) { return Vector2f(x*s, y*s); }
  Vector2f operator/(float s) { return Vector2f(x/s, y/s); }
  Vector2f operator-() { return Vector2f(-x, -y); }
  Vector2f operator-(Vector2f const o) { return Vector2f(x-o.x, y-o.y); }
  Vector2f operator+(Vector2f const o) { return Vector2f(x+o.x, y+o.y); }
};

struct Vertex2f {
  float x, y;

  Vertex2f(float _x = 0., float _y = 0.) {
    x = _x;
    y = _y;
  }

  Vertex2f operator*(float s) { return Vertex2f(x*s, y*s); }
  Vertex2f operator/(float s) { return Vertex2f(x/s, y/s); }
  Vertex2f operator-() { return Vertex2f(-x, -y); }
  Vertex2f operator-(Vertex2f const o) { return Vertex2f(x-o.x, y-o.y); }
  Vertex2f operator+(Vertex2f const o) { return Vertex2f(x+o.x, y+o.y); }
  Vertex2f operator-(Vector2f const o) { return Vertex2f(x-o.x, y-o.y); }
  Vertex2f operator+(Vector2f const o) { return Vertex2f(x+o.x, y+o.y); }  
};

struct Vector3f {    
  float x, y, z;
    
  Vector3f(float _x = 0., float _y = 0., float _z = 0.) {
      x = _x;
      y = _y;
      z = _z;
  }
    
  Vector3f operator*(float s) { return Vector3f(x*s, y*s, z*s); }
  Vector3f operator-() { return Vector3f(-x, -y, -z); }
  Vector3f operator-(Vector3f const o) { return Vector3f(x-o.x, y-o.y, z-o.z); }
  Vector3f operator+(Vector3f const o) { return Vector3f(x+o.x, y+o.y, z+o.z); }    
};

struct Vertex3f {
  float x, y, z;
  Vector3f n; // normal
  bool hasUniqueNormal;

  Vertex3f(float _x = 0., float _y = 0., float _z = 0.) {
    x = _x;
    y = _y;
    z = _z;
  }

  Vertex3f operator*(float s) { return Vertex3f(x*s, y*s, z*s); }
  Vertex3f operator-() { return Vertex3f(-x, -y, -z); }
  Vertex3f operator-(Vertex3f const o) { return Vertex3f(x-o.x, y-o.y, z-o.z); }
  Vertex3f operator+(Vertex3f const o) { return Vertex3f(x+o.x, y+o.y, z+o.z); }
  Vertex3f operator-(Vector3f const o) { return Vertex3f(x-o.x, y-o.y, z-o.z); }
  Vertex3f operator+(Vector3f const o) { return Vertex3f(x+o.x, y+o.y, z+o.z); }
};

struct Feature {
  Vertex2f p, q;

  Feature(Vertex2f _p = Vertex2f(), Vertex2f _q = Vertex2f()) : p (_p),
      q (_q) { }

  Feature operator*(float s) { return Feature(p*s,q*s); }
  Feature operator-() { return Feature(-p, -q); }
  Feature operator-(Feature const o) { return Feature(p-o.p, q-o.q); }
  Feature operator+(Feature const o) { return Feature(p+o.p, q+o.q); }    
  friend std::ostream& operator<<(std::ostream& os, Feature& feature) {
	os << feature.p.x << " " 
        << feature.p.y << " "
        << feature.q.x << " "
	    << feature.q.y << std::endl;
	return os;
  }
};

struct Triangle3f {
  Vertex3f a, b, c; // object space coordinates
  RGBColor a_color, b_color, c_color;

  Triangle3f(Vertex3f _a = Vertex3f(), Vertex3f _b = Vertex3f(),
      Vertex3f _c = Vertex3f()) {
    a = _a;
    b = _b;
    c = _c;
  }

  void color_verts(RGBColor _a, RGBColor _b, RGBColor _c) { a_color = _a; b_color = _b; c_color = _c;}

};

inline RGBColor linColor(RGBColor c1, RGBColor c2, float t){ return c1 * (1.f - t) + c2 * t; }

inline RGBColor random_rgbcolor(float scale)
{
    float r = random_float(0.f, scale);
    float g = random_float(0.f, scale);
    float b = random_float(0.f, scale);
    return RGBColor(r,g,b);
}

inline void get_random_color(float &r, float &g, float &b,
    float r_min, float r_max,
    float g_min, float g_max,
    float b_min, float b_max) {
  r = (r_min == r_max) ? r_min : random_float(r_min,r_max);
  g = (g_min == g_max) ? g_min : random_float(g_min,g_max);
  b = (b_min == b_max) ? b_min : random_float(b_min,b_max);
}

inline float length(Vector2f a) { return sqrt(dotProduct(a,a)); }
inline float length(Vector3f a) { return sqrt(dotProduct(a,a)); }

inline Vector2f normalize(Vector2f a) { return a * (1 / length(a)); }
inline Vector3f normalize(Vector3f a) { return a * (1 / length(a));}

inline float dotProduct(Vector2f a, Vector2f b) { return a.x * b.x + a.y * b.y; }
inline float dotProduct(Vector3f a, Vector3f b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

inline Vector3f crossProduct(Vector3f a, Vector3f b)
{
    Vector3f normal;
    normal.x = a.y * b.z - a.z * b.y;
    normal.y = a.z * b.x - a.x * b.z;
    normal.z = a.x * b.y - a.y * b.x;
    return normal;
}

inline Vector3f calculateNormal(Vertex3f a, Vertex3f b, Vertex3f c) {
  Vector3f u( b.x-a.x , b.y-a.y , b.z-a.z );
  Vector3f v( c.x-a.x , c.y-a.y , c.z-a.z );
  return normalize(crossProduct(u,v));
}

inline float Distance(Vertex2f a, Vertex2f b) {
  float xDist = a.x - b.x; float yDist = a.y - b.y;
  return sqrt(xDist * xDist + yDist * yDist);        
}

inline float Distance(Vertex3f a, Vertex3f b) {
  float xDist = a.x - b.x;
  float yDist = a.y - b.y;
  float zDist = a.z - b.z;
  return sqrt(xDist * xDist + yDist * yDist + zDist * zDist);
}

inline Vertex2f VecToVert(Vector2f v) {
  return Vertex2f(v.x, v.y);
}

inline Vertex3f VecToVert(Vector3f v) {
  return Vertex3f(v.x, v.y, v.z);
}

inline Vector2f VertToVec(Vertex2f v) {
  return Vector2f(v.x, v.y);
}

inline Vector3f VertToVec(Vertex3f v) {
  return Vector3f(v.x, v.y, v.z);
}

inline Vector2f FeatureVec(Feature f){
    return VertToVec(f.q - f.p);
}

inline float length(Feature f) {
    return length(FeatureVec(f));
}

inline Feature linFeature(Feature src, Feature dest, float t) {
  return  src * (1.f - t) + dest * t;
}

#endif