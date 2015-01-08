/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: USC_MOOSApp.h                                        */
/*    DATE: Jan 4th, 2011                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifndef USC_MOOSAPP_HEADER
#define USC_MOOSAPP_HEADER

#include <string>
#include "MOOS/libMOOS/MOOSLib.h"
#include "CurrentField.h"


class USC_MOOSApp : public CMOOSApp  
{
public:
  USC_MOOSApp();
  virtual ~USC_MOOSApp() {};

  bool Iterate();
  bool OnConnectToServer();
  bool OnDisconnectFromServer();
  bool OnStartUp();
  bool OnNewMail(MOOSMSG_LIST &NewMail);

 protected:
  bool setCurrentField(std::string);
  void postCurrentField();
  void registerVariables();
  void setScalarField(std::string);
  void postScalarField();
  int  roundup(int,int);
  int  rounddown(int, int);

 protected: // Configuration variables
  std::string  m_post_var;
  
  bool         scalar_field_active;   //must be initialized in configuration file if "SCALAR_FIELD_ACTIVE = false" is found
                                              //then uSimROMS_Scalar should behave just like uSimCurrent
  std::string  scalar_file_name;

 protected: // State variables
  CurrentField m_current_field;
  bool         m_pending_cfield;
  bool         m_cfield_active;
  double       m_posx;
  double       m_posy;
  bool         m_posx_init;
  bool         m_posy_init;

  double**     m_scalar;    //variables added to deal with a given scalar field
  int          m_xmax, m_ymax;
  int          tol;            //paramater must be "TOLERANCE" and should be a non zero, positive integer


  std::string  m_prev_posting;
};

 int  RoundUp(int,int);
 int  RoundDown(int, int);

#endif




