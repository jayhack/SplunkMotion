/* ---------------------------------------------- 	*
 *	File: J_SplunkDelegate.cpp
 *	--------------------------
 *	Class to take care of all interface with splunk,
 * 	namely dumping skeletons into the correct file.
 *
 *	----------------------------------------------	*/
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
#include "Utilities.h"
#include "J_SplunkDelegate.h"
#include "J_Skeleton.h"

/*--- Namespaces ---*/
using namespace std;
using namespace openni;

/* Function: constructor
 * ---------------------
 * opens the outfile
 */
J_SplunkDelegate::J_SplunkDelegate (string new_filename) {

	/*### filename ###*/
	splunk_filename = new_filename;


	/*### outfile - open it for appending ###*/
	splunk_outfile.open(splunk_filename.c_str(), std::ofstream::app);
	if (!splunk_outfile.is_open()) {
		print_error ("Splunk Delegate", "Could not open splunk dump file");
	}


}
/* Function: destructor
 * --------------------
 * closes the outfile
 */
J_SplunkDelegate::~J_SplunkDelegate () {

	/*### close the outfile ###*/
	splunk_outfile.close ();
}



/* Function: dump
 * --------------
 * dumps a skeleton into splunk. no need for a timestamp - splunk will mark that for us.
 */
void J_SplunkDelegate::dump (std::vector <J_Skeleton*> skeletons) {

	for (int i=0;i<skeletons.size();i++) {
		splunk_outfile << skeletons[i]->splunk_dump_string ();
	}

}