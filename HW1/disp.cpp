/*   CS580 HW   */
#include    "stdafx.h"  
#include	"Gz.h"
#include	"disp.h"


int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
/* create a framebuffer:
 -- allocate memory for framebuffer : (sizeof)GzPixel x width x height
 -- pass back pointer 
*/

	// DEBUG
	char dbstr[256];
	sprintf_s(dbstr, "GzNewFrameBuffer called\n");
	OutputDebugString(dbstr);

	// allocate
	*framebuffer = (char*) malloc(3 * width * height);
	if (framebuffer == NULL) {
		return GZ_FAILURE;
	}

	// fill with some color
	

	return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay	**display, GzDisplayClass dispClass, int xRes, int yRes)
{

/* create a display:
  -- allocate memory for indicated class and resolution
  -- pass back pointer to GzDisplay object in display
*/

	// DEBUG
	char dbstr[256];
	sprintf_s(dbstr, "GzNewDisplay called\n");
	OutputDebugString(dbstr);

	*display = (GzDisplay*) malloc(sizeof(GzDisplay*));
	if (*display == NULL) {
		return GZ_FAILURE;
	}
	(*display)->dispClass = dispClass;
	(*display)->xres = xRes;
	(*display)->yres = yRes;
	(*display)->fbuf = new GzPixel[xRes*yRes];


	if ((*display)->fbuf == NULL) {
		return GZ_FAILURE;
	}

	return GZ_SUCCESS;
}


int GzFreeDisplay(GzDisplay	*display)
{

		// DEBUG
	char dbstr[256];
	sprintf_s(dbstr, "GzFreeDisplay called\n");
	OutputDebugString(dbstr);

/* clean up, free memory */
	if (display != NULL) {
		display->xres = NULL;
		display->yres = NULL;
		display->dispClass = NULL;
		display->open = NULL;
		display->fbuf = NULL;
		free(display);
	}
	return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes, GzDisplayClass	*dispClass)
{
	// DEBUG
	char dbstr[256];
	sprintf_s(dbstr, "GzGetDisplayParams called\n");
	OutputDebugString(dbstr);

/* pass back values for an open display */
	if (display == NULL) {
		return GZ_FAILURE;
	}
	xRes = (int*) display->xres;
	yRes = (int*) display->yres;
	dispClass = (GzDisplayClass*) display->dispClass; // copy this?
	return GZ_SUCCESS;
}

// TODO
int GzInitDisplay(GzDisplay	*display)
{

	// DEBUG
	char dbstr[256];
	sprintf_s(dbstr, "GzInitDisplay called\n");
	OutputDebugString(dbstr);

	/* set everything to some default values - start a new frame */
	// TODO - DEFAULT VALS? FILL FRAME BUFFER WITH RANDOM COLOR?
	for (int i = 0; i < (display->xres * display->yres); ++i)  {
		GzPixel* newPixel = (GzPixel*) malloc(sizeof(GzPixel*));
		newPixel->red = 255;
		newPixel->green = 0;
		newPixel->blue = 0;
		newPixel->alpha = 1;
		newPixel->z = 0;
		display->fbuf[i] = *newPixel;
	}

	return GZ_SUCCESS;
}


int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
		// DEBUG
	char dbstr[256];
	

