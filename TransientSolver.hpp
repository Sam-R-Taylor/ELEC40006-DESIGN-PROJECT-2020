#ifndef TRANSIENTSOLVER_HPP
#define TRANSIENTSOLVER_HPP


#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <string>
#include "Component.hpp"
#include"Circuit.hpp"
#include "KCLSolver.hpp"
#include <memory> 
using Eigen::MatrixXd;


void TransientSolver(Circuit &circuit){
    bool incomplete = true;
    int current_iteration = 0;
    //iterate adjusting voltages each time
    while(incomplete){
        current_iteration++;
        //loop through the components
        for(Component* component: circuit.get_components()){
            //adjust all the diodes for the current voltage guess
            if(dynamic_cast<Diode*>(component)){
                ((Diode*)component)->set_vd(circuit.get_voltages()[component->get_anode()] - circuit.get_voltages()[component->get_cathode()]);
                ((Diode*)component)->set_id0(((Diode*)component)->get_current(circuit.get_voltages()));
            }
            if(dynamic_cast<BJT_Component*>(component)){
                //std::cout << "adjusting bjt" << std::endl;
                ((BJT_Component*)component)->set_op(circuit.get_voltages());
            }
        }
        //set the voltages to the output of the KCL with the components
        std::vector<double> old_voltages = circuit.get_voltages(); 
        for(auto x: old_voltages){
            std::cout << x << std::endl;
        }   
        NodeVoltageSolver(circuit);
        //check the error
        
        //set the stored voltages to the new voltages
        incomplete = false;
        for(int i = 0; i < old_voltages.size(); i++){
            if(!(abs(old_voltages[i] - circuit.get_voltages()[i]) < circuit.ABSTOL + circuit.RELTOL*old_voltages[i])){
                incomplete = true;
            }
        }
        //check that max iterations haven't occured
        if(current_iteration >= circuit.max_iterations){
            std::cerr << "Hit maximum iterations" << std::endl;
            exit(1);
            incomplete = false;
        }
        //std::vector<double> v = circuit.get_voltages();
        //for(auto x: v){
        //    std::cout << x << std::endl;
        //}
    }
    //std::cout << "Iterations " << current_iteration << std::endl;
}

#endif