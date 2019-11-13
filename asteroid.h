//
// Created by Tiffany Montgomery on 11/7/19.
//
#include <cmath>
#include <vector>

#pragma once



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
    static constexpr double PI = 3.141592653589793238463;
    Asteroid(){
        
    };
    Asteroid(double xcoor, double ycoor, double massarg){
        x = xcoor;
        y = ycoor;
        vel_x = 0;
        vel_y = 0; 
        mass = massarg;
    }

    template <typename T>
    static double distance(Asteroid &a, T &b){
        return sqrt( pow(a.x - b.x, 2) + pow(a.y - b.y, 2) );
    }

    template <typename T>
    static double angle_of_influence(Asteroid &a, T &b){
        double num = a.y - b.y;
        double dem = a.x - b.x; 
        double slope = num / dem;
        slope = (slope > 1) ? 1 : slope; //does this need to execute twice?
        slope = (slope < -1) ? -1 : slope;
        double angle = atan(slope);
        if (num > 0 && dem > 0){
            return angle;
        } else if (num < 0 && dem > 0){
            return PI - angle;  
        } else if (num < 0 && dem < 0){
            return PI + angle; 
        } else {
            return 2*PI - angle; 
        }
    }

    //this should take a list of asteroids

    static void calc_force(Asteroid &a, Asteroid &b){
        double G = 6.674* pow(10, -5);
        double f_x = ( (G*a.mass*b.mass) / distance(a, b)) * cos(angle_of_influence(a, b));  //less memory to pass parameters instead of copying whole class
        double f_y = ( (G*a.mass*b.mass) / distance(a, b)) * sin(angle_of_influence(a, b));

        

        a.forces_x.push_back(f_x);
        a.forces_y.push_back(f_y);

        b.forces_x.push_back(-f_x); 
        b.forces_y.push_back(-f_y);

         /**STEP BY STEP CALCULATIONS***/
        std::cout << (f_x < 0 ? 0 : 1) << " ";
        std::cout << (f_y < 0 ? 0 : 1) << " ";
        std::cout << sqrt(pow(f_y,2) + pow(f_x,2)) << " ";
        std::cout << tan(f_y/f_x) << " " << std::endl;  
    
    }

    static void calc_force(Asteroid &a, Planet &b){
        double G = 6.674* pow(10,-5);
        double f_x = (G*a.mass*b.mass / pow(distance(a, b),2) ) * cos(angle_of_influence(a, b));  //less memory to pass parameters instead of copying whole class
        double f_y = (G*a.mass*b.mass / pow(distance(a, b),2) ) * sin(angle_of_influence(a, b));

        a.forces_x.push_back(f_x);
        a.forces_y.push_back(f_y);

        /**STEP BY STEP CALCULATIONS***/
        std::cout << (f_x < 0 ? 0 : 1) << " ";
        std::cout << (f_y < 0 ? 0 : 1) << " ";
        std::cout << sqrt(pow(f_y,2) + pow(f_x,2)) << " ";
        std::cout << tan(f_y/f_x) << " " << std::endl; 
    }

    //used if the distance is less than something
    static void rebound_asteroid(Asteroid &a, Asteroid &b){
        double temp_vel_x = a.vel_x;
        double temp_vel_y = a.vel_y;
        a.vel_x = b.vel_x;
        a.vel_y = b.vel_y;
        b.vel_x = temp_vel_x;
        b.vel_y = temp_vel_y;
    }

    void update_kinematics(double time){
        //acceleration = sum of forces/mass
        double sum_forces_x = 0;
        double sum_forces_y = 0;
        std::cout << "All the forces..." << std::endl;
        for (unsigned int i = 0; i < forces_y.size(); ++i){
            //std::cout << "Force x..: " << forces_x[i] << " ";
            //std::cout << "Force y..: " << forces_y[i] << std::endl; 
            sum_forces_x += forces_x[i];
            sum_forces_y += forces_y[i];
        }


        //truncate to max value

        double total = sqrt(pow(sum_forces_x,2) + pow(sum_forces_y,2));
        if (total > 100){
            double angle = tan(sum_forces_y/sum_forces_x);
            sum_forces_x = cos(angle) * 100;
            sum_forces_y = sin(angle) * 100; 
        }

        double accel_x = sum_forces_x/mass;
        double accel_y = sum_forces_y/mass;

        //std::cout << "accel_x " << accel_x;
        //std::cout << " accel_y " << accel_y << std::endl;  

        vel_x = accel_x * time;
        vel_y = accel_y * time;

        //std::cout << "vel_x " << vel_x;
        //std::cout << " vel_y " << vel_y << std::endl;  

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
            vel_y *= 1;
        }
    }

    double x;
    double y;
    double vel_x;
    double vel_y;
    double mass;
    unsigned int id; 
    
    std::vector<double> forces_x;
    std::vector<double> forces_y;
};
