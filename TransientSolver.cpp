#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <string>
using namespace std;
using Eigen::MatrixXd;

struct Component{
    int anode;
    int cathode;
    string name;
    double value;
    char type;
    char get_type() {
        return type;
    }
    int get_anode(){
        return anode;
    }
    int get_cathode(){
        return cathode;
    }
    double get_value(){
        return value;
    }
    string get_name(){
        return name;
    }
};
struct Node{
        double voltage = 0;
        int index = 0;
        vector<Component> components;
        void set_index(int _index){index = _index;}
        int get_index(){return index;}
        double get_voltage(){return voltage;}
        void set_voltage(double _voltage){voltage = _voltage;}
        void add_component(Component component){
            components.push_back(component);
        }
        vector<Component> get_components(){
            return components;
        }
};


//Takes a vector of componenets and outputs a vector of nodes that each have their own vector of componenets
vector<Node> NodeGenerator(vector<Component> components){
    vector<Node> Nodes;
    //iterate through the components
    int index = 0;
    for(Component component: components){ 
        //ensure the node list has a node of high enough index 
        while(Nodes.size()<=component.get_cathode()){
            Node node;
            node.set_index(index);
            Nodes.push_back(node);
            index++;
        }
        while(Nodes.size()<=component.get_anode()){
            Node node;
            node.set_index(index);
            Nodes.push_back(node);
            index++;
        }
        //add the components to the nodes it is attached to
        Nodes[component.get_cathode()].add_component(component);
        Nodes[component.get_anode()].add_component(component);
    }
    return Nodes;
}


//Works out the total current at a node given the voltages in the circuit, the time, 
//the vector of nodes and the index of the node
//if current_componet isn't left as null the branch connected to this component will be ignored
double GetCurrent(int node_index, vector<double> voltages, vector<Node> nodes, double time, Component current_component = nullptr){
    double total_current = 0;
    //loop through all the components
    for(Component component: nodes[node_index].get_components()){
        //make sure that this isn't the current_component
        if(current_component == null || current_component.get_name() != component.get_name()){
            //if this is a capacitor or voltage source
            if(component.is_voltage()){
                //create a vector to store modified voltages
                vector<double> modified_voltages = voltages;
                if(component.get_anode() == node_index){
                    //change the voltage at the other end of the voltage source to this nodes voltage + the voltage source value
                    modified_voltages[component.get_cathode()] = voltages[node_index] - component.get_value();
                    //get the current at the other node using the modified voltages and ignoring this component
                    current += GetCurrent(component.get_cathode(),modified_voltages,time,nodes,component);
                }
                if(component.get_cathode() == node_index){
                    modified_voltages[component.get_anode()] = voltages[node_index] + component.get_value();
                    current += GetCurrent(component.get_anode(),modified_voltages,nodes,time,component);
                }
            }else{
                //if this isnt a voltage source add the current from this component (multiplied by -1 if this is the cathode)
                total_current += component.get_current(voltages) * component.get_anode()==node_index?1:-1;
            }
        }
    }
}


//Works out the total current parital derivatives at a node given the voltages in the circuit, time,
//the vector of nodes and the node index of this node
//if current_componet isn't left as null the branch connected to this component will be ignored
//output represents the partial derivative of current with respect to each node voltage 
vector<double> GetCurrentDerivative(int node_index, vector<double> voltages, vector<Node> nodes,double time, Component component = null){
    //functionaly same as GetCurrent except exporting the vector of current derivatives
    vector<double> total_derivative(volatges.size(),0);
    for(Component component: nodes[node_index].get_components()){
        if(current_component == null || current_component.get_name() != component.get_name()){
            if(component.is_voltage()){
                vector<double> modified_voltages = voltages;
                if(component.get_anode() == node_index){
                    modified_voltages[component.get_cathode()] = voltages[node_index] - component.get_value();
                    current += GetCurrent(component.get_cathode(),modified_voltages,nodes,component);
                    for(int i=0; i < total_derivative.size(); i++){
                        total_derivative[i] += GetCurrentDerivative(component.get_cathode(),modified_voltages,nodes,time,component)[i];
                    }
                }
                if(component.get_cathode() == node_index){
                    modified_voltages[component.get_anode()] = voltages[node_index] + component.get_value();
                    for(int i=0; i < total_derivative.size(); i++){
                        total_derivative[i] += GetCurrentDerivative(component.get_anode(),modified_voltages,nodes,time,component)[i];
                    }
                }
            }else{
                for(int i=0; i < total_derivative.size(); i++){
                    total_derivative[i] += component.get_current(voltages)[i] * component.get_anode()==node_index?1:-1;
                }
            }
        }
    }
    return total_derivative;
}

//takes the previous node voltages, time, components, max iterations and max error
//outputs the new node voltages at that time using multi variable newton raphson
//max iterations is the maximum times to use newton raphson
//max error represents the total current error at all the nodes summed together
vector<double> TransientSolver(vector<double> voltages, double time, vector<Component> components, int max_iterations, double max_error){
    //convert voltages into matrix format ingnoring 0 node
    MatrixXd outputMatrix(voltages.size()-1);
    for(int i==1; i < nodes.size(); i++){
        outputMatrix(i-1) = voltages[i];
    
    }
    vector<double> output(voltages.size(),0);
    //create a vector of nodes 
    vector<Node> nodes = NodeGenerator(components);

    bool incomplete = true;
    int current_iteration = 0;
    while(incomplete){
        current_iteration++;
        //create a matrix for the currents and Jacobian
        MatrixXd current(nodes.size()-1,1);
        MatrixXd Jacobian(nodes.size()-1,nodes.size()-1);
        double total_error = 0;
        for(int i==1; i < nodes.size(); i++){
            //set the total current at each node
            current(i-1) = GetCurrent(i,output,nodes);
            total_error += current(i-1)
        }
        if(total_error < max_error){
            incomplete = false;
        }else{
            for(int i==1; i < nodes.size(); i++){
                //get the partial derivatives at that node and input into jacobian
                vector<double> derivatives = GetCurrentDerative(i,output,nodes)
                for(int j = 1; j < nodes.size(); j++){
                    Jacobian(i-1,j-1) = derivatives[j];
                }
            }
            //adjust the output matrix
            outputMatrix = outputMatrix - Jacobian.inverse()*current;
        }

        //check that max iterations haven't occured
        if(current_iteration >= max_iterations){
            cout << "Hit maximum iterations in Newton-Raphson";
            incomplete = true;
        }
    }
    //convert the output matrix into a vector 
    for(int i==1; i < nodes.size(); i++){
        output[i] = outputMatrix(i-1);
    }
    return output;
}