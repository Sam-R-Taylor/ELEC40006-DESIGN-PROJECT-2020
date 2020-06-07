/*#include "Circuit.hpp"
#include "Component.hpp"
#include "KCLSolver.hpp"
#include "TransientSolver.hpp"
#include "TransientAnalysis.hpp"
#include <iostream>
#include <chrono>
#include <vector>
using namespace std;
int main(){
    Voltage_Source v2{1,0,"V1",5};
    //AC_Voltage_Source v2{1,0,"V1",5,1,0};
    Resistor r2{2,1,"R1",1000};
    Diode d1{2,0,"D1"};
    
    //v2.Set_Voltage(-0.2);
    //cout << v2.get_voltage();
    //BJT bj{1,2,3,"BJ", 0.67, 0.995,1,10,0.2,0.3};
    //Resistor r2{2,1,"R1",1000};
    //Resistor r5{2,0,"R1",1000};
    //Resistor r6{3,0,"R1",1000};
    vector<Component*> components{&v2,&r2,&d1};
    //vector<Component*> components{&v2,&r2,&r5,&r6,&bj};
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
    vector<double> v{0,0,0,0};
    circuit.set_voltages(v);

    
    //for(int i = 0; i < 1; i++){
    cout << "running transient" << endl;
    TransientAnalysis(circuit,1,1);
    
    
    //v = bj.get_current(circuit.get_voltages());
    //cout << v.size() << endl;
    cout << GetCurrent(circuit,&d1) <<endl;
    //cout << GetCurrent(circuit,&r5);
    //for(auto x: v){
    //    cout << x << endl;
    //}
    v = circuit.get_voltages();
    for(auto x: v){
        cout << x << endl;
    }
    

}*/