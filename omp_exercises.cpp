#include <iostream>
#include <omp.h>
#include <chrono>
#include <iomanip>
#include <vector>
#include <cmath>

using namespace std;
using namespace std::chrono;


/*
***************************************
Fork-Join Parallelism 
- master thread starts with main program
- parallel section starts a thread set
- parallelism can be nested
- parallel region marked as below
***************************************
*/
void fork_join(){

	int n = 0;
	omp_set_num_threads(2); //library function to set threads
	#pragma omp parallel //alternative: omp parallel num_threads(2)
	{ 
		int id  = omp_get_thread_num();
		cout << "Hello("  << id  << ") ";
		cout << "World(" << id  << ") ";
	}

	n = omp_get_num_threads();
	cout << "\n\nthreads = "  << n << "\n";
}

/*
***************************************
Time Measurement with std and omp
***************************************
*/
void time_seq(){
	constexpr long nsteps = 10000000;
	double step = 1.0 / double(nsteps);

	//using clk = high_resolution_clock; //this doesn't work for some reason
	auto t1 = high_resolution_clock::now();

	double sum = 0.0;
	for (int i=0; i < nsteps; ++i){
		double x = (i+0.5) * step;
		sum+= 4.0 / (1.0 + x * x);
	}

	double pi = step * sum; 

	auto t2 = high_resolution_clock::now();
	auto diff = duration_cast<microseconds>(t2-t1);

	cout << "PI= " << setprecision(10) << pi << endl;
	cout << "Time= " << diff.count() << "us" << endl;
}

void exercise2(){
	//Exercise 2: implement time_seq with openmp
	//print time measurements, num threads
	//idea: use array and accumulate partial sum for each thread
	//time measurement in openmp
	//double tick = omp_get_wtick(); //time between two ticks... why do I need this?

	constexpr long nsteps = 10000000;
	double step = 1.0 / double(nsteps);
	double t1 = omp_get_wtime();
	
	double sum = 0.0;
	for (int i=0; i < nsteps; ++i){
		double x = (i+0.5) * step;
		sum+= 4.0 / (1.0 + x * x);
	}

	double pi = step * sum; 

	double t2 = omp_get_wtime();
	double diff = t2 - t1; 

	cout << "PI= " << setprecision(10) << pi << endl;
	cout << "Time= " << diff*1000000 << "us" << endl;

}

/*
**********************************
Synchronization:
- mechanism used to establish constraints on access order
  to shared variables (avoid data races)
- high level keywords - critical, atomic, barrier, ordered
- low level keywords - flush, lock
**********************************
*/

int foo(int i){
	return ++i; 
}

int bar(int i){ 
	std::cout << "Thread id: " << omp_get_thread_num() << ": "; 
	//std::cout << "X: " << x << std::endl; 
	return --i; 
}

//critical - guarantees mutual exclusion
void sync_critical(){
	int x,y;

	#pragma omp parallel
	{
		for (int i=0; i < 3; ++i){
			x = foo(i);
			#pragma omp critical
			y = bar(i); //only one thread can enter at a time
		}
	}
}

//atomic - guarantee atomic updates
void sync_atomic(){
	int x,y;

	#pragma omp parallel
	{
		for (int i=0; i < 3; ++i){
			x = foo(i);
			#pragma omp atomic
			y += bar(i); //updates to y are atomic/thread-safe
		}
	}
}

void exercise3(){
	//modify program from exercise 2
	//evaluate critical section
	//atomic access
}

/*
******************************
Parallel Loops
******************************
*/
void loop_work_sharing(){
	#pragma omp parallel //can combine this: "#pragma omp parallel for"
	{
		#pragma omp for //generates a private copy of i for each thread
						//can also be done with private(i)
		for (int i = 0; i < 4; ++i)
		{
			foo(i);
		}
	}
}

//reduction - private copy of each variable 
//local copy updated in each iteration
//local copies are combined in the end
//operation needs to be associative
void reduction(){
	double sum = 0.0;
	vector<double> v(10);
	#pragma omp parallel for reduction (+:sum) 
	//syntax: reduction (<op1>:<var1>, <op2>:<var2>)
	for(int i = 0; i < 5; ++i){
		sum += v[i];
	}
}

void exercise4(){
	//modify pi program to use omp for loops
	constexpr long nsteps = 10000000;
	double step = 1.0 / double(nsteps);
	double t1 = omp_get_wtime();
	
	double sum = 0.0;
	#pragma omp parallel for reduction (+:sum)
	for (int i=0; i < nsteps; ++i){
		double x = (i+0.5) * step;
		sum+= 4.0 / (1.0 + x * x);
	}

	double pi = step * sum; 

	double t2 = omp_get_wtime();
	double diff = t2 - t1; 

	cout << "PI= " << setprecision(10) << pi << endl;
	cout << "Time= " << diff*1000000 << "us" << endl;
}

/*
**********************************
Synchronize with Master
**********************************
*/

//Barriers 
//wait until all threads arrive to barrier before completing
//master - marks a block that is only executed in master thread
//single - marks a block that is only executed in one thread
//ordered - executed in sequential order
//locks (want to avoid)
//other library functions listed in slides
//environment variables - OMP_NUM_THREADS, OMP_SCHEDULE

/*Data sharing*/
//shared variables - globals, static, dynamic
//private variables - local variables in a parallel region, local vars in a block

//shared -  
//private - creates a new local copy per thread, not initialized
//firstprivate - each private copy is initialized with the value of master thread
//lastprivate - pass value of private variable of last sequential iteration to global


/*
*************************************
Sections and Scheduling
*************************************
*/

//sections
//each section can be passed to a particular thread
//implicit barrier at the end of the section block

//loop scheduling
//see slides

int main() {
	time_seq();
	//sync_critical();
	//sync_atomic();
	exercise4();
}