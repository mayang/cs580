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
	float xmin = -0.5 - 0.5*4.0;
	float ymin = 0.0 - 0.5*4.0;
	float x = xmin + 4.0*u;
	float y = ymin + 4.0*v;

	float n = 0.0;
	float a = x;
	float b = y;
	float aa, bb, twoab,h;

	while (n < 20) {
		aa = a * a;
		bb = b * b;
		twoab = 2 * a * b;
		if ((aa + bb) > 256) break;
		n += 1;
		a = aa - bb + x;
		b = twoab + y;
	}

	float remap = n / 20;

	color[RED] = remap;
	color[GREEN] = remap;
	color[BLUE] = remap;

	//// convert n to hue (HSV)
	//float angle = (n / 20) * (3.14159 / 180.0);
	//h = 0.5 * (1.0 + sin(angle)); // s = 1; v = 1
	//// convert HSV to color RGB where each term [0,1]
	//float c = 1.0;
	//float h_prime = h / (3.14159 / 3);
	//float xc = c * (1 - abs((int)h_prime % 2 - 1.0));
	//float cr, cg, cb;
	//if (0 <= h_prime && h_prime < 1) {
	//	cr = c; cg = xc; cb = 0;
	//} else if (1 <= h_prime && h_prime < 2) {
	//	cr = xc; cg = c; cb = 0;
	//} else if (2 <= h_prime && h_prime < 3) {
	//	cr = 0; cg = c; cb = xc;
	//} else if (3 <= h_prime && h_prime < 4) {
	//	cr = 0, cg = xc; cb = c;
	//} else if (4 <= h_prime && h_prime < 5) {
	//	cr = xc; cg = 0; cb = c;
	//} else if (5 <= h_prime && h_prime < 6) {
	//	cr = c; cg = 0; cb = xc;
	//} else {
	//	cr = 0; cg = 0; cb = 0;
	//}

	//color[RED] = cr;
	//color[GREEN] = cg;
	//color[BLUE] = cb;

	return GZ_SUCCESS;
}

