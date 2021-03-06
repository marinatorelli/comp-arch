#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <fstream>
#include <iomanip>
#include "asteroid.h"

using namespace std;

//helpful functions
void print_ast(std::vector<Asteroid*> ast){
    for (unsigned int i = 0; i < ast.size(); ++i){
        cout << ast[i]->x << " " << ast[i]->y << " " << ast[i]->vel_x << " " << ast[i]->vel_y << " " << ast[i]->mass << endl;
    }
}

void print_planets(Planet* ast[], int limit){
    for (int i = 0; i < limit; ++i){
        cout << ast[i]->x << " " << ast[i]->y << " " << ast[i]->mass << endl;
    }
}


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

    //initialize asteroids
    double x,y,m;

    ofstream init_file;
    init_file.open ("init_conf.txt");
    init_file << std::fixed;
    init_file << std::setprecision(3);
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

        cout << "--- asteroids vs asteroids ---" << std::endl; 
        std::vector<Asteroid*> temp(ast);
        while (!temp.empty()) {
            for (unsigned int i = 0; i < ast.size(); ++i) {
                temp.erase(temp.begin());

                //parallize this
                for (unsigned int j = 0; j < temp.size(); ++j) {
                    if (Asteroid::distance(*ast[i], *temp[j]) <= 5) {
                        Asteroid::rebound_asteroid(*ast[i], *temp[j]);
                    } else {
                        cout << ast[i]->id << " " << temp[j]->id << " ";  //for step-by-step
                        Asteroid::calc_force(*ast[i], *temp[j]);
                    }
                }
            }
        }

        //compare each asteroid with the planets

        std::cout << "--- asteroids vs planets ---" << std::endl;
        for (unsigned int i = 0; i < planets.size(); ++i) { 
            for (unsigned int j = 0; j < ast.size(); ++j) {
                std::cout << planets[i]->id << " " << ast[j]->id << " " ;  //for step-by-step 
                Asteroid::calc_force(*ast[j], *planets[i]);
            }
        }

        for (unsigned int i = 0; i < ast.size(); ++i) {
            ast[i]->update_kinematics(0.1); 
            ast[i]->rebound_border(width, height);
        }

        cout << std::endl << "******************** ITERATION *******************" << std::endl; 
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