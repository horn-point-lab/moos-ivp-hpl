// This is a mashup/mod of the iMOOS2Serial class 
// written by Andrew Patikalakis.
// This code provides serial comms with an Arduino Uno, 
// for the purpose of toggling power to a Rockland MicroRider.
//
// nidzieko@umces.edu
// 18March2015
//
// CiSerialMR.cpp: implementation of the CiSerialMR class.
////////////////////////////////////////////////////////


#ifndef __CiSerialMR_h__
#define __CiSerialMR_h__

#include "MOOS/libMOOS/MOOSLib.h"
#include "CSerMR.h"

#define MAXBUFLEN 120 // Needs to match the arduino receive max

class CiSerialMR : public CMOOSApp
{
public:
	CiSerialMR();
	virtual ~CiSerialMR();

	bool OnConnectToServer();
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnStartUp();

protected:
	// insert local vars here
	CSerMR *bbb;

        static bool tramp(void *arg, std::string s) {
                return ((CiSerialMR *)arg)->handle(s);
        }
        bool handle(std::string s);

	std::string invar;
};

#endif /* __CiSerialMR_h__ */
