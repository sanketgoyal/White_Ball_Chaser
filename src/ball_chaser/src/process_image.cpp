#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float linear_x, float angular_z)
{
    // Requests a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = linear_x;
    srv.request.angular_z = angular_z;
    if (!client.call(srv)) {
    ROS_ERROR("Failed to call service drive_bot!");
    }
}

// TODO: Loop through each pixel in the image and check if there's a bright white one
// Then, identify if this pixel falls in the left, mid, or right side of the image
// Depending on the white ball position, call the drive_bot function and pass velocities to it
// Request a stop when there's no white ball seen by the camera

void process_image_callback(const sensor_msgs::Image img)
{
  int white_pixel = 255;
  int step = -1; 
  int detect_ball = 0; 
  for (int i = 0; i < img.height * img.step; i++) {
    if (img.data[i] == white_pixel && img.data[i+1] == white_pixel  && img.data[i+2] == white_pixel) {
      step = i % img.step;
      detect_ball = 1;
      break;
    } else {
       detect_ball = 0;
    }
    
  }
  if (detect_ball == 1) {
      if (step  <= img.step * 0.3 && step  >= 0) { 
      drive_robot(0, 0.3);
    } else if (step  > img.step * 0.7 && step <= img.step) { 
      drive_robot(0,-0.3);
    } else if (step != -1) {
      drive_robot(0.3, 0);
    }
  } else {
    drive_robot(0,0);
  }           
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}