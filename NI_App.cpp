/* ---------------------------------------------- 	*
 *	File: NI_app.cpp
 *	--------------
 *	Class to take care of all interface to device
 *	and body.
 *
 *	----------------------------------------------	*/
#if (defined _WIN32)
#define PRIu64 "llu"
#else
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#endif

/*--- Standard ---*/
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <string>
#include <sstream>

/*--- NiTE ---*/
#include "NiTE.h"

/*--- OPENGL ---*/
#if (ONI_PLATFORM == ONI_PLATFORM_MACOSX)
        #include <GLUT/glut.h>
#else
        #include <GL/glut.h>
#endif

/*--- My Files ---*/
#include "NI_App.h"

/*--- Namespaces ---*/
using namespace std;

/*--- Initialize Static Elements ---*/
NI_App* NI_App::self = NULL;


/* Function: destructor
 * --------------------
 * just need a definition (empty) in order to get everything compiling
 */
NI_App::~NI_App () {}


/* Function: initialize_APIs
 * -------------------------
 * initalizes OpenNI, NiTE and OpenGL
 */
void NI_App::initialize_APIs (int argc, char ** argv) {

	/*### Step 1: initialize OpenNI ###*/
	print_status ("Initialize_APIs", "Opening OpenNI");
	openni::Status rc = openni::OpenNI::initialize();
	if (rc != openni::STATUS_OK) {
		print_error("Failed to initialize OpenNI", openni::OpenNI::getExtendedError());
	}

	/*### Step 4: initialize NiTE ###*/
	print_status ("Initialize_APIs", "Opening NiTE");
	nite::NiTE::initialize();
	

	/*### Step 5: initialize OpenGL ###*/
	print_status ("Initialize_APIs (General)", "Opening OpenGL");
	InitOpenGL(argc, argv);
}



/* Function: InitOpenGL 
 * --------------------
 * initializes OpenGL
 */
openni::Status NI_App::InitOpenGL (int argc, char **argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	glutCreateWindow ("Display");
	
	/*--- uncomment for full screen ---*/
	// 	glutFullScreen();

	glutSetCursor(GLUT_CURSOR_NONE);
	InitOpenGLHooks();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	return openni::STATUS_OK;
}




/*########################################################################################################################*/
/*#####################[--- OPENGL INTERFACE FUNCTIONS ---]###############################################################*/
/*########################################################################################################################*/
/* Functions: glut(Idle|Display|Keyboard)
 * --------------------------------------
 * these functions serve as the interface to OpenGL;
 * they are static (?)
 */
void NI_App::glut_idle() {
	
	glutPostRedisplay();
}
void NI_App::glut_display() {

	NI_App::self->display();
}
void NI_App::glut_keyboard (unsigned char key, int x, int y) {
	
	NI_App::self->onkey(key, x, y);
}














