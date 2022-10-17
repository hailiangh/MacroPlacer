#include "ILPSolver.h"
#include "util.h"
#include <memory>
// #include "../or-tools/ortools/linear_solver/linear_solver.h"
#include "gurobi_c++.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>


/**
 * @brief An example of optimization in Gurobi library.
 * 
 * @return int 
 */
int ILPSolver::exampleGurobiOptimization() {
    std::cout << __func__ << "\n" ;

    std::string inputFileName = "/home/steven/Documents/gurobi952/linux64/examples/data/afiro.mps";

    
    GRBEnv* env = 0;
    GRBVar* v = 0;
    try
    {
        // Read model and determine whether it is an LP
        env = new GRBEnv();
        // GRBModel model = GRBModel(*env, argv[1]);
        GRBModel model = GRBModel(*env, inputFileName);
        if (model.get(GRB_IntAttr_IsMIP) != 0)
        {
            std::cout << "The model is not a linear program" << std::endl;
            return 1;
        }

        model.optimize();

        int status = model.get(GRB_IntAttr_Status);

        if ((status == GRB_INF_OR_UNBD) || (status == GRB_INFEASIBLE) ||
            (status == GRB_UNBOUNDED))
        {
        std::cout << "The model cannot be solved because it is "
        << "infeasible or unbounded" << std::endl;
        return 1;
        }

        if (status != GRB_OPTIMAL)
        {
        std::cout << "Optimization was stopped with status " << status << std::endl;
        return 0;
        }

        // Find the smallest variable value
        double minVal = GRB_INFINITY;
        int minVar = 0;
        v = model.getVars();
        for (int j = 0; j < model.get(GRB_IntAttr_NumVars); ++j)
        {
        double sol = v[j].get(GRB_DoubleAttr_X);
        if ((sol > 0.0001) && (sol < minVal) &&
            (v[j].get(GRB_DoubleAttr_LB) == 0.0))
        {
            minVal = sol;
            minVar = j;
        }
        }

        std::cout << "\n*** Setting " << v[minVar].get(GRB_StringAttr_VarName)
        << " from " << minVal << " to zero ***" << std::endl << std::endl;
        v[minVar].set(GRB_DoubleAttr_UB, 0.0);

        // Solve from this starting point
        model.optimize();

        // Save iteration & time info
        double warmCount = model.get(GRB_DoubleAttr_IterCount);
        double warmTime = model.get(GRB_DoubleAttr_Runtime);

        // Reset the model and resolve
        std::cout << "\n*** Resetting and solving "
        << "without an advanced start ***\n" << std::endl;
        model.reset();
        model.optimize();

        // Save iteration & time info
        double coldCount = model.get(GRB_DoubleAttr_IterCount);
        double coldTime = model.get(GRB_DoubleAttr_Runtime);

        std::cout << "\n*** Warm start: " << warmCount << " iterations, " <<
        warmTime << " seconds" << std::endl;
        std::cout << "*** Cold start: " << coldCount << " iterations, " <<
        coldTime << " seconds" << std::endl;

    }
    catch (GRBException e)
    {
        std::cout << "Error code = " << e.getErrorCode() << std::endl;
        std::cout << e.getMessage() << std::endl;
    }
    catch (...)
    {
        std::cout << "Error during optimization" << std::endl;
    }

    delete[] v;
    delete env;
    // DVD();
    return 0;
}



/**
 * @brief Builds a trivial MIP model, solves it, and prints the solution.
 * 
 * @ref Gurobi "https://www.gurobi.com/documentation/9.5/examples/mip1_cpp_cpp.html"
 * @return int 
 */
int ILPSolver::exampleMipGurobi() {
    try {

        // Create an environment
        GRBEnv env = GRBEnv(true);
        env.set("LogFile", "mip1.log");
        env.start();

        // Create an empty model
        GRBModel model = GRBModel(env);

        // Create variables
        GRBVar x = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "x");
        GRBVar y = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "y");
        GRBVar z = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "z");

        // Set objective: maximize x + y + 2 z
        model.setObjective(x + y + 2 * z, GRB_MAXIMIZE);

        // Add constraint: x + 2 y + 3 z <= 4
        model.addConstr(x + 2 * y + 3 * z <= 4, "c0");

        // Add constraint: x + y >= 1
        model.addConstr(x + y >= 1, "c1");

        // Optimize model
        model.optimize();

        std::cout << x.get(GRB_StringAttr_VarName) << " "
            << x.get(GRB_DoubleAttr_X) << std::endl;
        std::cout << y.get(GRB_StringAttr_VarName) << " "
            << y.get(GRB_DoubleAttr_X) << std::endl;
        std::cout << z.get(GRB_StringAttr_VarName) << " "
            << z.get(GRB_DoubleAttr_X) << std::endl;

        std::cout << "Obj: " << model.get(GRB_DoubleAttr_ObjVal) << std::endl;

    } catch(GRBException e) {
        std::cout << "Error code = " << e.getErrorCode() << std::endl;
        std::cout << e.getMessage() << std::endl;
    } catch(...) {
        std::cout << "Exception during optimization" << std::endl;
    }

    return 0;
}


/**
 * @brief Set the problem size. We want to assign an array (x*y) of cells to the grid-like sites (x*y). 
 * 
 * @param arraySizeX 
 * @param arraySizeY 
 * @param siteSizeX 
 * @param siteSizeY 
 */
void MacroPlacer::setProblemSize(int arraySizeY, int arraySizeX, int siteSizeY, int siteSizeX) {
    m_arraySizeX = arraySizeX;
    m_arraySizeY = arraySizeY;
    m_siteSizeX = siteSizeX;
    m_siteSizeY = siteSizeY;
    // DBG("setProblemSize: (%d, %d) mapping to (%d, %d).\n", arraySizeY, arraySizeX, siteSizeY, siteSizeX);
}

