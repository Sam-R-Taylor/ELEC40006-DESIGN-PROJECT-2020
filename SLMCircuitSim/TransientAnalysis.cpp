#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <string>
#include "Component.hpp"
#include <memory> 
#include "Circuit.hpp"
#include <fstream>

// HAD TO REMOVE BOTH KCL AND TRANSIENT SOLVER FROM THE #INCLUDE AS THEY DID NOT WORK 
// Both threw compilation errors.
//included fstream where for file output
using namespace std;
using Eigen::MatrixXd;

/*{
vector<vector<double>> TransientAnalysis(vector<Component*> components, double timeperiod, int timesteps){
    //vector to store voltages at each time step
    vector<vector<double>> output;
    double time = 0;
    double deltatime = timeperiod/timesteps;
    //create a vector of nodes
    vector<Node> nodes = NodeGenerator(components);
    //creating a vector of strting guess voltages (all 0) one for each node
    vector<double> voltages(nodes.size(),0);
    //create a vector of capacitors and inductors for integration later
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
    //loop through the time steps
    for(int step = 0; step < timesteps; step++){
        //solve the voltages at the current integration values of the components
        voltages = TransientSolver(voltages, time, components, 1000, 0.01, &nodes);
        //store the voltages
        output.push_back(voltages);
        //integrate the capacitors and inductors
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
        //increment the time
        time += deltatime;
    }
    return output;
}





int main(){
    Voltage_Source v1(1,0,"V1",5);
    //Voltage_Source v2(2,1,"V2",0);
    Capacitor c1(2,1,"C1",0.01);
    Resistor r1(3,2,"R1",100);
    Diode d1(3,0,"D1");
    vector<Component*> components{&v1,&c1,&d1,&r1};//,&d1};
    vector<vector<double>> output = TransientAnalysis(components, 1, 10);
    for(vector<double> vec: output){
        cout << vec[2] - vec[3] << endl;
    }
}
}
*/

void NodeVoltagesToFile(vector<double> CKTIn2 , double CurrentTime){
  fstream myfile;
  myfile.open("output.txt",fstream::app);
  
  if (myfile.is_open())
  {
        myfile << CurrentTime << "," ;
    for(int i = 0; i < CKTIn2.size(); i++) {
        myfile << CKTIn2.at(i) << ",";
    }
    //COULD NOT REMOVE THE TRAILING , HOPE no affect?
    myfile << "\n";
    myfile.close();
  }
  else cout << "Unable to open file";
}

void UpdateNodeVoltages(Circuit CKTIn){
     
     //NOT IMPLEMENTED

}

void TransientAnalysis(Circuit CKTIn , double TimePeriod , double TimeStep){
    double CurrentTime = 0;
    remove("output.txt");
    fstream myfile ("output.txt");
    
    do {
        NodeVoltagesToFile(CKTIn.get_voltages(),CurrentTime); 
        UpdateNodeVoltages(CKTIn); //UNimplemented

        CurrentTime = CurrentTime + TimeStep ; 
    }
    while (CurrentTime < TimePeriod);

    
}




int main(){
    double TimePeriod , TimeStep ; 
    //Circuit CK1 = ;
    cin >> TimePeriod;
    cin >> TimeStep ;
    //TransientAnalysis(CK1 , TimePeriod , TimeStep);
    cout << "Operation Complete";
    return 0;  

}