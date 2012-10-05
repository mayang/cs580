#include    "stdafx.h"  
#include	"Gz.h"
#include	"disp.h"

/*
 * Allocate a new frame buffer.
 *
 * framebuffer	Pointer to a framebuffer pointer. The reference to the
 *		allocated framebuffer is returned via this pointer.
 * width		The x resolution of the framebuffer
 * height		The y resolution of the framebuffer
 *
 * Return		Integer representing operation status. Check hearders for
 *		enumeration of the various status values.
 */
int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
	// Currently, the framebuffer only needs to store RGB values for each
	// pixel. More will be added later, but for now 3 is hardcoded.
	// Note, sizeof GzPixel should be used later.
	if (width <= 0 || height <= 0 || width > MAXXRES || height > MAXYRES) {
		return GZ_FAILURE;
	}

	// allocate
	//*framebuffer = (char*) malloc(3 * width * height);
	*framebuffer = new char[3 * width * height];
	if (framebuffer == NULL) {
		return GZ_FAILURE;
	}
	
	return GZ_SUCCESS;
}

/*
 * Allocate a new display buffer.
 *
 * display		Pointer to a GzDisplay pointer. The reference to the 
 *		allocated display is returned via this pointer.
 * dispClass	The class value for this display. Check headers for an
 *		enumeration of the various display classes.
 * xRes			The x resolution of the display
 * yRes			The y resolution of the display
 *
 * Return		Status flag
 */
int GzNewDisplay(GzDisplay	**display, GzDisplayClass dispClass, int xRes, int yRes)
{

		// bounds check
	if (xRes <= 0 || yRes <= 0 || xRes > MAXXRES || yRes > MAXYRES) {
		return GZ_FAILURE;
	}


	//*display = (GzDisplay*) malloc(sizeof(GzDisplay));
	*display = new GzDisplay();
	if (*display == NULL) {
		return GZ_FAILURE;
	}
	(*display)->dispClass = dispClass;
	(*display)->xres = xRes;
	(*display)->yres = yRes;
	(*display)->fbuf = new GzPixel[xRes*yRes];
	//(*display)->fbuf = (GzPixel*) malloc(sizeof(GzPixel*) * xRes * yRes);


	if ((*display)->fbuf == NULL) {
		return GZ_FAILURE;
	}


	return GZ_SUCCESS;
}

/*
 * Free up display memory.
 *
 * display      The GzDisplay pointer for which to free memory
 *
 * Return	    Status flag
 */
int GzFreeDisplay(GzDisplay	*display)
{
	if (display != NULL) {
		// free buffer????
		//for (int i = 0; i < (display->xres * display->yres); ++i) {
		//	free(&display->fbuf[i]);
		//	//*display->fbuf[i] = NULL;
		//}
		delete display->fbuf;
		//display->fbuf = NULL;
		//free(display);
		delete display;
	}
	return GZ_SUCCESS;
}

/*
 * Get a GzDisplay's parameters
 *
 * display		The GzDisplay from which values should be pulled.
 * xRes			The returned x resolution of the display
 * yRes			The returned y resolution of the display
 * dispClass	The returned GzDisplayClass of the display
 *
 * Return		Status flag
 */
int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes, GzDisplayClass	*dispClass)
{
	if (display == NULL) {
		return GZ_FAILURE;
	}
	*xRes = display->xres;
	*yRes = display->yres;
	*dispClass = display->dispClass; // copy this?
	return GZ_SUCCESS;
}

/*
 * Initialize a display to some default state
 *
 * display		The GzDisplay to initalize
 *
 * Return		Status flag
 */
int GzInitDisplay(GzDisplay	*display)
{	
	if (display == NULL) {
		return GZ_FAILURE;
	}

	/* set everything to some default values - start a new frame */
	// TODO - DEFAULT VALS? FILL FRAME BUFFER WITH RANDOM COLOR?
	for (int i = 0; i < (display->xres * display->yres); ++i)  {
		//GzPixel* newPixel = (GzPixel*) malloc(sizeof(GzPixel));
		//if (newPixel == NULL) {
		//	return GZ_FAILURE;
		//}
		//newPixel->red = 4095;
		//newPixel->green = 0;
		//newPixel->blue = 0;
		//newPixel->alpha = 1;
		//newPixel->z = 0;
		//display->fbuf[i] = *newPixel;
		display->fbuf[i].red = 2048;
		display->fbuf[i].green = 2048;
		display->fbuf[i].blue = 2048;
		display->fbuf[i].alpha = 1;
		display->fbuf[i].z = INT_MAX;
	}
	display->open = 1;
	return GZ_SUCCESS;
}

