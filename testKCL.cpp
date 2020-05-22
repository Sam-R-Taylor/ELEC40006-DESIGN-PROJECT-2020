
#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <string>
#include "Circuit.hpp"
#include "Component.hpp"
using namespace std;
using Eigen::MatrixXd;
//to compile use g++ -I eigen3 KCLSolver.cpp -o ...

//creates the kcl equation for a node in the circuit
//input is the current node and a list of nodes
//output is a vector of doubles representing each of the coeefiicients {a,b,c...,d}
//such that ax1 + bx2 + cx3 ...+ d =0
//if being used for a super node include the component connecting the two nodes as the last parameter
vector<double> coefficient_generator(Node &node, vector<Node&> &nodes, Component &source_component = nullptr){
    //create a vector to store the output coefficients
    vector<double> coefficients(nodes.size() + 1,0);
    //iterate through all the components connected to that node
    for(Component component: node.components()){
        //if the source component is not included just continue, otherwise only run if teh current component
        //isn't the connecting component
        if(&source_component == nullptr || &component != &source_component){
            //create a vector to store the contributions to the final coefficients of this component
            vector<double> sub_coefficients(nodes.size() + 1,0);
            //if a component is a diode use its linear properties
            if(dynamic_cast<Diode&>(component)){
                if(component.get_anode() == node.get_index()){
                    //assign 1/r and -1/r to each corresponding coefficient of the resistor nodes
                    sub_coefficients[component.get_anode()] += ((Diode&)component).get_conductance();
                    sub_coefficients[component.get_cathode()] += -((Diode&)component).get_conductance();
                }
                if(component.get_cathode() == node.index){
                    sub_coefficients[component.get_anode()] += -((Diode&)component).get_conductance();
                    sub_coefficients[component.get_cathode()] += ((Diode&)component).get_conductance();
                }
                sub_coefficients[nodes.size()] += ((Diode&)component).get_linear_current() * (component.get_anode() == node.index?1:-1);
            }
            //if component type is resistor
            if(dynamic_cast<Resistor&>(component)){
                //check with node of the resistor is the current node
                if(component.get_anode() == node.index){
                    //assign 1/r and -1/r to each corresponding coefficient of the resistor nodes
                    sub_coefficients[component.get_anode()] += 1/((Resistor&)component).get_value();
                    sub_coefficients[component.get_cathode()] += -1/((Resistor&)component).get_value();
                }
                if(component.get_cathode() == node.index){
                    sub_coefficients[component.get_anode()] += -1/((Resistor&)component).get_value();
                    sub_coefficients[component.get_cathode()] += 1/((Resistor&)component).get_value();
                }
            }
            //if component type is current source
            if(dynamic_cast<Current_source&>(component)){
                //add the current source value to the final constant in the list of coefficients
                sub_coefficients[nodes.size()] += ((Current_source&)component).get_current() * (component.get_anode() == node.index?-1:1);
            }
            //if component is a voltage source
            if(dynamic_cast<Voltage_Component&>(component)){
                //check which end of the voltage source is connected to the current node
                if(component.get_anode() == node.index{
                    //generate the coeffiecient for node on the other side of the voltage source
                    //leaving out he connection to the current node by adding the source_component parameter
                    sub_coefficients = 
                    coefficient_generator(nodes[component.get_cathode()],nodes,component);
                    //correct the coefficient by swapping "N2" with "N1 + V" and add the voltage constant
                    //multiplied by the coefficient value to the constant term of the coefficients.
                    sub_coefficients[node.get_index()] += sub_coefficients[component.get_cathode()];
                    sub_coefficients[nodes.size()] += sub_coefficients[component.get_cathode()] * ((Voltage_Component&)component).get_voltage() *-1;
                    sub_coefficients[component.get_cathode()] = 0;
                }
                if(component.get_cathode() == node.index){
                    sub_coefficients = 
                    coefficient_generator(nodes[component.get_anode()], nodes,component);
                    sub_coefficients[node.get_index()] += sub_coefficients[component.get_anode()];
                    sub_coefficients[nodes.size()] += sub_coefficients[component.get_anode()] * ((Voltage_Component&)component).get_voltage();
                    sub_coefficients[component.get_anode()] = 0;
                }
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
vector<double> MatrixSolver(vector<Node&> &input){
    //create a matrix with the correct dimensions
    MatrixXd matrix(input.size()-1,input.size()-1);
    //create vectors for the left and right hand side of the equation
    MatrixXd result(input.size()-1,1);
    MatrixXd constants(input.size()-1,1);
    //iterate through all the inputs
    for(int i=0; i < input.size()-1; i++){
            //for each input extract the row data
            vector<double> row = coefficient_generator(input[i+1],input);
            //fill the corresponding matrix row with the input data
            for(int j=1; j<input.size(); j++){
                matrix(i,j-1) = row[j];
            }
            //add the final input data value to the constants, make negative 
            //as moved to other side of equation
            constants(i) = -row[input.size()];
    }
    //invert matrix and solve equation
    matrix = matrix.inverse();
    result = matrix * constants;
    //convert the output matrix to vector format
    vector<double> output;
    output.push_back(0);
    for(int i=0; i < input.size() -1; i++){
            output.push_back(result(i));
    }
    //return output
    return output;
}

double ComponentCurrent(vector<double> &voltages, vector<Node&> &nodes, Component& component){
    vector<double> coefficients = coefficient_generator(nodes[component.get_anode()], nodes, component);
    double current = 0;
    for(int i = 0; i<voltages.size(); i++){
        current += voltages[i] * coefficients[i];
    }
    return -current;
}

//outputs the vector of voltages given the vector of components, optional vector of nodes used to increase speed
void NodeVoltageSolver(Circuit &circuit){   //vector<Node&> &nodes){
    circuit.set_voltages(MatrixSolver(circuit.get_nodes()));
}

int main(){
    
}