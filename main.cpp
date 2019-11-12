#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <fstream>
#include <iomanip>
#include "asteroid.h"

using namespace std;

void print_ast(std::vector<Asteroid*> ast){
    for (unsigned int i = 0; i < ast.size(); ++i){
        std::cout << ast[i]->x << " " << ast[i]->y << " " << ast[i]->vel_x << " " << ast[i]->vel_y << " " << ast[i]->mass << std::endl;
    }
}

void print_planets(Planet* ast[], int limit){
    for (int i = 0; i < limit; ++i){
        std::cout << ast[i]->x << " " << ast[i]->y << " " << ast[i]->mass << std::endl;
    }
}


int main(int argc, char *argv[]) {
    //process input args
    if (argc != 5) {
        std::cerr << "Error: Wrong number of parameters\n";
        return -1;
    }
    int num_asteroids = atoi(argv[1]);
    int num_iter = atoi(argv[2]);
    int num_planets = atoi(argv[3]);
    int seed = atoi(argv[4]);

    //info to generate the asteroids
    int width = 200;
    int height = 200;
    double mass = 1000;
    double sdm = 50;

    //random number generator
    default_random_engine re{seed};
    uniform_real_distribution<double> xdist{0.0, std::nextafter(width, std::numeric_limits<double>::max())};
    uniform_real_distribution<double> ydist{0.0, std::nextafter(height, std::numeric_limits<double>::max())};
    normal_distribution<double> mdist{mass, sdm};

    //collection of asteroids and planets
    vector<Asteroid*> ast(num_asteroids); //everything is initialzed already? Maybe don't make new instances below
    vector<Planet*> planets(num_planets);

    //initialize asteroids
    double x,y,m;
	
	// Initiale input file with arguments
    ofstream init_file;
    init_file.open ("init.txt");
    init_file << std::fixed;
    init_file << std::setprecision(3);
    init_file << num_asteroids << " " << num_iter << " " << num_planets << "  " << seed << "\n";

    for (int i = 0; i < num_asteroids; ++i){
        x = xdist(re);
        y = ydist(re);
        m = mdist(re);
        Asteroid *a = new Asteroid(x, y, m);
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
        Planet *p = new Planet(x,y,m);
        planets[i] = p;
        init_file << x << " " << y << " " << m << std::endl;
    }

    init_file.close();

    //simulate asteriods num

	// Print information of each iteration in a file
	ofstream steps;
	steps.open("steps.txt");
	
    for (int k = 0; k < num_iter; ++k) {
		double force, angle;
        //compare each asteroid with each other
		steps << "--- asteroids vs asteroids ---\n";
        for (unsigned int i = 0; i < ast.size(); ++i) {
            for (unsigned int j = i+1; j < ast.size(); ++j) {
                if (Asteroid::distance(*ast[i], *ast[j]) <= 5) {
                    Asteroid::rebound_asteroid(*ast[i], *ast[j]);
                } else {
					force = Asteroid::calc_force(*ast[i], *ast[j]);
                }
				angle = Asteroid::angle_of_influence(*ast[i], *ast[j]);
				steps << i << " " << j << " " << force << " " << angle << "\n";
            }
        }

		steps << "--- asteroids vs planets ---\n";
        //compare each asteroid with the planets
        for (unsigned int i = 0; i < ast.size(); ++i) { //probably don't need this loop twice. Keeping for readability
            for (unsigned int j = 0; j < planets.size(); ++j) {
                force = Asteroid::calc_force(*ast[i], *planets[j]);
				angle = Asteroid::angle_of_influence(*ast[i], *planets[j]);
				steps << i << " " << j << " " << force << " " << angle << "\n";
            }
        }
		
		// Update the values of the fields of the asteroid and check for rebounds against the borders.
        for (unsigned int i = 0; i < ast.size(); ++i) {
            ast[i]->update_kinematics(0.1); 
            ast[i]->rebound_border(width, height);
        }
		steps << "********Iteration********\n";
    }
	steps.close();

    ofstream out_file;
    out_file.open("output.txt");
    out_file << std::fixed;
    out_file << std::setprecision(3);
    for(unsigned int i = 0; i < ast.size(); ++i) {
        out_file << ast[i]->x << " " << ast[i]->y << " " << ast[i]->vel_x << " " << ast[i]->vel_y << " " << ast[i]->mass << std::endl;
    }
	out_file.close();

    return 0;
}