#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "ILPSolver.h"

int main(int argc, char** argv)  {


    if (argc < 3) { 
        // return 0;
    }
    else {

    }
	for (size_t i = 0; i < argc; ++i) { std::cout << argv[i] << " "; }
    
    MacroPlacer solver;
    solver.setProblemSize(3, 3, 10, 2);
    solver.run2();
    // solver.exampleGurobiOptimization();
    // solver.exampleMipGurobi();

    return 0;
}
