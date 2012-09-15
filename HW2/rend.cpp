#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

short	ctoi(float color);


int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay *display)
{
/* 
- malloc a renderer struct
- keep closed until BeginRender inits are done
- span interpolator needs pointer to display for pixel writes // WHAT?
- check for legal class GZ_Z_BUFFER_RENDER // WHAT?
*/

	// new render
	*render = new GzRender();
	// check render
	if (*render == NULL) {
		return GZ_FAILURE;
	}

	// check class
	if (renderClass != GZ_Z_BUFFER_RENDER) {
		return GZ_FAILURE;
	}

	// check display
	if (display == NULL) {
		return GZ_FAILURE;
	}

	(*render)->renderClass = renderClass;
	(*render)->display = display;
	(*render)->open = 0; //?????

	return GZ_SUCCESS;
}


int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/
	if (render == NULL) {
		return GZ_FAILURE;
	}

	render->display = NULL;
	delete render;

	return GZ_SUCCESS;
}


int GzBeginRender(GzRender	*render)
{
/* 
- set up for start of each frame - init frame buffer

*/
	if (render == NULL) {
		return GZ_FAILURE;
	}

	GzInitDisplay(render->display);
	if (render->display == NULL) {
		return GZ_FAILURE;
	}
	render->open = 1;

	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer *valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/

	if (render == NULL)  {
		return GZ_FAILURE;
	}
	if (nameList == NULL) {
		return GZ_FAILURE;
	}
	if (valueList == NULL) {
		return GZ_FAILURE;
	}

	for (int i = 0; i < numAttributes; ++i) {
		if (nameList[i] = GZ_RGB_COLOR) {
			//*render->flatcolor = (GzColor*) valueList[i];
			//*render->flatcolor = (float)(valueList[i]);
			//GzColor* c = static_cast<GzColor*>  (valueList[i]);
			//*render->flatcolor = **c; // does this even do what i want it to do?
			//*render->flatcolor = **c; // is this even the right setting?
			// THIS ONLY GETS THE FIRST VALUE WHY
			GzColor* c = (GzColor*) valueList[i];
			*render->flatcolor = **c;
		}
	}


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
	// error checking
	if (render == NULL) {
		return GZ_FAILURE;
	}
	if (nameList == NULL) {
		return GZ_FAILURE;
	}
	if (valueList == NULL) {
		return GZ_FAILURE;
	}

	for (int i = 0; i < numParts; ++i) {
		if (nameList[i] == GZ_NULL_TOKEN) {
			continue;
		}
		if (nameList[i] == GZ_POSITION) {
			// Stuff

			// get points
			GzCoord* tri = (GzCoord*) valueList[i];

			// sort verts by Y
			float y0 = tri[0][Y];
			float y1 = tri[1][Y];
			float y2 = tri[2][Y];
			int minY = 0;
			for (int i = 0; i < 2; ++i) {
				minY = i;
				for (int j = i + 1; j < 3; ++j) {
					float y_0 = tri[i][Y];
					float y_1 = tri[j][Y];
					if (tri[minY][Y] > tri[j][Y]) {
						//if (tri[i][Y] == tri[j][Y]) {
						//	float x0 = tri[i][X];
						//	float x1 = tri[j][X];
						//	if (tri[i][X] < tri[j][X]) {
						//		continue;
						//	}
						//}
						minY = j;
					}
				}
				// swapping
				if (minY != i) {
					float tempX = tri[i][X];
					float tempY = tri[i][Y];
					float tempZ = tri[i][Z];
					tri[i][X] = tri[minY][X];
					tri[i][Y] = tri[minY][Y];
					tri[i][Z] = tri[minY][Z];
					tri[minY][X] = tempX;
					tri[minY][Y] = tempY;
					tri[minY][Z] = tempZ;
				}
			}
			y0 = tri[0][Y];
			y1 = tri[1][Y];
			y2 = tri[2][Y];



			// FIND L/R relationship
			// if top 2 are a horizontoal line
			if (tri[0][Y] == tri[1][Y]) {
				// go clockwise, swap if needed
				if (tri[0][X] > tri[1][X]) {
					float tempX = tri[0][X];
					float tempY = tri[0][Y];
					float tempZ = tri[0][Z];
					tri[0][X] = tri[1][X];
					tri[0][Y] = tri[1][Y];
					tri[0][Z] = tri[1][Z];
					tri[1][X] = tempX;
					tri[1][Y] = tempY;
					tri[1][Z] = tempZ;
				}
			}
			// if bottom 2 are a horizontal line
			else if (tri[1][Y] == tri[2][Y]) {
				// go clockwise so opposite of previous case
				if (tri[1][X] < tri[2][X]) {
					float tempX = tri[1][X];
					float tempY = tri[1][Y];
					float tempZ = tri[1][Z];
					tri[1][X] = tri[2][X];
					tri[1][Y] = tri[2][Y];
					tri[1][Z] = tri[2][Z];
					tri[2][X] = tempX;
					tri[2][Y] = tempY;
					tri[2][Z] = tempZ;
				}
			}
			// find L/R relationship
			else {
				////float A = tri[2][Y] - tri[0][Y];
				//float A = tri[0][Y] - tri[2][Y];
				////float B = tri[0][X] - tri[2][X];
				//float B = tri[2][X] - tri[0][X];
				////float C = tri[2][X]*tri[0][Y] - tri[0][X]*tri[2][Y];//??
				////float C = (tri[2][X] - tri[0][X])*tri[0][Y] - (tri[2][Y] - tri[0][Y])*tri[0][X];
				//float C = (tri[0][X] * tri[2][Y]) - (tri[0][Y] * tri[2][X]);
				//float x_intercept = (-C - (B*tri[1][Y])) / A;
				//float x1 = tri[1][X];
				//// if x intercpet is greater than the x of v1 than 
				//// v1 should be on L
				//if ( x_intercept > tri[1][X]) { // <?
				//	float tempX = tri[1][X];
				//	float tempY = tri[1][Y];
				//	float tempZ = tri[1][Z];
				//	tri[1][X] = tri[2][X];
				//	tri[1][Y] = tri[2][Y];
				//	tri[1][Z] = tri[2][Z];
				//	tri[2][X] = tempX;
				//	tri[2][Y] = tempY;
				//	tri[2][Z] = tempZ;
				//}
				//if ( tri[2][X] > tri[1][X]) { // <?
				//	float tempX = tri[1][X];
				//	float tempY = tri[1][Y];
				//	float tempZ = tri[1][Z];
				//	tri[1][X] = tri[2][X];
				//	tri[1][Y] = tri[2][Y];
				//	tri[1][Z] = tri[2][Z];
				//	tri[2][X] = tempX;
				//	tri[2][Y] = tempY;
				//	tri[2][Z] = tempZ;
				//}
			}

			// get bounding box
			int topY = floor(tri[0][Y]);
			int bottomY = ceil(tri[2][Y]);
			int leftX, rightX;
			// 0 comes before 1
			if (tri[0][X] < tri[1][X]) {
				// 201
				if (tri[2][X] < tri[0][X]) {
					leftX = floor(tri[2][X]);
					rightX = ceil(tri[1][X]);
				} else { //0
					leftX = floor(tri[0][X]);
					// 021
					if (tri[2][X] < tri[1][X]) {
						rightX = ceil(tri[1][X]);
					} else { // 012
						rightX = ceil(tri[2][X]);
					}
				}
			} else { // 1 comes before 0
				//210
				if (tri[2][X] < tri[1][X]) {
					leftX = floor(tri[2][X]);
					rightX = ceil(tri[0][X]);
				} else { //1
					leftX = floor(tri[1][X]);
					// 120
					if (tri[2][X] < tri[0][X]) {
						rightX = ceil(tri[0][X]);
					} else { // 102
						rightX = ceil(tri[2][X]);
					}
				}
			}

			float x0 = tri[0][X];
			y0 = tri[0][Y];
			float x1 = tri[1][X];
			y1 = tri[1][Y];
			float x2 = tri[2][X];
			y2 = tri[2][Y];

			// For interpolating Z
			// Ax + By + Cz + D = 0;
			// tri[0] x tri[1] = (A, B, C)
			GzCoord edge01;
			edge01[X] = tri[1][X]- tri[0][X];
			edge01[Y] = tri[1][Y] - tri[0][Y];
			edge01[Z] = tri[1][Z] - tri[0][Z];
			GzCoord edge12;
			edge12[X] = tri[2][X] - tri[1][X];
			edge12[Y] = tri[2][Y] - tri[1][Y];
			edge12[Z] = tri[2][Z] - tri[1][Z];

			float A = edge01[Y]*edge12[Z] - edge01[Z]*edge12[Y];
			float B = edge01[Z]*edge12[X] - edge01[X]*edge12[Z];
			float C = edge01[X]*edge12[Y] - edge01[Y]*edge12[X];
			// get D
			float D = -(A*tri[0][X]) - (B*tri[0][Y]) - (C*tri[0][Z]);
			// A, B, C
			//float A01 = A, B01 = B, C01 = C, D01 = D;

			//float A = tri[0][Y]*tri[1][Z] - tri[0][Z]*tri[1][Y];
			//float B = tri[0][Z]*tri[1][X] - tri[0][X]*tri[1][Z];
			//float C = tri[0][X]*tri[1][Y] - tri[0][Y]*tri[1][X];
			//// get D
			//float D = -(A*tri[0][X]) - (B*tri[0][Y]) - (C*tri[0][Z]);

			// for pixels in this bounding box
			float interpZ;
			for (int i = leftX; i < rightX; ++i) {
				// bounds check
				if (i < 0 || i > render->display->xres) {
					continue;
				}
				for (int j = topY; j < bottomY; ++j) {
					// bounds check
					if (j < 0 || j > render->display->yres) {
						continue;
					}

					// Compute LEES
					// E(x, y) = dY(x-X) - dX(y-Y)
					// EDGE 0-1
					float e01 = (tri[1][Y] - tri[0][Y])*((float)i - tri[0][X]) 
								- (tri[1][X] - tri[0][X])*((float)j - tri[0][Y]);
					// EDGE 1-2
					float e12 = (tri[2][Y] - tri[1][Y])*((float)i - tri[1][X]) 
								- (tri[2][X] - tri[1][X])*((float)j - tri[1][Y]);
					// EDGE 2-0
					float e20 = (tri[0][Y] - tri[2][Y])*((float)i - tri[2][X]) 
								- (tri[0][X] - tri[2][X])*((float)j - tri[2][Y]);

					// if all have same sign then this pixel should be drawn
					if (e01 == 0) {
						interpZ = (-(A*i) - (B*j) - D) / C;

						// get current z at this pixel
						GzIntensity r, g, b, a;
						GzDepth z = 0;
						GzGetDisplay(render->display, i, j, &r, &g, &b, &a, &z);
						// compare, if interpZ less than draw over
						if (interpZ < z) {
							r = (GzIntensity ) ctoi((float) render->flatcolor[0]);
							g = (GzIntensity ) ctoi((float)render->flatcolor[1]);
							b = (GzIntensity ) ctoi((float)render->flatcolor[2]);
							z = interpZ;
							GzPutDisplay(render->display, i, j, r, g, b, a, z);
						}
					} else if (e12 == 0) {
						interpZ = (-(A*i) - (B*j) - D) / C;

						// get current z at this pixel
						GzIntensity r, g, b, a;
						GzDepth z = 0;
						GzGetDisplay(render->display, i, j, &r, &g, &b, &a, &z);
						// compare, if interpZ less than draw over
						if (interpZ < z) {
							r = (GzIntensity ) ctoi((float) render->flatcolor[0]);
							g = (GzIntensity ) ctoi((float)render->flatcolor[1]);
							b = (GzIntensity ) ctoi((float)render->flatcolor[2]);
							z = interpZ;
							GzPutDisplay(render->display, i, j, r, g, b, a, z);
						}
					} else if (e20 == 0) {
						interpZ = (-(A*i) - (B*j) - D) / C;

						// get current z at this pixel
						GzIntensity r, g, b, a;
						GzDepth z = 0;
						GzGetDisplay(render->display, i, j, &r, &g, &b, &a, &z);
						// compare, if interpZ less than draw over
						if (interpZ < z) {
							r = (GzIntensity ) ctoi((float) render->flatcolor[0]);
							g = (GzIntensity ) ctoi((float)render->flatcolor[1]);
							b = (GzIntensity ) ctoi((float)render->flatcolor[2]);
							z = interpZ;
							GzPutDisplay(render->display, i, j, r, g, b, a, z);
						}
					}
					else if ( ((e01 > 0) && (e12 > 0) && (e20 > 0)) || ((e01 < 0) && (e12 < 0) && (e20 < 0))) {
						//(e01 == 0 && e12 == 0 && e20 == 0) 
						// Interpolate Z value
						interpZ = (-(A*i) - (B*j) - D) / C;

						// get current z at this pixel
						GzIntensity r, g, b, a;
						GzDepth z = 0;
						GzGetDisplay(render->display, i, j, &r, &g, &b, &a, &z);
						// compare, if interpZ less than draw over
						if (interpZ < z) {
							r = (GzIntensity ) ctoi((float) render->flatcolor[0]);
							g = (GzIntensity ) ctoi((float)render->flatcolor[1]);
							b = (GzIntensity ) ctoi((float)render->flatcolor[2]);
							z = interpZ;
							GzPutDisplay(render->display, i, j, r, g, b, a, z);
						} 

					}

				}
			} 

			//GzDisplay* disp = render->display;
			


		}
	}


	return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */

//void setUpDDA(DDA* dda, GzCoord start, GzCoord end) {
//	dda->start[X] = start[X];
//	dda->start[Y] = start[Y];
//	dda->start[Z] = start[Z];
//	dda->end[X] = end[X];
//	dda->end[Y] = end[Y];
//	dda->end[Z] = end[Z];
//	dda->current[X] = start[X];
//	dda->current[Y] = start[Y];
//	dda->current[Z] = start[Z];
//	dda->slopeX = (end[X] - start[X]) / (end[Y] - start[Y]);
//	dda->slopeZ = (end[Z] - start[Z]) / (end[Z] - start[Z]); 
//}

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}

