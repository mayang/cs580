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


int GzNormalizeVector(GzCoord vec) {
	if (vec == NULL) {
		return GZ_FAILURE;
	}

	float norm = sqrt((vec[X]*vec[X]) + (vec[Y]*vec[Y]) + (vec[Z]*vec[Z]));

	vec[X] = vec[X] / norm;
	vec[Y] = vec[Y] / norm;
	vec[Z] = vec[Z] / norm;

	return GZ_SUCCESS;
}

float GzDotProduct(GzCoord v1, GzCoord v2) {
	return v1[X]*v2[X] + v1[Y]*v2[Y] + v1[Z]*v2[Z];
}

int GzShadingEquation(GzRender *render, GzColor color, GzCoord norm ) {
		// computer color at each vertex
		GzNormalizeVector(norm);	
		// E should just be camera lookat reversed? Actually no goddamn it
		GzCoord E;
		E[X] = 0; 
		E[Y] = 0;
		E[Z] = -1;
		GzNormalizeVector(E);
		// calculate Rs for each point
		GzCoord* R = new GzCoord[render->numlights];
		// vertex 0
		float NdotL;
		int* liteCases = new int[render->numlights];
		// check N dot L and N dot E
		// if both positive fine liteCases[i] = 1;
		// if both negative flip normal, liteCases[i] = -1;
		// if different signs, skip, liteCases = 0;

		float NdotE = GzDotProduct(norm, E);
		for (int i = 0; i < render->numlights; ++i) {
			NdotL = GzDotProduct(norm, render->lights[i].direction);
			if (NdotL >= 0 && NdotE >= 0) {
				liteCases[i] = 1;
				R[i][X] = 2*NdotL*norm[X] - render->lights[i].direction[X];
				R[i][Y] = 2*NdotL*norm[Y] - render->lights[i].direction[Y];
				R[i][Z] = 2*NdotL*norm[Z] - render->lights[i].direction[Z];
				GzNormalizeVector(R[i]);
			} else if (NdotL < 0 && NdotE < 0) {
				liteCases[i] = -1;
				R[i][X] = 2*NdotL*(-norm[X]) - render->lights[i].direction[X];
				R[i][Y] = 2*NdotL*(-norm[Y]) - render->lights[i].direction[Y];
				R[i][Z] = 2*NdotL*(-norm[Z]) - render->lights[i].direction[Z];
				GzNormalizeVector(R[i]);
			} else {
				liteCases[i] = 0;
				continue;
			}
		}
		// check N dot L and N dot E, if both positi


		// sum all lights for Specular
		// Ks * sigma (le * (R dot E)^s) 
		GzColor specLightSum = {0, 0, 0};
		for (int i = 0; i < render->numlights; ++i) {
			if (liteCases[i] == 0) continue;
			float RdotE = GzDotProduct(R[i], E);
			if (RdotE < 0) RdotE = 0;
			if (RdotE > 1) RdotE = 1;
			// R
			specLightSum[0] += render->lights[i].color[0] * pow(RdotE, render->spec);
			// G
			specLightSum[1] += render->lights[i].color[1] * pow(RdotE, render->spec);
			// B
			specLightSum[2] += render->lights[i].color[2] * pow(RdotE, render->spec);
		}
		GzColor specComp;
		specComp[0] = render->Ks[0] * specLightSum[0]; // R
		specComp[1] = render->Ks[1] * specLightSum[1]; // G
		specComp[2] = render->Ks[2] * specLightSum[2]; // B
		
		// sum all lights for Diffuse
		// Kd * sigma (le * N dot L)
		GzColor diffLightSum = {0, 0, 0};
		for (int i = 0; i < render->numlights; ++i) {
			if (liteCases[i] == 0) continue;
			if (liteCases[i] == 1) {
				// R
				diffLightSum[0] += render->lights[i].color[0] * GzDotProduct(norm, render->lights[i].direction);
				// G
				diffLightSum[1] += render->lights[i].color[1] * GzDotProduct(norm, render->lights[i].direction);
				// B
				diffLightSum[2] += render->lights[i].color[2] * GzDotProduct(norm, render->lights[i].direction);
			} else if (liteCases[i] == -1) {
				GzCoord negN = {-norm[X], -norm[Y], -norm[Z]};
				// R
				diffLightSum[0] += render->lights[i].color[0] * GzDotProduct(negN, render->lights[i].direction);
				// G
				diffLightSum[1] += render->lights[i].color[1] * GzDotProduct(negN, render->lights[i].direction);
				// B
				diffLightSum[2] += render->lights[i].color[2] * GzDotProduct(negN, render->lights[i].direction);
			}
		}
		GzColor diffComp;
		diffComp[0] = render->Kd[0] * diffLightSum[0]; // R
		diffComp[1] = render->Kd[1] * diffLightSum[1]; // G
		diffComp[2] = render->Kd[2] * diffLightSum[2]; // B

		// Ambient Component
		GzColor ambComp;
		ambComp[0] = render->Ka[0] * render->ambientlight.color[0]; // R
		ambComp[1] = render->Ka[1] * render->ambientlight.color[1]; // G
		ambComp[2] = render->Ka[2] * render->ambientlight.color[2]; // B

		color[0] = specComp[0] + diffComp[0] + ambComp[0]; // R
		color[1] = specComp[1] + diffComp[1] + ambComp[1]; // G
		color[2] = specComp[2] + diffComp[2] + ambComp[2]; // B

		delete [] R;
		delete [] liteCases;
		return GZ_SUCCESS;		
}

