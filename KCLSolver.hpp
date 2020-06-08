#ifndef KCLSOLVER_HPP
#define KCLSOLVER_HPP

#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <string>
#include "Component.hpp"
#include "Circuit.hpp"
#include <chrono>
using namespace std;
using Eigen::MatrixXd;
//to compile use g++ -I eigen3 KCLSolver.cpp -o ...

//creates the kcl equation for a node in the circuit
//input is the current node and a list of nodes
//output is a vector of doubles representing each of the coeefiicients {a,b,c...,d}
//such that ax1 + bx2 + cx3 ...+ d =0
//if being used for a super node include the component connecting the two nodes as the last parameter
vector<double> coefficient_generator(Node *node, vector<Node> *nodes, Component *source_component = nullptr){
    //create a vector to store the output coefficients
    vector<double> coefficients(nodes->size() + 1,0);
    //iterate through all the components connected to that node
    for(Component* component: node->components_attached){
        //if the source component is not included just continue, otherwise only run if teh current component
        //isn't the connecting component
        if(source_component == nullptr || component != source_component){
            //create a vector to store the contributions to the final coefficients of this component
            vector<double> sub_coefficients(nodes->size() + 1,0);
            if(dynamic_cast<BJT*>(component)){
                sub_coefficients[component->get_anode()] += ((BJT*)component)->get_collector_coefficient(node->index);
                sub_coefficients[component->get_cathode()]+= ((BJT*)component)->get_emmitter_coefficient(node->index);
                sub_coefficients[((BJT*)component)->get_base()] += ((BJT*)component)->get_base_coefficient(node->index);
                sub_coefficients[nodes->size()] += ((BJT*)component)->get_constant_coefficient(node->index);
            }
            //if a component is a diode use its linear properties
            if(dynamic_cast<Diode*>(component)){
                ((Diode*)component)->set_conductance();
                if(component->get_anode() == node->index){
                    //assign 1/r and -1/r to each corresponding coefficient of the resistor nodes
                    sub_coefficients[component->get_anode()] += ((Diode*)component)->get_anode_coefficient();
                    sub_coefficients[component->get_cathode()] += ((Diode*)component)->get_cathode_coefficient();
                }
                if(component->get_cathode() == node->index){
                    sub_coefficients[component->get_anode()] += -((Diode*)component)->get_anode_coefficient();
                    sub_coefficients[component->get_cathode()] += -((Diode*)component)->get_cathode_coefficient();
                }
                sub_coefficients[nodes->size()] += ((Diode*)component)->get_constant_coefficient() * (component->get_anode() == node->index?1:-1);
            }
            //if component type is resistor
            else if(dynamic_cast<Resistor*>(component)){
                //check with node of the resistor is the current node
                if(component->get_anode() == node->index){
                    //assign 1/r and -1/r to each corresponding coefficient of the resistor nodes
                    sub_coefficients[component->get_anode()] += ((Resistor*)component)->get_conductance();//1/((Resistor*)component)->get_value();
                    sub_coefficients[component->get_cathode()] += -((Resistor*)component)->get_conductance();//1/((Resistor*)component)->get_value();
                }
                else if(component->get_cathode() == node->index){
                    sub_coefficients[component->get_anode()] += -((Resistor*)component)->get_conductance();//1/((Resistor*)component)->get_value();
                    sub_coefficients[component->get_cathode()] += ((Resistor*)component)->get_conductance();//1/((Resistor*)component)->get_value();
                }
            }
            //if component type is current source
            else if(dynamic_cast<Current_source*>(component)){
                //add the current source value to the final constant in the list of coefficients
                sub_coefficients[nodes->size()] += ((Current_source*)component)->get_current() * (component->get_anode() == node->index?-1:1);
            }
            else if(dynamic_cast<Inductor*>(component)){
                sub_coefficients[nodes->size()] -= ((Inductor*)component)->get_linear_current() * (component->get_anode() == node->index?-1:1);
                //sub_coefficients[component->get_anode()] += ((Capacitor*)component)->get_conductance() * (component->get_anode() == node->index?-1:1);
                //sub_coefficients[component->get_cathode()] += -((Capacitor*)component)->get_conductance() * (component->get_anode() == node->index?-1:1); 
                if(component->get_anode() == node->index){
                    //assign 1/r and -1/r to each corresponding coefficient of the resistor nodes
                    sub_coefficients[component->get_anode()] += ((Inductor*)component)->get_conductance();//1/((Resistor*)component)->get_value();
                    sub_coefficients[component->get_cathode()] += -((Inductor*)component)->get_conductance();//1/((Resistor*)component)->get_value();
                }
                else if(component->get_cathode() == node->index){
                    sub_coefficients[component->get_anode()] += -((Inductor*)component)->get_conductance();//1/((Resistor*)component)->get_value();
                    sub_coefficients[component->get_cathode()] += ((Inductor*)component)->get_conductance();//1/((Resistor*)component)->get_value();
                }
            }
            else if(dynamic_cast<Capacitor*>(component)){
                cout << ((Capacitor*)component)->get_linear_current() << endl;
                cout << ((Capacitor*)component)->get_conductance() << endl;
                sub_coefficients[nodes->size()] += ((Capacitor*)component)->get_linear_current() * (component->get_anode() == node->index?-1:1);
                //sub_coefficients[component->get_anode()] += ((Capacitor*)component)->get_conductance() * (component->get_anode() == node->index?-1:1);
                //sub_coefficients[component->get_cathode()] += -((Capacitor*)component)->get_conductance() * (component->get_anode() == node->index?-1:1); 
                if(component->get_anode() == node->index){
                    //assign 1/r and -1/r to each corresponding coefficient of the resistor nodes
                    sub_coefficients[component->get_anode()] += ((Capacitor*)component)->get_conductance();//1/((Resistor*)component)->get_value();
                    sub_coefficients[component->get_cathode()] += -((Capacitor*)component)->get_conductance();//1/((Resistor*)component)->get_value();
                }
                else if(component->get_cathode() == node->index){
                    sub_coefficients[component->get_anode()] += -((Capacitor*)component)->get_conductance();//1/((Resistor*)component)->get_value();
                    sub_coefficients[component->get_cathode()] += ((Capacitor*)component)->get_conductance();//1/((Resistor*)component)->get_value();
                }
            }
            //if component is a voltage source
            else if(dynamic_cast<Voltage_Component*>(component)){
                //check which end of the voltage source is connected to the current node
                if(component->get_anode() == node->index){
                    std::cout << "get current "<< std::endl;
                    //generate the coeffiecient for node on the other side of the voltage source
                    //leaving out he connection to the current node by adding the source_component parameter
                    sub_coefficients = 
                        coefficient_generator(&((*nodes)[component->get_cathode()]), nodes,component);
                    ((Voltage_Component*)component)->set_coefficients(sub_coefficients);
                    //correct the coefficient by swapping "N2" with "N1 + V" and add the voltage constant
                    //multiplied by the coefficient value to the constant term of the coefficients.
                    if(dynamic_cast<Voltage_Controlled_Voltage_Source*>(component)){
                        sub_coefficients[node->index] += sub_coefficients[component->get_cathode()];
                        sub_coefficients[((Voltage_Controlled_Voltage_Source*)component)->get_control_anode()] += sub_coefficients[component->get_cathode()] * ((Voltage_Component*)component)->get_voltage() *-1;
                        sub_coefficients[((Voltage_Controlled_Voltage_Source*)component)->get_control_cathode()] += sub_coefficients[component->get_cathode()] * ((Voltage_Component*)component)->get_voltage();
                    }
                    else{
                        sub_coefficients[node->index] += sub_coefficients[component->get_cathode()];
                        sub_coefficients[nodes->size()] += sub_coefficients[component->get_cathode()] * ((Voltage_Component*)component)->get_voltage() *-1;
                    }
                    sub_coefficients[component->get_cathode()] = 0;
                }
                if(component->get_cathode() == node->index){
                    sub_coefficients = 
                    coefficient_generator(&((*nodes)[component->get_anode()]), nodes,component);
                    if(dynamic_cast<Voltage_Controlled_Voltage_Source*>(component)){
                        sub_coefficients[node->index] += sub_coefficients[component->get_anode()];
                        sub_coefficients[((Voltage_Controlled_Voltage_Source*)component)->get_control_anode()] += sub_coefficients[component->get_anode()] * ((Voltage_Component*)component)->get_voltage();
                        sub_coefficients[((Voltage_Controlled_Voltage_Source*)component)->get_control_cathode()] += sub_coefficients[component->get_anode()] * ((Voltage_Component*)component)->get_voltage() * -1;
                    }
                    else{
                        sub_coefficients[node->index] += sub_coefficients[component->get_anode()];
                        sub_coefficients[nodes->size()] += sub_coefficients[component->get_anode()] * ((Voltage_Component*)component)->get_voltage();
                    }
                    sub_coefficients[component->get_anode()] = 0;
                }
            }
            else if(dynamic_cast<Voltage_Controlled_Current_Source*>(component)){
                sub_coefficients[((Voltage_Controlled_Current_Source*)component)->get_control_anode()] += ((Current_source*)component)->get_current() * (component->get_anode() == node->index?-1:1);
                sub_coefficients[((Voltage_Controlled_Current_Source*)component)->get_control_cathode()] += ((Current_source*)component)->get_current() * (component->get_anode() == node->index?1:-1);
            }
            //add the sub coefficients to the output
            for(int i=0; i < coefficients.size(); i++){
                coefficients[i] += sub_coefficients[i];
            }
        }
    }
    //return the output
    return coefficients;
}

