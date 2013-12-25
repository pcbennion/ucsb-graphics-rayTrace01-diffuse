#ifndef RAYTRACE_H_
#define RAYTRACE_H_

#include <cstdlib>
#include <cmath>
#include <iostream>

/**
 * Class definitions for Spheres and Lights
 */

class Sphere
{
public:
    Sphere(float x, float y, float z, float radius, float R, float G, float B);
    float findIntersect(float* rayDir, float* rayOrig);
    float* getCenter();
    float* getColor();
protected:
private:
    float* center;
    float r;
    float* color;
};

class Light
{
public:
    Light(float x, float y, float z, float R, float G, float B);
    void calcLighting(float* &out, float* kd, float* eye, float* p, float* n);
    float* getCenter();
    float* getColor();
protected:
private:
    float* center;
    float* color;
};

#endif /* RAYTRACE_H_ */
