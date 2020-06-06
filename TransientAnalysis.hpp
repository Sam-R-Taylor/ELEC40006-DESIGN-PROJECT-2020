#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <string>
#include "Component.hpp"
#include <memory> 
#include "Circuit.hpp"
#include <fstream>
#include "TransientSolver.hpp"
#include "KCLSolver.hpp"

// HAD TO REMOVE BOTH KCL AND TRANSIENT SOLVER FROM THE #INCLUDE AS THEY DID NOT WORK 
// Both threw compilation errors.
//included fstream where for file output
using namespace std;
using Eigen::MatrixXd;


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

void UpdateNodeVoltages(Circuit &CKTIn , double CurrentTime){

    //std::cerr <<"in UpdateNodeVoltages"<< std::endl;
     //All this function does is update the integrals of each component and then passes the updates CKT to Transient Solver. 
     //Update Capacitors and inductors integrals
    double Vn;
    double In;
     //LOOPS THROUGH TO UPDATE NOT INTEGRAL COMPONENTS I.E AC SOURCE
      //IMPLEMENTATION ONLY WORKS FOR RLC 
    for(int i = 0 ; i < CKTIn.get_components().size() ; i++){
        if (dynamic_cast<AC_Voltage_Source*>(CKTIn.get_components().at(i))){
            ((AC_Voltage_Source*)(CKTIn.get_components()[i]))->Set_Voltage(CurrentTime);
        }
    }

    //std::cerr <<"before TransientSolver"<< std::endl;
    TransientSolver(CKTIn);
    //std::cerr <<"after TransientSolver"<< std::endl;
    //LOOPS THROUGH TO UPDATE INTEGRAL COMPONENTS
    for(int i = 0 ; i < CKTIn.get_components().size() ; i++){
        if(dynamic_cast<Capacitor*>(CKTIn.get_components().at(i))){ //DETERMINES THAT THE COMPONENT IS A CAPACITOR
            Vn = (CKTIn.get_voltages()[CKTIn.get_components()[i]->get_anode()]) - ((CKTIn.get_voltages()[CKTIn.get_components()[i]->get_cathode()])) ;
            ((Capacitor*)(CKTIn.get_components()[i]))->set_linear_current(Vn);
        }
        else if (dynamic_cast<Inductor*>(CKTIn.get_components().at(i)))
        {
            In = GetCurrent(CKTIn,CKTIn.get_components()[i]); //GETCURRENT WILL BE IMPLEMENTED BY MAX ,
            ((Inductor*)(CKTIn.get_components()[i]))->set_linear_current(In) ; 
        }
    }  
}

void SetConductancesForSim(Circuit &CKTIn , double deltatime){
    for(int i = 0 ; i < CKTIn.get_components().size() ; i++){
        if(dynamic_cast<Capacitor*>(CKTIn.get_components().at(i))){ //DETERMINES THAT THE COMPONENT IS A CAPACITOR
           ((Capacitor*)(CKTIn.get_components()[i]))->set_conductance(deltatime); 
        }
        else if (dynamic_cast<Inductor*>(CKTIn.get_components().at(i)))
        {
            ((Inductor*)(CKTIn.get_components()[i]))->set_conductance(deltatime); 
        }
    }  

}

void TransientAnalysis(Circuit &CKTIn , double TimePeriod , int TimeStep){

    //std::cerr<<"in TransientAnalysis" << std::endl;

    double CurrentTime = 0;
    double deltaTime = (TimePeriod/TimeStep);
    remove("output.txt");
    fstream myfile ("output.txt");
    SetConductancesForSim(CKTIn,deltaTime); //SETS THE CONDUCTANCE FOR EACH INDUCTOR AND CAP THAT DEPENDS ON DELTA TIME (BUT REMAINS CONSTANT THROUGH SIM)

    //std::cerr<<"setConductances" << std::endl;

    for(double i = 0 ; i <= TimeStep ; i++){
        //std::cerr<<"in for loop" << std::endl;
        UpdateNodeVoltages(CKTIn , CurrentTime); 
        //std::cerr<<"NodeVoltagesToFile" << std::endl;
        NodeVoltagesToFile(CKTIn.get_voltages(),CurrentTime);
        CurrentTime = CurrentTime + deltaTime;
        //POSSIBLE SHIFT ERROR MAY OCCUR. 
    }
}