/**
 * @brief Set the weight in x/y direction. 
 * 
 * @param weightX 
 * @param weightY 
 */
void MacroPlacer::setXYWeight(int weightX, int weightY) {
    m_weightX = weightX;
    m_weightY = weightY;
    // DBG("setXYWeight: %d, %d.\n", weightX, weightY);
}

/**
 * @brief Set if relative constraints in X direction is applied. (x0 <= x1 <= ... xn)
 * 
 * @param b 
 */
void MacroPlacer::setRelativeConstraintX(bool b) {
    m_relativeConstraintX = b;
}

/**
 * @brief Set if relative constraints in Y direction is applied. (y0 <= y1 <= ... yn)
 * 
 * @param b 
 */
void MacroPlacer::setRelativeConstraintY(bool b) {
    m_relativeConstraintY = b;
}

/**
 * @brief Set if relative constraints in X/Y direction is applied. 
 * 
 * @param bx 
 * @param by 
 */
void MacroPlacer::setRelativeConstraintXY(bool bx, bool by) {
    m_relativeConstraintX = bx;
    m_relativeConstraintY = by;
}

/**
 * @brief Set the time limit (in seconds) for the solver. 
 * 
 * @param timeLimit 
 */
void MacroPlacer::setTimeLimit(double timeLimit) {
    m_timeLimit = timeLimit;
}

/**
 * @brief Entry function of the macro placer.
 * 
 */
void MacroPlacer::run() {
    // // DVD();
    // // DBG("%s.\n", __func__);
    
    // GRBEnv env = GRBEnv();
    // GRBModel model = GRBModel(env);

    // // Add decision variables.
    
    // // x[i][j][k][p] == 1 means cell[i][j] is assigned to site[k][p]. 

    // // DBG("Adding variables..\n");
    // GRBVar x[m_arraySizeY][m_arraySizeX][m_siteSizeY][m_siteSizeX]; 

    // int i, j, m, n;
    // std::string s;
    // for (i = 0; i < m_arraySizeY; i++) {
    //     for (j = 0; j < m_arraySizeX; j++) {
    //         for (m = 0; m < m_siteSizeY; m++) {
    //             for (n = 0; n < m_siteSizeX; n++) {
    //                 s = "cell[" + std::to_string(i) + "][" + std::to_string(j) + "]2site[" + std::to_string(m) + "][" + std::to_string(n) + "]";
    //                 x[i][j][m][n] = model.addVar(0, 1, 0, GRB_BINARY, s);
    //             }
    //         }
    //     }
    // }

    // // Add constraints.
    // // DBG("Adding constraints..\n");

    // // // x0 <= x1, y0 <= y1 to remove mirrored solutions.
    // // model.addConstr(x[0][0] <= x[m_arraySizeY-1][m_arraySizeX-1], "no_mirror_x");
    // // model.addConstr(y[0][0] <= y[m_arraySizeY-1][m_arraySizeX-1], "no_mirror_y");

    // // For each cell, it only get assigned to one site.
    
    // for (i = 0; i < m_arraySizeY; i++) {
    //     for (j = 0; j < m_arraySizeX; j++) {
    //         GRBLinExpr expr = 0;
    //         for (m = 0; m < m_siteSizeY; m++) {
    //             for (n = 0; n < m_siteSizeX; n++) {
    //                 expr += x[i][j][m][n];
    //             }
    //         }
    //         s = "cnstr_cell[" + std::to_string(i) + "][" + std::to_string(j) + "]"; 
    //         model.addConstr(expr == 1.0, s);
    //     }
    // }

    // // For each site, at most one cell get assigned to it.

    // for (m = 0; m < m_siteSizeY; m++) {
    //     for (n = 0; n < m_siteSizeX; n++) {
    //         GRBLinExpr expr = 0;
    //         for (i = 0; i < m_arraySizeY; i++) {
    //             for (j = 0; j < m_arraySizeX; j++) {
    //                 expr += x[i][j][m][n];
    //             }
    //         }
    //         s = "cnstr_site[" + std::to_string(m) + "][" + std::to_string(n) + "]"; 
    //         model.addConstr(expr <= 1.0, s);
    //     }
    // }
    
    // // Set objectives.

    // int m0, m1, n0, n1, i0, i1, j0, j1;
    // GRBQuadExpr obj = 0;
    // for (i0 = 0; i0 < m_arraySizeY; i0++) {
    //     for (j0 = 0; j0 < m_arraySizeX; j0++) {
    //         // For each cell[i0][j0].

    //         for (i1 = 0; i1 < m_arraySizeY; i1++) {
    //             for (j1 = 0; j1 < m_arraySizeX; j1++) {
    //                 // For each cell[i1][j1].

    //                 // If cell[0] is connected with cell[1], then add the net length into the objective.

    //                 if (isConnected(i0, j0, i1, j1)) {

    //                     for (m0 = 0; m0 < m_siteSizeY; m0++) {
    //                         for (n0 = 0; n0 < m_siteSizeX; n0++) {
    //                             // For each site[m0][n0].

    //                             for (m1 = 0; m1 < m_siteSizeY; m1++) {
    //                                 for (n1 = 0; n1 < m_siteSizeX; n1++) {
    //                                     // For each site[m1][n1].

    //                                     // Calculate the distance of the two sites. (Net Length)
    //                                     int dist = manhDist(n0, m0, n1, m1);
    //                                     // Add into the obj.
    //                                     obj += 0.5 * dist * x[i0][j0][m0][n0] * x[i1][j1][m1][n1];
                                        
    //                                 }
    //                             }

    //                         }
    //                     }
    //                 }
    //             }
    //         }

    //     }
    // }

    // model.setObjective(obj, GRB_MINIMIZE);

    // // DBG("Optimize model..\n");
    // model.optimize();

    // DBG("Writing model..\n");
    // std::string fileName = "ILPSol/macroPl_" + std::to_string(m_arraySizeY) + "_" + std::to_string(m_arraySizeX) 
    //                         + "_to_" + std::to_string(m_siteSizeY) + "_" + std::to_string(m_siteSizeX) + "_run1"; 
    // try {
    //     model.write(fileName + ".sol");
    //     DBG("Solution written to %s\n", fileName.c_str());
    // } catch (GRBException e) {
    //     DBG("%s\n", e.getMessage().c_str());
    // }

    // DVD();
}

