//
// Created by Tiffany Montgomery on 11/7/19.
//
#include <cmath>
#include <vector>
#include <fstream>

#pragma once

using namespace std;
using std::vector;

class Planet{
public:
    Planet(){ }
    Planet(unsigned int idarg, double xcoor, double ycoor, double massarg){
		id = idarg;
        x = xcoor;
        y = ycoor;

        mass = massarg;
    }
    double x;
    double y;
    double mass;

    unsigned int id; 
};

class Asteroid{
public:
    double x;
    double y;
	double mass;
	
    double vel_x;
    double vel_y;
	
    unsigned int id; 
    
    vector <double> forces_x;
    vector <double> forces_y;

    Asteroid() {}
    Asteroid(unsigned int idarg, double xcoor, double ycoor, double massarg, int nbodies){
		id = idarg;
        x = xcoor;
        y = ycoor;
		mass = massarg;
        vel_x = 0;
        vel_y = 0; 
		vector <double> temp(nbodies);
		forces_x = temp;
		forces_y = temp;
		
    }
	void update_kinematics(double time){
		//acceleration = sum of forces/mass
		double sum_forces_x = 0;
		double sum_forces_y = 0;

		for (unsigned int i = 0; i < forces_y.size(); ++i){
			sum_forces_x += forces_x[i];
			sum_forces_y += forces_y[i];
		}
		double accel_x = sum_forces_x/mass;
		double accel_y = sum_forces_y/mass;
			
		vel_x += accel_x * time;
		vel_y += accel_y * time;

		x += vel_x * time;
		y += vel_y * time;
	}
	void rebound_border(int width, int height){
		//check the borders
		if (x <= 0){
			x = 5;
			vel_x *= -1;
		}
		if (x >= width){
			x = width-5;
			vel_x *= -1;
		}
		if (y <= 0) {
			y = 5;
			vel_y *= -1;
		}
		if (y >= height) {
			y = height-5;
			vel_y *= -1;
		}
	}
};
