/* ---------------------------------------------- 	*
 *	File: NI_Recorder.h
 *	-------------------
 *	Class to take care of all recording of skeleton
 * 	and device streams
 *
 *	----------------------------------------------	*/
#ifndef _NI_RECORD_H_
#define _NI_RECORD_H_

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
#include "NI_App.h"
#include "J_DeviceDelegate.h"
#include "J_SplunkDelegate.h"

/*--- Constants ---*/
#define MAX_FILENAME_LENGTH 150
#define FRAMES_PER_BEAT 20 			//program will mark a 'beat' every 15 frames


/*--- Namespaces ---*/
using namespace std;


class NI_Recorder: public NI_App {
private:

	/*--- Self ---*/
	static NI_Recorder *self;

	/*--- Device --*/
	J_DeviceDelegate *device_delegate;	//for getting frames from device
	J_SplunkDelegate *splunk_delegate;	//for dumping frames into splunk

	/*--- Keyboard/Display for OpenGL ---*/
	void InitOpenGLHooks 		();
	static void glut_keyboard 	(unsigned char key, int x, int y);
	static void glut_display 	();
	static void glut_idle 		();
	void onkey 					(unsigned char key, int x, int y);
	void display 				();


public:

	/*--- Constructor/Destructor ---*/
	NI_Recorder 	(string splunk_filepath, int argc, char** argv);
	~NI_Recorder 	();


	/*--- Getting user state, etc ---*/
	void 			updateUserState	(const nite::UserData& user, uint64_t ts);
	openni::Status	Run				();


};



#endif //_NI_RECORD_H_