/**
 * @brief Another ILP routine with differenct formulation from run();
 * 
 */
void MacroPlacer::run2() {
    // DVD();
    // DBG("%s.\n", __func__);
    // DBG("Problem size: mapping %d x %d => %d x %d \n", m_arraySizeY, m_arraySizeX, m_siteSizeY, m_siteSizeX);
    printf("Problem size: mapping %d x %d => %d x %d \n", m_arraySizeY, m_arraySizeX, m_siteSizeY, m_siteSizeX);
    
    GRBEnv env = GRBEnv();
    GRBModel model = GRBModel(env);

    // Set time limit.
    if (m_timeLimit > 0) {
        model.set(GRB_DoubleParam_TimeLimit, m_timeLimit);
    }

    // Add decision variables.
    
    // DBG("Adding variables..\n");
    GRBVar x[m_arraySizeY][m_arraySizeX]; 
    GRBVar y[m_arraySizeY][m_arraySizeX]; 
    
    // dx[i0][j0][i1][j1] = x[i0][j0] - x[i1][j1]
    // dy[i0][j0][i1][j1] = y[i0][j0] - y[i1][j1]
    GRBVar dx[m_arraySizeY][m_arraySizeX][m_arraySizeY][m_arraySizeX];
    GRBVar dy[m_arraySizeY][m_arraySizeX][m_arraySizeY][m_arraySizeX];

    // absDx[i0][j0][i1][j1] = |dx[i0][j0][i1][j1]|
    // absDy[i0][j0][i1][j1] = |dy[i0][j0][i1][j1]|
    GRBVar absDx[m_arraySizeY][m_arraySizeX][m_arraySizeY][m_arraySizeX];
    GRBVar absDy[m_arraySizeY][m_arraySizeX][m_arraySizeY][m_arraySizeX];


    // Add constraints.

    int i, j, m, n;
    int i0, j0, i1, j1;
    std::string s, s_index;

    // 0 <= xi <= m_siteSizeX.
    // 0 <= yi <= m_siteSizeY.

    for (i = 0; i < m_arraySizeY; i++) {
        for (j = 0; j < m_arraySizeX; j++) {
            s = "X_" + std::to_string(i) + "_" + std::to_string(j);
            x[i][j] = model.addVar(0, m_siteSizeX - 1, 0, GRB_INTEGER, s);
            s = "Y_" + std::to_string(i) + "_" + std::to_string(j);
            y[i][j] = model.addVar(0, m_siteSizeY - 1, 0, GRB_INTEGER, s);
        }
    }

    // x0 <= x1, y0 <= y1 to remove mirrored solutions.
    model.addConstr(x[0][0] <= x[m_arraySizeY-1][m_arraySizeX-1], "no_mirror_x");
    model.addConstr(y[0][0] <= y[m_arraySizeY-1][m_arraySizeX-1], "no_mirror_y");


    // |x0 - x1| + |y0 - y1| >= 1 to make sure cell[0] and cell[1] don't overlap.
    // DBG("Setting constraints..\n");
    for (i0 = 0; i0 < m_arraySizeY; i0++) {
        for (j0 = 0; j0 < m_arraySizeX; j0++) {
            for (i1 = i0; i1 < m_arraySizeY; i1++) {
                for (j1 = 0; j1 < m_arraySizeX; j1++) {
                    
                    if (i0 == i1 && j0 >= j1) {
                        continue;
                    }
                    
                    s_index = "[" + std::to_string(i0) + "][" + std::to_string(j0) + "]_["
                        + std::to_string(i1) + "][" + std::to_string(j1) + "]";

                    dx[i0][j0][i1][j1] = model.addVar(-GRB_INFINITY, GRB_INFINITY, 0, GRB_INTEGER, "dx" + s_index);
                    model.addConstr(dx[i0][j0][i1][j1] == x[i0][j0] - x[i1][j1], "constr_dx" + s_index);

                    // DBG("AddVar: absDx[%d][%d][%d][%d]\n", i0, j0, i1, j1);
                    absDx[i0][j0][i1][j1] = model.addVar(0, GRB_INFINITY, 0, GRB_INTEGER, "absDx" + s_index);
                    model.addGenConstrAbs(absDx[i0][j0][i1][j1], dx[i0][j0][i1][j1], "constr_absDx" + s_index);

                    dy[i0][j0][i1][j1] = model.addVar(-GRB_INFINITY, GRB_INFINITY, 0, GRB_INTEGER, "dy" + s_index);
                    model.addConstr(dy[i0][j0][i1][j1] == y[i0][j0] - y[i1][j1], "constr_dy" + s_index);

                    // DBG("AddVar: absDy[%d][%d][%d][%d]\n", i0, j0, i1, j1);
                    absDy[i0][j0][i1][j1] = model.addVar(0, GRB_INFINITY, 0, GRB_INTEGER, "absDy" + s_index);
                    model.addGenConstrAbs(absDy[i0][j0][i1][j1], dy[i0][j0][i1][j1], "constr_absDy" + s_index);

                    model.addConstr(absDx[i0][j0][i1][j1] + absDy[i0][j0][i1][j1] >= 1, "no_overlap" + s_index);


                }
            }
        }
    } 


    if (m_relativeConstraintX || m_relativeConstraintY) {
        // set additional constraints on relative position.
        for (i = 0; i < m_arraySizeY; i++) {
            for (j = 0; j < m_arraySizeX; j++) {
                if (m_relativeConstraintX && j < m_arraySizeX - 1) {
                    s = "const_relativeX_" + std::to_string(i) + "_" + std::to_string(j);
                    model.addConstr(x[i][j] <= x[i][j+1], s);
                }
                if (m_relativeConstraintY && i < m_arraySizeY - 1) {
                    s = "const_relativeY_" + std::to_string(i) + "_" + std::to_string(j);
                    model.addConstr(y[i][j] <= y[i+1][j], s);
                }
            }
        }
    }


    // DBG("Setting Objective..\n");
    // objective

    GRBLinExpr objTotalWl = 0;

    for (i0 = 0; i0 < m_arraySizeY; i0++) {
        for (j0 = 0; j0 < m_arraySizeX; j0++) {
            
            // top neighbor.

            i1 = i0 + 1;
            j1 = j0;

            if (i1 < m_arraySizeY && j1 < m_arraySizeX) {
                // DBG("AddObj: absDx[%d][%d][%d][%d] + absDy[%d][%d][%d][%d]\n", i0, j0, i1, j1, i0, j0, i1, j1);
                objTotalWl += m_weightX * absDx[i0][j0][i1][j1] + m_weightY * absDy[i0][j0][i1][j1];
            }

            // right neighbor.
            i1 = i0;
            j1 = j0 + 1;

            if (i1 < m_arraySizeY && j1 < m_arraySizeX) {
                // DBG("AddObj: absDx[%d][%d][%d][%d] + absDy[%d][%d][%d][%d]\n", i0, j0, i1, j1, i0, j0, i1, j1);
                objTotalWl += m_weightX * absDx[i0][j0][i1][j1] + m_weightY * absDy[i0][j0][i1][j1];
            }
            
        }
    }

    // Set cell[0][0] to the lower-left corner if possible.
    objTotalWl += 0.01 * (x[0][0] + y[0][0]);

    // DBG("Setting Objective..\n");
    try {
        model.setObjective(objTotalWl, GRB_MINIMIZE);
        // assert(0);
    } catch (GRBException e) {
        // DBG("%s\n", e.getMessage().c_str());
    }

    // model.setObjective(objTotalWl, GRB_MINIMIZE);

    // DBG("Solve model..\n");

    model.optimize();

    // DBG("Optimize() done.\n");
    // DVD();

    // DBG("Writing model..\n");
    std::string fileName = "output/macroPl_" + std::to_string(m_arraySizeY) + "_" + std::to_string(m_arraySizeX) 
                            + "_to_" + std::to_string(m_siteSizeY) + "_" + std::to_string(m_siteSizeX); 
        
    // Relative position constraints in X/Y direction.
    fileName += "_rpXY_" + std::to_string(m_relativeConstraintX) + "_" + std::to_string(m_relativeConstraintY);

    // Weights in X/Y direction.
    fileName += "_wtXY_" + std::to_string(m_weightX) + "_" + std::to_string(m_weightY);

    try {
        model.write(fileName + ".sol");
        // DBG("Solution written to %s\n", fileName.c_str());
    } catch (GRBException e) {
        // DBG("%s\n", e.getMessage().c_str());
    }
    // model.write(fileName + "pl");
    // model.write(fileName + "sol");
    // model.write(fileName + "json");
}

