//
//  General_functions.cpp
//  Cell-Durotaxis
//
//  Created by Ali Farnudi on 27/08/2017.
//  Copyright © 2017 Ali Farnudi. All rights reserved.
//

#include "General_functions.hpp"
#include <math.h>


void crossvector( double c[3], double a[3],double b[3] ) // cross porduct
{
    c[0]=a[1]*b[2]-a[2]*b[1];    // normal vector to plane (not unitary length)
    c[1]=a[2]*b[0]-a[0]*b[2];
    c[2]=a[0]*b[1]-a[1]*b[0];
}

double innerproduct(double n1[3],double n2[3])
{
    return n1[0]*n2[0]+n1[1]*n2[1]+n1[2]*n2[2];
}

double vector_length(double v[3]) // calculate length of vector
{
    return  sqrt( v[0]*v[0]+v[1]*v[1]+v[2]*v[2] );
}

double vector_length_squared(double v[3]) // calculate length of vector
{
    return  v[0]*v[0]+v[1]*v[1]+v[2]*v[2];
}

double sign_function(double x)
{
    if(x<0){
        return -1.0;
    }
    
    return 1.0;
}

//double periodiccondition(double dx )
//{
//    if( dx>Lbox/2.0  )
//    {
//        return dx-(Lbox+1.0); /// so the new dx is <0
//    }
//    else if(dx<-Lbox/2.0  )
//    {
//        return (Lbox+1.0)+dx;  /// so the new dx is >0
//    }
//    else
//    {
//        return dx;
//    }
//}

void Vector_transformation (double MV[3],double  M[3][3] ,double V[3])
{
    MV[0]= M[0][0] * V [0] + M[0][1] * V [1]  +M[0][2] * V [2] ;
    MV[1]= M[1][0] * V [0] + M[1][1] * V [1]  +M[1][2] * V [2] ;
    MV[2]= M[2][0] * V [0] + M[2][1] * V [1]  +M[2][2] * V [2] ;
}


void matrix_inverse (double mat[3][3]){
    int i, j;
    float determinant = 0;
    
    //finding determinant
    for(i = 0; i < 3; i++)
        determinant += (mat[0][i] * (mat[1][(i+1)%3] * mat[2][(i+2)%3] - mat[1][(i+2)%3] * mat[2][(i+1)%3]));
    
    double mat_temp[3][3]={0};
    for(i = 0; i < 3; i++){
        for(j = 0; j < 3; j++)
            mat_temp[i][j]=((mat[(j+1)%3][(i+1)%3] * mat[(j+2)%3][(i+2)%3]) - (mat[(j+1)%3][(i+2)%3] * mat[(j+2)%3][(i+1)%3]))/ determinant;
        
    }
    for(i = 0; i < 3; i++){
        for(j = 0; j < 3; j++)
            mat[i][j]=mat_temp[i][j];
        
    }
}
