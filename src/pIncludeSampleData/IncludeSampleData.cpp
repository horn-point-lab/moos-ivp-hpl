/************************************************************/
/*    NAME: cmoran                                               */
/*    ORGN: UCSB Coastal Oceanography and Autonomous Systems Lab */
/*    FILE: IncludeSampleData.cpp                                */
/*    DATE: 13 February 2019                                     */
/************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <new>
#include <algorithm>
#include <typeinfo> // for debugging
#include <vector>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <cmath>
#include <random>

#include "MBUtils.h"
#include "IncludeSampleData.h"

using namespace std;

//---------------------------------------------------------
// Constructor

IncludeSampleData::IncludeSampleData()
{
    m_nav_x = 0.0;
    m_nav_y = 0.0;
    m_nav_heading = 0.0;

    m_iterations = 0;
		m_input_filename = "csv_image_import.csv";
		m_output_filename = "output.csv";
		m_colCount = 0;
		m_rowCount = 0;
		m_mode = "";

}

//---------------------------------------------------------
// Procedure: OnNewMail

bool IncludeSampleData::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
  	CMOOSMsg &msg = *p;
    //m_iterator++;
    string key   = msg.GetKey();
    double dval  = msg.GetDouble();
		string sval  = msg.GetString();

         if(key=="NAV_X") {
             m_nav_x = dval;
             //cout << "NAV_X = " << m_nav_x << endl;
         }

         if(key=="NAV_Y") {
             m_nav_y = dval;
						 //cout << "NAV_Y = " << m_nav_y << endl;
         }

         if(key=="NAV_HEADING") {
           	m_nav_heading = dval;
  					//cout << "NAV_HEADING = " << m_nav_heading << endl;
         }

				 if(key=="MODE") {
						m_mode = sval;
				 }

     }
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool IncludeSampleData::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);

   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void IncludeSampleData::RegisterVariables()
{
  if(m_nav_x_received != "")
       m_Comms.Register(m_nav_x_received, 0);
  if(m_nav_y_received != "")
       m_Comms.Register(m_nav_y_received, 0);
  if(m_nav_heading_received != "")
       m_Comms.Register(m_nav_heading_received, 0);
	if(m_mode_received != "")
	 		 m_Comms.Register(m_mode_received, 0);
}


//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool IncludeSampleData::Iterate()
{
	cout << "m_mode = " << m_mode << endl;
	ofstream output_file_position ("position_output.csv",ios::app);
	int max_index = 82; // This needs to go here into order to be available during lifetimes
										  // Estimated this value, since we don't want it to ever be unassigned
											// TO_DO: Make this more resilient
	int max_index_padded = 82;
	// TO_DO: Feels like this could be the place for a switch statement or other FSM pattern

	if (m_mode == "ACTIVE:SURVEYING:LINE_FOLLOWING") {
			// Open the input and output files, output file such that values append
			ifstream input_file (m_input_filename);
			ofstream output_file (m_output_filename,ios::app);
			ofstream output_file_padded ("padded_output.csv",ios::app);
			ofstream output_file_maximums ("maximums_output.csv",ios::app);

			// Opens array
			// TO_DO: Is this actually necessary? Or should be just go straight from vect(i), declaring here instead?
			// TO_DO: Might be adding unnecessary computation by doing this conversion ^. Check back after ping analysis methodology is finalized
			int arr[m_colCount * m_rowCount] = {};
			std::vector<int> vect;
			if (input_file.is_open() && output_file.is_open() && m_iterations < m_rowCount) {

				string line;
				char delim = ',';
				string delimiter = ",";

				unsigned int row_num = 0;
				unsigned int col_num = 0;
				while (std::getline(input_file, line))
				{
					if (row_num == m_iterations) {
						//cout << line << endl;
						std::stringstream ss(line);
						//std::vector<int> vect;

						int i;
						while (ss >> i)
						{
								vect.push_back(i);
								if (ss.peek() == ',') {
										ss.ignore();
								}
						}

						for (i=0; i< vect.size(); i++) {
								//arr[col_num + row_num*m_colCount] = vect.at(i); // If was maintaining entire array instead of one line at a time
								arr[col_num] = vect.at(i);
								col_num++;
						}
					}
					// TO_DO: Might be unnecessarily iterating through the entire file on each run-through, possible to simplify?
					row_num++;
					col_num = 0;
				}

				// NOTE: Here's where any operations on the line array should occur; could end up publishing resulting value
				// Finds the maximum value of the ping for standard arr[]
				int max = arr[0]; // First assignment of the max value and index
				max_index = 0;
				int j = 0;
				int bottom_edge = 150; // This is a value at which the bottom return of the acoustic image appears
				for (j = 0; j < bottom_edge; j++) {
					if (arr[j] > max) {
						max = arr[j];
						max_index = j;
					}
				}

				// Writes maximum values to file
				output_file_maximums << max_index << "," << max << endl;

				// Intermediate step of padding output and fluctuating arr[]'s locations within it
				// Not actually going to perform operations on arr[]; prefer to leave it alone, at least while we're still in simulation

				double pad_percentage = 0.2;
			  int offset = round(pad_percentage * m_colCount / 2);
			  int padded_size = m_colCount + (offset * 2);
			  int padded_arr[padded_size] = {};

				// Generates oscillation of using a random number generator
				int bounds = 10; // sets the max and min bounds
				random_device rd;     // used to initialise (seed) engine
				mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
				uniform_int_distribution<int> uni(-bounds,bounds); // guaranteed unbiased; uni(min,max)
				int wiggle = uni(rng);

				// Needs to make sure the the wiggle bounding stays inside the maximum array size
				if (bounds < offset) {
				  for (int k = 0; k < padded_size; k++) {
				    if (k >= (offset + wiggle) && k < (offset + wiggle + m_colCount) ) {
				        padded_arr[k] = arr[k-(offset + wiggle)];
				    }
				    else {
				      padded_arr[k] = 0;
				    }
				  }

					// Finds the maximum value of the ping for the padded_arr[]
					int max_padded = padded_arr[0]; // First assignment of the max value and index
					max_index_padded = 0;
					int h = 0;
					int bottom_edge_padded = 150; // This is a value at which the bottom return of the acoustic image appears
					for (h = 0; h < bottom_edge_padded; h++) {
						if (padded_arr[h] > max_padded) {
							max_padded = padded_arr[h];
							max_index_padded = h;
						}
					}

					// Printing padded_arr[] to file; appends row values to the end of the output file
					for (int n = 0; n < padded_size; n++) {
						if (n == (padded_size - 1)) {
							output_file_padded << padded_arr[n] << "\n";
						}
						else {
							output_file_padded << padded_arr[n] << ",";
						}
					}
				}
				else {
					output_file_padded << "$bounds is less that $offset, would write out of bounds" << endl;
					cout << "$bounds is less that $offset, would write out of bounds" << endl;
				}


				// Printing arr[] to file; appends row values to the end of the output file
				for (int m = 0; m < m_colCount; m++) {
					if (m == (m_colCount - 1)) {
						//printf("%d \n",arr[m]);
						output_file << arr[m] << "\n";

					}
					else {
						//printf("%d, ",arr[m]);
						output_file << arr[m] << ",";
					}
				}
				cout << "From m_iterations, wrote row number " << m_iterations << " to file" << endl;

			} else { cout << "Did NOT succesfully open INPUT or OUTPUT file successfully, OR passed end of file " << endl; }
	m_iterations++; // Putting this INSIDE the if-statement, so that we don't end up with discontinuities in the image
	}

	cout << "Max value index is: " << max_index << endl;
	// TO_DO: This conversion factor's NOT ACCURATE, but is chosen for convenience in the simulation for the moment
	double conversion_factor = 10.5/105; // meters per pixel, 200 m / 500 pixel width? 200/m_colCount
	double distance_from_pixels = max_index * conversion_factor;
	Notify(m_outgoing_var,distance_from_pixels);

	output_file_position << m_nav_x << "," << m_nav_y << ","<< m_nav_heading << "," << max_index << "," << distance_from_pixels << "," << endl;

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool IncludeSampleData::OnStartUp()
{

	// Start by converting the specified image to a .csv file
	// Rust binary works like `$ ./convert-to-csv <path_to_image> <csv_image_export_path>`
	// NOTE: Rust binary and the image file need to be located in the mission directory
	cout << "STARTING image->.csv file conversion using Rust binary" << endl;
	const char *exe_path = "./convert-to-csv";
	const char *image_path = "./synthetic_image.png";
	const char *csv_image_export = "csv_image_import.csv";
  int pid = fork();
  switch(pid) {
      case -1: perror("The following error occurred: ");
               break;
       case 0: execl(exe_path,exe_path,image_path,csv_image_export,(char*)0);
               _exit (EXIT_FAILURE);
  }
	// NOTE: If $duration is too short, then the input file will not appear in time to row/colCount to open it, and will return counts=0
	chrono::milliseconds duration(500); // This is a not-great hack for making sure the threads execute in the proper order
  this_thread::sleep_for(duration);  // Sleep thread for $<duration> ms
	cout << "FINISHED image->.csv conversion using Rust " << endl;

	// Open input file and find rowCount and colCount numbers
	string line;
	char delim = ',';
	string delimiter = ",";
	size_t num_of_delim;
	ifstream input_file (m_input_filename);
	if (input_file.is_open())   // Accurately return the number of rows and columns in file
  {
    while ( getline (input_file,line) )
    {
      // Counts columns, dynamically evaluates 'colCount'
      if (m_rowCount == 0) {
        num_of_delim = std::count(line.begin(), line.end(), delim);
        m_colCount = (num_of_delim+1);
      }
      m_rowCount++; // Increases rowCount by 1 until the EOF
    }
  } else { cout << "Unable to open file"; }
	cout << "In OnStartUp, m_rowCount and m_colCount are " << m_rowCount << " " << m_colCount << endl;


  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfiguration(GetAppName(), sParams);

	// Get variables from environment and Register
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine     = *p;
    string sVarName  = MOOSChomp(sLine, "=");
    sLine = stripBlankEnds(sLine);

		if(MOOSStrCmp(sVarName, "OUTGOING_VAR")) {
			if(!strContains(sLine, " "))
	  m_outgoing_var = stripBlankEnds(sLine);
	  }

    if(MOOSStrCmp(sVarName, "NAV_X_RECEIVED")) {
      if(!strContains(sLine, " "))
    m_nav_x_received = stripBlankEnds(sLine);
    }

		if(MOOSStrCmp(sVarName, "NAV_Y_RECEIVED")) {
	     if(!strContains(sLine, " "))
	  m_nav_y_received = stripBlankEnds(sLine);
	  }

		if(MOOSStrCmp(sVarName, "NAV_HEADING_RECEIVED")) {
		    if(!strContains(sLine, " "))
		  m_nav_heading_received = stripBlankEnds(sLine);
		}

		if(MOOSStrCmp(sVarName, "MODE_RECEIVED")) {
				if(!strContains(sLine, " "))
			m_mode_received = stripBlankEnds(sLine);
		}


  }
  RegisterVariables();

  return(true);
}