/**
 * @brief Mapping an m x n array into one column.
 * 
 */
void MacroPlacer::run3() {
    printf("Problem size: mapping %d x %d => %d x %d \n", m_arraySizeY, m_arraySizeX, m_siteSizeY, m_siteSizeX);
    if (m_siteSizeX != 1) {
        printf("WRN: %s is only used for mapping into one column! Function stops.\n", __func__);
        return;
    }
    
    GRBEnv env = GRBEnv();
    GRBModel model = GRBModel(env);

    // Set time limit.
    if (m_timeLimit > 0) {
        model.set(GRB_DoubleParam_TimeLimit, m_timeLimit);
    }

    // Add decision variables.
    
    // DBG("Adding variables..\n");
    // GRBVar x[m_arraySizeY][m_arraySizeX]; 
    GRBVar y[m_arraySizeY][m_arraySizeX]; 
    
    // // dx[i0][j0][i1][j1] = x[i0][j0] - x[i1][j1]
    // // dy[i0][j0][i1][j1] = y[i0][j0] - y[i1][j1]
    // GRBVar dx[m_arraySizeY][m_arraySizeX][m_arraySizeY][m_arraySizeX];
    // GRBVar dy[m_arraySizeY][m_arraySizeX][m_arraySizeY][m_arraySizeX];

    // // absDx[i0][j0][i1][j1] = |dx[i0][j0][i1][j1]|
    // // absDy[i0][j0][i1][j1] = |dy[i0][j0][i1][j1]|
    // GRBVar absDx[m_arraySizeY][m_arraySizeX][m_arraySizeY][m_arraySizeX];
    // GRBVar absDy[m_arraySizeY][m_arraySizeX][m_arraySizeY][m_arraySizeX];


    // Add constraints.

    int i, j, m, n;
    int i0, j0, i1, j1;
    std::string s, s_index;

    // 0 <= xi <= m_siteSizeX.
    // 0 <= yi <= m_siteSizeY.

    for (i = 0; i < m_arraySizeY; i++) {
        for (j = 0; j < m_arraySizeX; j++) {
            // s = "X_" + std::to_string(i) + "_" + std::to_string(j);
            // x[i][j] = model.addVar(0, m_siteSizeX - 1, 0, GRB_INTEGER, s);
            s = "Y_" + std::to_string(i) + "_" + std::to_string(j);
            y[i][j] = model.addVar(0, m_siteSizeY - 1, 0, GRB_INTEGER, s);
        }
    }

    // x0 <= x1, y0 <= y1 to remove mirrored solutions.
    // model.addConstr(x[0][0] <= x[m_arraySizeY-1][m_arraySizeX-1], "no_mirror_x");
    model.addConstr(y[0][0] <= y[m_arraySizeY-1][m_arraySizeX-1], "no_mirror_y");


    // No-overlap constraint (NOC) and relative ordering constraint (ROC):
    // NOC: For each pair, y0 != y1. 
    // ROC: If cell 0 and cell 1 are in the same row or column in the array, y0 + 1 <= y1.
    // If the ROC is applied, we can skip the NOC as it is implicitly satisfied. 

    // There are two methods to add the NOC: (y0 != y1) => ( abs(y0-y1) >= 1 )

    // 0: 
    // dy = y1 - y0;
    // dyAbs = abs(dy);
    // dyAbs >= 1;

    // 1:
    // b0 == 1 if y0 - y1 >= 1; 
    // b1 == 1 if y1 - y0 >= 1; 
    // b = b0 OR b1;
    // b == True;

    // Toggle between these two methods by assigning values to this variable:
    int NOCMode = 0;

    GRBVar dy[m_arraySizeY][m_arraySizeX][m_arraySizeY][m_arraySizeX];
    GRBVar dyAbs[m_arraySizeY][m_arraySizeX][m_arraySizeY][m_arraySizeX];

    GRBVar bList[m_arraySizeY][m_arraySizeX][m_arraySizeY][m_arraySizeX][2];
    // GRBVar b1[m_arraySizeY][m_arraySizeX][m_arraySizeY][m_arraySizeX];
    GRBVar b[m_arraySizeY][m_arraySizeX][m_arraySizeY][m_arraySizeX];

    // Add the NOC and ROC is enabled. 
    for (i0 = 0; i0 < m_arraySizeY; i0++) {
        for (j0 = 0; j0 < m_arraySizeX; j0++) {
            for (i1 = i0; i1 < m_arraySizeY; i1++) {
                for (j1 = 0; j1 < m_arraySizeX; j1++) {

                    if (i0 == i1 && j0 >= j1) {
                        continue;
                    }

                    // Double for-loop: for each cell 0 and cell 1 that cell0.row <= cell1.row, and cell0.col < cell1.col.

                    s_index = "[" + std::to_string(i0) + "][" + std::to_string(j0) + "]_["
                        + std::to_string(i1) + "][" + std::to_string(j1) + "]";
                    
                    bool enNOC = true; // By default, NOC is enabled.
                    
                    // Add ROC if enabled.

                    if (m_relativeConstraintY) {

                        // Since (y2 >= y1 + 1 and y1 >= y0 + 1) implies (y2 >= y0 + 2),
                        // we only add ROC for neighbors in the same row or column.

                        // ROC for neighbors in the same row.
                        if ((i0 == i1) && (j0 + 1 == j1)) {
                            s = "ROC_" + s_index;
                            model.addConstr(y[i0][j0] + 1 <= y[i1][j1], s);
                        }

                        // ROC for neighbors in the same column.
                        if ((j0 == j1) && (i0 + 1 == i1)) {
                            s = "ROC_" + s_index;
                            model.addConstr(y[i0][j0] + 1 <= y[i1][j1], s);
                        }

                        // NOC is not needed for the cells in the same row or column (for both neighbors and non-neighbors).
                        if ((i0 == i1) || (j0 == j1)) {
                            enNOC = false;
                        }
                    }

                    // Add NOC if needed.

                    if (enNOC) {
                        if (NOCMode == 0) {
                            // dy = y0 - y1;
                            dy[i0][j0][i1][j1] = model.addVar(-GRB_INFINITY, GRB_INFINITY, 0, GRB_INTEGER, "dy" + s_index);
                            model.addConstr(dy[i0][j0][i1][j1] == y[i0][j0] - y[i1][j1], "constr_dy" + s_index);

                            // dyAbs = abs(dy);
                            dyAbs[i0][j0][i1][j1] = model.addVar(0, GRB_INFINITY, 0, GRB_INTEGER, "absDy" + s_index);
                            model.addGenConstrAbs(dyAbs[i0][j0][i1][j1], dy[i0][j0][i1][j1], "constr_absDy" + s_index);

                            // dyAbs >= 1;
                            model.addConstr(dyAbs[i0][j0][i1][j1] >= 1, "no_overlap" + s_index);
                        }
                        else if (NOCMode == 1) {
                            // b0 == true if y0 - y1 >= 1; 
                            bList[i0][j0][i1][j1][0] = model.addVar(0, 1, 0, GRB_BINARY, s); 
                            model.addGenConstrIndicator(bList[i0][j0][i1][j1][0], true, y[i0][j0] - y[i1][j1] >= 1);
                            
                            // b1 == 1 if y1 - y0 >= 1; 
                            bList[i0][j0][i1][j1][1] = model.addVar(0, 1, 0, GRB_BINARY, s); 
                            model.addGenConstrIndicator(bList[i0][j0][i1][j1][1], true, y[i1][j1] - y[i0][j0] >= 1);

                            // b == b0 OR b1;
                            model.addGenConstrOr(b[i0][j0][i1][j1], bList[i0][j0][i1][j1], 2);

                            // b == True;
                            model.addConstr(b[i0][j0][i1][j1] == true);
                        }
                        else {
                            printf("ERR: Unexpected NOCMode: %d.\n", NOCMode);
                            // assert(false);
                        }
                    }

                }
            }
        }
    } 



    // // |x0 - x1| + |y0 - y1| >= 1 to make sure cell[0] and cell[1] don't overlap.
    // // DBG("Setting constraints..\n");
    // for (i0 = 0; i0 < m_arraySizeY; i0++) {
    //     for (j0 = 0; j0 < m_arraySizeX; j0++) {
    //         for (i1 = i0; i1 < m_arraySizeY; i1++) {
    //             for (j1 = 0; j1 < m_arraySizeX; j1++) {
                    
    //                 if (i0 == i1 && j0 >= j1) {
    //                     continue;
    //                 }
                    
    //                 s_index = "[" + std::to_string(i0) + "][" + std::to_string(j0) + "]_["
    //                     + std::to_string(i1) + "][" + std::to_string(j1) + "]";

    //                 // dx[i0][j0][i1][j1] = model.addVar(-GRB_INFINITY, GRB_INFINITY, 0, GRB_INTEGER, "dx" + s_index);
    //                 // model.addConstr(dx[i0][j0][i1][j1] == x[i0][j0] - x[i1][j1], "constr_dx" + s_index);

    //                 // DBG("AddVar: absDx[%d][%d][%d][%d]\n", i0, j0, i1, j1);
    //                 absDx[i0][j0][i1][j1] = model.addVar(0, GRB_INFINITY, 0, GRB_INTEGER, "absDx" + s_index);
    //                 model.addGenConstrAbs(absDx[i0][j0][i1][j1], dx[i0][j0][i1][j1], "constr_absDx" + s_index);

    //                 dy[i0][j0][i1][j1] = model.addVar(-GRB_INFINITY, GRB_INFINITY, 0, GRB_INTEGER, "dy" + s_index);
    //                 model.addConstr(dy[i0][j0][i1][j1] == y[i0][j0] - y[i1][j1], "constr_dy" + s_index);

    //                 // DBG("AddVar: absDy[%d][%d][%d][%d]\n", i0, j0, i1, j1);
    //                 absDy[i0][j0][i1][j1] = model.addVar(0, GRB_INFINITY, 0, GRB_INTEGER, "absDy" + s_index);
    //                 model.addGenConstrAbs(absDy[i0][j0][i1][j1], dy[i0][j0][i1][j1], "constr_absDy" + s_index);

    //                 model.addConstr(absDx[i0][j0][i1][j1] + absDy[i0][j0][i1][j1] >= 1, "no_overlap" + s_index);


    //             }
    //         }
    //     }
    // } 


    // // if (m_relativeConstraintX || m_relativeConstraintY) {
    // if (true) {
    //     // set additional constraints on relative position.
    //     for (i = 0; i < m_arraySizeY; i++) {
    //         for (j = 0; j < m_arraySizeX; j++) {
    //             if (m_relativeConstraintX && j < m_arraySizeX - 1) {
    //                 s = "const_relativeX_" + std::to_string(i) + "_" + std::to_string(j);
    //                 model.addConstr(y[i][j] + 1 <= y[i][j+1], s);
    //             }
    //             if (i < m_arraySizeY - 1) {
    //                 s = "const_relativeY_" + std::to_string(i) + "_" + std::to_string(j);
    //                 model.addConstr(y[i][j] + 1 <= y[i+1][j], s);
    //             }
    //         }
    //     }
    // }


    // DBG("Setting Objective..\n");
    // objective

    GRBLinExpr objTotalWl = 0;

    // When relative constraints are satisfied, total WL only depends on the coordinates of the cells on the boundaries of the PE array.

    // Top and bottom boundaries.
    for (j = 0; j < m_arraySizeX; j++) {
        
        i = m_arraySizeY - 1;
        objTotalWl += y[i][j];

        i = 0;
        objTotalWl -= y[i][j];
    }

    // Left and right boundaries.
    for (i = 0; i < m_arraySizeY; i++) {

        j = 0;
        objTotalWl -= y[i][j];

        j = m_arraySizeX - 1;
        objTotalWl += y[i][j];
    }
    // objTotalWl += y[0][0];

    // Set cell[0][0] to the lower-left corner if possible.
    // objTotalWl += 0.01 * (x[0][0] + y[0][0]);

    printf("Setting Objective..\n");
    try {
        model.setObjective(objTotalWl, GRB_MINIMIZE);
        printf("Setting Objective..\n");
        // assert(0);
    } catch (GRBException e) {
        printf("%s\n", e.getMessage().c_str());
    }

    // model.setObjective(objTotalWl, GRB_MINIMIZE);

    printf("Solve model..\n");

    model.optimize();

    // DBG("Optimize() done.\n");
    // DVD();

    // DBG("Writing model..\n");
    std::string fileName = "output/macroPl_" + std::to_string(m_arraySizeY) + "_" + std::to_string(m_arraySizeX) 
                            + "_to_" + std::to_string(m_siteSizeY) + "_" + std::to_string(m_siteSizeX); 
        
    // Relative position constraints in X/Y direction.
    fileName += "_rpXY_" + std::to_string(m_relativeConstraintX) + "_" + std::to_string(m_relativeConstraintY);

    // Weights in X/Y direction.
    fileName += "_wtXY_" + std::to_string(m_weightX) + "_" + std::to_string(m_weightY);

    try {
        model.write(fileName + ".sol");
        // DBG("Solution written to %s\n", fileName.c_str());
    } catch (GRBException e) {
        // DBG("%s\n", e.getMessage().c_str());
    }
    // model.write(fileName + "pl");
    // model.write(fileName + "sol");
    // model.write(fileName + "json");
}

