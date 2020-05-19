#ifndef KCLSOLVER_HPP
#define KCLSOLVER_HPP

#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <string>
#include "Component.hpp"
using namespace std;
using Eigen::MatrixXd;
//to compile use g++ -I eigen3 KCLSolver.cpp -o ...

struct Node{
        double voltage = 0;
        int index = 0;
        vector<Component*> components;
        void set_index(int _index){index = _index;}
        int get_index(){return index;}
        double get_voltage(){return voltage;}
        void set_voltage(double _voltage){voltage = _voltage;}
        void add_component(Component* component){
            components.push_back(component);
        }
        vector<Component*> get_components(){
            return components;
        }
};

//creates the kcl equation for a node in the circuit
//input is the current node and a list of nodes
//output is a vector of doubles representing each of the coeefiicients {a,b,c...,d}
//such that ax1 + bx2 + cx3 ...+ d =0
//if being used for a super node include the component connecting the two nodes as the last parameter
vector<double> coefficient_generator(Node *node, vector<Node> nodes, Component *source_component = nullptr){
    //create a vector to store the output coefficients
    vector<double> coefficients(nodes.size() + 1,0);
    //iterate through all the components connected to that node
    for(Component* component: node->get_components()){
        //if the source component is not included just continue, otherwise only run if teh current component
        //isn't the connecting component
        if(source_component == nullptr || component != source_component){
            //create a vector to store the contributions to the final coefficients of this component
            vector<double> sub_coefficients(nodes.size() + 1,0);
            //if a component is a diode use its linear properties
            if(dynamic_cast<Diode*>(component)){
                if(component->get_anode() == node->get_index()){
                    //assign 1/r and -1/r to each corresponding coefficient of the resistor nodes
                    sub_coefficients[component->get_anode()] += ((Diode*)component)->get_conductance();
                    sub_coefficients[component->get_cathode()] += -((Diode*)component)->get_conductance();
                }
                if(component->get_cathode() == node->get_index()){
                    sub_coefficients[component->get_anode()] += -((Diode*)component)->get_conductance();
                    sub_coefficients[component->get_cathode()] += ((Diode*)component)->get_conductance();
                }
                sub_coefficients[nodes.size()] += ((Diode*)component)->get_linear_current() * (component->get_anode() == node->get_index()?1:-1);
            }
            //if component type is resistor
            if(dynamic_cast<Resistor*>(component)){
                //check with node of the resistor is the current node
                if(component->get_anode() == node->get_index()){
                    //assign 1/r and -1/r to each corresponding coefficient of the resistor nodes
                    sub_coefficients[component->get_anode()] += 1/((Resistor*)component)->get_value();
                    sub_coefficients[component->get_cathode()] += -1/((Resistor*)component)->get_value();
                }
                if(component->get_cathode() == node->get_index()){
                    sub_coefficients[component->get_anode()] += -1/((Resistor*)component)->get_value();
                    sub_coefficients[component->get_cathode()] += 1/((Resistor*)component)->get_value();
                }
            }
            //if component type is current source
            if(dynamic_cast<Current_source*>(component)){
                //add the current source value to the final constant in the list of coefficients
                sub_coefficients[nodes.size()] += ((Current_source*)component)->get_current() * (component->get_anode() == node->get_index()?-1:1);
            }
            //if component is a voltage source
            if(dynamic_cast<Voltage_Source*>(component)){
                //check which end of the voltage source is connected to the current node
                if(component->get_anode() == node->get_index()){
                    //generate the coeffiecient for node on the other side of the voltage source
                    //leaving out he connection to the current node by adding the source_component parameter
                    sub_coefficients = 
                    coefficient_generator(&nodes[component->get_cathode()], nodes,component);
                    //correct the coefficient by swapping "N2" with "N1 + V" and add the voltage constant
                    //multiplied by the coefficient value to the constant term of the coefficients.
                    sub_coefficients[node->get_index()] += sub_coefficients[component->get_cathode()];
                    sub_coefficients[nodes.size()] += sub_coefficients[component->get_cathode()] * ((Voltage_Component*)component)->get_voltage() *-1;
                    sub_coefficients[component->get_cathode()] = 0;
                }
                if(component->get_cathode() == node->get_index()){
                    sub_coefficients = 
                    coefficient_generator(&nodes[component->get_anode()], nodes,component);
                    sub_coefficients[node->get_index()] += sub_coefficients[component->get_anode()];
                    sub_coefficients[nodes.size()] += sub_coefficients[component->get_anode()] * ((Voltage_Component*)component)->get_voltage();
                    sub_coefficients[component->get_anode()] = 0;
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


//takes a vector of objects (should be components)
//outputs a vector of objects (should be nodes)
//each node has a correct vector of components
//template<typename Component> 
vector<Node> NodeGenerator(vector<Component*> components){
    vector<Node> Nodes;
    //iterate through the components
    int index = 0;
    for(Component* component: components){ 
        //ensure the node list has a node of high enough index 
        while(Nodes.size()<=component->get_cathode()){
            Node node;
            node.set_index(index);
            Nodes.push_back(node);
            index++;
        }
        while(Nodes.size()<=component->get_anode()){
            Node node;
            node.set_index(index);
            Nodes.push_back(node);
            index++;
        }
        //add the components to the nodes it is attached to
        Nodes[component->get_cathode()].add_component(component);
        Nodes[component->get_anode()].add_component(component);
    }
    return Nodes;
}

//takes a vector of an object (ie nodes)
//the object type named here as Data must have a method get_data that outputs the 
//constants of a line of a matrix in the form {a,b,c, ... ,d} such that
//ax1 + bx2 + cx3 .... + d = 0
//the matrix outputs the solution to the matrix equation in a vector float
//template<typename Node>
vector<double> MatrixSolver(vector<Node> input){
    int ground_node = 0;
    //create a matrix with the correct dimensions
    MatrixXd matrix(input.size()-1,input.size()-1);
    //create vectors for the left and right hand side of the equation
    MatrixXd result(input.size()-1,1);
    MatrixXd constants(input.size()-1,1);
    //iterate through all the inputs
    for(int i=0; i < input.size()-1; i++){
            //for each input extract the row data
            vector<double> row = coefficient_generator(&input[i+1],input);
            //fill the corresponding matrix row with the input data
            for(int j=1; j<input.size(); j++){
                matrix(i,j-1) = row[j];
            }
            //add the final input data value to the constants, make negative 
            //as moved to other side of equation
            constants(i) = -row[input.size()];
    }
    //invert matrix and solve equation
    //cout << matrix << endl;
    matrix = matrix.inverse();
    result = matrix * constants;
    
    //cout << constants << endl;
    //cout << result << endl;
    //convert the output matrix to vector format
    vector<double> output;
    output.push_back(0);
    for(int i=0; i < input.size() -1; i++){
            output.push_back(result(i));
    }
    //return output
    return output;
}

//just combines both funcitons into one
vector<double> NodeVoltageSolver(vector<Component*> components){
    return MatrixSolver(NodeGenerator(components));
}


#endif