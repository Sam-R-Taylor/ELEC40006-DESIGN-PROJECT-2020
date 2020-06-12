#ifndef OPANALYSIS_HPP
#define OPANALYSIS_HPP

#include <vector>
#include <iostream>
#include <eigen3/Eigen/Dense>
#include "Component.hpp"
#include "Circuit.hpp"
#include "TransientSolver.hpp"
#include "SparseKCLSolver.hpp"
#include "AnalysisOutput.hpp"
#include "KCLSolver.hpp"
#include <memory> 

void OPAnalysis(Circuit &circuit){
    //remove and recreate output files
    remove("output_voltage.txt");
    fstream myfile ("output_voltage.txt");
    remove("output_current.txt");
    fstream myfile2 ("output_current.txt");
    //solve circuit voltages
    if(IsLinear(circuit)){
        Sparse_Matrix_solver(circuit,true);
    }else{
        TransientSolver(circuit,true);
    }
    //solve circuit currents
    Calculate_currents(circuit,true);
    //print the voltages and currents to files setting current time to -1 tells output not to print time
    NodeVoltagesToFile(circuit,-1);
}

#endif