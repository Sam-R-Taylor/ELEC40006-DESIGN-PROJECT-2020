#ifndef OPANALYSIS_HPP
#define OPANALYSIS_HPP

#include <vector>
#include <iostream>
#include <eigen3/Eigen/Dense>
#include "Component.hpp"
#include "Circuit.hpp"
#include "TransientSolver.hpp"
#include "KCLSolver.hpp"
#include <memory> 

void OPAnalysis(Circuit &circuit){
    if(IsLinear(circuit)){
        Matrix_solver(circuit,true);
    }else{
        TransientSolver(circuit,true);
    }
    for(auto v: circuit.get_voltages()){
        std::cout << v << std::endl;
    }
}

#endif