#include "NiTE.h"
#include "J_Joint.h"
#include "J_Skeleton.h"
#include "J_Drawer.h"

#include <GLUT/glut.h>



/*########################################################################################################################*/
/*###############################[ --- Constructor/Initialization --- ]###################################################*/
/*########################################################################################################################*/
/* Function: Constructor
 * ---------------------
 * Initializes internal parameters
 */
J_Drawer::J_Drawer () {

	/*--- initialize draw mode ---*/
	draw_mode = -1;

	/*--- initialize colors ---*/
	Colors[0][0] = 1;
	Colors[0][1] = 0;
	Colors[0][2] = 0;

	Colors[1][0] = 0;
	Colors[1][1] = 1;
	Colors[1][2] = 0;

	Colors[2][0] = 0;
	Colors[2][1] = 0;
	Colors[2][2] = 1;

	Colors[3][0] = 1;
	Colors[3][1] = 1;
	Colors[3][2] = 1;

	colorCount = 3;

	g_visibleUsers[0] =  false;
	g_skeletonStates[0] = nite::SKELETON_NONE;

	g_userStatusLabels[0][0] = 0;
	g_generalMessage[0] = 0;

	g_drawSkeleton = true;
	g_drawCenterOfMass = false;
	g_drawStatusLabel = true;
	g_drawBoundingBox = false;
	g_drawBackground = true;
	g_drawDepth = true;
	g_drawFrameId = false;

	g_nXRes = 0; 
	g_nYRes = 0;

	g_poseTimeoutToExit = 2000;

	g_generalMessage[0] = 0;

	pixel_texture_map = NULL;
}








/*########################################################################################################################*/
/*###############################[ --- Depth Frame Utilities --- ]########################################################*/
/*########################################################################################################################*/
void J_Drawer::calculateHistogram(float* pHistogram, int histogramSize, J_VideoFrameRef* depthFrame) {

	const openni::DepthPixel* pDepth = (const openni::DepthPixel*)depthFrame->getData();
	int width = depthFrame->getWidth();
	int height = depthFrame->getHeight();
	// Calculate the accumulative histogram (the yellow display...)
	memset(pHistogram, 0, histogramSize*sizeof(float));
	int restOfRow = depthFrame->getStrideInBytes() / sizeof(openni::DepthPixel) - width;

	unsigned int nNumberOfPoints = 0;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x, ++pDepth)
		{
			if (*pDepth != 0)
			{
				pHistogram[*pDepth]++;
				nNumberOfPoints++;
			}
		}
		pDepth += restOfRow;
	}
	for (int nIndex=1; nIndex<histogramSize; nIndex++)
	{
		pHistogram[nIndex] += pHistogram[nIndex-1];
	}
	if (nNumberOfPoints)
	{
		for (int nIndex=1; nIndex<histogramSize; nIndex++)
		{
			pHistogram[nIndex] = (256 * (1.0f - (pHistogram[nIndex] / nNumberOfPoints)));
		}
	}
}























/*########################################################################################################################*/
/*###############################[--- J_Skeleton drawing functions ---]###################################################*/
/*########################################################################################################################*/

/* Function: draw_J_Limb
 * ---------------------
 * draws a limb, specified by two joints (and a color), to the screen
 */
void J_Drawer::draw_J_Limb (J_Joint* joint1, J_Joint* joint2, int color) {

	/*### Step 1: get absolute coordinates ###*/
	float coordinates[6] = {0};
	nite::Point3f joint1_position_absolute = joint1->getPositionAbsolute ();
	nite::Point3f joint2_position_absolute = joint2->getPositionAbsolute ();	
	coordinates[0] = joint1_position_absolute.x;
	coordinates[1] = joint1_position_absolute.y;
	coordinates[3] = joint2_position_absolute.x;
	coordinates[4] = joint2_position_absolute.y;

	/*--- (coordinates need scaling) ---*/
	coordinates[0] *= GL_WIN_SIZE_X/(float)g_nXRes;
	coordinates[1] *= GL_WIN_SIZE_Y/(float)g_nYRes;
	coordinates[3] *= GL_WIN_SIZE_X/(float)g_nXRes;
	coordinates[4] *= GL_WIN_SIZE_Y/(float)g_nYRes;

	/*### Line color ###*/
	glColor3f(0.0f, 0.0f, 0.7f);
	glPointSize(2);
	glVertexPointer(3, GL_FLOAT, 0, coordinates);
	glDrawArrays(GL_LINES, 0, 2);
	glPointSize(10);

	/*### half of the vertexes...? ###*/
	glColor3f (1.0f, 0.0f, 0.0f);
	glVertexPointer(3, GL_FLOAT, 0, coordinates);
	glDrawArrays(GL_POINTS, 0, 1);

	/*### other half of vertexes ###*/
	glColor3f (1.0f, 0.0f, 0.0f);
	glVertexPointer(3, GL_FLOAT, 0, coordinates+3);
	glDrawArrays(GL_POINTS, 0, 1);
}


