#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>

#include "RayTrace.h"

using namespace std;

// viewport variables
unsigned char* pixels;
int width, height;

// debug buffers for u and v
unsigned char* xComp;
unsigned char* yComp;

// camera info
float* eye; float* focus; float* up;
float fovy;
float nearz, nearx, neary;

// scene objects
Sphere* spheres[3];
Light* lights[2];

#define PI 3.14159265

/**
 * Writes a new PPM file
 */
void writePPM(const char* filename, unsigned char* pixels, int width, int height)
{
    // create a buffer to save
    unsigned char buffer[width*height*3];

    // init buffer as upside-down so it will save properly
    int index, reverseindex;
    for(int i = 0; i<height; i++)
    {
        for(int j = 0; j<3*width; j++)
        {
              index = j + 3*width*i;
              reverseindex = j + 3*width*(height-i);
              buffer[index] = pixels[reverseindex];
        }
    }

    // create file
    FILE* file;
    file = fopen(filename, "wb");

    // write header, contents
    fprintf(file, "P6\n%d %d\n255\n", width, height);
    fwrite(buffer, 1, 3*width*height, file);

    // clean up
    fclose(file);
}

//////////////////////////////////////////////////////////////////////////////////
// Draws to the OpenGL window
//////////////////////////////////////////////////////////////////////////////////
void display()
{
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  int i, j;
  int index;
  int sphere;
  int LOS;

  float* rayDir = new float[3];
  float* color =  new float[3];
  float* p = new float[3];
  float* n = new float[3];
  float* kd = new float[3];
  float* tmp;

  float r = nearx, l = -nearx;
  float t = neary, b = -neary;
  float u, v;

  for(int x=0; x<width; x++)
  {
      for(int y=0; y<height; y++)
      {
          index = 3*width*y + 3*x;

          // calculate ray direction
          u = l + (r-l)*(x+0.5)/width;
          v = b + (t-b)*(y+0.5)/height;
          rayDir[0] = u; rayDir[1] = v; rayDir[2] = focus[2];

          // find closest sphere intersect point
          float t = -1, current;
          for(i=0; i<3; i++)
          {
              current = spheres[i]->findIntersect(rayDir, eye);
              if(t<0 || (current > 0 && current < t))
              {
                  t = current;
                  sphere = i;

                  // grab sphere color for lighting later
                  tmp = spheres[i]->getColor();
                  kd[0]=tmp[0]; kd[1]=tmp[1]; kd[2]=tmp[2];
              }
          }

          // if intersect exists, determine pixel color from lighting
          if(t>=1)
          {
              // calculate coords of collision, normal vector
              p[0] = eye[0] + t * rayDir[0];
              p[1] = eye[1] + t * rayDir[1];
              p[2] = eye[2] + t * rayDir[2];
              tmp = spheres[sphere]->getCenter();
              n[0] = p[0] - tmp[0]; n[1] = p[1] - tmp[1]; n[2] = p[2] - tmp[2];

              // for each light, do lighting computations
              color[0] = 0; color[1] = 0; color[2] = 0; // zero out pixel color
              // if there is ambient light, it goes in this line
              for(i=0; i<2; i++)
              {
                  // find vector from p to light
                  tmp = lights[i]->getCenter();
                  rayDir[0] = tmp[0]-p[0]; rayDir[1] = tmp[1]-p[1]; rayDir[2] = tmp[2]-p[2];

                  // for each scene object, check line of sight from point to light
                  LOS = 1;
                  for(j=0; j<3; j++)
                  {
                      if(j == sphere) continue; // don't check sphere p lies on
                      current = spheres[j]->findIntersect(rayDir, p);
                      // if distance < 1 vector length, there is something between p and light
                      if(current<1 && current>0) {LOS=0; break;}
                  }

                  // calculate lighting if there is line of sight
                  if(LOS>0) lights[i]->calcLighting(color, kd, eye, p, n);
              }

              // prevent color components from going out of bounds
              color[0] = (color[0]>1) ? 1 : color[0];
              color[1] = (color[1]>1) ? 1 : color[1];
              color[2] = (color[2]>1) ? 1 : color[2];

              // place pixel colors in pixel buffer
              pixels[index+0]=(int)(color[0]*255);
              pixels[index+1]=(int)(color[1]*255);
              pixels[index+2]=(int)(color[2]*255);
          } else {pixels[index+0]=0; pixels[index+1]=0; pixels[index+2]=0;} // if no intersect, color black
      }
  }

  // clean up
  delete rayDir; delete color; delete p; delete n; delete kd;

  // Render picture
  glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);


  glutSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////////////
