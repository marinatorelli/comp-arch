#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <fstream>
#include "asteroid.h"
//#include "gtest/gtest.h"

using namespace std;

void print_ast(std::vector<Asteroid*> ast){
    for (int i = 0; i < ast.size(); ++i){
        std::cout << ast[i]->x << " " << ast[i]->y << " " << ast[i]->vel_x << " " << ast[i]->vel_y << " " << ast[i]->mass << std::endl;
    }
}

void print_planets(Planet* ast[], int limit){
    for (int i = 0; i < limit; ++i){
        std::cout << ast[i]->x << " " << ast[i]->y << " " << ast[i]->mass << std::endl;
    }
}


int main() {


    //info to generate the asteroids
    int width = 200;
    int height = 200;
    double mass = 1000;
    double sdm = 50;

    //parameters (to be command line args)
    int num_asteroids = 2;
    int num_iter = 2;
    int num_planets = 2;
    unsigned int seed = 2;

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

    ofstream outfile;
    outfile.open ("init_conf.txt");

    for (int i = 0; i < num_asteroids; ++i){
        x = xdist(re);
        y = ydist(re);
        m = mdist(re);
        Asteroid a = Asteroid(x, y, m); //dynamic memory necessary?
        ast[i] = &a;
        outfile << x << " " << y << " " << m << std::endl;
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
        Planet p = Planet(x,y,m);
        planets[i] = &p;
        outfile << x << " " << y << " " << m << std::endl;
    }

    outfile.close();

    print_ast(ast);
    //simulate asteriods num

    for (int k = 0; k < num_iter; ++k) {

        //compare each asteroid with each other

        std::vector<Asteroid*> temp(ast);
        while (!temp.empty()) {
            for (int i = 0; i < ast.size(); ++i) {
                temp.erase(temp.begin());
                for (int j = 0; j < temp.size(); ++j) {
                    if (Asteroid::distance(*ast[i], *temp[i]) <= 5) {
                        Asteroid::rebound_asteroid(*ast[i], *temp[i]);
                    } else {
                        Asteroid::calc_force(*ast[i], *temp[i]);
                    }
                }
            }
        }

        //compare each asteroid with the planets
        for (int i = 0; i < ast.size(); ++i) { //probably don't need this loop twice. Keeping for readability
            for (int j = 0; j < planets.size(); ++j) {
                Asteroid::calc_force(*ast[i], *planets[i]);
            }
        }

        for (int i = 0; i < ast.size(); ++i) {
            ast[i]->update_kinematics(0.1); //which one comes first?
            ast[i]->rebound_border(width, height);
        }
    }

    print_ast(ast);

    return 0;
}