/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

#define PI 3.14159265

short	ctoi(float color);

int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
	if (mat == NULL)
		return GZ_FAILURE;

	float rads = degree * (PI / 180.0);

	mat[0][0] = 1;
	mat[1][1] = cos(rads);
	mat[1][2] = -sin(rads);
	mat[2][1] = sin(rads);
	mat[2][2] = cos(rads);

	return GZ_SUCCESS;
}


int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
	if (mat == NULL)
		return GZ_FAILURE;

	float rads = degree * (PI / 180.0);

	mat[0][0] = cos(rads);
	mat[0][2] = sin(rads);
	mat[1][1] = 1;
	mat[2][0] = -sin(rads);
	mat[2][2] = cos(rads);

	return GZ_SUCCESS;
}


int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
	
	if (mat == NULL)
		return GZ_FAILURE;
	
	float rads = degree * (PI / 180.0);

	mat[0][0] = cos(rads);
	mat[0][1] = -sin(rads);
	mat[1][0] = sin(rads);
	mat[1][1] = cos(rads);
	mat[2][2] = 1;

	return GZ_SUCCESS;
}


int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value
	if (mat == NULL)
		return GZ_FAILURE;

	mat[0][3] = translate[X];
	mat[1][3] = translate[Y];
	mat[2][3] = translate[Z];

	return GZ_SUCCESS;
}


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
// Create scaling matrix
// Pass back the matrix using mat value
	if (mat == NULL)
		return GZ_FAILURE;

	mat[0][0] = scale[X];
	mat[1][1] = scale[Y];
	mat[2][2] = scale[Z];

	return GZ_SUCCESS;
}


int normalizeVector(GzCoord vec) {
	if (vec == NULL) {
		return GZ_FAILURE;
	}

	float norm = sqrt((vec[X]*vec[X]) + (vec[Y]*vec[Y]) + (vec[Z]*vec[Z]));

	vec[X] = vec[X] / norm;
	vec[Y] = vec[Y] / norm;
	vec[Z] = vec[Z] / norm;

	return GZ_SUCCESS;
}


//----------------------------------------------------------
// Begin main functions