/* write pixel values into the display */
	// check bounds 
	if (ARRAY(i, j) >= (display->xres * display->yres)) {
		return GZ_FAILURE;
	}
	if (ARRAY(i, j) < 0) {
		return GZ_FAILURE;
	}

	// check values or should i clamp
	if (r < 0) {
		r = 0;
	}
	if (r > 4095) {
		r = 4095;
	}
	if (g < 0) {
		g = 0;
	}
	if (g > 4095) {
		g = 4095;
	}
	if (b < 0) {
		b = 0;
	}
	if (b > 4095) {
		b = 4095;
	}

	// 
	sprintf_s(dbstr, "put %d %d %d\n", r, g, b);
	OutputDebugString(dbstr);
	// New Pixel
	//GzPixel* newPixel = (GzPixel*) malloc(sizeof(GzPixel*)); // make new pixel
	//newPixel->red = r;
	//newPixel->blue = b;
	//newPixel->green = g;
	//newPixel->alpha = a;
	//newPixel->z = z;

	// add to frame buff
	//display->fbuf[(display->xres*j) + i] = *newPixel; // ok then....
	int ind = ARRAY(i, j);
	
	display->fbuf[ind].red = r;
	display->fbuf[ind].green = g;
	display->fbuf[ind].blue = b;
	display->fbuf[ind].alpha = a;
	display->fbuf[ind].z = z;

	return GZ_SUCCESS;
}


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{

	// DEBUG
	char dbstr[256];
	sprintf_s(dbstr, "GzGetDisplay called\n");
	OutputDebugString(dbstr);

	/* pass back pixel value in the display */ // what?
	/* check display class to see what vars are valid */
	// check bounds
	if ((i * j) >= (display->xres * display->yres)) {
		return GZ_FAILURE;
	}
	if ((i*j) <= 0) {
		return GZ_FAILURE;
	}

	GzPixel pix = display->fbuf[(display->xres*j) + i];
	r = (GzIntensity*) pix.red;
	g = (GzIntensity*) pix.green;
	b = (GzIntensity*) pix.blue;
	a = (GzIntensity*) pix.alpha;
	z = (GzDepth*) pix.z;
	return GZ_SUCCESS;
}


int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{

	// DEBUG
	char dbstr[256];
	sprintf_s(dbstr, "GzFlusDisplay2File called\n");
	OutputDebugString(dbstr);

	/* write pixels to ppm file based on display class -- "P6 %d %d 255\r" */
	//test file
	if (outfile == NULL) {
		return GZ_FAILURE;
	}

	// test display
	if (display == NULL) {
		return GZ_FAILURE;
	}

	// first line
	fprintf(outfile, "P6 %d %d 255\n", display->xres, display->yres); // this should just be a string right?
	
	// write all pixels
	for (int i = 0; i < (display->xres * display->yres); ++i) {
		GzPixel curr = display->fbuf[i];
		
		// convert for ppm
		//short short_red = curr.red >> 4; // man i hope this isn't in place
		unsigned char ppm_red = (unsigned char) curr.red >> 4;
		unsigned char ppm_green = (unsigned char) curr.green >> 4;
		unsigned char ppm_blue = (unsigned char) curr.blue >> 4;
		
		// write to file, does outfile keep track of the end or does it just go over
		// fwrite returns num of bytes written
		// TODO: CHECK THIS
		fwrite(&ppm_red, 1, 1, outfile);
		fwrite(&ppm_green, 1, 1, outfile);
		fwrite(&ppm_blue, 1, 1, outfile);
	}

	// close file
	fclose(outfile);

	return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{

	// DEBUG
	char dbstr[256];
	sprintf_s(dbstr, "GzFlushDisplay2FrameBuffer called\n");
	OutputDebugString(dbstr);

	/* write pixels to framebuffer: 
		- Put the pixels into the frame buffer
		- Caution: store the pixel to the frame buffer as the order of blue, green, and red 
		- Not red, green, and blue !!!
	*/

	// test framebuffer
	if (framebuffer == NULL) {
		return GZ_FAILURE;
	}

	// test display
	if (display == NULL) {
		return GZ_FAILURE;
	}

	char* buf = framebuffer;
	int pos = 0;

	for (int i = 0; i < (display->xres * display->yres); ++i) {
		GzPixel curr = display->fbuf[i];
		
		// convert for ppm
		//short short_red = curr.red >> 4; // man i hope this isn't in place
		unsigned char ppm_red = (unsigned char) curr.red >> 4;
		unsigned char ppm_green = (unsigned char) curr.green >> 4;
		unsigned char ppm_blue = (unsigned char) curr.blue >> 4;

		pos += sprintf(buf + pos, "%c", ppm_blue);
		pos += sprintf(buf + pos, "%c", ppm_green);
		pos += sprintf(buf + pos, "%c", ppm_red);

	}	
	

	return GZ_SUCCESS;
}