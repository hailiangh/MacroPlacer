#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>

#include "ILPSolver.h"

int main(int argc, char** argv)  {

	for (size_t i = 0; i < argc; ++i) { std::cout << argv[i] << " "; }
    std::cout << "\n";
    std::cout << argc << "\n";

    if (argc == 1) {
        MacroPlacer solver;
        solver.setProblemSize(3, 3, 10, 2);
        solver.run3();
    }
    else if (argc == 3) {
        if ((strcmp(argv[1], "--batch") == 0 ) || (strcmp(argv[1], "-b") == 0 )) {
            const std::string batchFileName = argv[2];
            printf("Run batch mode from file <%s>.\n", argv[2]);
            MacroPlacer solver;
            solver.runBatchFromFile(argv[2]);
        }
    }
    
    // solver.exampleGurobiOptimization();
    // solver.exampleMipGurobi();

    return 0;
}