//takes a vector of an object (ie nodes)
//the object type named here as Data must have a method get_data that outputs the 
//constants of a line of a matrix in the form {a,b,c, ... ,d} such that
//ax1 + bx2 + cx3 .... + d = 0
//the matrix outputs the solution to the matrix equation in a vector float
vector<double> MatrixSolver(Circuit &circuit){  //vector<Node> &input){

    //create a matrix with the correct dimensions
    MatrixXd matrix(circuit.get_number_of_nodes()-1,circuit.get_number_of_nodes()-1);
    //create vectors for the left and right hand side of the equation
    MatrixXd result(circuit.get_number_of_nodes()-1,1);
    MatrixXd constants(circuit.get_number_of_nodes()-1,1);
    //iterate through all the inputs
    for(int i=0; i < circuit.get_number_of_nodes()-1; i++){
            //for each input extract the row data
            vector<double> row = coefficient_generator(&circuit.get_nodes_ptr()->at(i+1),circuit.get_nodes_ptr());
            //fill the corresponding matrix row with the input data
            for(int j=1; j<circuit.get_number_of_nodes(); j++){
                matrix(i,j-1) = row[j];
            }
            //add the final input data value to the constants, make negative 
            //as moved to other side of equation
            constants(i) = -row[circuit.get_number_of_nodes()];
    }
    //invert matrix and solve equation


    //MatrixXd matrix2 = matrix;
    //auto start = chrono::steady_clock::now();
    //for(int i = 0; i < 1; i++){
        //cout << matrix <<endl;
        //cout << "matrix" <<endl; 
    matrix = matrix.inverse();
    result = matrix * constants;
    //result = matrix.householderQr().solve(constants);
    //}
    //auto end = chrono::steady_clock::now();
    //auto diff = end - start;
    //cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;
    
    
    
    //convert the output matrix to vector format
    vector<double> output;
    output.push_back(0);
    for(int i=0; i < circuit.get_number_of_nodes() -1; i++){
            output.push_back(result(i));
    }
    //return output
    return output;
}


//returns the current through a component
double GetCurrent(Circuit &circuit, Component* component){
//double ComponentCurrent(vector<double> &voltages, vector<Node> &nodes, Component* component){
    double current = 0;
    if(dynamic_cast<Voltage_Component*>(component)){
        current = ((Voltage_Component*)component)->get_current(circuit.get_voltages());
    }else if(dynamic_cast<Current_Component*>(component)){
        return ((Current_Component*)component)->get_current(circuit.get_voltages());
    }
    return current;
}


//outputs the vector of voltages given the vector of components, optional vector of nodes used to increase speed
void NodeVoltageSolver(Circuit &circuit){  //vector<Component*> &components, vector<Node> *nodes = nullptr){
    circuit.set_voltages(MatrixSolver(circuit));
}


#endif


