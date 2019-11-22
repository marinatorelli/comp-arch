#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <fstream>
#include <iomanip>
#include "asteroid.h"

using namespace std; 

int main(int argc, char *argv[]){
	if (argc != 1){
		cerr << "wrong num" << endl;
	}
    unsigned int num_iter = atoi(argv[1]);


	//test rebound
	 
	unsigned int width = 200;
	unsigned int height = 200; 

	vector<Asteroid*> ast;
	Asteroid a = Asteroid(100,105,50);
	a.id = 1;
	Asteroid b = Asteroid(100,110,50);
	b.id = 2; 
	ast.push_back(&a);
	ast.push_back(&b);

	//planet init
	std::vector<Planet*> planets;
	Planet p1 = Planet(100,0,1000);
	p1.id = 1; 
	Planet p2 = Planet(0,200,10000000);
	p2.id = 2; 
	//Planet p3 = Planet(1,0,20000000);
	//p3.id = 3; 
	//Planet p4 = Planet(1,1,500);
	//p4.id = 4; 

	planets.push_back(&p1);
	planets.push_back(&p2);	
	planets.push_back(&p3);
	planets.push_back(&p4);	

	//simulate asteriods num

    for (unsigned int k = 0; k < num_iter; ++k) {

        //compare each asteroid with each other

        cout << "--- asteroids vs asteroids ---" << std::endl; 
        std::vector<Asteroid*> temp(ast);
        while (!temp.empty()) {
            for (unsigned int i = 0; i < ast.size(); ++i) {
                temp.erase(temp.begin());

                //parallize this
                for (unsigned int j = 0; j < temp.size(); ++j) {
                    if (Asteroid::distance(*ast[i], *temp[j]) <= 5) {
                    	//cout << "ID" << ast[i]->id << " " << temp[j]->id << " ";
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