/*
 * Set the value of a pixel in a GzDisplay
 *
 * display		The display in which the pixel value should be set
 * i			The x display coordinate to set
 * j			The y display coordinate to set
 * r			The red component to set 
 * g			The green component to set
 * b			The blue component to set
 * a			The alpha component to set
 * z			The depth buffer value to set
 *
 * Return		Status flag
 */
int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
	if (display == NULL) {
		return GZ_FAILURE;
	}

/* write pixel values into the display */
	// check bounds 
	if (ARRAY(i, j) >= (display->xres * display->yres)) {
		return GZ_FAILURE;
	}
	if (ARRAY(i, j) < 0) {
		return GZ_FAILURE;
	}

	// check values and CLAMP
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
	//sprintf_s(dbstr, "put %d %d %d\n", r, g, b);
	//OutputDebugString(dbstr);
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

/*
 * Get the component values of a pixel coordinate from a GzDisplay
 *
 * display		The display to pull values from
 * i			The x display coordinate to get pixel values from
 * j			The y display coordinate to get pixel values from
 * r			The pointer to pass the red compenent back through
 * g			The pointer to pass the green component back through
 * b			The pointer to pass the blue component back through
 * a			The pointer to pass the alpha component back through
 * z			The pointer to pass the z buffer value back through
 *
 * Return		Status flag
 */
int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
	if (display == NULL) {
		return GZ_FAILURE;
	}

	/* pass back pixel value in the display */ // what?
	/* check display class to see what vars are valid */
	// check bounds
	if (ARRAY(i, j) >= (display->xres * display->yres)) {
		return GZ_FAILURE;
	}
	if (ARRAY(i, j) <= 0) {
		return GZ_FAILURE;
	}

	GzPixel pix = display->fbuf[ARRAY(i, j)];
	*r = pix.red;
	*g = pix.green;
	*b =  pix.blue;
	*a =  pix.alpha;
	*z =  pix.z;
	return GZ_SUCCESS;
}

/*
 * Flush the display contents to a ppm file.
 *
 * outfile		The output file handle where data should be written
 * display		The display to flush
 *
 * Return		Status flag
 */
int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{
	if (outfile == NULL) {
		return GZ_FAILURE;
	}

	// test display
	if (display == NULL) {
		return GZ_FAILURE;
	}

	FILE* fptr = outfile;

	// first line
	fprintf(fptr, "P6 %d %d 255\n", display->xres, display->yres); // this should just be a string right?
	
	// write all pixels
	for (int i = 0; i < (display->xres * display->yres); ++i) {
		GzPixel curr = display->fbuf[i];
		
		// convert for ppm
		//short short_red = curr.red >> 4; // man i hope this isn't in place
		unsigned char ppm_red = curr.red >> 4;
		unsigned char ppm_green = curr.green >> 4;
		unsigned char ppm_blue = curr.blue >> 4;
		
		// write to file, does outfile keep track of the end or does it just go over
		// fwrite returns num of bytes written
		// TODO: CHECK THIS
		fwrite(&ppm_red, 1, 1, fptr);
		fwrite(&ppm_green, 1, 1, fptr);
		fwrite(&ppm_blue, 1, 1, fptr);
	}

	return GZ_SUCCESS;
}

/*
 * Flush a display to a frame buffer.
 *
 * framebuffer		The framebuffer to flush the display to
 * display			The display to flush
 *
 * Return			Status flag
 */
int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{

	// test framebuffer
	if (framebuffer == NULL) {
		return GZ_FAILURE;
	}

	// test display
	if (display == NULL) {
		return GZ_FAILURE;
	}

	GzPixel* curr; 
  
	for (int i = 0; i < (display->xres * display->yres); ++i) { 
            // Note, the framebuffer contains slots for each r, g, and b triple. 
            // This won't be hard coded later. 
            int offset = 3 * i; 
            GzPixel* curr = &(display->fbuf[i]); 

			unsigned char ppm_red = curr->red >> 4;
			unsigned char ppm_green = curr->green >> 4;
			unsigned char ppm_blue = curr->blue >> 4;
			 
            // Don't forget, the frame buffer expects colors in bgr order 
            framebuffer[offset] = ppm_blue; 
            framebuffer[offset + 1] = ppm_green ; 
            framebuffer[offset + 2] = ppm_red; 
	} 
    


	return GZ_SUCCESS;
}