/**
 * @brief Run n problems at once. 
 * 
 */
void MacroPlacer::runBatch() {

    setRelativeConstraintXY(1, 1);
    setXYWeight(1, 1);

    // setProblemSize(5, 5, 25, 3); 
    // run2();

    // setProblemSize(5, 5, 25, 2); 
    // run2();

    // Try this:
    // setProblemSize(5, 3, 15, 2);
    // run2();

    // setProblemSize(6, 3, 15, 2);
    // run2();

    // setProblemSize(7, 3, 20, 2);
    // run2();

    setProblemSize(8, 3, 20, 2);
    run2();

    // setXYWeight(2, 1);
    // run2();

    // setXYWeight(4, 1);
    // run2();

    // setXYWeight(8, 1);
    // run2();

    // setProblemSize(6, 4, 24, 3); 

    // setXYWeight(1, 1);
    // run2();

    // setXYWeight(2, 1);
    // run2();

    // setXYWeight(4, 1);
    // run2();

    // setXYWeight(8, 1);
    // run2();


    // setProblemSize(5, 3, 15, 2); 
    // run2();
    // setProblemSize(3, 3, 5, 2); 
    // run2();
    // setProblemSize(3, 3, 5, 3); 
    // run2();
    // setProblemSize(6, 3, 20, 2); 
    // run2();

    // setProblemSize(4, 4, 20, 1); 
    // run2();
    // setProblemSize(4, 4, 10, 2); 
    // run2();
    // setProblemSize(4, 4, 10, 4); 
    // run2();
    // setProblemSize(4, 4, 10, 3); 
    // run2();

    // setProblemSize(5, 4, 10, 3); 
    // run();
    // // run2();
    // setProblemSize(6, 4, 12, 3); 
    // run();
    // run2();
    // setProblemSize(7, 4, 15, 3); 
    // run2();
    // run2();
    // setProblemSize(8, 4, 20, 3); 
    // run2();


    // setProblemSize(5, 5, 25, 1); 
    // run2();
    // setProblemSize(5, 5, 15, 2); 
    // run2();
    // setProblemSize(5, 5, 10, 3); 
    // run2();

    // setProblemSize(5, 5, 10, 4); 
    // run2();
    // setProblemSize(5, 5, 8, 5); 
    // run2();

    return;

    setProblemSize(8, 8, 80, 1); 
    run2();
    setProblemSize(8, 8, 40, 2); 
    run2();
    setProblemSize(8, 8, 30, 3); 
    run2();
    setProblemSize(8, 8, 20, 4); 
    run2();
    setProblemSize(8, 8, 20, 5); 
    run2();
    setProblemSize(8, 8, 20, 6); 
    run2();
    setProblemSize(8, 8, 20, 7); 
    run2();
    setProblemSize(8, 8, 20, 8); 
    run2();
}

