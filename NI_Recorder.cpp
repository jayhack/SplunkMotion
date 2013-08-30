/* ---------------------------------------------- 	*
 *	File: NI_Recorder.h
 *	-------------------
 *	Class to take care of all recording of skeleton
 * 	and device streams
 *
 *	----------------------------------------------	*/
/*--- Standard ---*/
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>

/*--- NiTE ---*/
#include "NiTE.h"

/*--- OpenGL ---*/
#include <GLUT/glut.h>

/*--- My Files ---*/
#include "Utilities.h"
#include "J_Skeleton.h"
#include "J_Frame.h"
#include "NI_Recorder.h"
#include "J_SplunkDelegate.h"

/*--- Namespaces ---*/
using namespace std;

/*--- Initialize Static Elements ---*/
NI_Recorder* NI_Recorder::self = NULL;



/*########################################################################################################################*/
/*###############################[--- Constructor/Initialization/Destructor ---] #########################################*/
/*########################################################################################################################*/
/* Function: Constructor
 * ---------------------
 * initializes all forms of recording, though does not start writing to output
 */
NI_Recorder::NI_Recorder (string splunk_filepath, int argc, char** argv) {
	self = this;

	/*### Step 1: initialize the APIs (OpenNI, NiTE, OpenGL) ###*/
	print_status ("Initialization (Recorder)", "Initializing APIs");
	initialize_APIs (argc, argv);

	/*### Step 2: initialize the device delegate ###*/
	print_status ("Initialization (Recorder)", "Creating Device Delegate");
	device_delegate = new J_DeviceDelegate ();

	/*### Step 3: initialize the splunk delegate ###*/
	print_status ("Initialization (Recorder)", "Creating Splunk Delegate");
	splunk_delegate = new J_SplunkDelegate (splunk_filepath);

}

/* Function: Destructor
 * --------------------
 * stops recording and closes all files
 */
NI_Recorder::~NI_Recorder () {
		
	/*### Step 1: free all memory allocatd for the device delegate ###*/
	print_status ("Finalization (Recorder)", "Deleting device delegate");
	delete device_delegate;

	/*### Step 2: free all memory allocatd for the device delegate ###*/
	print_status ("Finalization (Recorder)", "Deleting splunk delegate");
	delete splunk_delegate;
}








/*########################################################################################################################*/
/*###############################[--- OpenGL Interface ---] ##############################################################*/
/*########################################################################################################################*/
/* Function: InitOpenGLHooks
 * -------------------------
 * sets up OpenGL hooks 
 */
void NI_Recorder::InitOpenGLHooks () {
	glutKeyboardFunc 	(glut_keyboard);
	glutDisplayFunc		(glut_display);
	glutIdleFunc		(glut_idle);
}

void NI_Recorder::glut_idle () {
	
	glutPostRedisplay();
}
void NI_Recorder::glut_display () {

	NI_Recorder::self->display();
}
void NI_Recorder::glut_keyboard (unsigned char key, int x, int y) {
	
	NI_Recorder::self->onkey(key, x, y);
}

/* Function: onkey
 * ---------------
 * callback function for handling keyboard events.
 * here, it mainly controls wether recording is active or not, 
 * also quitting the application.
 */
void NI_Recorder::onkey (unsigned char key, int x, int y) {
	
	switch (key)
	{
		/*### ESC: exits application ###*/
		case 27:	
			// Finalize();
			exit (1);
	}
}

void NI_Recorder::display () {

	/*### Step 1: get a list of all skeletons ###*/
	J_Frame * frame = device_delegate->get_frame ();

	/*### Step 2: draw it to the screen ###*/
	// print_status ("Display", "Drawing frame");
	drawer.draw (frame);

	/*### Step 3: dump it to splunk ###*/
	// print_status ("Display", "Dumping to splunk");
	std::vector <J_Skeleton *> skeletons;
	frame->get_skeletons(&skeletons);
	splunk_delegate->dump (skeletons);

	/*### Step 4: free all memory dedicated to the frame, increment frame number ###*/
	delete frame;
}

/* Function: Run
 * -------------
 * engages in the main loop, does not return.
 */
openni::Status NI_Recorder::Run() {	
	glutMainLoop();
	return openni::STATUS_OK;
}










