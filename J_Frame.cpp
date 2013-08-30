/* ---------------------------------------------- 	*
 *	File: J_Frame.cpp
 *	-----------------
 *	Class that contains everything for dealing with 
 * 	a single "frame" from a video, including a J_Skeleton
 *	and a VideoFrameRef
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

/*--- My Files ---*/
#include "Utilities.h"
#include "J_Skeleton.h"
#include "J_Frame.h"

 /*--- Namespaces ---*/
 using namespace std;
 using namespace openni;


/*########################################################################################################################*/
/*###############################[--- Constructor/Initialization/Destructor ---] #########################################*/
/*########################################################################################################################*/
/* Function : Default Constructor 
 * ---------------------
 * initializes the skeleton/frame_ref to null and is_valid to false.
 */
J_Frame::J_Frame () { 
	
	is_valid = false;
	depth_frame = NULL;
	color_frame = NULL;
}

/* Function: Constructor
 * ---------------------
 * given a pointer to a filled-out skeleton and a filled-out frame_ref,
 * this function will make a new J_Frame from it.
 */
J_Frame::J_Frame 	(	std::vector<J_Skeleton *> new_skeletons, 
						J_VideoFrameRef *new_depth_frame, 
						J_VideoFrameRef *new_color_frame) {

	skeletons 		= new_skeletons;
	depth_frame 	= new_depth_frame;
	color_frame 	= new_color_frame;
	is_valid 		= true;
}
J_Frame::~J_Frame () {

	for (int i=0;i<skeletons.size();i++) {
		if (skeletons[i] != NULL) delete skeletons[i];
	}

	if (depth_frame != NULL) delete depth_frame;
	if (color_frame != NULL) delete color_frame;
}


/* Function: isValid
 * -----------------
 * returns true if this J_Frame contains actual data
 */
bool J_Frame::isValid () {
	return is_valid;
}


/*########################################################################################################################*/
/*###############################[--- Getters ---] #######################################################################*/
/*########################################################################################################################*/

/* Function: get_skeletons
 * -----------------------
 * pass in a vector of J_Skeleton pointers by reference and this function
 * will fill it.
 */
void J_Frame::get_skeletons (std::vector <J_Skeleton*> *fill_skeletons) {
	for (int i=0;i<skeletons.size();i++) {
		fill_skeletons->push_back (skeletons[i]);
	}
	return;
}

J_VideoFrameRef * 	J_Frame::get_depth_frame 	() {
	return depth_frame;
}

J_VideoFrameRef * 	J_Frame::get_color_frame	() {
	return color_frame;
}



/*########################################################################################################################*/
/*###############################[--- Debugging ---] #######################################################################*/
/*########################################################################################################################*/
/* Function: print_data
 * --------------------
 * prints out data on the currently observed skeletons
 */
void J_Frame::print_data () {

	cout << "#####[ --- FRAME: --- ]#####" << endl;

	/*### Step 1: loop through all the joints and print out their locations ###*/
	if (!skeletons.size() > 0) {
		cout << ">> NO USERS DETECTED <<" << endl;
	}
	else {
		for (int j=0;j<skeletons.size();j++) {
			J_Skeleton *skeleton = skeletons[j];
			for (int i=0;i<JSKEL_NUM_OF_JOINTS;i++) {

				nite::Point3f position = skeleton->getJoint((nite::JointType) i)->getPosition();
				cout << i << ": " << position.x << ", " << position.y << ", " << position.z << endl;
			}
		}
	}

}