/**
 * @brief Run in batch mode. Jobs are specified in file <batchFileName>.
 * 
 * @param batchFileName 
 */
void MacroPlacer::runBatchFromFile(const std::string batchFileName) {
    // Read the batch file.
    std::ifstream fs(batchFileName);
    if (!fs.good()) {
        printf("ERR: Read file [%s] failed!\n", batchFileName.c_str());
    }
    std::vector<std::string> tokens;
    while (read_line_as_tokens(fs, tokens)) {
        // Skip lines starting with "#".
        if (strncmp(tokens[0].c_str(), "#", 1) == 0) {
        // if (strcmp(tokens[0], "#") == 0) {
            continue;
        }
        else if (tokens.size() == 9) {
            m_jobList.emplace_back(tokens[0],stoi(tokens[1]),stoi(tokens[2]),stoi(tokens[3]),stoi(tokens[4]),stod(tokens[5]),stod(tokens[6]),stoi(tokens[7]),stoi(tokens[8]));
        }
        else if (tokens.size() == 10) {
            m_jobList.emplace_back(tokens[0],stoi(tokens[1]),stoi(tokens[2]),stoi(tokens[3]),stoi(tokens[4]),stod(tokens[5]),stod(tokens[6]),stoi(tokens[7]),stoi(tokens[8]), stod(tokens[9]));
        }
        else {
            printf("ERR: Unexpected input length: %d", tokens.size());
        }
    }
    printf("Job size: %d\n", m_jobList.size());

    fs.close();

    // Run jobs.
    runJobs();

    printf("Job List Finished.\n");
    printf("--------------------------------\n");
}

