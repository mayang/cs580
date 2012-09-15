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
	//char dbstr[256];
	//sprintf_s(dbstr, "GzNewFrameBuffer called\n");
	//OutputDebugString(dbstr);

	// bounds check
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

int GzNewDisplay(GzDisplay	**display, GzDisplayClass dispClass, int xRes, int yRes)
{

/* create a display:
  -- allocate memory for indicated class and resolution
  -- pass back pointer to GzDisplay object in display
*/

	// DEBUG
	//char dbstr[256];
	//sprintf_s(dbstr, "GzNewDisplay called\n");
	//OutputDebugString(dbstr);

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


int GzFreeDisplay(GzDisplay	*display)
{

		// DEBUG
	//char dbstr[256];
	//sprintf_s(dbstr, "GzFreeDisplay called\n");
	//OutputDebugString(dbstr);

/* clean up, free memory */
	if (display != NULL) {
		// free buffer????
		//for (int i = 0; i < (display->xres * display->yres); ++i) {
		//	free(&display->fbuf[i]);
		//	//*display->fbuf[i] = NULL;
		//}
		delete display->fbuf;
		display->fbuf = NULL;
		//free(display);
		delete display;
	}
	return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes, GzDisplayClass	*dispClass)
{
	// DEBUG
	//char dbstr[256];
	//sprintf_s(dbstr, "GzGetDisplayParams called\n");
	//OutputDebugString(dbstr);

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
	//char dbstr[256];
	//sprintf_s(dbstr, "GzInitDisplay called\n");
	//OutputDebugString(dbstr);

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
		display->fbuf[i].z = 2147483647;
	}

	return GZ_SUCCESS;
}


int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
		// DEBUG
	//char dbstr[256];
	//sprintf_s(dbstr, "GzPutDisplay called\n");
	//OutputDebugString(dbstr);

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


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{

	//// DEBUG
	//char dbstr[256];
	//sprintf_s(dbstr, "GzGetDisplay called\n");
	//OutputDebugString(dbstr);

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


int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{

	// DEBUG
	//char dbstr[256];
	//sprintf_s(dbstr, "GzFlusDisplay2File called\n");
	//OutputDebugString(dbstr);

	/* write pixels to ppm file based on display class -- "P6 %d %d 255\r" */
	//test file
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

	// close file
	//int check = fclose(outfile);
	//if (check == 0) {
	//	sprintf_s(dbstr, "file close successful\n");
	//} else {
	//	sprintf_s(dbstr, "file close fail\n");
	//}
	//OutputDebugString(dbstr);
	return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{

	// DEBUG
	/*char dbstr[256];
	sprintf_s(dbstr, "GzFlushDisplay2FrameBuffer called\n");
	OutputDebugString(dbstr);*/

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

	//char* buf = framebuffer;
	int pos = 0;

	for (int i = 0; i < (display->xres * display->yres); ++i) {
		GzPixel curr = display->fbuf[i];
		
		// convert for ppm
		//short short_red = curr.red >> 4; // man i hope this isn't in place
		unsigned char ppm_red = curr.red >> 4;
		unsigned char ppm_green = curr.green >> 4;
		unsigned char ppm_blue = curr.blue >> 4;

		// write to buffer
		pos += sprintf(framebuffer + pos, "%c", ppm_blue);
		pos += sprintf(framebuffer + pos, "%c", ppm_green);
		pos += sprintf(framebuffer + pos, "%c", ppm_red);

	}	
	

	return GZ_SUCCESS;
}