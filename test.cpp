#include "Circuit.hpp"
#include "Component.hpp"
#include "KCLSolver.hpp"
#include <iostream>
#include <chrono>
using namespace std;
int main(){
    Resistor r2{2,0,"R2",100};
    Resistor r1{1,2,"R1",100};
    Voltage_Source v1{1,0,"V1",5};
    vector<Component*> components{&v1,&r1,&r2};
    Circuit circuit;
    for(Component* component: components){
        circuit.add_component(component);
    }
    vector<Node> nodes = NodeGenerator(circuit.get_components());
    //cout << nodes.size();
    for(Node node: nodes){
        circuit.add_node(node);
    }
    vector<double> v(nodes.size(),0);
    circuit.set_voltages(v);

    auto start = chrono::steady_clock::now();
    for(int i = 0; i < 100000; i++){
        NodeVoltageSolver(circuit);
    }
    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;
    v = circuit.get_voltages();

        //for(auto x: v){
    //    cout << x << endl;
    //}
}