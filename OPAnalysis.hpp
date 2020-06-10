#include <vector>
#include <iostream>
#include <eigen3/Eigen/Dense>
#include <string>
#include "Component.hpp"
#include <memory> 
#include "Circuit.hpp"
#include <fstream>
#include "TransientSolver.hpp"
#include "leo_KCLSolver2.hpp"
using Eigen::MatrixXd;

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