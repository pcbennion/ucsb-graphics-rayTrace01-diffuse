#ifndef VECTOR3_H
#define VECTOR3_H

/**
 *  Utility functions for 3D vectors
 */

// Assigns a 3x1 vector pointing at the origin
void getZeroVector(float* &v)
{ for(int i=0; i<3; i++) v[i] = 0; }

// Prints vector for testing accuracy
void vectorPrint(float* v, std::string name)
{
    std::cout<<"Vector "<<name<<":\n";
    int i;
    for(i=0; i<3; i++) {
        std::cout<<v[i];
        if(i<2) std::cout<<", ";
        std::cout<<std::endl;
    }
    std::cout<<std::endl;
}

// Computes cross product of two vectors. Answer stored to first param.
void vectorCrossProduct(float* &v1, float* v2)
{
    float tmp[] = {	v1[1]*v2[2] - v1[2]*v2[1],
                    v1[0]*v2[2] - v1[2]*v2[0],
                    v1[0]*v2[1] - v1[1]*v2[0]};
    for(int i=0; i<4; i++) v1[i] = tmp[i];
}

// Computes dot product of two vectors.
float vectorDotProduct(float* v1, float* v2)
{
    return (v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]);
}

// Scales the vector into a unit vector
void vectorNormalize(float* &v)
{
    // length of v
    float length = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    v[0] = v[0]/length;
    v[1] = v[1]/length;
    v[2] = v[2]/length;
}

#endif // VECTOR3_H
