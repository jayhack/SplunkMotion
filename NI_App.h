/* ---------------------------------------------- 	*
 *	File: NI_app.h
 *	--------------
 *	an abstract class that both NI_Recorder and NI_Player
 *	inherit from. both of them contain:
 *	- J_StorageDelegate
 *	- J_Drawer
 *	- 
 *	----------------------------------------------	*/

#ifndef _NI_APP_H_
#define _NI_APP_H_

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

/*--- My Files ---*/
#include "Utilities.h"
#include "J_Drawer.h"


/*--- Namespaces ---*/
using namespace std;



/*########################################################################################################################*/
/*###############################[--- NI_app CLASS DEF ---]#######################################################################*/
/*########################################################################################################################*/

class NI_App {
protected:

	/*--- Name/Self ---*/
	static NI_App * 	self;			/*static reference to the active instance*/
	char				app_name[150];

	/*--- Drawing --*/
	J_Drawer drawer;

	/*--- Initialize APIs ---*/
	void initialize_APIs (int argc, char **argv);

	/*--- OpenGL Inialization ---*/
	void glut_idle ();
	void glut_display ();
	void glut_keyboard (unsigned char key, int x, int y);
	virtual	void 	InitOpenGLHooks	() = 0;	
	openni::Status 	InitOpenGL 		(int argc, char **argv);


	/*--- Keyboard/Display for OpenGL ---*/
	virtual void onkey 		(unsigned char key, int x, int y) = 0;
	virtual void display 	() = 0;



public:



	/*--- Constructor - won't be inherited ---*/
	virtual ~NI_App () = 0;


};


#endif // _NI_APP_H_
