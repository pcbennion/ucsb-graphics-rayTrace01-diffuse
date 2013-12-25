#include "RayTrace.h"
#include "Vector3.h"

// shere constructor
Sphere::Sphere(float x, float y, float z, float radius, float R, float G, float B)
{
    center = new float[3];
    center[0] = x; center[1] = y; center[2] = z;

    r = radius;

    color = new float[3];
    color[0] = R; color[1] = G; color[2] = B;
}

// implements the quadratic formula to find how many ray lengths until the ray intersects the sphere
// returns # of ray lengths if successful
// returns -1 if no intersect
float Sphere::findIntersect(float *rayDir, float *raySrc)
{
    int i;

    // origin vector minus center vector
    float* omc = new float[3];
    for(i=0; i<3; i++) {omc[i] = raySrc[i] - center[i];}

    // compute three parts of quadratic equation:
    float a = vectorDotProduct(rayDir, rayDir);
    float b = 2* vectorDotProduct(rayDir, omc);
    float c = vectorDotProduct(omc, omc) - r*r;

    // quadratic formula
    // calculate discriminant. if <0, no interect so return -1
    float disc = b*b - 4*a*c;
    if(disc<0.0f) return -1;
    // remaining quadratic stuff
    float t1 = (-b + sqrt(disc))/(2 * a);
    float t2 = (-b - sqrt(disc))/(2 * a);

    // clean up
    delete omc;

    // return smaller t
    if(t2<t1 && t2>0) return t2;
    return t1;
}

// sphere accessors
float* Sphere::getCenter(){return center;}
float* Sphere::getColor(){return color;}

// light constructor
Light::Light(float x, float y, float z, float R, float G, float B)
{
    center = new float[3];
    center[0] = x; center[1] = y; center[2] = z;

    color = new float[3];
    color[0] = R; color[1] = G; color[2] = B;
}

// uses Blinn-Phong model to increment color of pixel
// out is the color array
// kd is the color of the sphere
// eye is the eye location, p is the point on the sphere, n is the normal at p
void Light::calcLighting(float* &out, float *kd, float *eye, float *p, float *n)
{
    int i;

    // calculate and normalize useful vectors
    vectorNormalize(n);
    float* v = new float[3]; // vector to the eye ray
    v[0] = eye[0] - p[0]; v[1] = eye[1] - p[1]; v[2] = eye[2] - p[2];
    vectorNormalize(v);
    float* l = new float[3]; // vector to the light source
    l[0] = center[0] - p[0]; l[1] = center[1] - p[1]; l[2] = center[2] - p[2];
    vectorNormalize(l);
    float* h = new float[3]; // intermediate vector between l and v
    h[0] = v[0] + l[0]; h[1] = v[1] + l[1]; h[2] = v[2] + l[2];
    vectorNormalize(h);

    // get dot products
    float nl = vectorDotProduct(n, l), nh = vectorDotProduct(n, h);
    nl = (nl>0) ? nl : 0;
    nh = (nh>0) ? nh : 0;

    // "perfect reflection" vector
    float* r = new float[3];
    r[0] = 2*nl*n[0] - l[0]; r[1] = 2*nl*n[1] - l[1]; r[2] = 2*nl*n[2] - l[2];
    vectorNormalize(r);
    float rv = vectorDotProduct(r, v);
    rv = (rv>0) ? rv : 0;

    // calculate lighting
    for(i=0; i<3; i++)
        out[i]+=/* Diffuse: */    kd[i] * color[i] * nl
              + /* Specular:*/    kd[i] * color[i] * pow(rv, 10);

    // clean up
    delete v; delete l; delete h;
}

// light accessors
float* Light::getCenter(){return center;}
float* Light::getColor(){return color;}
