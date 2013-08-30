/* ---------------------------------------------- 	*
 *	File: J_SplunkDelegate.h
 *	--------------------------
 *	Class to take care of all interface with splunk,
 * 	namely dumping skeletons into the correct file.
 *
 *	----------------------------------------------	*/
#ifndef _J_SPLUNKDEL_H_
#define _J_SPLUNKDEL_H_
/*--- Standard ---*/
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>

/*--- My Files ---*/
#include "J_Skeleton.h"

/*--- Namespaces ---*/
using namespace std;
using namespace openni;

class J_SplunkDelegate {
private:

	string splunk_filename;
	ofstream splunk_outfile;

public:

	/*--- constructor/destructor ---*/
	J_SplunkDelegate 	(string new_filename);
	~J_SplunkDelegate 	();

	/*--- dumping to splunk ---*/
	void dump (std::vector<J_Skeleton*> skeleton);



};
#endif //_J_SPLUNKDEL_H_