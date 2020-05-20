#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <string>
#include "Component.hpp"
#include "KCLSolver.hpp"
#include "TransientSolver.hpp"
#include <memory> 
using namespace std;
using Eigen::MatrixXd;


vector<vector<double>> TransientAnalysis(vector<Component*> components, double timeperiod, int timesteps){
    vector<vector<double>> output;
    double time = 0;
    double deltatime = timeperiod/timesteps;
    vector<Node> nodes = NodeGenerator(components);
    vector<double> voltages(nodes.size(),0);
    vector<Capacitor*> capacitors;
    vector<Inductor*> inductors;
    for(Component* component: components){
        if(dynamic_cast<Capacitor*>(component)){
            capacitors.push_back(((Capacitor*)component));
        }
        if(dynamic_cast<Inductor*>(component)){
            inductors.push_back(((Inductor*)component));
        }
    }
    for(int step = 0; step < timesteps; step++){
        voltages = TransientSolver(voltages, time, components, 1000, 0.01, &nodes);
        output.push_back(voltages);
        for(Inductor* inductor: inductors){
          inductor->update_integral((voltages[inductor->get_anode()] - voltages[inductor->get_cathode()])*deltatime); 
        }
        for(Capacitor* capacitor: capacitors){
            cout << "Current " <<ComponentCurrent(voltages, nodes, ((Component*)capacitor)) << endl;
            capacitor->update_integral(ComponentCurrent(voltages, nodes, ((Component*)capacitor))*deltatime); 
        }
        for(auto x: voltages){
            cout << x << " ";
        }
        cout <<endl;
        time += deltatime;
    }
    return output;
}





int main(){
    Voltage_Source v1(1,0,"V1",5);
    Capacitor c1(2,1,"C1",0.1);
    Resistor r1(2,0,"R1",100);
    //Diode d1(3,0,"D1");
    vector<Component*> components{&v1,&c1,&r1};//,&d1};
    vector<vector<double>> output = TransientAnalysis(components, 1, 10);
    for(vector<double> vec: output){
        cout << vec[1] - vec[2] << endl;
    }
}