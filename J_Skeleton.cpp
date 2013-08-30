/* ---------------------------------------------- 	*
 *	File: J_Skeleton.cpp
 *	--------------------
 *	My personal implementation of nite::Skeleton
 *
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
#include <ctime>

/*--- NiTE ---*/
#include "NiTE.h"

/*--- cpp_json ---*/
#include "cpp_json/json.h"

/*--- My Files ---*/
#include "J_Skeleton.h"

/*--- Namespaces ---*/
using namespace std;




/*########################################################################################################################*/
/*###############################[--- Constructor/Destructors ---]########################################################*/
/*########################################################################################################################*/

/* Function: initialize_joints
 * ---------------------------
 * initializes joints (without a value) 
 */
void J_Skeleton::initialize () {

	/*### Step 1: initialize timestamp, pop, is_valid ###*/
	is_valid		= false;
	timestamp 		= -1;
				
	/*### Step 2: initialize joints ###*/
	for (int i=0;i<JSKEL_NUM_OF_JOINTS;i++) {
		J_Joint * joint = new J_Joint (nite::Point3f (0, 0, 0), nite::Point3f(0, 0, 0), nite::Quaternion (0, 0, 0, 0));
		joints[i] = joint;
	}
}

/* Constructor: 
 * -----------------------------------------------
 * initializes via a map from nite::JoinType to J_Joints
 */
J_Skeleton::J_Skeleton () {

	/*### Step 1: initialize joints ###*/
	initialize ();

}


/* Constructor: nite::Skeleton
 * ---------------------------------
 * initializes via an nite::Skeleton
 */
J_Skeleton::J_Skeleton (nite::Skeleton *skeleton, nite::UserTracker *user_tracker) {

	/*### Step 1: initialize joints ###*/
	initialize ();

	/*### Step 1: set joint positions/orientations ###*/
	for (unsigned int i=0; i<JSKEL_NUM_OF_JOINTS;i++) {
		
		/*--- get position, position_absolute and orientation  ---*/
		nite::SkeletonJoint current_joint = skeleton->getJoint ((nite::JointType) i);
		nite::Point3f position = current_joint.getPosition ();
		nite::Point3f position_absolute = nite::Point3f (0, 0, 0);
		user_tracker->convertJointCoordinatesToDepth (position.x, position.y, position.z, &position_absolute.x, &position_absolute.y);
		nite::Quaternion orientation = current_joint.getOrientation ();

		/*--- set the joint's properties ---*/
		joints[i]->set (position, position_absolute, orientation);

	}

	is_valid = true;
	return;
}


/* Constructor: J_Skeleton *skeleton
 * ---------------------------------
 * initializes via a J_Skeleton (essentially a copy constructor)
 * if the skeleton passed in is null, this skeleton is set as not valid.
 */
J_Skeleton::J_Skeleton (J_Skeleton *skeleton) {

	/*### Step 1: initialize joints ###*/
	initialize ();

	if (skeleton == NULL) {
		is_valid = false;
		return;
	}
	else {

		/*### Step 1: initialize joints ###*/
		initialize ();

		/*### Step 1: set joint positions/orientations ###*/
		for (unsigned int i=0; i<JSKEL_NUM_OF_JOINTS;i++) {
			J_Joint *current_joint = skeleton->getJoint ((nite::JointType) i);
			joints[i]->set (current_joint);
		}
		is_valid = true;
		return;
	}
}
	


J_Skeleton::~J_Skeleton () {

	/*### Step 1: delete all memory allocated to the joints ###*/
	for (int i = 0;i<JSKEL_NUM_OF_JOINTS;i++) {
		delete joints[i];
	}
}




/*########################################################################################################################*/
/*###############################[--- Computing misc. properties ---]#####################################################*/
/*########################################################################################################################*/
/* Function: getBoundingBox
 * ------------------------
 * returns the convex hull (bounding box) of the current skeleton
 */
nite::BoundingBox J_Skeleton::get_bounding_box () {

	nite::Point3f top_left, bottom_right;

	top_left.x 		= 1000000;
	top_left.y 		= 1000000;
	bottom_right.x 	= 0;
	bottom_right.y	= 0;

	for (int i=0;i<JSKEL_NUM_OF_JOINTS;i++) {

		nite::Point3f position_absolute = joints[i]->getPositionAbsolute ();

		if (position_absolute.x < top_left.x) 		top_left.x = position_absolute.x;
		if (position_absolute.y < top_left.y) 		top_left.y = position_absolute.y;
		if (position_absolute.x > bottom_right.x) 	bottom_right.x = position_absolute.x;
		if (position_absolute.y > bottom_right.y) 	bottom_right.y = position_absolute.y;			

	}
	bounding_box =  nite::BoundingBox (top_left, bottom_right);
	return bounding_box;
}