int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay	*display)
{
/*  
- malloc a renderer struct 
- keep closed until all inits are done 
- setup Xsp and anything only done once - the hell this means?
- span interpolator needs pointer to display 
- check for legal class GZ_Z_BUFFER_RENDER 
- init default camera 
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
	(*render)->matlevel = -1; // stack empty

	// set up Xsp ???? default
	(*render)->Xsp[0][0] = (*render)->Xsp[0][3] = (*render)->display->xres / 2;
	(*render)->Xsp[0][1] = (*render)->Xsp[0][2] = 0.0;
	(*render)->Xsp[1][0] = 0.0;
	(*render)->Xsp[1][1] = -((*render)->display->yres / 2);
	(*render)->Xsp[1][2] = 0.0;
	(*render)->Xsp[1][3] = (*render)->display->yres / 2;
	(*render)->Xsp[2][0] = (*render)->Xsp[2][1] = 0.0;
//	(*render)->Xsp[2][2] = 677099008.0; //NO IDEA HOW I GET THIS ONE
	(*render)->Xsp[2][3] = 0.0;
	(*render)->Xsp[3][0] = (*render)->Xsp[3][1] = (*render)->Xsp[3][2] = 0.0;
	(*render)->Xsp[3][3] = 1.0;

	// Initialize camera to defaults
	(*render)->camera.position[X] = DEFAULT_IM_X;      
	(*render)->camera.position[Y] = DEFAULT_IM_Y;
  	(*render)->camera.position[Z] = DEFAULT_IM_Z;

  	(*render)->camera.lookat[X] = 0.0;
  	(*render)->camera.lookat[Y] = 0.0;
  	(*render)->camera.lookat[Z] = 0.0;

  	(*render)->camera.worldup[X] = 0.0;
  	(*render)->camera.worldup[Y] = 1.0;
  	(*render)->camera.worldup[Z] = 0.0;
	// normalize up
	//normalizeVector((*render)->camera.worldup);

	(*render)->camera.FOV = DEFAULT_FOV;  

	(*render)->Xsp[2][2] = 2147483647 * tan(((*render)->camera.FOV / 2.0) * (PI / 180.0));

	(*render)->numlights = 0;
	(*render)->open = 0;

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
	//render->display == NULL;
	GzFreeDisplay(render->display);
	delete render->Ximage;
	//////render->Ximage = NULL;
	delete render->Xsp;
	//////render->Xsp = NULL
	delete render->camera.Xiw;
	delete render->camera.Xpi;
	delete render->camera.lookat;
	delete render->camera.position;
	delete render->camera.worldup;
	delete render;

	return GZ_SUCCESS;
}


int GzBeginRender(GzRender *render)
{
/*  
- set up for start of each frame - clear frame buffer 
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms if it want to. 
*/ 
	if (render == NULL) {
		return GZ_FAILURE;
	}

	// clear frame buffer
	GzInitDisplay(render->display);
	if (render->display == NULL) {
		return GZ_FAILURE;
	}

	// computer Xiw
	// camera z axis
	GzCoord cl, camZ;
	cl[X] = render->camera.lookat[X] - render->camera.position[X];
	cl[Y] = render->camera.lookat[Y] - render->camera.position[Y];
	cl[Z] = render->camera.lookat[Z] - render->camera.position[Z];
	normalizeVector(cl);
	camZ[X] = cl[X];
	camZ[Y] = cl[Y];
	camZ[Z] = cl[Z];
	normalizeVector(camZ);
	
	// camera y axis
	GzCoord camUp, camY;
	float upDotZ = render->camera.worldup[X]*camZ[X] + render->camera.worldup[Y]*camZ[Y] +
		render->camera.worldup[Z]*camZ[Z];
	camUp[X] = render->camera.worldup[X] - upDotZ*camZ[X];
	camUp[Y] = render->camera.worldup[Y] - upDotZ*camZ[Y];
	camUp[Z] = render->camera.worldup[Z] - upDotZ*camZ[Z];
	normalizeVector(camUp);
	camY[X] = camUp[X];
	camY[Y] = camUp[Y];
	camY[Z] = camUp[Z];
	normalizeVector(camY);

	// camera x axis
	GzCoord camX;
	camX[X] = camY[Y]*camZ[Z] - camY[Z]*camZ[Y];
	camX[Y] = camY[Z]*camZ[X] - camY[X]*camZ[Z];
	camX[Z] = camY[X]*camZ[Y] - camY[Y]*camZ[X];
	normalizeVector(camX);

	// Build Xiw
	render->camera.Xiw[0][0] = camX[X];
	render->camera.Xiw[0][1] = camX[Y];
	render->camera.Xiw[0][2] = camX[Z];
	render->camera.Xiw[0][3] = -(camX[X]*render->camera.position[X] 
								+ camX[Y]*render->camera.position[Y]
								+ camX[Z]*render->camera.position[Z]);
	render->camera.Xiw[1][0] = camY[X];
	render->camera.Xiw[1][1] = camY[Y];
	render->camera.Xiw[1][2] = camY[Z];
	render->camera.Xiw[1][3] = //2.98e-07; // TEMP ERROR IN CALCULATION
							-(camY[X]*render->camera.position[X] 
							+ camY[Y]*render->camera.position[Y]
							+ camY[Z]*render->camera.position[Z]); // WHY IS THIS WRONG OTHER 2 ARE FINE
	render->camera.Xiw[2][0] = camZ[X];
	render->camera.Xiw[2][1] = camZ[Y];
	render->camera.Xiw[2][2] = camZ[Z];
	render->camera.Xiw[2][3] = -(camZ[X]*render->camera.position[X]
								+ camZ[Y]*render->camera.position[Y]
								+ camZ[Z]*render->camera.position[Z]);
	render->camera.Xiw[3][0] = render->camera.Xiw[3][1] = render->camera.Xiw[3][2] = 0.0;
	render->camera.Xiw[3][3] = 1.0;

	// Build xpi
	float rads = (render->camera.FOV / 2.0) * (PI / 180.0);
	//float d = 1/ tan(rads);
	render->camera.Xpi[0][0] = render->camera.Xpi[1][1] = 
		render->camera.Xpi[2][2] = render->camera.Xpi[3][3] = 1.0;
	render->camera.Xpi[0][1] = render->camera.Xpi[0][2] = render->camera.Xpi[0][3] =
		render->camera.Xpi[1][0] = render->camera.Xpi[1][2] = render->camera.Xpi[1][3] =
		render->camera.Xpi[2][0] = render->camera.Xpi[2][1] = render->camera.Xpi[2][3] = 
		render->camera.Xpi[3][0] = render->camera.Xpi[3][1] = 0.0;
	render->camera.Xpi[3][2] = tan(rads);

	// init Ximage
	//render->Ximage[render->matlevel] = render->Xsp;
	// push Xsp
	render->matlevel=-1;
	GzPushMatrix(render, render->Xsp);
	// push Xpi
	GzPushMatrix(render, render->camera.Xpi);
	// push Xiw
	GzPushMatrix(render, render->camera.Xiw);

	render->open = 1; // I don't even know what this does

	return GZ_SUCCESS;
}

