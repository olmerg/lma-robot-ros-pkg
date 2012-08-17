#include "ros/ros.h"
#include "std_msgs/Float64.h"

#include <sstream>
#include <signal.h>
#include <termios.h>
#include <stdio.h>

#define KEYCODE_R 0x43 
#define KEYCODE_L 0x44
#define KEYCODE_U 0x41
#define KEYCODE_D 0x42
#define KEYCODE_Q 0x71


int kfd=0;
struct termios cooked, raw;

class TeleopRobotLMA
{
public:
  TeleopRobotLMA();
  void keyLoop();
private:  
  ros::NodeHandle nh_;
  double velocidad_, steeringp_;
  double vel_step_, steeringp_step_;
  ros::Publisher robot_vel_pub_;
  ros::Publisher robot_steerp_pub_;
};

TeleopRobotLMA::TeleopRobotLMA():
  velocidad_(0),
  steeringp_(0),
  vel_step_(1.0),
  steeringp_step_(0.1)
{
  nh_.param("step_velocity",vel_step_, vel_step_);
  nh_.param("step_steeringp",steeringp_step_,steeringp_step_);
  robot_vel_pub_ = nh_.advertise<std_msgs::Float64>("lma_robot/command_velocity", 1);
  robot_steerp_pub_ = nh_.advertise<std_msgs::Float64>("lma_robot/command_steerp", 1);
}



void quit(int sig)
{
  tcsetattr(kfd, TCSANOW, &cooked);
  ros::shutdown();
  exit(0);
}

int main(int argc, char **argv)
{
  //leer argumentos y pasar a roscore
  ros::init(argc, argv, "lma_input_ros");
  TeleopRobotLMA teleop_robotlma;
  signal(SIGINT,quit);
  teleop_robotlma.keyLoop();
  return 0;
}



void TeleopRobotLMA::keyLoop()
{
  char c;
  bool dirty=false;


  // get the console in raw mode                                                              
  tcgetattr(kfd, &cooked);
  memcpy(&raw, &cooked, sizeof(struct termios));
  raw.c_lflag &=~ (ICANON | ECHO);
  // Setting a new line, then end of file                         
  raw.c_cc[VEOL] = 1;
  raw.c_cc[VEOF] = 2;
  tcsetattr(kfd, TCSANOW, &raw);

  puts("Reading from keyboard");
  puts("---------------------------");
  puts("Use arrow keys to move the robot LMA.");


  for(;;)
  {
    // get the next event from the keyboard  
    if(read(kfd, &c, 1) < 0)
    {
      perror("read():");
      exit(-1);
    }

//     double velocidad_, steeringp_, vel_step_, steeringp_step_;
    ROS_DEBUG("value: 0x%02X\n", c);
  
    switch(c)
    {
      case KEYCODE_L:
        ROS_DEBUG("steer++");
        steeringp_ +=steeringp_step_;
        dirty = true;
        break;
      case KEYCODE_R:
        ROS_DEBUG("steer--");
        steeringp_ -=steeringp_step_;
        dirty = true;
        break;
      case KEYCODE_U:
        ROS_DEBUG("vel++");
        velocidad_=velocidad_+ vel_step_;
        dirty = true;
        break;
      case KEYCODE_D:
        ROS_DEBUG("vel--");
        velocidad_=velocidad_- vel_step_;
        dirty = true;
        break;
    }
   

    if(dirty ==true)
    {
	std_msgs::Float64 valor;
	valor.data=velocidad_;
 	robot_vel_pub_.publish(valor);
	valor.data=steeringp_;
	robot_steerp_pub_.publish(valor);
      dirty=false;
    }
  }
  return;
}