// Special shading for texture gouraud, it doesn't multiply the Koefficients
int GzShadingEquationTextureGouraud(GzRender *render, GzColor color, GzCoord norm ) {
		// computer color at each vertex
		GzNormalizeVector(norm);	
		// E should just be camera lookat reversed? Actually no goddamn it
		GzCoord E;
		E[X] = 0; 
		E[Y] = 0;
		E[Z] = -1;
		GzNormalizeVector(E);
		// calculate Rs for each point
		GzCoord* R = new GzCoord[render->numlights];
		// vertex 0
		float NdotL;
		int* liteCases = new int[render->numlights];
		// check N dot L and N dot E
		// if both positive fine liteCases[i] = 1;
		// if both negative flip normal, liteCases[i] = -1;
		// if different signs, skip, liteCases = 0;

		float NdotE = GzDotProduct(norm, E);
		for (int i = 0; i < render->numlights; ++i) {
			NdotL = GzDotProduct(norm, render->lights[i].direction);
			if (NdotL >= 0 && NdotE >= 0) {
				liteCases[i] = 1;
				R[i][X] = 2*NdotL*norm[X] - render->lights[i].direction[X];
				R[i][Y] = 2*NdotL*norm[Y] - render->lights[i].direction[Y];
				R[i][Z] = 2*NdotL*norm[Z] - render->lights[i].direction[Z];
				GzNormalizeVector(R[i]);
			} else if (NdotL < 0 && NdotE < 0) {
				liteCases[i] = -1;
				R[i][X] = 2*NdotL*(-norm[X]) - render->lights[i].direction[X];
				R[i][Y] = 2*NdotL*(-norm[Y]) - render->lights[i].direction[Y];
				R[i][Z] = 2*NdotL*(-norm[Z]) - render->lights[i].direction[Z];
				GzNormalizeVector(R[i]);
			} else {
				liteCases[i] = 0;
				continue;
			}
		}
		// check N dot L and N dot E, if both positi


		// sum all lights for Specular
		// Ks * sigma (le * (R dot E)^s) 
		GzColor specLightSum = {0, 0, 0};
		for (int i = 0; i < render->numlights; ++i) {
			if (liteCases[i] == 0) continue;
			float RdotE = GzDotProduct(R[i], E);
			if (RdotE < 0) RdotE = 0;
			if (RdotE > 1) RdotE = 1;
			// R
			specLightSum[0] += render->lights[i].color[0] * pow(RdotE, render->spec);
			// G
			specLightSum[1] += render->lights[i].color[1] * pow(RdotE, render->spec);
			// B
			specLightSum[2] += render->lights[i].color[2] * pow(RdotE, render->spec);
		}
		
		// sum all lights for Diffuse
		// Kd * sigma (le * N dot L)
		GzColor diffLightSum = {0, 0, 0};
		for (int i = 0; i < render->numlights; ++i) {
			if (liteCases[i] == 0) continue;
			if (liteCases[i] == 1) {
				// R
				diffLightSum[0] += render->lights[i].color[0] * GzDotProduct(norm, render->lights[i].direction);
				// G
				diffLightSum[1] += render->lights[i].color[1] * GzDotProduct(norm, render->lights[i].direction);
				// B
				diffLightSum[2] += render->lights[i].color[2] * GzDotProduct(norm, render->lights[i].direction);
			} else if (liteCases[i] == -1) {
				GzCoord negN = {-norm[X], -norm[Y], -norm[Z]};
				// R
				diffLightSum[0] += render->lights[i].color[0] * GzDotProduct(negN, render->lights[i].direction);
				// G
				diffLightSum[1] += render->lights[i].color[1] * GzDotProduct(negN, render->lights[i].direction);
				// B
				diffLightSum[2] += render->lights[i].color[2] * GzDotProduct(negN, render->lights[i].direction);
			}
		}

		color[0] = specLightSum[0] + diffLightSum[0] + render->ambientlight.color[0]; // R
		color[1] = specLightSum[1] + diffLightSum[1] + render->ambientlight.color[1]; // G
		color[2] = specLightSum[2] + diffLightSum[2] + render->ambientlight.color[2]; // B

		delete [] R;
		delete [] liteCases;

		return GZ_SUCCESS;		
}

