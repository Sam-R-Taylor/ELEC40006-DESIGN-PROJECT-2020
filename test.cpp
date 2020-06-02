#include "Circuit.hpp"
#include "Component.hpp"
#include "KCLSolver.hpp"
#include "TransientSolver.hpp"
#include "TransientAnalysis.hpp"
#include <iostream>
#include <chrono>
#include <vector>
using namespace std;
int main(){
    
    Voltage_Source v1{1,0,"V1",5};
    Diode d1{1,2,"D1"};
    Resistor r1{0,3,"R1",1000};
    Resistor r2{2,3,"R1",1000};
    //Resistor r1{1,2,"R1",500};
    //Resistor r2{0,2,"R2",500};
    //BJT_Component bj1{1,2,0,"BJ1",false,4.97512*pow(10,-3),0.25,100,10,0.3,0.2};
    //BJT_Component bj2{1,2,0,"BJ2",true,4.97512*pow(10,-3),0.25,1,10,0.3,0.2};
    //bj1.set_bjt(&bj2);
    //bj2.set_bjt(&bj1);
    //vector<Component*> components{&v1,&r2,&r1,&bj1,&bj2};
    vector<Component*> components{&v1,&r1,&d1,&r2};
    Circuit circuit;
    for(Component* component: components){
        circuit.add_component(component);
        cout << "added component " << component->get_name() <<endl;
    }
    //vector<Node> nodes = NodeGenerator(circuit.get_components());
    circuit.build_nodes();
    cout << "built node " << endl;
    vector<double> v{0,0,0};
    circuit.set_voltages(v);
    //auto start = chrono::steady_clock::now();
    for(int i = 0; i < 1; i++){
        cout << "running transient" << endl;
        TransientSolver(circuit);
    }
    //auto end = chrono::steady_clock::now();
    //auto diff = end - start;
    //cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;
    v = circuit.get_voltages();
    for(auto x: v){
        cout << x << endl;
    }
}