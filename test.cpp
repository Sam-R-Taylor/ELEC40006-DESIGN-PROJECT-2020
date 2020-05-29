#include "Circuit.hpp"
#include "Component.hpp"
#include "KCLSolver.hpp"
#include "TransientSolver.hpp"
#include <iostream>
#include <chrono>
using namespace std;
int main(){
    
    Voltage_Source v2{1,0,"V1",5};
    //Voltage_Source v1{1,0,"V1",5};
    Resistor r2{1,2,"R2",1000};
    Resistor r3{2,3,"R2",0.568};
    //Resistor r1{3,4,"R1",100};
    //Diode r3{4,5,"R3"};
    //Resistor r4{5,6,"R4",100};
    //Resistor r5{6,7,"R5",100};
    //Resistor r6{7,8,"R6",100};
    //Resistor r7{8,9,"R7",100};
    //Resistor r8{9,10,"R8",100};
    //Diode d1{10,11,"D1"};
    Diode d2{3,0,"D2"};
    vector<Component*> components{&v2,&r2,&d2,&r3};
    Circuit circuit;
    for(Component* component: components){
        circuit.add_component(component);
    }
    //vector<Node> nodes = NodeGenerator(circuit.get_components());
    circuit.build_nodes();
    //cout << nodes.size();
    //for(Node node: nodes){
    //    circuit.add_node(node);
    //}
    //vector<double> v(circuit.get_nodes().size(),0.7);
    vector<double> v{0,5,0.7,0.7};
    circuit.set_voltages(v);

    auto start = chrono::steady_clock::now();
    for(int i = 0; i < 1; i++){
        TransientSolver(circuit);
    }
    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;
    v = circuit.get_voltages();
    for(auto x: v){
        cout << x << endl;
    }
    //cout << "hello" << endl;
}