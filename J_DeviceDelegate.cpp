/* ---------------------------------------------- 	*
 *	File: J_DeviceDelegate.cpp
 *	--------------------------
 *	Class to take care of all interface with the device;
 *	you can poll it for a new J_Frame at any point with read_J_Frame
 *
 *	----------------------------------------------	*/
/*--- Standard ---*/
#include <stdlib.h>
#include <stdio.h>

/*--- NiTE ---*/
#include "NiTE.h"


/*--- My Files ---*/
#include "J_Frame.h"
#include "J_DeviceDelegate.h"



/*########################################################################################################################*/
/*###############################[--- Constructor/Initialization/Destructor ---] #########################################*/
/*########################################################################################################################*/
 /* Function: constructor
  * ---------------------
  * sets up the device and the user tracker
  */
J_DeviceDelegate::J_DeviceDelegate () {

	openni::Status rc;

 	/*### Step 1: allocate and open the device ###*/
	device = new openni::Device ();
	rc = device->open(openni::ANY_DEVICE);


	/*### Step 2: ensure that it was opened successfully ###*/
	if ( (rc != openni::STATUS_OK) || (!device->isValid ())) {
		print_error ("Failed to open device", openni::OpenNI::getExtendedError());
	}

	/*### Step 3: allocate and initialize the user tracker, make sure it was opened correctly ###*/
	user_tracker = new nite::UserTracker ();
	if (user_tracker->create(device) != nite::STATUS_OK) {
		print_error ("Could not create the user tracker", "Not sure what to do here...");
	}
}


/* Function: destructor
 * --------------------
 * frees memory allocated for the user tracker and device
 */
J_DeviceDelegate::~J_DeviceDelegate () {
	delete device;
	delete user_tracker;
}



/*########################################################################################################################*/
/*###############################[--- Reading Frames ---] ################################################################*/
/*########################################################################################################################*/
/* Function: readFrame
 * -------------------
 * reads in a frame and returns a vector of skeletons from it.
 * Note: the user is responsible for deleting all of the skeletons in the returned vector!
 */
J_Frame * J_DeviceDelegate::get_frame () {

	/*--- temporary objects we will use ---*/
	nite::UserTrackerFrameRef userTrackerFrame;

	/*### Step 1: get a userTrackerFrame from user_tracker, depth frame ###*/
	nite::Status rc = user_tracker->readFrame(&userTrackerFrame);
	if (rc != nite::STATUS_OK) print_error ("Display", "Could not get a next frame from user_tracker");

	/*### Step 2: get the UserData objects out of it ###*/
	const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();

	std::vector <J_Skeleton*> skeletons;

	for (int i=0;i<users.getSize(); i++) {

		const nite::UserData & user = users[i];
		if (user.isNew ()) {
			user_tracker->startSkeletonTracking(user.getId());
			user_tracker->startPoseDetection(user.getId(), nite::POSE_CROSSED_HANDS);
		}
		else if (!user.isLost()) {
			if (users[0].getSkeleton().getState() == nite::SKELETON_TRACKED) {

					nite::Skeleton nite_skeleton = users[i].getSkeleton ();
					J_Skeleton *skeleton = new J_Skeleton (&nite_skeleton, user_tracker);
					skeletons.push_back (skeleton);
			}
		}
	}

	/*### Step 3: assemble and return the frame ###*/
	openni::VideoFrameRef temp_frame = userTrackerFrame.getDepthFrame ();
	J_VideoFrameRef *depth_frame = new J_VideoFrameRef (&temp_frame);
	J_Frame *frame = new J_Frame (skeletons, depth_frame, NULL);
	return frame;
}