// finds aread of a triangle given the verticies
float GzTriangleArea(GzCoord v0, GzCoord v1, GzCoord v2) {
	return abs(.5 * (v0[X]*v1[Y] + v0[Y]*v2[X] + v1[X]*v2[Y] - v1[Y]*v2[X] - v0[Y]*v1[X] - v0[X]*v2[Y]));
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
	
	for (int i = 0; i < 6; ++i) {
		GzNewDisplay(&(*render)->subsampledDisplays[i], GZ_RGBAZ_DISPLAY, 256, 256);
	}

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
	for (int i = 0; i < 6; ++i) {
		GzFreeDisplay(render->subsampledDisplays[i]);
	}
	for (int i = 0; i < render->matlevel; ++i) {
		delete render->Ximage[i];
		delete render->Xnorm[i];
	}
	delete [] render->Ximage;
	delete [] render->Xnorm;
	for (int i = 0; i < render->numlights; ++i) {
		delete [] render->lights[i].color;
		delete [] render->lights[i].direction;
	}
	delete [] render->lights;
	//////render->Ximage = NULL;
	delete [] render->Xsp;
	//////render->Xsp = NULL
	delete [] render->camera.Xiw;
	delete [] render->camera.Xpi;
	delete [] render->camera.lookat;
	delete [] render->camera.position;
	delete [] render->camera.worldup;
	delete render;

	return GZ_SUCCESS;
}