int GzPutCamera(GzRender *render, GzCamera *camera)
{
/*
- overwrite renderer camera structure with new camera definition
*/
	if (render == NULL) {
		return GZ_FAILURE;
	}
	if (camera == NULL) {
		return GZ_FAILURE;
	}

	//render->camera = camera;

	render->camera.FOV = camera->FOV;
	render->camera.lookat[X] = camera->lookat[X];
	render->camera.lookat[Y] = camera->lookat[Y];
	render->camera.lookat[Z] = camera->lookat[Z];
	render->camera.position[X] = camera->position[X];
	render->camera.position[Y] = camera->position[Y];
	render->camera.position[Z] = camera->position[Z];
	render->camera.worldup[X] = camera->worldup[X];
	render->camera.worldup[Y] = camera->worldup[Y];
	render->camera.worldup[Z] = camera->worldup[Z];
	normalizeVector(render->camera.worldup);

	// I should have to redo this right since camera.fov's changed
	render->Xsp[2][2] = 2147483647 * tan((render->camera.FOV / 2.0) * (PI / 180.0));

	return GZ_SUCCESS;	
}

int GzPushMatrix(GzRender *render, GzMatrix	matrix)
{
/*
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	if (render == NULL) {
		return GZ_FAILURE;
	}
	if (matrix == NULL) {
		return GZ_FAILURE;
	}
	
	// overflow! stack is full!
	render->matlevel = render->matlevel + 1;
	if (render->matlevel >= MATLEVELS) {
		return GZ_FAILURE;
	}

	memcpy(render->Ximage[render->matlevel], matrix, sizeof(GzMatrix));
	//copyMatrix(render->Ximage[render->matlevel], matrix);

	return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render)
{
/*
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if (render == NULL) {
		return GZ_FAILURE;
	}
	
	// underflow, empty stack!
	if (render->Ximage[0] == NULL) {
		return GZ_FAILURE;
	}
	if (render->matlevel < 0) {
		return GZ_FAILURE;
	}

	--render->matlevel;

	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer	*valueList) /* void** valuelist */
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
		if (nameList[i] == GZ_RGB_COLOR) {

			GzColor* c = (GzColor*) valueList[i];
			float r = c[0][0];
			float g = c[0][1];
			float b = c[0][2];

			render->flatcolor[0] = r;
			render->flatcolor[1] = g; 
			render->flatcolor[2] = b; 
		} else if (nameList[i] == GZ_DIRECTIONAL_LIGHT) {
			// so this kinda assumes that the dir lights are all in one go, 
			GzLight* dirLite = (GzLight*) valueList[i];
			render->lights[render->numlights] = *dirLite;
			render->numlights += 1;
		} else if (nameList[i] == GZ_AMBIENT_LIGHT) {
			GzLight* ambLite = (GzLight*) valueList[i];
			render->ambientlight = *ambLite;
		} else if (nameList[i] == GZ_DIFFUSE_COEFFICIENT) {
			GzColor* diffColor = (GzColor*) valueList[i];
			
			float diffR = diffColor[0][0];
			float diffG = diffColor[0][1];
			float diffB = diffColor[0][2];

			render->Kd[0] = diffR;
			render->Kd[1] = diffG;
			render->Kd[2] = diffB;
		} else if (nameList[i] == GZ_INTERPOLATE) {
			render->interp_mode = (int) valueList[i];
		} else if (nameList[i] == GZ_AMBIENT_COEFFICIENT) {
			GzColor* ambColor = (GzColor*) valueList[i];
			
			float ambR = ambColor[0][0];
			float ambG = ambColor[0][1];
			float ambB = ambColor[0][2];

			render->Ka[0] = ambR;
			render->Ka[1] = ambG;
			render->Ka[2] = ambB;
		} else if (nameList[i] == GZ_SPECULAR_COEFFICIENT) {
			GzColor* specColor = (GzColor*) valueList[i];
			
			float specR = specColor[0][0];
			float specG = specColor[0][1];
			float specB = specColor[0][2];

			render->Ks[0] = specR;
			render->Ks[1] = specG;
			render->Ks[2] = specB;
		} else if (nameList[i] == GZ_DISTRIBUTION_COEFFICIENT) {
			float* specCoeff = (float*) valueList[i]; // ugh why, int is fine!
			render->spec = *specCoeff;
		}
	}
	
	return GZ_SUCCESS;
}

