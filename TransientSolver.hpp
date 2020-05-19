#ifndef TRANSIENTSOLVER_HPP
#define TRANSIENTSOLVER_HPP


#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <string>
#include "Component.hpp"
#include "KCLSolver.hpp"
#include <memory> 
using namespace std;
using Eigen::MatrixXd;



/*
Takes the previous node voltages or a guess at them, the time, circuit components, max iterations and max error.
Outputs the new node voltages at that time using multi variable newton raphson iterative method
/max iterations is the maximum iterations in newton raphson
/max error represents the maximum voltage error at anyone node
*/
vector<double> TransientSolver(vector<double> voltages, double time, vector<Component*>& components, int max_iterations, double max_error, vector<Node> *nodes = nullptr){
    //store the voltages in a new vector that can be adjusted each iteration
    vector<double> current_voltages = voltages;

    bool incomplete = true;
    int current_iteration = 0;
    //create a vector of Nodes if one isn't given
    vector<Node> _nodes;
    vector<Node>* _nodesptr;
    if(nodes == nullptr){
       _nodes = NodeGenerator(components);
       _nodesptr = &_nodes;
    }else{
        _nodesptr = nodes;
    }
    //iterate adjusting voltages each time
    while(incomplete){
        current_iteration++;
        cout << "Iteration " << current_iteration << endl;
        //loop through the components
        for(Component* component: components){
            //adjust all the diodes for the current voltage guess
            if(dynamic_cast<Diode*>(component)){
                ((Diode*)component)->set_vd(current_voltages[component->get_anode()] - current_voltages[component->get_cathode()]);
                ((Diode*)component)->set_id0(((Diode*)component)->get_current(current_voltages));
            }
        }
        //set the voltages to the output of the KCL with the components
        vector<double> new_voltages = NodeVoltageSolver(components,_nodesptr);
        //check the error
        for(int i=0; i<voltages.size(); i++){
            incomplete = (abs(current_voltages[i] - new_voltages[i])>max_error);
        }
        //set the stored voltages to the new voltages
        if(incomplete){
            current_voltages = new_voltages;
        }
        //check that max iterations haven't occured
        if(current_iteration >= max_iterations){
            cout << "Hit maximum iterations in Newton-Raphson";
            incomplete = false;
        }
    }
    //output the voltages
    return current_voltages;
}


#endif