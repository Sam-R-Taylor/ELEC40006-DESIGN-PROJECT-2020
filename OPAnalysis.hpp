#ifndef OPANALYSIS_HPP
#define OPANALYSIS_HPP

#include <vector>
#include <iostream>
#include <eigen3/Eigen/Dense>
#include "Component.hpp"
#include "Circuit.hpp"
#include "TransientSolver.hpp"
#include "KCLSolver.hpp"
#include "AnalysisOutput.hpp"
#include <memory> 

void OPAnalysis(Circuit &circuit){
    //remove and recreate output files
    remove("output_voltage.txt");
    fstream myfile ("output_voltage.txt");
    remove("output_current.txt");
    fstream myfile2 ("output_current.txt");

    //solve circuit voltages
    if(IsLinear(circuit)){
        Matrix_solver(circuit,true);
    }else{
        TransientSolver(circuit,true);
    }
    //solve circuit currents
    Calculate_currents(circuit);
    //print the voltages and currents to files setting current time to -1 tells output not to print time
    NodeVoltagesToFile(circuit,-1);
    for(auto v: circuit.get_voltages()){
        std::cout << v << std::endl;
    }
}

#endif