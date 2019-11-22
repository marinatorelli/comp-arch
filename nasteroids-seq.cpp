#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <omp.h>
#include <chrono>
#include "asteroid.h"

using namespace std;
using namespace std::chrono;

int num_asteroids, num_planets, num_iter, seed;
double G = 6.674* pow(10, -5);
int width = 200;
int height = 200;
double mass = 1000;
double sdm = 50;
int min_distance = 5;
float delta_time = 0.1;

template <typename T>
static double distance(Asteroid &a, T &b){
    return sqrt( pow(a.x - b.x, 2) + pow(a.y - b.y, 2) );
}

template <typename T>
static double angle_of_influence(Asteroid &a, T &b){
    double slope = (a.y - b.y) / (a.x - b.x);
    slope = (slope > 1) ? 1 : slope;
    slope = (slope < -1) ? -1 : slope;
    return atan(slope);
}

static void calc_force(Asteroid &a, Asteroid &b){
    double f = (G*a.mass*b.mass) / pow(distance(a, b),2) ;  //less memory to pass parameters instead of copying whole class
      
    //assign component forces, truncate the |f| value if needed
	double f_x, f_y; 
	double angle = angle_of_influence(a, b);
	if (f > 100){
		f_x =  100 * cos(angle);
		f_y =  100 * sin(angle); 
	} else {
		f_x = f * cos(angle);
		f_y = f * sin(angle);
    }
	a.forces_x[b.id] = f_x;
	a.forces_y[b.id] = f_y;

	b.forces_x[a.id] = -f_x; 
	b.forces_y[a.id] = -f_y;
}

static void calc_force(Asteroid &a, Planet &b){
	double f = (G*a.mass*b.mass) / pow(distance(a, b),2) ;  //less memory to pass parameters instead of copying whole class
       
	//assign component forces, truncate the |f| value if needed
	double f_x, f_y; 
	double angle = angle_of_influence(a, b);
	if (f > 100){
		f_x =  100 * cos(angle);
		f_y =  100 * sin(angle); 
	} else {
		f_x = f * cos(angle);
		f_y = f * sin(angle);
	}
	a.forces_x[b.id+num_asteroids] = f_x;
	a.forces_y[b.id+num_asteroids] = f_y;
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

int main(int argc, char *argv[]) {
	// Get starting timepoint 
 	auto start = high_resolution_clock::now(); 
     //process input args
    if (argc != 5) {
        std::cerr << "nasteroids-seq: Wrong arguments.\nCorrect use:\nnasteroids-seq num_asteroids num_iterations num_planets seed\n";
        return -1;
    }
    num_asteroids = atoi(argv[1]);
    num_iter = atoi(argv[2]);
    num_planets = atoi(argv[3]);
    seed = atoi(argv[4]);
	if (num_asteroids <= 0 || num_iter <= 0 || num_planets <= 0 || seed <= 0) {
		cerr << "Error: None of the parameters can be negative\n";
		return -1;
	}

    //random number generator
    default_random_engine re{seed};
    uniform_real_distribution<double> xdist{0.0, std::nextafter(width, std::numeric_limits<double>::max())};
    uniform_real_distribution<double> ydist{0.0, std::nextafter(height, std::numeric_limits<double>::max())};
    normal_distribution<double> mdist{mass, sdm};

    //collection of asteroids and planets
    vector <Asteroid> ast(num_asteroids);
    vector <Planet> planets(num_planets);

    //initialize asteroids and planets
    double x,y,m;

    ofstream init_file;
    init_file.open ("init_conf.txt");
    init_file << std::fixed;
    init_file << std::setprecision(3); // Print three digits of double numbers
    init_file << num_asteroids << " " << num_iter << " " << num_planets << "  " << seed << "\n";

    for (int i = 0; i < num_asteroids; ++i){
        x = xdist(re);
        y = ydist(re);
        m = mdist(re);
        Asteroid a(i,x, y, m, num_asteroids+num_planets); //you can't assign the values directly to ast[i]. interesting
        ast[i] = a;
        init_file << x << " " << y << " " << m << std::endl;
    }

    //initialize planets
    for (int i = 0; i < num_planets; ++i){
        int edge = i%4;

        switch(edge) {
            case 0: //left
                x = 0;
                y = ydist(re);
                break;
            case 1: //top
                x = xdist(re);
                y = 0;
                break;
            case 2: //right
                x = width;
                y = ydist(re);
                break;
            case 3: //bottom
                x = xdist(re);
                y = height;
        }

        m = mdist(re)*10;
        Planet p(i,x,y,m);
        planets[i] = p;
        init_file << x << " " << y << " " << m << std::endl;
    }

    init_file.close();

    //simulate asteriods num
    for (int k = 0; k < num_iter; ++k) {
        //compare each asteroid with each other
        for (unsigned int i = 0; i < ast.size(); ++i) {
			for (unsigned int j = i+1; j < ast.size(); ++j) {
				if (distance(ast[i], ast[j]) <= min_distance) {
                    rebound_asteroid(ast[i], ast[j]);
                } else {
                    calc_force(ast[i], ast[j]);
                }
            }
        }
        //compare each asteroid with the planets
        for (unsigned int i = 0; i < planets.size(); ++i) { 
            for (unsigned int j = 0; j < ast.size(); ++j) {
                calc_force(ast[j], planets[i]);
            }
        }
        for (unsigned int i = 0; i < ast.size(); ++i) {
            ast[i].update_kinematics(delta_time); 
            ast[i].rebound_border(width, height);
        }
    }

    ofstream out_file;
    out_file.open("out.txt");
    out_file << std::fixed;
    out_file << std::setprecision(3);
    for(unsigned int i = 0; i < ast.size(); ++i) {
        out_file << ast[i].x << " " << ast[i].y << " " << ast[i].vel_x << " " << ast[i].vel_y << " " << ast[i].mass << std::endl;
    }
	// Get ending timepoint 
	auto stop = high_resolution_clock::now(); 
	auto duration = duration_cast<microseconds>(stop - start); 
  
    cout << "Time taken by function: " << duration.count() << " microseconds" << endl; 
    return 0;
}