int GzPutTriangle(GzRender	*render, int numParts, GzToken *nameList, 
				  GzPointer	*valueList)
/* numParts : how many names and values */
{
/*  
- pass in a triangle description with tokens and values corresponding to 
      GZ_POSITION:3 vert positions in model space 
- Xform positions of verts  
- Clip - just discard any triangle with verts behind view plane // ALL OF THEM? or any one of them?
       - test for triangles with all three verts off-screen 
- invoke triangle rasterizer  
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

	// get Xsm
	if (render->matlevel > 0) {
		GzMatrix Xsm, temp;
		memcpy(Xsm, render->Ximage[0], sizeof(GzMatrix)); // copy first one 
		//copyMatrix(Xsm, render->Ximage[0]);
		for (int i = 1; i <= render->matlevel; ++i) {
			memcpy(temp, Xsm, sizeof(GzMatrix));
			//copyMatrix(temp, Xsm);
			Xsm[0][0] = temp[0][0]*render->Ximage[i][0][0] + temp[0][1]*render->Ximage[i][1][0]
						+ temp[0][2]*render->Ximage[i][2][0] + temp[0][3]*render->Ximage[i][3][0];
			Xsm[0][1] = temp[0][0]*render->Ximage[i][0][1] + temp[0][1]*render->Ximage[i][1][1]
						+ temp[0][2]*render->Ximage[i][2][1] + temp[0][3]*render->Ximage[i][3][1];
			Xsm[0][2] = temp[0][0]*render->Ximage[i][0][2] + temp[0][1]*render->Ximage[i][1][2]
						+ temp[0][2]*render->Ximage[i][2][2] + temp[0][3]*render->Ximage[i][3][2];
			Xsm[0][3] = temp[0][0]*render->Ximage[i][0][3] + temp[0][1]*render->Ximage[i][1][3]
						+ temp[0][2]*render->Ximage[i][2][3] + temp[0][3]*render->Ximage[i][3][3];
			/////
			Xsm[1][0] = temp[1][0]*render->Ximage[i][0][0] + temp[1][1]*render->Ximage[i][1][0]
						+ temp[1][2]*render->Ximage[i][2][0] + temp[1][3]*render->Ximage[i][3][0];
			Xsm[1][1] = temp[1][0]*render->Ximage[i][0][1] + temp[1][1]*render->Ximage[i][1][1]
						+ temp[1][2]*render->Ximage[i][2][1] + temp[1][3]*render->Ximage[i][3][1];
			Xsm[1][2] = temp[1][0]*render->Ximage[i][0][2] + temp[1][1]*render->Ximage[i][1][2]
						+ temp[1][2]*render->Ximage[i][2][2] + temp[1][3]*render->Ximage[i][3][2];
			Xsm[1][3] = temp[1][0]*render->Ximage[i][0][3] + temp[1][1]*render->Ximage[i][1][3]
						+ temp[1][2]*render->Ximage[i][2][3] + temp[1][3]*render->Ximage[i][3][3];
			////////////
			Xsm[2][0] = temp[2][0]*render->Ximage[i][0][0] + temp[2][1]*render->Ximage[i][1][0]
						+ temp[2][2]*render->Ximage[i][2][0] + temp[2][3]*render->Ximage[i][3][0];
			Xsm[2][1] = temp[2][0]*render->Ximage[i][0][1] + temp[2][1]*render->Ximage[i][1][1]
						+ temp[2][2]*render->Ximage[i][2][1] + temp[2][3]*render->Ximage[i][3][1];
			Xsm[2][2] = temp[2][0]*render->Ximage[i][0][2] + temp[2][1]*render->Ximage[i][1][2]
						+ temp[2][2]*render->Ximage[i][2][2] + temp[2][3]*render->Ximage[i][3][2];
			Xsm[2][3] = temp[2][0]*render->Ximage[i][0][3] + temp[2][1]*render->Ximage[i][1][3]
						+ temp[2][2]*render->Ximage[i][2][3] + temp[2][3]*render->Ximage[i][3][3];
			/////////////
			Xsm[3][0] = temp[3][0]*render->Ximage[i][0][0] + temp[3][1]*render->Ximage[i][1][0]
						+ temp[3][2]*render->Ximage[i][2][0] + temp[3][3]*render->Ximage[i][3][0];
			Xsm[3][1] = temp[3][0]*render->Ximage[i][0][1] + temp[3][1]*render->Ximage[i][1][1]
						+ temp[3][2]*render->Ximage[i][2][1] + temp[3][3]*render->Ximage[i][3][1];
			Xsm[3][2] = temp[3][0]*render->Ximage[i][0][2] + temp[3][1]*render->Ximage[i][1][2]
						+ temp[3][2]*render->Ximage[i][2][2] + temp[3][3]*render->Ximage[i][3][2];
			Xsm[3][3] = temp[3][0]*render->Ximage[i][0][3] + temp[3][1]*render->Ximage[i][1][3]
						+ temp[3][2]*render->Ximage[i][2][3] + temp[3][3]*render->Ximage[i][3][3];
		}
		// POP EVERYTHING
		for (int i = render->matlevel; i >= 0; --i) {
			GzPopMatrix(render);
		}
		// push on Xsm
		GzPushMatrix(render, Xsm);
	}

	// Get vert positions
	for (int i = 0; i < numParts; ++i) {
		if (nameList[i] == GZ_POSITION) {
			// get points
			GzCoord* tri = (GzCoord*) valueList[i];
			GzCoord* xformTri = new GzCoord[3];
			bool behindVP = false;
			GzMatrix topMat;
			int top = render->matlevel;
			memcpy(topMat, render->Ximage[top], sizeof(GzMatrix));
			//copyMatrix(topMat, render->xim
			float W;

			for (int j = 0; j < 3; ++j) {
				// xform verticies
				
				xformTri[j][X] = topMat[0][0]*tri[j][X] + topMat[0][1]*tri[j][Y] + topMat[0][2]*tri[j][Z]
					+ topMat[0][3]*1.0;
				xformTri[j][Y] = topMat[1][0]*tri[j][X] + topMat[1][1]*tri[j][Y] + topMat[1][2]*tri[j][Z]
					+ topMat[1][3]*1.0;
				xformTri[j][Z] = topMat[2][0]*tri[j][X] + topMat[2][1]*tri[j][Y] + topMat[2][2]*tri[j][Z]
					+ topMat[2][3]*1.0;
				W = topMat[3][0]*tri[j][X] + topMat[3][1]*tri[j][Y] + topMat[3][2]*tri[j][Z]
					+ topMat[3][3]*1;
				xformTri[j][X] /= W;
				xformTri[j][Y] /= W;
				xformTri[j][Z] /= W;
			
				// check if any vets in behind view plane
				if (xformTri[j][Z] < render->camera.position[Z]) {
					behindVP = true;
					break;
				}
			}
	//	float x0 = tri[0][X];
	//		float x1 = tri[1][X];
	//		float x2 = tri[2][X];
	//		float tx0 = xformTri[0][X];
	//		float tx1 = xformTri[1][X];
	//		float tx2 = xformTri[2][X];

	//		float y0 = tri[0][Y];
	//		float y1 = tri[1][Y];
	//		float y2 = tri[2][Y];
	//		float ty0 = xformTri[0][Y];
	//		float ty1 = xformTri[1][Y];
	//		float ty2 = xformTri[2][Y];

	//		float z0 = tri[0][Z];
	//		float z1 = tri[1][Z];
	//		float z2 = tri[2][Z];
	//		float tz0 = xformTri[0][Z];
	//		float tz1 = xformTri[1][Z];
	//		float tz2 = xformTri[2][Z];

			// this tri is in front of viewing plane
			if (behindVP == true) {
				break;
			}
			// CLIPPING
			//if (xformTri[0][Z] < render->camera.position[Z] 
			//	&& xformTri[1][Z] < render->camera.position[Z]
			//	&& xformTri[2][Z] < render->camera.position[Z]) {
			//	break;
			//}

			//if ((xformTri[0][X] < 0 && xformTri[1][

			// check if all verts are in screen (think i handle this in my rasterizer)
			// RASTERIZE
			// sort by Y
			int minY = 0;
			for (int i = 0; i < 2; ++i) {
				minY = i;
				for (int j = i + 1; j < 3; ++j) {
					if (xformTri[minY][Y] > xformTri[j][Y]) {
						minY = j;
					}
				}
				// swapping
				if (minY != i) {
					float tempX = xformTri[i][X];
					float tempY = xformTri[i][Y];
					float tempZ = xformTri[i][Z];
					xformTri[i][X] = xformTri[minY][X];
					xformTri[i][Y] = xformTri[minY][Y];
					xformTri[i][Z] = xformTri[minY][Z];
					xformTri[minY][X] = tempX;
					xformTri[minY][Y] = tempY;
					xformTri[minY][Z] = tempZ;
				}
			}


			// Get bounding box
			int topY = floor(xformTri[0][Y]);
			int bottomY = ceil(xformTri[2][Y]);
			int leftX, rightX;
			// 0 comes before 1
			if (xformTri[0][X] < xformTri[1][X]) {
				// 201
				if (xformTri[2][X] < xformTri[0][X]) {
					leftX = floor(xformTri[2][X]);
					rightX = ceil(xformTri[1][X]);
				} else { //0
					leftX = floor(xformTri[0][X]);
					// 021
					if (xformTri[2][X] < xformTri[1][X]) {
						rightX = ceil(xformTri[1][X]);
					} else { // 012
						rightX = ceil(xformTri[2][X]);
					}
				}
			} else { // 1 comes before 0
				//210
				if (xformTri[2][X] < xformTri[1][X]) {
					leftX = floor(xformTri[2][X]);
					rightX = ceil(xformTri[0][X]);
				} else { //1
					leftX = floor(xformTri[1][X]);
					// 120
					if (xformTri[2][X] < xformTri[0][X]) {
						rightX = ceil(xformTri[0][X]);
					} else { // 102
						rightX = ceil(xformTri[2][X]);
					}
				}
			}

			// For Z Interpolation
			// Ax + By + Cz + D = 0;
			// xformTri[0] x xformTri[1] = (A, B, C)
			GzCoord edge01;
			edge01[X] = xformTri[1][X]- xformTri[0][X];
			edge01[Y] = xformTri[1][Y] - xformTri[0][Y];
			edge01[Z] = xformTri[1][Z] - xformTri[0][Z];
			GzCoord edge12;
			edge12[X] = xformTri[2][X] - xformTri[1][X];
			edge12[Y] = xformTri[2][Y] - xformTri[1][Y];
			edge12[Z] = xformTri[2][Z] - xformTri[1][Z];

			float A = edge01[Y]*edge12[Z] - edge01[Z]*edge12[Y];
			float B = edge01[Z]*edge12[X] - edge01[X]*edge12[Z];
			float C = edge01[X]*edge12[Y] - edge01[Y]*edge12[X];
			// get D
			float D = -(A*xformTri[0][X]) - (B*xformTri[0][Y]) - (C*xformTri[0][Z]);

			// DRAW PIXELS
			float interpZ;
			for (int i = leftX; i < rightX; ++i) {
				// bounds check
				if (i < 0 || i > render->display->xres) {
					continue;
				}
				for (int j = topY; j < bottomY; ++j) {
					//// bounds check
					if (j < 0 || j > render->display->yres) {
						continue;
					}

					// Compute LEES
					// E(x, y) = dY(x-X) - dX(y-Y)
					// EDGE 0-1
					float e01 = (xformTri[1][Y] - xformTri[0][Y])*((float)i - xformTri[0][X]) 
								- (xformTri[1][X] - xformTri[0][X])*((float)j - xformTri[0][Y]);
					// EDGE 1-2
					float e12 = (xformTri[2][Y] - xformTri[1][Y])*((float)i - xformTri[1][X]) 
								- (xformTri[2][X] - xformTri[1][X])*((float)j - xformTri[1][Y]);
					// EDGE 2-0
					float e20 = (xformTri[0][Y] - xformTri[2][Y])*((float)i - xformTri[2][X]) 
								- (xformTri[0][X] - xformTri[2][X])*((float)j - xformTri[2][Y]);

					// if all have same sign then this pixel should be drawn
					if (e01 == 0 || e12 == 0 || e20 == 0) {
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

		}
	}

	return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}

