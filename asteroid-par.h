#include <cmath>
#include <vector>
#include <fstream>
#include <omp.h>

#pragma once

extern std::ofstream step_by_step;

class Planet{
public:
    Planet(){ }
    Planet(double xcoor, double ycoor, double massarg){
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
    double vel_x;
    double vel_y;
    double mass;
    unsigned int id; 
    
    std::vector<double> forces_x;
    std::vector<double> forces_y;

    Asteroid(){ }
    Asteroid(double xcoor, double ycoor, double massarg){
        x = xcoor;
        y = ycoor;
        vel_x = 0;
        vel_y = 0; 
        mass = massarg;
    }

    template <typename T>
    static double distance(Asteroid &a, T &b){
        return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
    }

    template <typename T>
    static double angle_of_influence(Asteroid &a, T &b){
        double slope = (a.y - b.y) / (a.x - b.x);
        slope = (slope > 1) ? 1 : slope;
        slope = (slope < -1) ? -1 : slope;
        return atan(slope);
    }

    static void calc_force(Asteroid &a, Asteroid &b){
        double G = 6.674* pow(10, -5);
        double f = (G*a.mass*b.mass) / pow(distance(a, b),2) ;  //less memory to pass parameters instead of copying whole class
       
        //assign component forces, truncate the |f| value if needed
        double f_x, f_y; 
		double angle = angle_of_influence(a, b);
		#pragma omp critical
		{
			if (f > 100){
				f_x =  100 * cos(angle);
				f_y =  100 * sin(angle); 
			} else {
				f_x = f * cos(angle);
				f_y = f * sin(angle);
			}
       
			a.forces_x.push_back(f_x);
			a.forces_y.push_back(f_y);

			b.forces_x.push_back(-f_x); 
			b.forces_y.push_back(-f_y);
		}
    }

    static void calc_force(Asteroid &a, Planet &b){
        double G = 6.674* pow(10, -5);
        double f = (G*a.mass*b.mass) / pow(distance(a, b),2) ;  //less memory to pass parameters instead of copying whole class
       
        //assign component forces, truncate the |f| value if needed
        double f_x, f_y; 
		double angle = angle_of_influence(a, b);
		#pragma omp critical
		{
			if (f > 100){
				f_x =  100 * cos(angle);
				f_y =  100 * sin(angle); 
			} else {
				f_x = f * cos(angle);
				f_y = f * sin(angle);
			}
			a.forces_x.push_back(f_x);
			a.forces_y.push_back(f_y);
		}
    }

    //used if the distance is less than something
    static void rebound_asteroid(Asteroid &a, Asteroid &b){
        double temp_vel_x = a.vel_x;
        double temp_vel_y = a.vel_y;
		#pragma omp critical
		{
			a.vel_x = b.vel_x;
			a.vel_y = b.vel_y;
			b.vel_x = temp_vel_x;
			b.vel_y = temp_vel_y;
		}
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
		
        vel_x = accel_x * time;
        vel_y = accel_y * time;

        x += vel_x * time;
        y += vel_y * time;
		// Restore the forces vectors for the next iteration
		forces_x.clear();
		forces_y.clear();

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