/* Function: draw_J_Skeleton
 * ---------------------------------
 * Draws a J_Skeleton to the screen
 */
void J_Drawer::draw_J_Skeleton (J_Skeleton *skeleton) {

	glLineWidth(3.0f);

	/*--- Head + Neck ---*/
	draw_J_Limb(skeleton->getJoint(nite::JOINT_HEAD), skeleton->getJoint(nite::JOINT_NECK), 0);

	/*--- Left Shoulder/Elbow/Hand---*/
	draw_J_Limb(skeleton->getJoint(nite::JOINT_LEFT_SHOULDER), skeleton->getJoint(nite::JOINT_LEFT_ELBOW), 0);
	draw_J_Limb(skeleton->getJoint(nite::JOINT_LEFT_ELBOW), skeleton->getJoint(nite::JOINT_LEFT_HAND), 0);

	/*--- Right Shoulder/Elbow/Hand ---*/
	draw_J_Limb(skeleton->getJoint(nite::JOINT_RIGHT_SHOULDER), skeleton->getJoint(nite::JOINT_RIGHT_ELBOW), 0);
	draw_J_Limb(skeleton->getJoint(nite::JOINT_RIGHT_ELBOW), skeleton->getJoint(nite::JOINT_RIGHT_HAND), 0);

	/*--- Left/Right Shoulders ---*/
	draw_J_Limb(skeleton->getJoint(nite::JOINT_LEFT_SHOULDER), skeleton->getJoint(nite::JOINT_RIGHT_SHOULDER), 0);

	/*--- Shoulder - torso ---*/
	draw_J_Limb(skeleton->getJoint(nite::JOINT_LEFT_SHOULDER), skeleton->getJoint(nite::JOINT_TORSO), 0);
	draw_J_Limb(skeleton->getJoint(nite::JOINT_RIGHT_SHOULDER), skeleton->getJoint(nite::JOINT_TORSO), 0);

	/*--- Torso/Hip ---*/
	draw_J_Limb(skeleton->getJoint(nite::JOINT_TORSO), skeleton->getJoint(nite::JOINT_LEFT_HIP), 0);
	draw_J_Limb(skeleton->getJoint(nite::JOINT_TORSO), skeleton->getJoint(nite::JOINT_RIGHT_HIP), 0);

	/*--- Hips ---*/
	draw_J_Limb(skeleton->getJoint(nite::JOINT_LEFT_HIP), skeleton->getJoint(nite::JOINT_RIGHT_HIP), 0);

	/*--- Left leg ---*/
	draw_J_Limb(skeleton->getJoint(nite::JOINT_LEFT_HIP), skeleton->getJoint(nite::JOINT_LEFT_KNEE), 0);
	draw_J_Limb(skeleton->getJoint(nite::JOINT_LEFT_KNEE), skeleton->getJoint(nite::JOINT_LEFT_FOOT), 0);

	/*--- Right leg ---*/
	draw_J_Limb(skeleton->getJoint(nite::JOINT_RIGHT_HIP), skeleton->getJoint(nite::JOINT_RIGHT_KNEE), 0);
	draw_J_Limb(skeleton->getJoint(nite::JOINT_RIGHT_KNEE), skeleton->getJoint(nite::JOINT_RIGHT_FOOT), 0);

}




















/*########################################################################################################################*/
/*###############################[--- Frame Drawing ---]##################################################################*/
/*########################################################################################################################*/

/* Function: draw_depth_frame
 * --------------------------
 * given the depth frame, this will draw it in pixel_texture_map
 */
