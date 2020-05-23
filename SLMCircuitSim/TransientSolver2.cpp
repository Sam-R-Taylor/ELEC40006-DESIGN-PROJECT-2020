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
vector<double> TransientSolver(vector<double> voltages, double time, vector<Component*>& components, int max_iterations, double max_error){
    //store the voltages in a new vector that can be adjusted each iteration
    vector<double> current_voltages = voltages;

    bool incomplete = true;
    int current_iteration = 0;
    //create a vector of Nodes
    vector<Node> nodes = NodeGenerator(components);
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
        vector<double> new_voltages = NodeVoltageSolver(components,&nodes);
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

int main(){
    Voltage_Source v1(1,0,"V1",6);
    Resistor r1(1,2,"R1",5);
    Resistor r2(2,3,"R2",5);
    Diode d1(0,3,"D1");
    vector<Component*> components;
    components.push_back(&v1);
    components.push_back(&r1);
    components.push_back(&r2);
    components.push_back(&d1);
    vector<double> Voltages{0,0,0,0};
    Voltages = TransientSolver(Voltages,0,components,10000,0.001);
    cout << "Test 1:" << endl;
    cout << "V0 " << Voltages[0];
    cout << " ,V1 " << Voltages[1];
    cout << " ,V2 " << Voltages[2];
    cout << " ,V3 " << Voltages[3] << endl;
}