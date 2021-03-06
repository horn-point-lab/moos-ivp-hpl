#ifndef USC_MOOSAPP_HEADER
#define USR_MOOSAPP_HEADER

#include <string>
#include <string.h>//not totally sure why this needs to be here, but strcmp isn't found when it's not
#include "MOOS/libMOOS/MOOSLib.h"
#include "netcdfcpp.h"
#include "MOOSGeodesy.h"
#include "netcdf.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include <iostream>
#include <cmath>
#include <fstream>


class USR_MOOSApp : public CMOOSApp  
{
public:
  USR_MOOSApp();
  virtual ~USR_MOOSApp() {};

  bool Iterate();
  bool OnConnectToServer();
  bool OnDisconnectFromServer();
  bool OnStartUp();
  bool OnNewMail(MOOSMSG_LIST &NewMail);

 protected:
  void registerVariables();
  bool ReadNcFile(); //this is defined in a seperate file 
  bool LatLontoIndex(int eta[4], int xi[4], double dist[4], double, double);
  bool LatlontoMeters();
  bool GetS_rho();
  double GetValue();
  double GetValueAtTime(int);
  double WeightedAvg(double*,double*, int*, int);
  bool GetTimeInfo();
  bool GetBathy(int eta[4], int xi[4], double dist[4], double &depth);
  bool GetSafeDepth();
  bool ConvertToMeters();
  

 protected: // Configuration variables
  std::string ncFileName;
  std::string varName; 
  std::string maskRhoVarName;
  std::string latVarName;
  std::string lonVarName;
  std::string timeVarName;
  std::string sVarName;
  std::string bathyVarName;
  std::string scalarOutputVar;
  std::string safeDepthVar;

  int time_vals; //number of time vals
  int s_rho;  //number of s_rho points
  int eta_rho;//number of eta_rho points
  int xi_rho; //number of xi_rho points

  //determines if the user wants to override the size of the matrix
  bool time_override;
  bool s_override;
  bool eta_override;
  bool xi_override;
  
  bool bathy_only;

 protected: // State variables

  double start_time;
  double current_time;

  // x/y positions, dpeth, and altitude of current location 
  double       m_posx;
  double       m_posy;
  double       m_depth;
  double       m_head;
  std::string  m_rTime;
  double       floor_depth;
  double       look_fwd; //distance forward (in meters) to check for dangerous bathymetry
  double       safe_depth; //deepest safe depth based on the distance in safe_dist
  double       m_altitude;    

  //current depth level, and distances to nearest s_levels
  int          s_level;
  double       distSigma;
  double       distSp1;
  bool         above_s_level;
  
  //closest 4 eta/xi pairs(so closest_eta[0] and closest_xi[0] form one pair)and the respecitve distances to them
  int          closest_eta[4];
  int          closest_xi[4];
  double       closest_distance[4];
  double       closest_dist_meters[4];
  
  //our current lon/lat coordinate
  double       current_lat;
  double       current_lon;

  //the current time step, as well as the time from it and the time until the next one
  int          time_step;
  double       time_until;
  double       time_since;
  bool         more_time;
  bool         time_message_posted;

  //the value represnting a nonexistent value
  double       bad_val;
  //geodesy class
  CMOOSGeodesy geodesy;
  
  //stores variables from the CDF file
  double****   vals;
  int**        maskRho;
  double**     lat;
  double**     lon;
  double**     meters_n;
  double**     meters_e;
  double**     bathy;
  double*      time;
  double*      s_values;
  


  friend bool CMOOSGeodesy::LocalGrid2LatLong(double dfEast, double dfNorth, double &dfLat, double &dfLon) ;

};

 


#endif




