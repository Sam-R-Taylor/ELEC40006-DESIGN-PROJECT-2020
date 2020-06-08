#include "Circuit.hpp"
#include "Component.hpp"
#include "KCLSolver.hpp"
#include "TransientSolver.hpp"
#include "TransientAnalysis.hpp"
#include "leo_KCLSolver2.hpp"
#include <iostream>
#include <chrono>
#include <vector>
using namespace std;
int main(){
    Voltage_Source v2{1,0,"V1",5};
    //BJT bj{3,2,1,"BJ", 0.67, 0.995,1,10,0.2,0.3,true};
    //Resistor r2{2,1,"R1",1000};
    Resistor r5{2,0,"R1",1000};
    Resistor r6{1,2,"R1",1000};
    //vector<Component*> components{&v2,&r2,&d1,&r3};
    //vector<Component*> components{&v2,&r2,&r5,&r6,&bj};
    vector<Component*> components{&v2,&r5,&r6};
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
    vector<double> v{0,0,0};
    circuit.set_voltages(v);
    
    auto start = chrono::steady_clock::now();
    
    for(int i = 0; i < 1; i++){
        Matrix_solver(circuit);
    }
    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    //cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;

    v = circuit.get_voltages();
    for(auto x: v){
        cout << x << endl;
    }
    

}