void MacroPlacer::runJobs() {
    for (const JOB &job: m_jobList) {
        setProblemSize(job.arraySizeY, job.arraySizeX, job.siteSizeY, job.siteSizeX);
        setXYWeight(job.weightX, job.weightY);
        setRelativeConstraintXY(job.relativeConstraintX, job.relativeConstraintY);
        setTimeLimit(job.timeLimit);

        printf("--------------------------------\n");
        printf("Run Job [%s]..\n", job.name.c_str());

        if (job.siteSizeX == 1) {
            run3();
        }
        else {
            run2();
        }

        printf("--------------------------------\n");

    }
}


/**
 * @brief cost of flow (connection) between cell[i] and cell[j] 
 * 
 * @param i 
 * @param j 
 * @return int 
 */
int MacroPlacer::flow(int i, int j) {
    return 0;
}

/**
 * @brief Check if cell 0 (array[row0][col0]) and cell 1 (array[row1][col1]) is connected.
 * 
 * @param row0 
 * @param col0 
 * @param row1 
 * @param col1 
 * @return true 
 * @return false 
 */
bool MacroPlacer::isConnected(int row0, int col0, int row1, int col1) {

    #ifdef DEBUG
    // Assert(row0 >= 0);
    // Assert(row0 < m_arraySizeY);

    // Assert(col0 >= 0);
    // Assert(col0 < m_arraySizeX);

    // Assert(row1 >= 0);
    // Assert(row1 < m_arraySizeY);

    // Assert(col1 >= 0);
    // Assert(col1 < m_arraySizeX);
    #endif

    return (manhDist(col0, row0, col1, row1) == 1);

}

