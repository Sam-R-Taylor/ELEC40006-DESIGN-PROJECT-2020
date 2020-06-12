#include "Circuit.hpp"
#include "Component.hpp"
#include "SparseKCLSolver.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <string>
using namespace std;

int main(){
    fstream myfile;
    myfile.open("complexity.txt",fstream::app);
    
for(int n = 5; n<2000; n+=5){
        //cout << n << endl;
        Voltage_Source* v = new Voltage_Source(1,0,"V1",1*n);
        vector<Component*> components{v};
        for(int i = 1; i<n; i++){
            Diode* r = new Diode(i+1,i,"D1");
            components.push_back(r);
        }
        Diode* d = new Diode(0,n,"D1");
        components.push_back(d);
        Circuit circuit;
        for(Component* component: components){
            circuit.add_component(component);
           // cout << "added component " << component->get_name() <<endl;
        } 
        //cout << "testing" << endl;
        circuit.build_nodes();
        //cout << "hello" << endl;
        auto start = chrono::steady_clock::now();
        for(int i = 0; i < 25; i++){
            Sparse_Matrix_solver(circuit);
        }
        //cout << "hello" << endl;
        //if(n == 100){
        //vector<double> voltages = circuit.get_voltages();
        //for(auto x: voltages){
        //   cout << x << endl;
        //}
        //}
        auto end = chrono::steady_clock::now();
        auto diff = end - start;
        myfile << (chrono::duration <double, milli> (diff).count()) / 25<< endl;
        
        
}

}