/*******************************************************************************
*                                                                              *
*   PrimeSense NiTE 2.0 - User Viewer Sample                                   *
*   Copyright (C) 2012 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/

/*--- Standard ---*/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*--- My Files ---*/
#include "Utilities.h"
#include "NI_Recorder.h"


/*--- Namespaces ---*/
using namespace std;

int main(int argc, char** argv)
{



	string splunk_filename = "/Users/jhack/Programming/NI/SplunkMotion/splunk_input/data.txt";

	print_status ("MAIN", "Beginning Initializion of ni_recorder");
	NI_Recorder ni_recorder (splunk_filename, argc, argv);
	print_status ("MAIN", "Finished initializion of ni_recorder");
	ni_recorder.Run ();

	return 0;
}







