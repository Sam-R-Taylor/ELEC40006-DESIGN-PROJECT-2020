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
#include "leo_KCLSolver2.hpp"
using Eigen::MatrixXd;


void TransientSolver(Circuit &circuit){
    //std::cout<< "in transient solver " << std::endl;
    bool incomplete = true;
    int current_iteration = 0;
    //iterate adjusting voltages each time
    while(incomplete){
        current_iteration++;
        //std::cout<< "in itterater " << std::endl;
        //loop through the components
        for(Component* component: circuit.get_components()){
            //adjust all the diodes for the current voltage guess
            if(dynamic_cast<Diode*>(component)){
                //std::cout<< "in diode " << std::endl;
                ((Diode*)component)->set_vd(circuit.get_voltages()[component->get_anode()] - circuit.get_voltages()[component->get_cathode()]);
                ((Diode*)component)->set_id0(((Diode*)component)->get_current(circuit.get_voltages()));
                //std::cout<< "out of diode " << std::endl;
            }
            //adjust all the bjts for the current voltage guess
            if(dynamic_cast<BJT*>(component)){
                //std::cout << "adjusting bjt" << std::endl;
                ((BJT*)component)->set_op(circuit.get_voltages());
            }
        }
        //set the voltages to the output of the KCL with the components
        std::vector<double> old_voltages = circuit.get_voltages(); 
        std::cout <<"old voltages are" << std::endl;
        for(auto x: old_voltages){          
            std::cout << x << std::endl;
        }   
        //Matrix_solver(circuit);
        NodeVoltageSolver(circuit);
        //check the error
        
        //set the stored voltages to the new voltages
        incomplete = false;
        for(int i = 0; i < old_voltages.size(); i++){
            if(!(abs(old_voltages[i] - circuit.get_voltages()[i]) < circuit.ABSTOL + circuit.RELTOL*abs(old_voltages[i]))){
                incomplete = true;
            }
        }
        //check that max iterations haven't occured
        if(current_iteration >= circuit.max_iterations){
            cout << "max iterations";
            exit(1);
            incomplete = false;
        }
    }
    //std::cout << "Iterations " << current_iteration << std::endl;
}

#endif
