#ifndef __ILPSOLVER_H__
#define __ILPSOLVER_H__

#include "gurobi_c++.h"



 
class ILPSolver
{    
public:
    int exampleGurobiOptimization();
    int exampleMipGurobi();


};


class MacroPlacer
{
public:
    // explicit MacroPlacer(Database &db, Netlist &nl) 
    //     : m_db(db), m_nl(nl)
    // {}
    // MacroPlacer ();


    // For trials using ILP.
    void    setProblemSize(int arraySizeY, int arraySizeX, int siteSizeY, int sizeSizeX); // TBF
    void    setXYWeight(int weightX, int weightY);
    void    setRelativeConstraintX(bool b);
    void    setRelativeConstraintY(bool b);
    void    setRelativeConstraintXY(bool bx, bool by);
    void    run();
    void    run2();
    void    runBatch();

    // DSP placement prior to global placement.
    void    placeAndFixDSP();




private:

    int     flow(int i, int j);
    bool    isConnected(int row0, int col0, int row1, int col1);
    int     manhDist(int x0, int y0, int x1, int y1);

    void    setProblemSizeFromNetlist();
    void    fillDspIdArray();
    void    placeDspInterleaved();


    void    dbg_printDSPInNetlist();
    void    dbg_printDspIdArray();


private:
    // Database & m_db;
    // Netlist & m_nl;


    int m_arraySizeX = 0;
    int m_arraySizeY = 0;
    int m_siteSizeX = 0;
    int m_siteSizeY = 0; 

    int m_weightX = 1;
    int m_weightY = 1;

    bool m_relativeConstraintX = false;
    bool m_relativeConstraintY = false;

    // Vector2D<IndexType> m_dspIdArray;
};



#endif