int GzPushNormalMatrix(GzRender *render, GzMatrix matrix) {
	// replaces top with new matrix
	
	// make it a unitary rotation matrix
	float K = 1 / sqrt((matrix[0][0]*matrix[0][0] + matrix[0][1]*matrix[0][1] +
					matrix[0][2]*matrix[0][2] + matrix[0][3]*matrix[0][3]));

	matrix[0][0] *= K;
	matrix[0][1] *= K;
	matrix[0][2] *= K;
	matrix[0][3] *= K;
	/////////////////
	matrix[1][0] *= K;
	matrix[1][1] *= K;
	matrix[1][2] *= K;
	matrix[1][3] *= K;
	//////////////////
	matrix[2][0] *= K;
	matrix[2][1] *= K;
	matrix[2][2] *= K;
	matrix[2][3] *= K;
	//////////////////
	matrix[3][0] *= K;
	matrix[3][1] *= K;
	matrix[3][2] *= K;
	matrix[3][3] *= K;

	memcpy(render->Xnorm[render->matlevel], matrix, sizeof(GzMatrix));
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
 
	GzMatrix identity = 
	{ 
	1.0,	0.0,	0.0,	0.0, 
	0.0,	1.0,	0.0,	0.0, 
	0.0,	0.0,	1.0,	0.0, 
	0.0,	0.0,	0.0,	1.0 
	};

	// computer Xiw
	// camera z axis
	GzCoord cl, camZ;
	cl[X] = render->camera.lookat[X] - render->camera.position[X];
	cl[Y] = render->camera.lookat[Y] - render->camera.position[Y];
	cl[Z] = render->camera.lookat[Z] - render->camera.position[Z];
	GzNormalizeVector(cl);
	camZ[X] = cl[X];
	camZ[Y] = cl[Y];
	camZ[Z] = cl[Z];
	GzNormalizeVector(camZ);
	
	// camera y axis
	GzCoord camUp, camY;
	float upDotZ = render->camera.worldup[X]*camZ[X] + render->camera.worldup[Y]*camZ[Y] +
		render->camera.worldup[Z]*camZ[Z];
	camUp[X] = render->camera.worldup[X] - upDotZ*camZ[X];
	camUp[Y] = render->camera.worldup[Y] - upDotZ*camZ[Y];
	camUp[Z] = render->camera.worldup[Z] - upDotZ*camZ[Z];
	GzNormalizeVector(camUp);
	camY[X] = camUp[X];
	camY[Y] = camUp[Y];
	camY[Z] = camUp[Z];
	GzNormalizeVector(camY);

	// camera x axis
	GzCoord camX;
	camX[X] = camY[Y]*camZ[Z] - camY[Z]*camZ[Y];
	camX[Y] = camY[Z]*camZ[X] - camY[X]*camZ[Z];
	camX[Z] = camY[X]*camZ[Y] - camY[Y]*camZ[X];
	GzNormalizeVector(camX);

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
	// push Xsp on Ximage, idenity on Xnorm
	render->matlevel=-1;
	GzPushMatrix(render, render->Xsp);
	GzPushNormalMatrix(render, identity);
	// push Xpi on Ximage, identity on Xnorm
	GzPushMatrix(render, render->camera.Xpi);
	GzPushNormalMatrix(render, identity);
	// push Xiw on Ximage, on Xnorm
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
	GzNormalizeVector(render->camera.worldup);

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

	GzMatrix top;

	// push onto Ximage
	memcpy(render->Ximage[render->matlevel], matrix, sizeof(GzMatrix));

	GzPushNormalMatrix(render, matrix);

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
			int* mode = (int*) valueList[i];
			render->interp_mode = *mode;
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
		} else if (nameList[i] == GZ_TEXTURE_MAP) {
			render->tex_fun = (GzTexture) valueList[i]; // what am i doing here?! I have no diea
		} else if (nameList[i] == GZ_AASHIFTX) {
			float* table = (float*) valueList[i];
			int ti = 0;
			for (int j = 0; j < 6; ++j) {
				render->filterTable[j][X] = table[ti++];
				render->filterTable[j][Y] = table[ti++];
				render->filterTable[j][2] = table[ti++];
			}

		} /*else if (nameList[i] == GZ_AASHIFTY) {
			float* offY = (float*) valueList[i];
		}*/
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
		GzMatrix Xsm, temp, Xn;
		memcpy(Xsm, render->Ximage[0], sizeof(GzMatrix)); // copy first one 
		memcpy(Xn, render->Xnorm[0], sizeof(GzMatrix));
		//copyMatrix(Xsm, render->Ximage[0]);
		for (int i = 1; i <= render->matlevel; ++i) {
			memcpy(temp, Xsm, sizeof(GzMatrix));
			//////////////////
			Xsm[0][0] = temp[0][0]*render->Ximage[i][0][0] + temp[0][1]*render->Ximage[i][1][0]
						+ temp[0][2]*render->Ximage[i][2][0] + temp[0][3]*render->Ximage[i][3][0];
			Xsm[0][1] = temp[0][0]*render->Ximage[i][0][1] + temp[0][1]*render->Ximage[i][1][1]
						+ temp[0][2]*render->Ximage[i][2][1] + temp[0][3]*render->Ximage[i][3][1];
			Xsm[0][2] = temp[0][0]*render->Ximage[i][0][2] + temp[0][1]*render->Ximage[i][1][2]
						+ temp[0][2]*render->Ximage[i][2][2] + temp[0][3]*render->Ximage[i][3][2];
			Xsm[0][3] = temp[0][0]*render->Ximage[i][0][3] + temp[0][1]*render->Ximage[i][1][3]
						+ temp[0][2]*render->Ximage[i][2][3] + temp[0][3]*render->Ximage[i][3][3];
			/////////////////
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
			////////////////////
			memcpy(temp, Xn, sizeof(GzMatrix));
			//////////////////
			Xn[0][0] = temp[0][0]*render->Xnorm[i][0][0] + temp[0][1]*render->Xnorm[i][1][0]
						+ temp[0][2]*render->Xnorm[i][2][0] + temp[0][3]*render->Xnorm[i][3][0];
			Xn[0][1] = temp[0][0]*render->Xnorm[i][0][1] + temp[0][1]*render->Xnorm[i][1][1]
						+ temp[0][2]*render->Xnorm[i][2][1] + temp[0][3]*render->Xnorm[i][3][1];
			Xn[0][2] = temp[0][0]*render->Xnorm[i][0][2] + temp[0][1]*render->Xnorm[i][1][2]
						+ temp[0][2]*render->Xnorm[i][2][2] + temp[0][3]*render->Xnorm[i][3][2];
			Xn[0][3] = temp[0][0]*render->Xnorm[i][0][3] + temp[0][1]*render->Xnorm[i][1][3]
						+ temp[0][2]*render->Xnorm[i][2][3] + temp[0][3]*render->Xnorm[i][3][3];
			/////////////////
			Xn[1][0] = temp[1][0]*render->Xnorm[i][0][0] + temp[1][1]*render->Xnorm[i][1][0]
						+ temp[1][2]*render->Xnorm[i][2][0] + temp[1][3]*render->Xnorm[i][3][0];
			Xn[1][1] = temp[1][0]*render->Xnorm[i][0][1] + temp[1][1]*render->Xnorm[i][1][1]
						+ temp[1][2]*render->Xnorm[i][2][1] + temp[1][3]*render->Xnorm[i][3][1];
			Xn[1][2] = temp[1][0]*render->Xnorm[i][0][2] + temp[1][1]*render->Xnorm[i][1][2]
						+ temp[1][2]*render->Xnorm[i][2][2] + temp[1][3]*render->Xnorm[i][3][2];
			Xn[1][3] = temp[1][0]*render->Xnorm[i][0][3] + temp[1][1]*render->Xnorm[i][1][3]
						+ temp[1][2]*render->Xnorm[i][2][3] + temp[1][3]*render->Xnorm[i][3][3];
			////////////
			Xn[2][0] = temp[2][0]*render->Xnorm[i][0][0] + temp[2][1]*render->Xnorm[i][1][0]
						+ temp[2][2]*render->Xnorm[i][2][0] + temp[2][3]*render->Xnorm[i][3][0];
			Xn[2][1] = temp[2][0]*render->Xnorm[i][0][1] + temp[2][1]*render->Xnorm[i][1][1]
						+ temp[2][2]*render->Xnorm[i][2][1] + temp[2][3]*render->Xnorm[i][3][1];
			Xn[2][2] = temp[2][0]*render->Xnorm[i][0][2] + temp[2][1]*render->Xnorm[i][1][2]
						+ temp[2][2]*render->Xnorm[i][2][2] + temp[2][3]*render->Xnorm[i][3][2];
			Xn[2][3] = temp[2][0]*render->Xnorm[i][0][3] + temp[2][1]*render->Xnorm[i][1][3]
						+ temp[2][2]*render->Xnorm[i][2][3] + temp[2][3]*render->Xnorm[i][3][3];
			/////////////
			Xn[3][0] = temp[3][0]*render->Xnorm[i][0][0] + temp[3][1]*render->Xnorm[i][1][0]
						+ temp[3][2]*render->Xnorm[i][2][0] + temp[3][3]*render->Xnorm[i][3][0];
			Xn[3][1] = temp[3][0]*render->Xnorm[i][0][1] + temp[3][1]*render->Xnorm[i][1][1]
						+ temp[3][2]*render->Xnorm[i][2][1] + temp[3][3]*render->Xnorm[i][3][1];
			Xn[3][2] = temp[3][0]*render->Xnorm[i][0][2] + temp[3][1]*render->Xnorm[i][1][2]
						+ temp[3][2]*render->Xnorm[i][2][2] + temp[3][3]*render->Xnorm[i][3][2];
			Xn[3][3] = temp[3][0]*render->Xnorm[i][0][3] + temp[3][1]*render->Xnorm[i][1][3]
						+ temp[3][2]*render->Xnorm[i][2][3] + temp[3][3]*render->Xnorm[i][3][3];
		}
		// POP EVERYTHING
		for (int i = render->matlevel; i >= 0; --i) {
			GzPopMatrix(render);
		}
		// push on Xsm
		GzPushMatrix(render, Xsm);
		GzPushNormalMatrix(render, Xn);
	}
	

	// get verticies, normals, uv coords
	GzCoord* xformTri = new GzCoord[3];
	GzCoord* xformNs = new GzCoord[3];
	GzTextureIndex* UVTextCoord = new GzTextureIndex[3];
	bool behindVP = false;
	GzMatrix topMat, topNMat;
	int top = render->matlevel;
	memcpy(topMat, render->Ximage[top], sizeof(GzMatrix));
	memcpy(topNMat, render->Xnorm[top], sizeof(GzMatrix));
	//copyMatrix(topMat, render->xim
	float W;
	for (int i = 0; i < numParts; ++i) {
		// Get vert positions
		if (nameList[i] == GZ_POSITION) {
			GzCoord* tri = (GzCoord*) valueList[i];
			// Transform Verticies into screen space
			for (int j = 0; j < 3; ++j) {
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
					//break;
				}
			}
		}
		// get vert normals
		if(nameList[i] == GZ_NORMAL) {
			GzCoord* Ns = (GzCoord*) valueList[i];
			// don't need to transform these? though I should probably normalize
			GzNormalizeVector(Ns[0]);
			GzNormalizeVector(Ns[1]);
			GzNormalizeVector(Ns[2]);
			for (int j = 0; j < 3; ++j) {
				xformNs[j][X] = topNMat[0][0]*Ns[j][X] + topNMat[0][1]*Ns[j][Y] + topNMat[0][2]*Ns[j][Z];
				xformNs[j][Y] = topNMat[1][0]*Ns[j][X] + topNMat[1][1]*Ns[j][Y] + topNMat[1][2]*Ns[j][Z];
				xformNs[j][Z] = topNMat[2][0]*Ns[j][X] + topNMat[2][1]*Ns[j][Y] + topNMat[2][2]*Ns[j][Z];
			}
			GzNormalizeVector(xformNs[0]);
			GzNormalizeVector(xformNs[1]);
			GzNormalizeVector(xformNs[2]);
		}

		// get texture uv coordinates
		if (nameList[i] == GZ_TEXTURE_INDEX) {
			GzTextureIndex* uv = (GzTextureIndex*) valueList[i];
			for (int k = 0; k < 3; ++k) {
				// transform uv into perspective space UV for each vertex
				float Vz = xformTri[k][Z] / (INT_MAX - xformTri[k][Z]);
				UVTextCoord[k][0] = uv[k][0] / (Vz + 1);
				UVTextCoord[k][1] = uv[k][1] / (Vz + 1);	

			}
		}

	}



	// this tri is in front of viewing plane
	// RASTERIZE!!
	if (behindVP == false) {
		int leftX, rightX;
		int topY, bottomY;
		// sort by Y
		int minY = 0;
		for (int i = 0; i < 2; ++i) {
			minY = i;
			for (int j = i + 1; j < 3; ++j) {
				if (xformTri[minY][Y] > xformTri[j][Y]) {
					minY = j;
				}
			}
			// swapping, have to swap norms too i guess...?
			
			if (minY != i) {
				float tempU, tempV;
				// verts & norms
				float tempX = xformTri[i][X];
				float tempNX = xformNs[i][X];
				float tempY = xformTri[i][Y];
				float tempNY = xformNs[i][Y];
				float tempZ = xformTri[i][Z];
				float tempNZ = xformNs[i][Z];
				// text coords
				if (render->tex_fun != NULL) {
					tempU = UVTextCoord[i][0];
					tempV = UVTextCoord[i][1];
				}
				// verts & norms
				xformTri[i][X] = xformTri[minY][X];
				xformNs[i][X] = xformNs[minY][X];
				xformTri[i][Y] = xformTri[minY][Y];
				xformNs[i][Y] = xformNs[minY][Y];
				xformTri[i][Z] = xformTri[minY][Z];
				xformNs[i][Z] = xformNs[minY][Z];
				// text coords
				if (render->tex_fun != NULL) {
					UVTextCoord[i][0] = UVTextCoord[minY][0];
					UVTextCoord[i][1] = UVTextCoord[minY][1];
				}
				// verts and norms
				xformTri[minY][X] = tempX;
				xformNs[minY][X] = tempNX;
				xformTri[minY][Y] = tempY;
				xformNs[minY][Y] = tempNY;
				xformTri[minY][Z] = tempZ;
				xformNs[minY][Z] = tempNZ;
				// text coords
				if (render->tex_fun != NULL) {
					UVTextCoord[minY][0] = tempU;
					UVTextCoord[minY][1] = tempV;
				}
			}
		}

		// Offset here!
		//int k = 5;
		for (int k = 0; k < 6; ++k) {
			GzCoord offsetTri[3];
			float offX = render->filterTable[k][X];
			float offY = render->filterTable[k][Y];
			offsetTri[0][X] = xformTri[0][X] - offX;
			offsetTri[1][X] = xformTri[1][X] - offX;
			offsetTri[2][X] = xformTri[2][X] - offX;
			offsetTri[0][Y] = xformTri[0][Y] - offY;
			offsetTri[1][Y] = xformTri[1][Y] - offY;
			offsetTri[2][Y] = xformTri[2][Y] - offY;
			offsetTri[0][Z] = xformTri[0][Z];
			offsetTri[1][Z] = xformTri[1][Z];
			offsetTri[2][Z] = xformTri[2][Z];

			// Get bounding box
			topY = floor(offsetTri[0][Y]);
			bottomY = ceil(offsetTri[2][Y]);
			
			// 0 comes before 1
			if (offsetTri[0][X] < offsetTri[1][X]) {
				// 201
				if (offsetTri[2][X] < offsetTri[0][X]) {
					leftX = floor(offsetTri[2][X]);
					rightX = ceil(offsetTri[1][X]);
				} else { //0
					leftX = floor(offsetTri[0][X]);
					// 021
					if (offsetTri[2][X] < offsetTri[1][X]) {
						rightX = ceil(offsetTri[1][X]);
					} else { // 012
						rightX = ceil(offsetTri[2][X]);
					}
				}
			} else { // 1 comes before 0
				//210
				if (offsetTri[2][X] < offsetTri[1][X]) {
					leftX = floor(offsetTri[2][X]);
					rightX = ceil(offsetTri[0][X]);
				} else { //1
					leftX = floor(offsetTri[1][X]);
					// 120
					if (offsetTri[2][X] < offsetTri[0][X]) {
						rightX = ceil(offsetTri[0][X]);
					} else { // 102
						rightX = ceil(offsetTri[2][X]);
					}
				}
			}

			// calculate colors for verticies, write them into frame buffer
			/////////// GOURAUD ////////////////////////////////////////////
			// calculate colors at each vertex
			GzColor colorV0, colorV1, colorV2;
			if (render->interp_mode == GZ_COLOR) {
				if (render->tex_fun == NULL) {
					GzShadingEquation(render ,colorV0, xformNs[0]);
					GzShadingEquation(render, colorV1, xformNs[1]);
					GzShadingEquation(render, colorV2, xformNs[2]);
				} else {
					GzShadingEquationTextureGouraud(render, colorV0, xformNs[0]);
					GzShadingEquationTextureGouraud(render, colorV1, xformNs[1]);
					GzShadingEquationTextureGouraud(render, colorV2, xformNs[2]);
				}
			}
		
			// For interpolating
			// area of whole triangle
			// total area of triangle for interpolation Z, both Gouraud and Phong
			float triA = GzTriangleArea(offsetTri[0], offsetTri[1], offsetTri[2]);
			//////////////////////////////////////////////////////////

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
					float e01 = (offsetTri[1][Y] - offsetTri[0][Y])*((float)i - offsetTri[0][X]) 
								- (offsetTri[1][X] - offsetTri[0][X])*((float)j - offsetTri[0][Y]);
					// EDGE 1-2
					float e12 = (offsetTri[2][Y] - offsetTri[1][Y])*((float)i - offsetTri[1][X]) 
								- (offsetTri[2][X] - offsetTri[1][X])*((float)j - offsetTri[1][Y]);
					// EDGE 2-0
					float e20 = (offsetTri[0][Y] - offsetTri[2][Y])*((float)i - offsetTri[2][X]) 
								- (offsetTri[0][X] - offsetTri[2][X])*((float)j - offsetTri[2][Y]);

					if ( e01 == 0 || e12 == 0 || e20 == 0 || 
						((e01 > 0) && (e12 > 0) && (e20 > 0)) || 
						((e01 < 0) && (e12 < 0) && (e20 < 0))) {
						// Interpolate Z value
						//interpZ = (-(A*i) - (B*j) - D) / C;

						// get current z at this pixel
						GzCoord p = {i, j, 1};
						GzIntensity r, g, b, a;
						GzDepth z = 0;

						// areas of each inner tris
						// For Baycentric Interpolation
						float A0 = GzTriangleArea(offsetTri[1], p, offsetTri[2]);
						float A1 = GzTriangleArea(offsetTri[0], p, offsetTri[2]);
						float A2 = GzTriangleArea(offsetTri[0], p, offsetTri[1]);

						// Interpolate Z for this point
						interpZ = (A0*offsetTri[0][Z] + A1*offsetTri[1][Z] + A2*offsetTri[2][Z]) / triA;

						// get current values at this point
						GzGetDisplay(render->subsampledDisplays[k], i, j, &r, &g, &b, &a, &z);
						//GzGetDisplay(render->display, i, j, &r, &g, &b, &a, &z);
						// compare, if interpZ less than draw over
						if (interpZ < z) {

							// Interpolate UV
							GzTextureIndex UV;
							GzColor texColor;
							if (render->tex_fun != NULL) {	
								UV[0] = (A0*UVTextCoord[0][0] + A1*UVTextCoord[1][0] + A2*UVTextCoord[2][0]) / triA;
								UV[1] = (A0*UVTextCoord[0][1] + A1*UVTextCoord[1][1] + A2*UVTextCoord[2][1]) / triA;
						
								// back to uv because reasons
								GzTextureIndex uv;
								float Vz = interpZ / (INT_MAX - interpZ);
								uv[0] = UV[0] * (Vz + 1);
								uv[1] = UV[1] * (Vz + 1);
						
								// look up texture color at this pixel
						
						
								render->tex_fun(uv[0], uv[1], texColor); 
							}

							// GOURAUD SHADING
							if (render->interp_mode == GZ_COLOR) {

								// interpolate color
								float rf = (A0*colorV0[0] + A1*colorV1[0] + A2*colorV2[0]) / triA;
								float gf = (A0*colorV0[1] + A1*colorV1[1] + A2*colorV2[1]) / triA;
								float bf = (A0*colorV0[2] + A1*colorV1[2] + A2*colorV2[2]) / triA;
								if(render->tex_fun != NULL) {
									rf *= texColor[RED];
									gf *= texColor[GREEN];
									bf *= texColor[BLUE];
								}
								if (rf > 1.0) rf = 1.0;
								if (gf > 1.0) gf = 1.0;
								if (bf > 1.0) bf = 1.0;
								r = (GzIntensity) ctoi(rf);
								g = (GzIntensity) ctoi(gf);
								b = (GzIntensity) ctoi(bf);
								z = interpZ;

							// PHONG SHADING
							} else if (render->interp_mode == GZ_NORMALS) {

								// interpolate Normal of this point
								GzCoord pN;
								pN[X] = (A0*xformNs[0][X] + A1*xformNs[1][X] + A2*xformNs[2][X]) / triA;
								pN[Y] = (A0*xformNs[0][Y] + A1*xformNs[1][Y] + A2*xformNs[2][Y]) / triA;
								pN[Z] = (A0*xformNs[0][Z] + A1*xformNs[1][Z] + A2*xformNs[2][Z]) / triA;
								GzNormalizeVector(pN);

								// for textures!
								if (render->tex_fun != NULL) {
									render->Ka[RED] = render->Kd[RED] = texColor[RED];
									render->Ka[GREEN] = render->Kd[GREEN] = texColor[GREEN];
									render->Ka[BLUE] = render->Kd[BLUE] = texColor[BLUE];
								}

								// calculate color
								GzColor color;
								GzShadingEquation(render, color, pN);
								if (color[0] > 1.0) color[0] = 1.0;
								if (color[1] > 1.0) color[1] = 1.0;
								if (color[2] > 1.0) color[2] = 1.0;

								r = (GzIntensity) ctoi(color[0]);
								g = (GzIntensity) ctoi(color[1]);
								b = (GzIntensity) ctoi(color[2]);
								z = interpZ;

							// FLAT SHADING
							} else {
							//if (render->interp_mode = GZ_FLAT) { GZ_FLAT DOESN'T EXIST
								// flatcolor never gets set here because GZ_RGB_COLOR doesn't get passed in
								r = (GzIntensity ) ctoi((float) render->flatcolor[0]);
								g = (GzIntensity ) ctoi((float)render->flatcolor[1]);
								b = (GzIntensity ) ctoi((float)render->flatcolor[2]);
								z = interpZ;
							}
							// anti aliasing
							if (render->filterTable != NULL) {
							}
							// write to buffer
							GzPutDisplay(render->subsampledDisplays[k], i, j, r, g, b, a, z);
							//GzPutDisplay(render->display, i, j, r, g, b, a, z);
						}
					}

				}
			}
		}
		// Combine displays here
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
		//		GzIntensity r, g, b, a;
		//		GzDepth z = 0;
		//		GzGetDisplay(render->subsampledDisplays[0], i, j, &r, &g, &b, &a, &z);
		//		GzPutDisplay(render->display, i, j, r,g, b, a, z);
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

