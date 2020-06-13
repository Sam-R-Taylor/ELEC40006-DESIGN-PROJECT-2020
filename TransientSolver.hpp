#ifndef TRANSIENTSOLVER_HPP
#define TRANSIENTSOLVER_HPP

#include <vector>
#include <iostream>
#include <eigen3/Eigen/Dense>
#include <string>
#include "Component.hpp"
#include"Circuit.hpp"
#include <memory> 
#include "KCLSolver.hpp"
#include "SparseKCLSolver.hpp"
using Eigen::MatrixXd;

void TransientSolver(Circuit &circuit, bool OP = false){
    bool incomplete = true;
    int current_iteration = 0;
    //iterate adjusting voltages each time
    while(incomplete){
        current_iteration++;
        //loop through the components
        for(Component* component: circuit.get_components()){
            //adjust all the diodes for the current voltage guess
            if(dynamic_cast<Diode*>(component)){
                //((Diode*)component)->set_vd(circuit.get_voltages()[component->get_anode()] - circuit.get_voltages()[component->get_cathode()]);
                ((Diode*)component)->set_vd(circuit.get_voltages());
                ((Diode*)component)->set_id0(((Diode*)component)->get_current(circuit.get_voltages()));
                ((Diode*)component)->set_conductance();
            }
            //adjust all the bjts for the current voltage guess
            if(dynamic_cast<BJT*>(component)){
                ((BJT*)component)->set_op(circuit.get_voltages());
            }
        }
        //set the voltages to the output of the KCL with the components
        std::vector<double> old_voltages = circuit.get_voltages(); 
        Matrix_solver(circuit,OP); 
        //for(auto x: circuit.get_voltages()){
        //    std::cout << x << std::endl;
        //}  
        //set the stored voltages to the new voltages
        incomplete = false;
        for(int i = 0; i < old_voltages.size(); i++){
            if(!(abs(old_voltages[i] - circuit.get_voltages()[i]) < circuit.ABSTOL + circuit.RELTOL*abs(old_voltages[i]))){
                incomplete = true;
            }
        }
        //check that max iterations haven't occured
        if(current_iteration >= circuit.max_iterations){
            std::cout << "max iterations" << std::endl;
            exit(1);
            incomplete = false;
        }
    }
}

#endif
