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
    for(int n = 100; n<102; n++){
        Voltage_Source* v = new Voltage_Source(1,0,"V1",5);
        vector<Component*> components{v};
        for(int i = 1; i<n; i++){
            Resistor* r = new Resistor(i+1,i,"R1",1000);
            components.push_back(r);
        }
        Resistor* r = new Resistor(0,n,"R1",1000);
        components.push_back(r);
        Circuit circuit;
        for(Component* component: components){
            circuit.add_component(component);
           // cout << "added component " << component->get_name() <<endl;
        } 
        circuit.build_nodes();

        auto start = chrono::steady_clock::now();
        for(int i = 0; i < 3000; i++){
            NodeVoltageSolver(circuit);
        }
        auto end = chrono::steady_clock::now();
        auto diff = end - start;
        cout << chrono::duration <double, milli> (diff).count() << endl;
        
        
    }
}*/