/* Function: get_center_of_mass 
 * ----------------------------
 * computes the center of mass, stores it in the instance variable center_of_mass,
 * then returns it. (this is in relative coordinates, all that we really need)
 */
nite::Point3f J_Skeleton::get_center_of_mass () {

	float x_total = 0;
	float y_total = 0;
	float z_total = 0;
	for (int i=0;i<JSKEL_NUM_OF_JOINTS;i++) {
		nite::Point3f position =  joints[i]->getPosition ();
		x_total += position.x;
		y_total += position.y;
		z_total += position.z;
	}
	float x_avg = x_total / float(JSKEL_NUM_OF_JOINTS);
	float y_avg = y_total / float(JSKEL_NUM_OF_JOINTS);
	float z_avg = z_total / float(JSKEL_NUM_OF_JOINTS);

	center_of_mass = nite::Point3f (x_avg, y_avg, z_avg);
	return center_of_mass;
}





/*########################################################################################################################*/
/*###############################[--- Splunk Dumping ---]################################################################*/
/*########################################################################################################################*/


/* Function: splunk_dump_string
 * ----------------------------
 * returns a string that is a splunk-readable representation
 * of this particular skeleton.
 */
string J_Skeleton::splunk_dump_string () {

	json::object json_skel;
	stringstream dump_stream;

	/*### Step 1: timestamp ###*/
	/*--- Note: for now, default to the time splunk enters it in ---*/
	// struct timeval tv;
	// gettimeofday (&tv, NULL);
	// json::object timestamp;
	// timestamp.insert ("seconds", tv.tv_sec);
	// timestamp.insert ("microseconds", tv.tv_usec);
	// json_skel.insert("timestamp", timestamp);


	/*### Step 2: center of mass ###*/
	get_center_of_mass ();
	json::object json_center_of_mass;
	json_center_of_mass.insert ("x", center_of_mass.x);
	json_center_of_mass.insert ("y", center_of_mass.y);
	json_center_of_mass.insert ("z", center_of_mass.z);
	json_skel.insert ("center_of_mass", json_center_of_mass);


	/*### Step 3: bounding box ###*/
	get_bounding_box ();
	json::object json_bounding_box;
	json::object top_left;
	json::object bottom_right;
	top_left.insert ("x", bounding_box.min.x);
	top_left.insert ("y", bounding_box.min.y);
	bottom_right.insert ("x", bounding_box.max.x);
	bottom_right.insert ("y", bounding_box.max.y);	
	json_bounding_box.insert ("top_left", top_left);
	json_bounding_box.insert ("bottom_right", bottom_right);
	json_skel.insert ("bounding_box", json_bounding_box);

	/*### Step 4: joints ###*/
	json::object json_joints;
	for (int i=0;i<JSKEL_NUM_OF_JOINTS;i++) {

		J_Joint * current_joint = joints[i];
		json::object json_joint;
		json_joint.insert ("x", current_joint->getPosition().x);
		json_joint.insert ("y", current_joint->getPosition().y);		
		json_joint.insert ("z", current_joint->getPosition().z);

		stringstream index;
		index << i;
		json_joints.insert (index.str().c_str(), json_joint); 
	}
	json_skel.insert ("joints", json_joints);

	dump_stream << "[" << json::pretty_print (json_skel) << "]" << endl;
	return dump_stream.str();
}


















/*########################################################################################################################*/
/*###############################[--- Setters/Getters ---]################################################################*/
/*########################################################################################################################*/
/* Setter/Getter: validity
 * -----------------------
 * sets this skeleton as valid or otherwise
 */
void J_Skeleton::setValid (bool value) {
	is_valid = value;
}
bool J_Skeleton::isValid () {
	return is_valid;
}

/* Setter/Getter: get/set index
 * ----------------------------
 * gets/sets the timestamp
 */
 int J_Skeleton::getTimestamp () {
 	return timestamp;
 }
 void J_Skeleton::setTimestamp (int new_timestamp) {
 	timestamp = new_timestamp;
 }	



/* Setter/Getter: get/setJoint
 * ---------------------------
 * gets/sets a particular joint
 */
J_Joint *J_Skeleton::getJoint (nite::JointType joint_type) {
	return joints [(int) joint_type];
}
void J_Skeleton::setJoint (nite::JointType joint_type, nite::Point3f position, nite::Point3f position_absolute, nite::Quaternion orientation) {
	joints [(int) joint_type]->set (position, position_absolute, orientation);
}