// Handles keyboard events
//////////////////////////////////////////////////////////////////////////////////
void keyboard(unsigned char k, int x, int y)
{
  switch (k)
  {
    // the escape key and 'q' quit the program
    case 27:
    case 'q':
      exit(0);
      break;
    case 'w':
      writePPM("output.ppm", pixels, width, height);
      break;
  }
  glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////////////
// Called occasionally to see if anything's happening
//////////////////////////////////////////////////////////////////////////////////
void idle()
{
  glutPostRedisplay();
}


//////////////////////////////////////////////////////////////////////////////////
// Read in a raw PPM file of the "P6" style.
//
// Input: "filename" is the name of the file you want to read in
// Output: "pixels" will point to an array of pixel values
//         "width" will be the width of the image
//         "height" will be the height of the image
//
// The PPM file format is:
//
//   P6
//   <image width> <image height>
//   255
//   <raw, 8-bit binary stream of RGB values>
//
// Open one in a text editor to see for yourself.
//
//////////////////////////////////////////////////////////////////////////////////
void readPPM(const char* filename, unsigned char*& pixels, int& width, int& height)
{
  // try to open the file
  FILE* file;
  file = fopen(filename, "rb");
  if (file == NULL)
  {
    cout << " Couldn't open file " << filename << "! " << endl;
    exit(1);
  }

  // read in the image dimensions
  fscanf(file, "P6\n%d %d\n255\n", &width, &height);
  int totalPixels = width * height;

  // allocate three times as many pixels since there are R,G, and B channels
  pixels = new unsigned char[3 * totalPixels];
  fread(pixels, 1, 3 * totalPixels, file);
  fclose(file);

  // reverse y coord of pixels so they display right side up
  int index, reverseindex;
  unsigned char buf;
  for(int i = 0; i<height/2; i++)
  {
      for(int j = 0; j<3*width; j++)
      {
            index = j + 3*width*i;
            reverseindex = j + 3*width*(height-i);
            buf = pixels[index];
            pixels[index] = pixels[reverseindex];
            pixels[reverseindex] = buf;
      }
  }
  
  // output some success information
  cout << " Successfully read in " << filename << " with dimensions: " 
       << width << " " << height << endl;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  // init frame dimensions and buffer
  width = 800;
  height = 600;
  pixels = new unsigned char[3*width*height];

  // init camera
  eye = new float[3];
  eye[0] = 0;   eye[1] = 0;   eye[2] = 0;
  focus = new float[3];
  focus[0] = 0; focus[1] = 0; focus[2] = 1;
  up = new float[3];
  up[0] = 0;    up[1] = 1;    up[2] = 0;
  fovy = 65;
  // calculate near plane
  nearz = 1;
  neary = focus[2] * tan(fovy/2 * (PI/180));
  nearx = neary * ((float)width/(float)height);

  // init scene objects
  spheres[0] = new Sphere(0, -1000, 10, 997, 0.5, 0.5, 0.5);
  spheres[1] = new Sphere(-3.5, 0, 10, 3, 1, 0.25, 0.25);
  spheres[2] = new Sphere(3.5, 0, 10, 3, 0.25, 0.25, 1);

  lights[0] = new Light(10, 3, 5, 1, 1, 1);
  lights[1] = new Light(-10, 3, 7.5, 0.5, 0, 0);

  /* debug u and v ray components
  xComp = new unsigned char[3*width*height];
  yComp = new unsigned char[3*width*height];
  float r = nearx, l = -nearx;
  float t = neary, b = -neary;
  float u, v;
  for(int x=0; x<width; x++)
  {
      for(int y=0; y<height; y++)
      {
          int index = 3*width*y + 3*x;
          u = abs(l + (r-l)*(x+0.5)/width)  * 255;
          v = abs(b + (t-b)*(y+0.5)/height) * 255;
          xComp[index+0]=u; xComp[index+1]=0; xComp[index+2]=0;
          yComp[index+0]=0; yComp[index+1]=v; yComp[index+2]=0;
      }
  }
  writePPM("xComp.ppm", xComp, width, height);
  writePPM("yComp.ppm", yComp, width, height);
  delete xComp; delete yComp;*/

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("CMPSC 180, Homework 4");

  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(idle);

  glutMainLoop();

  return 0;
}


