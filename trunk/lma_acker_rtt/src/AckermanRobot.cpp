/***************************************************************************
  tag: Olmer Garcia    AckermanRobot.cpp

                        AckermanRobot.cpp -  description
                           -------------------
    begin                : 13/08/2012
    copyright            : (C) 2012 Olmer Garcia
    email                : olmerg@gmail.com

The code contain code of rtt_ros_integration_example

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#include <rtt/TaskContext.hpp>
#include <rtt/Port.hpp>
#include <std_msgs/Float64.h>
#include <rtt/Component.hpp>
#include <rtt/os/TimeService.hpp>
#include <rtt/Time.hpp>
#include <cmath>

using namespace RTT;

class AckermanRobot : public RTT::TaskContext{
private:  
  InputPort<std_msgs::Float64> steeringp;
  InputPort<std_msgs::Float64> velocidad;
  OutputPort<std_msgs::Float64> robot_x;
  OutputPort<std_msgs::Float64> robot_y;
  OutputPort<std_msgs::Float64> robot_tetha;
  OutputPort<std_msgs::Float64> robot_steer;
  OutputPort<std_msgs::Float64> robot_time;
  
  std::string prop_answer;
  double length;
  double x0,y0,tetha0,gamma0,gammap,vel,time; 
RTT::os::TimeService::ticks ts;
public:
  AckermanRobot(const std::string& name):
    TaskContext(name),
    steeringp("command_steerp"),velocidad("command_velocity"),
    robot_x("robot_x"),robot_y("robot_y"),robot_tetha("robot_tetha"),robot_steer("robot_steer"),
    robot_time("robot_time"),
    length(3),x0(0),y0(0),tetha0(0),gamma0(0),gammap(0),vel(0),time(0)
  {
    this->addEventPort(steeringp).doc("Receiving the speed of the steering");
    this->addEventPort(velocidad).doc("Receiving the speed of the motor of the car");
    this->addPort(robot_x).doc("Position x of the robot");
    this->addPort(robot_y).doc("Position x of the robot");
    this->addPort(robot_tetha).doc("Angle \\tetha of the robot");
    this->addPort(robot_steer).doc("Angle \\Gamma of the steering of the robot");
    this->addPort(robot_time).doc("time of calculated");
    this->addProperty("length",length).doc("length of the car in used int the model of bycicle");    
    ts=RTT::os::TimeService::Instance()->getTicks();
    time=(double)RTT::os::TimeService::Instance()->getSeconds(ts);
	
  }
  ~AckermanRobot(){}
private:
  void updateHook(){
    time=(double)RTT::os::TimeService::Instance()->getSeconds(ts);
    ts=RTT::os::TimeService::Instance()->getTicks();
    std_msgs::Float64 fdata;
    if(NewData==steeringp.read(fdata)){
      gammap=fdata.data;	
      log(Info)<<"Steer speed: "<<gammap<<endlog();
    }
    if(NewData==velocidad.read(fdata)){
	vel=fdata.data;
      log(Info)<<"velocity: "<<vel<<endlog();
    }
 //TODO: hacer esto en una forma mas elegante	
    double dt=time;
    //TimeService::ticks ts=TimeService::Instance()->getTicks();
    //time=time-ts.secs+ts.msecs/1000.0+ts.usecs/1000000.0;
    
    x0=x0+dt*vel*cos(tetha0);
    y0=y0+dt*vel*sin(tetha0);	
// deberia actualizar primero gamma y luego tetha????

    gamma0=fmod(gamma0+dt*gammap,3.14159265);
    if(gamma0>0.785398162) //pi/4
	gamma0=0.785398162;
    if(gamma0<-0.785398162) //pi/4
	gamma0=-0.785398162;
    tetha0=fmod(tetha0+dt*vel*tan(gamma0)/length,3.14159265);


    fdata.data=x0;
    robot_x.write(fdata);
    fdata.data=y0;
    robot_y.write(fdata);
    fdata.data=tetha0;
    robot_tetha.write(fdata);
    fdata.data=gamma0;
    robot_steer.write(fdata);
    fdata.data=time;
    robot_time.write(fdata);
  }
};
ORO_CREATE_COMPONENT(AckermanRobot)
