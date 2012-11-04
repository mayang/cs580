/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include <math.h>
#include	"Gz.h"

GzColor	*image;
int xs, ys;
int reset = 1;

int imageIndex(int x, int y) {
	return x + (y * xs);
}

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
	  unsigned char		pixel[3];
	  unsigned char     dummy;
	  char  		foo[8];
	  int   		i, j;
	  FILE			*fd;

	  if (reset) {          /* open and load texture file */
		fd = fopen ("texture", "rb");
		if (fd == NULL) {
		  fprintf (stderr, "texture file not found\n");
		  exit(-1);
		}
		fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
		image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
		if (image == NULL) {
		  fprintf (stderr, "malloc for texture image failed\n");
		  exit(-1);
		}

		for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
		  fread(pixel, sizeof(pixel), 1, fd);
		  image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
		  image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
		  image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
		  }

		reset = 0;          /* init is done */
		fclose(fd);
	  }

	/* bounds-test u,v to make sure nothing will overflow image array bounds */
	/* determine texture cell corner values and perform bilinear interpolation */
	/* set color to interpolated GzColor value and return */
	  // bounds test and clamp?
	  if (u < 0) {
		u = 0;
	  } else if (u > 1) {
		u = 1;
	  }
	  if (v < 0) {
		v = 0;
	  } else if (v > 1) {
		v = 1;
	  }

	  // for look up and interpolating
	  float mapU = u * (xs - 1);
	  float mapV = v * (ys - 1);
	  int Ax = floor(mapU);
	  int Ay = floor(mapV);
	  int Bx = ceil(mapU);
	  int By = floor(mapV);
	  int Cx = ceil(mapU);
	  int Cy = ceil(mapV);
	  int Dx = floor(mapU);
	  int Dy = ceil(mapV);

	  int A = imageIndex(Ax, Ay);
	  int B = imageIndex(Bx, By);
	  int C = imageIndex(Cx, Cy);
	  int D = imageIndex(Dx, Dy);

	// Interpolate
	color[RED] = u*v*image[C][RED] + (1-u)*v*image[D][RED] + u*(1-v)*image[B][RED] + (1-u)*(1-v)*image[A][RED];
	color[GREEN] = u*v*image[C][GREEN] + (1-u)*v*image[D][GREEN] + u*(1-v)*image[B][GREEN] + (1-u)*(1-v)*image[A][GREEN];
	color[BLUE] = u*v*image[C][BLUE] + (1-u)*v*image[D][BLUE] + u*(1-v)*image[B][BLUE] + (1-u)*(1-v)*image[A][BLUE];

	return GZ_SUCCESS;
}


/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
	// Mandelbrot fractal - based on code in The RenderMan Shading Language Guide - Rudy Cortes & Saty Raghavachary
	// convert u,v to x,y
	float xc = -0.5;
	float yc = 0.0;
	float sz = 4.0;
	int maxiter = 20;

	float xmin = xc - 0.5*sz;
	float ymin = yc - 0.5*sz;
	float x = xmin + sz*u;
	float y = ymin + sz*v;

	float n = 0.0;
	float a = x;
	float b = y;
	float aa, bb, twoab,h;

	while (n < maxiter) {
		aa = a * a;
		bb = b * b;
		twoab = 2 * a * b;
		if ((aa + bb) > 256) break;
		n += 1;
		a = aa - bb + x;
		b = twoab + y;
	}

	//float remap = n / maxiter; // remap to [0,1]
	float remap = n - log(log(aa+bb)/2.0) / log(2.0);
	remap = 1 - remap;

	color[RED] = remap;
	color[GREEN] = remap;
	color[BLUE] = remap;


	return GZ_SUCCESS;
}