/**
 * @brief Manhatan distance between (x0,y0) and (x1,y1).
 * 
 * @param x0 
 * @param y0 
 * @param x1 
 * @param y1 
 * @return int 
 */
int MacroPlacer::manhDist(int x0, int y0, int x1, int y1) {
    return abs(x0-x1) + abs(y0-y1);
}

void MacroPlacer::placeAndFixDSP() {
    // DBG("%s...\n", __func__);
    setProblemSizeFromNetlist();
    fillDspIdArray();
    // dbg_printDspIdArray();
    placeDspInterleaved();
    // generateTclForDsp();
    ;
}

/**
 * @brief Set the problem size from netlist, which is parsed from design.rglr.
 * 
 */
void MacroPlacer::setProblemSizeFromNetlist() {
    // DBG("%s...\n", __func__);
    // Assert()
    // setProblemSize(m_nl.m_rowsOfPeArray, m_nl.m_colsOfPeArray, m_nl.m_rowsOfPeLayout, m_nl.m_colsOfPeLayout);
}


/**
 * @brief Fill m_dspIdArray with node ID from netlist.
 * 
 */
void MacroPlacer::fillDspIdArray() {
    // DBG("%s...\n", __func__);

    // m_dspIdArray.resize(m_arraySizeX, m_arraySizeY, INDEX_TYPE_MAX);

    // for (Node &nd: m_nl.nodeArray()) {
    //     if (isDSP(nd.nodeType())) {
    //         IndexType rowIdx = nd.rowIdxNl();
    //         IndexType colIdx = nd.colIdxNl();

    //         #ifdef DEBUG
    //         Assert(m_dspIdArray.at(colIdx, rowIdx) == INDEX_TYPE_MAX);
    //         #endif

    //         m_dspIdArray.at(colIdx, rowIdx) = nd.id();
    //     }
    // }
}

/**
 * @brief Assign the DSPs to each column in an interleaved way.
 * 
 */
void MacroPlacer::placeDspInterleaved() {
    // DBG("%s...\n", __func__);

    // // Only 8x8 => 16x4, 16x16 => 64x4 are supported at this moment.

    // // 
    // // Assign a m * n array to p * q sites.   
    // // Currently we only support evenly split, i.e., n mod q == 0.
    // // Assert(m_arraySizeX % m_siteSizeX == 0);
    // if (m_arraySizeX % m_siteSizeX == 0) {
    //     int colFactor = m_arraySizeX / m_siteSizeX;

    //     for (int blockIdx = 0; blockIdx < m_siteSizeX; blockIdx++) {
    //         for (int x = blockIdx * colFactor; x < (blockIdx+1) * colFactor; x++) {
    //             for (int y = 0; y < m_arraySizeY; y++) {
    //                 int rowIdx = x - blockIdx * colFactor + y * colFactor;

    //                 // dsp from array[x][y] is assigned to site array at col = blockIdx, row = rowIdx.

    //                 int coordX = m_db.colDSP.at(blockIdx);
    //                 int coordY = m_db.rowDSP.at(rowIdx);
    //                 int id = m_dspIdArray.at(x, y);

    //                 Node &nd = m_nl.node(id);
    //                 nd.setX(coordX);
    //                 nd.setY(coordY);
    //                 nd.setFixed(true);

    //                 DBG("DSP at <col = %2d, row = %2d> is placed to (%3d, %3d). %s\n", x, y, coordX, coordY, nd.name().c_str());
    //             }
    //         }
    //     }
    // }
    
    // else {

    // }
}





/**
 * @brief Print all DSPs from the netlist along with its colIdx and rowIdx.
 * 
 */
void MacroPlacer::dbg_printDSPInNetlist() {
    // DBG("%s...\n", __func__);
    // for (Node &nd: m_nl.nodeArray()) {
    //     if (isDSP(nd.nodeType())) {
    //         DBG("node<%s>: col: %d, row: %d.\n", nd.name().c_str(), nd.colIdxNl(), nd.rowIdxNl());
    //     }
    // }
}

void MacroPlacer::dbg_printDspIdArray() {
    // DBG("%s...\n", __func__);
    // for (int x = 0; x < m_dspIdArray.xSize(); x++) {
    //     for (int y = 0; y < m_dspIdArray.ySize(); y++) {
    //         IndexType id = m_dspIdArray.at(x, y);
    //         if (id == INDEX_TYPE_MAX) {
    //             WRN("m_dspIdArray[%2d][%2d]: id == %d!. \n", x, y, id);
    //         }
    //         else {
    //             const Node &nd = m_nl.node(id);
    //             DBG("m_dspIdArray[%2d][%2d]: <%s>. \n", x, y, nd.name().c_str());
    //         }
    //     }
    // }
}


