/*  
    Copyright (C) 2001, 2002  Jesus Calvino-Fraga, jesusc@ieee.org 

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

union float_long
{
    float f;
    long l;
};

#define HALF_PI     1.5707963268
#define iPI         0.3183098862
#define EPS2 59.6046E-9

/* fabsf: Returns the absolute value of a 32-bit float. */
static float fabsf(const float x)
{
    union float_long fl;

    fl.f = x;
    fl.l &= 0x7fffffff;
    return fl.f;
}

/* 
   sincosf: Computes sin or cos of a 32-bit float as outlined in
   [1] William James Cody and W.  M.  Waite.  _Software manual for the
   elementary functions_, Englewood Cliffs, N.J.:Prentice-Hall, 1980.
*/
#define r1      (-0.1666665668E+0)
#define r2      (0.8333025139E-2)
#define r3      (-0.1980741872E-3)
#define r4       (0.2601903036E-5)

/* PI=C1+C2 */
#define C1       3.140625
#define C2       9.676535897E-4

/*A reasonable value for YMAX is the int part of PI*B**(t/2)=3.1416*2**(12)*/
#define YMAX     12867.0

static float sincosf(float x, int iscos)
{
    float y, f, r, g, XN;
    int N;
    char sign;

    if(iscos)
    {
        y=fabsf(x)+HALF_PI;
        sign=0;
    }
    else
    {
        if(x<0.0)
            { y=-x; sign=1; }
        else
            { y=x; sign=0; }
    }

    if(y>YMAX)
    {
        //errno=ERANGE;
        return 0.0;
    }

    /*Round y/PI to the nearest integer*/
    N=((y*iPI)+0.5); /*y is positive*/

    /*If N is odd change sign*/
    if(N&1) sign=!sign;

    XN=N;
    /*Cosine required? (is done here to keep accuracy)*/
    if(iscos) XN-=0.5;

    y=fabsf(x);
    r=(int)y;
    g=y-r;
    f=((r-XN*C1)+g)-XN*C2;

    g=f*f;
    if(g>EPS2) //Used to be if(fabsf(f)>EPS)
    {
        r=(((r4*g+r3)*g+r2)*g+r1)*g;
        f+=f*r;
    }
    return (sign?-f:f);
}

float sinf(float x)
{
    if (x==0.0) return 0.0;
    return sincosf(x, 0);
}

float cosf(float x)
{
    if (x==0.0) return 1.0;
    return sincosf(x, 1);
}