void J_Drawer::draw_depth (J_VideoFrameRef *depth_frame) {



	/*### Step 2: calculate depth_histogram ###*/
	calculateHistogram(depth_histogram, MAX_DEPTH, depth_frame);



	/*### Step 3: if the depth frame is valid, draw it in pixel_texture_map ###*/
	float factor[3] = {1, 1, 1};
	if (depth_frame->isValid ()) {

		// const nite::UserId* pLabels = userLabels.getPixels();

		const openni::DepthPixel* pDepthRow = (const openni::DepthPixel*)depth_frame->getData();
		openni::RGB888Pixel* pTexRow = pixel_texture_map + depth_frame->getCropOriginY() * texture_map_x;
		int rowSize = depth_frame->getStrideInBytes() / sizeof(openni::DepthPixel);

		for (int y = 0; y < depth_frame->getHeight(); ++y)
		{
			const openni::DepthPixel* pDepth = pDepthRow;
			openni::RGB888Pixel* pTex = pTexRow + depth_frame->getCropOriginX();

			// for (int x = 0; x < depth_frame->getWidth(); ++x, ++pDepth, ++pTex, ++pLabels)
			for (int x = 0; x < depth_frame->getWidth(); ++x, ++pDepth, ++pTex)
			{
				if (*pDepth != 0)
				{
					// if (*pLabels == 0)
					// {
						if (!g_drawBackground)
						{
							factor[0] = factor[1] = factor[2] = 0;

						}
						else
						{
							factor[0] = Colors[colorCount][0];
							factor[1] = Colors[colorCount][1];
							factor[2] = Colors[colorCount][2];
						}
					// }
					// else
					// {
						// factor[0] = drawer.Colors[*pLabels % drawer.colorCount][0];
						// factor[1] = drawer.Colors[*pLabels % drawer.colorCount][1];
						// factor[2] = drawer.Colors[*pLabels % drawer.colorCount][2];
					// }

					int nHistValue = depth_histogram[*pDepth];
					pTex->r = nHistValue*factor[0];
					pTex->g = nHistValue*factor[1];
					pTex->b = nHistValue*factor[2];

					factor[0] = factor[1] = factor[2] = 1;
				}
			}

			pDepthRow += rowSize;
			pTexRow += texture_map_x;
		}
	}

	return;
}



/* Function: draw
 * -----------------------
 * draws the skeleton and depth map onscreen. 
 */
void J_Drawer::draw (J_Frame *frame) {

	/*### Step 0: extract important info - skeletons and depth frame ###*/
	J_VideoFrameRef *depth_frame = frame->get_depth_frame ();
	std::vector <J_Skeleton*> skeletons;
	frame->get_skeletons (&skeletons);

	/*### Step 1: initialize pixel_texture_map if necessary ###*/
	if (pixel_texture_map == NULL) {
		texture_map_x = MIN_CHUNKS_SIZE(depth_frame->getResolutionX(), TEXTURE_SIZE);
		texture_map_y = MIN_CHUNKS_SIZE(depth_frame->getResolutionY(), TEXTURE_SIZE);
		pixel_texture_map = new openni::RGB888Pixel[texture_map_x * texture_map_y];
	}

	/*### Step 2: OpenGL Stuff... not sure what this does --- ###*/
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, GL_WIN_SIZE_X, GL_WIN_SIZE_Y, 0, -1.0, 1.0);


	/*### Step 3: clear pixel_texture_map to zero ###*/
	int data_size = texture_map_y*texture_map_x*sizeof(openni::RGB888Pixel);
	memset(pixel_texture_map, 0, data_size);


	/*### Step 4: draw the color/depth map ###*/
	draw_depth (depth_frame);
	g_nXRes = depth_frame->getResolutionX();
	g_nYRes = depth_frame->getResolutionY();


	/*### Step 5: get ready to display it ###*/
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_map_x, texture_map_y, 0, GL_RGB, GL_UNSIGNED_BYTE, pixel_texture_map);

	// Display the OpenGL texture map
	glColor4f(1,1,1,1);

	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	// upper left
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);
	// upper right
	glTexCoord2f((float)g_nXRes/(float)texture_map_x, 0);
	glVertex2f(GL_WIN_SIZE_X, 0);
	// bottom right
	glTexCoord2f((float)g_nXRes/(float)texture_map_x, (float)g_nYRes/(float)texture_map_y);
	glVertex2f(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	// bottom left
	glTexCoord2f(0, (float)g_nYRes/(float)texture_map_y);
	glVertex2f(0, GL_WIN_SIZE_Y);

	glEnd();
	glDisable(GL_TEXTURE_2D);
	

	/*### Step 6: Draw the skeletons ###*/
	for (int i=0;i<skeletons.size();i++) {
		J_Skeleton *current_skeleton = skeletons[i];
		if (current_skeleton != NULL) {
			draw_J_Skeleton (current_skeleton);
		}
	}



	/*### Swap buffers (display) ###*/
	glutSwapBuffers ();
}











