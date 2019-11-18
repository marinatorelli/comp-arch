#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <omp.h>
#include "asteroid-seq.h"

using namespace std;

int main(int argc, char *argv[]) {
     //process input args
    if (argc != 5) {
        cerr << "Error: Wrong number of parameters\n";
        return -1;
    }
    int num_asteroids = atoi(argv[1]);
    int num_iter = atoi(argv[2]);
    int num_planets = atoi(argv[3]);
    int seed = atoi(argv[4]);
	if (num_asteroids <= 0 || num_iter <= 0 || num_planets <= 0 || seed <= 0) {
		cerr << "Error: None of the parameters can be negative\n";
		return -1;
	}
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
    vector<Asteroid*> ast(num_asteroids);
    vector<Planet*> planets(num_planets);

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
        Asteroid *a = new Asteroid(x, y, m); //you can't assign the values directly to ast[i]. interesting
        a->id = i; 
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
        p->id = i; 
        planets[i] = p;
        init_file << x << " " << y << " " << m << std::endl;
    }

    init_file.close();

    //simulate asteriods num
    for (int k = 0; k < num_iter; ++k) {
        //compare each asteroid with each other
        for (unsigned int i = 0; i < ast.size(); ++i) {
			for (unsigned int j = i+1; j < ast.size(); ++j) {
				if (Asteroid::distance(*ast[i], *ast[j]) <= 5) {
                    Asteroid::rebound_asteroid(*ast[i], *ast[j]);
                } else {
                    Asteroid::calc_force(*ast[i], *ast[j]);
                }
            }
        }
        //compare each asteroid with the planets
        for (unsigned int i = 0; i < planets.size(); ++i) { 
            for (unsigned int j = 0; j < ast.size(); ++j) {
                Asteroid::calc_force(*ast[j], *planets[i]);
            }
        }
        for (unsigned int i = 0; i < ast.size(); ++i) {
            ast[i]->update_kinematics(0.1); 
            ast[i]->rebound_border(width, height);
        }
    }

    ofstream out_file;
    out_file.open("out.txt");
    out_file << std::fixed;
    out_file << std::setprecision(3);
    for(unsigned int i = 0; i < ast.size(); ++i) {
        out_file << ast[i]->x << " " << ast[i]->y << " " << ast[i]->vel_x << " " << ast[i]->vel_y << " " << ast[i]->mass << std::endl;
    }

    return 0;
}