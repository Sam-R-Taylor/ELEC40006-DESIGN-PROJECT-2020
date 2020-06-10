/*#include "Circuit.hpp"
#include "Component.hpp"
//#include "KCLSolver.hpp"
//#include "TransientSolver.hpp"
//#include "TransientAnalysis.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include"leo_KCLSolver.hpp"
using namespace std;
int main(){
    
    Voltage_Source v1{1,0,"V1",4};
    //Inductor i1{2,0,"I1",10};
    Resistor r1{2,1,"R1",1000};
    Resistor r2{2,3,"R2",5000};
    Resistor r3{4,0,"R3",2000};
    Voltage_Controlled_Voltage_Source v2{4,3,"V2",6,2,0};
    //Diode d1{2,3,"D1"};
    //Current_source i1{0,2,"I1",0.005};
    //(BJT bj{1,2,3,"BJ",0.67,0.995,1,10,0.2,0.3};
    //Resistor r1{2,1,"R1",1000};
    //Resistor r5{3,0,"R1",1000};
    //Resistor r6{3,0,"R1",1000};
    //vector<Component*> components{&v2,&r2,&r3,&r4,&r5,&r6,&d1,&d2,&vc1,&vc2};
    vector<Component*> components{&v2,&r2,&r1,&v1,&r3};
    Circuit circuit;
    for(Component* component: components){
        circuit.add_component(component);
        cout << "added component " << component->get_name() <<endl;
    } //eqn1 = (10-V1) + (V0-V1)/1000 -1 + 0.5*((V2-V0)/1000 + 1) == 0
    //eqn2 = (0-V2) + (V0-V2)/1000 -1 + 0.5*((V1-V0)/1000 + 1) == 0
    //eqn3 = (V0-5) + (V1-10) + (V0-0) == 0
    //vector<Node> nodes = NodeGenerator(circuit.get_components());
    circuit.build_nodes();
    //cout << "built node " << endl;
    vector<double> v{0,0,0,0,0};
    circuit.set_voltages(v);


    


    
    for(int i = 0; i < 1; i++){
        cout << "running transient" << endl;
        //TransientAnalysis(circuit,5,5);
        //TransientSolver(circuit);
        Matrix_solver(circuit);
    }
    

   std::cout << "circuit voltages are " << std::endl;
    
    
    v = circuit.get_voltages();
    for(auto x: v){
        cout << x << endl;
    }
    
}*/