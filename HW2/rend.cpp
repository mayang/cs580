#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"


int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay *display)
{
/* 
- malloc a renderer struct
- keep closed until BeginRender inits are done
- span interpolator needs pointer to display for pixel writes
- check for legal class GZ_Z_BUFFER_RENDER
*/

	return GZ_SUCCESS;
}


int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/

	return GZ_SUCCESS;
}


int GzBeginRender(GzRender	*render)
{
/* 
- set up for start of each frame - init frame buffer
*/
	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer *valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/

	return GZ_SUCCESS;
}


int GzPutTriangle(GzRender *render, int	numParts, GzToken *nameList,
	GzPointer *valueList) 
/* numParts - how many names and values */
{
/* 
- pass in a triangle description with tokens and values corresponding to
      GZ_NULL_TOKEN:		do nothing - no values
      GZ_POSITION:		3 vert positions in model space
- Invoke the scan converter and return an error code
*/

	return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}

