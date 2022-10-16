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
    struct JOB {
        JOB(const std::string name, int arrSzY, int arrSzX, int stSzY, int stSzX, int wtX, int wtY, bool rltCstrX, bool rltCstrY) :
            name(name), arraySizeY(arrSzY), arraySizeX(arrSzX), siteSizeY(stSzY), siteSizeX(stSzX), 
            weightX(wtX), weightY(wtY), relativeConstraintX(rltCstrX), relativeConstraintY(rltCstrY) {}

        JOB(const std::string name, int arrSzY, int arrSzX, int stSzY, int stSzX, int wtX, int wtY, bool rltCstrX, bool rltCstrY, double timeLimit) :
            name(name), arraySizeY(arrSzY), arraySizeX(arrSzX), siteSizeY(stSzY), siteSizeX(stSzX), 
            weightX(wtX), weightY(wtY), relativeConstraintX(rltCstrX), relativeConstraintY(rltCstrY), timeLimit(timeLimit) {}

        std::string     name;
        int             arraySizeY;
        int             arraySizeX;
        int             siteSizeY;
        int             siteSizeX;
        double          weightX = 1;
        double          weightY = 1;
        bool            relativeConstraintX = 0;
        bool            relativeConstraintY = 0;
        double          timeLimit = -1;

    };

    // For trials using ILP.
    void    setProblemSize(int arraySizeY, int arraySizeX, int siteSizeY, int sizeSizeX); // TBF
    void    setXYWeight(int weightX, int weightY);
    void    setRelativeConstraintX(bool b);
    void    setRelativeConstraintY(bool b);
    void    setRelativeConstraintXY(bool bx, bool by);
    void    setTimeLimit(double timeLimit);
    void    run();
    void    run2();
    void    run3();
    void    runBatch();
    void    runBatchFromFile(const std::string batchFileName);
    void    runJobs();

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

    double m_timeLimit = -1;

    // Vector2D<IndexType> m_dspIdArray;
    std::vector<JOB> m_jobList;
};



#endif