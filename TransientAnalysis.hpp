#include <vector>
#include <iostream>
#include <eigen3/Eigen/Dense>
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


void NodeVoltagesToFile(Circuit& CKTIn , double CurrentTime){
    fstream myfile;
    myfile.open("output_voltage.txt",fstream::app);
    fstream myfile2;
    myfile2.open("output_current.txt",fstream::app);
    if (myfile.is_open())
    {
        myfile << CurrentTime << "," ;
        for(int i = 0; i < CKTIn.get_voltages().size(); i++) {
            myfile << CKTIn.get_voltages().at(i) << ",";
        }
        //myfile << CKTIn.get_voltages().at(CKTIn.get_voltages().size()-1);
        //COULD NOT REMOVE THE TRAILING , HOPE no affect?
        myfile << "\n";
        myfile.close();
    }else cout << "Unable to open voltage file";
    if(myfile2.is_open()){
        if(CurrentTime == 0){
            for(int i = 0; i < CKTIn.get_components().size(); i++){
            //for(Component* component: CKTIn.get_components()){
                if(dynamic_cast<BJT*>(CKTIn.get_components().at(i))){
                    myfile2 << CKTIn.get_components().at(i)->get_name() << "C" << ", ";
                    myfile2 << CKTIn.get_components().at(i)->get_name() << "B" << ", ";
                    myfile2 << CKTIn.get_components().at(i)->get_name() << "E" << ", ";
                }else{
                    myfile2 << CKTIn.get_components().at(i)->get_name() << ", ";
                }
            }
            /*if(dynamic_cast<BJT*>(CKTIn.get_components().at(CKTIn.get_components().size()-1))){
                myfile2 << CKTIn.get_components().at(CKTIn.get_components().size()-1)->get_name() << "C" << ", ";
                myfile2 << CKTIn.get_components().at(CKTIn.get_components().size()-1)->get_name() << "B" << ", ";
                myfile2 << CKTIn.get_components().at(CKTIn.get_components().size()-1)->get_name() << "E";
            }else{
                myfile2 << CKTIn.get_components().at(CKTIn.get_components().size()-1)->get_name();
            }*/
            myfile2 << "\n";
        }
        for(int i = 0; i < CKTIn.get_components().size(); i++){
        //for(Component* component: CKTIn.get_components()){
                if(dynamic_cast<BJT*>(CKTIn.get_components().at(i))){
                    std::vector<double> current = ((BJT*)CKTIn.get_components().at(i))->get_current(CKTIn.get_voltages());
                    myfile2 << current[0] << ", ";
                    myfile2 << current[1] << ", ";
                    myfile2 << current[2] << ", ";
                }else{
                    myfile2 << GetCurrent(CKTIn,CKTIn.get_components().at(i)) << ", ";
                }
        }
        /*if(dynamic_cast<BJT*>(CKTIn.get_components().at(CKTIn.get_components().size()-1))){
            std::vector<double> current = ((BJT*)CKTIn.get_components().at(CKTIn.get_components().size()-1))->get_current(CKTIn.get_voltages());
            myfile2 << current[0] << ", ";
            myfile2 << current[1] << ", ";
            myfile2 << current[2];
        }else{
            myfile2 << GetCurrent(CKTIn,CKTIn.get_components().at(CKTIn.get_components().size()-1));
        }*/
        myfile2 << "\n";
  }
  else cout << "Unable to open current file";
}

void UpdateNodeVoltages(Circuit &CKTIn , double CurrentTime,bool isLinear){
    //std::cout<< "update nodevoltages" << std::endl;
     //All this function does is update the integrals of each component and then passes the updates CKT to Transient Solver. 
     //Update Capacitors and inductors integrals
    double Vn;
    double In;
     //LOOPS THROUGH TO UPDATE NOT INTEGRAL COMPONENTS I.E AC SOURCE
      //IMPLEMENTATION ONLY WORKS FOR RLC 
    for(int i = 0 ; i < static_cast<int>(CKTIn.get_components().size()) ; i++){
        if (dynamic_cast<AC_Voltage_Source*>(CKTIn.get_components().at(i))){
            ((AC_Voltage_Source*)(CKTIn.get_components()[i]))->Set_Voltage(CurrentTime);
        }
    }
    if(isLinear){
        NodeVoltageSolver(CKTIn);
    }else{
        TransientSolver(CKTIn);
    }
    //std::cout<<"out of transientsolver " << std::endl;
    //LOOPS THROUGH TO UPDATE INTEGRAL COMPONENTS
    for(int i = 0 ; i < static_cast<int>(CKTIn.get_components().size()) ; i++){
        if(dynamic_cast<Capacitor*>(CKTIn.get_components().at(i))){ //DETERMINES THAT THE COMPONENT IS A CAPACITOR
            Vn = (CKTIn.get_voltages()[CKTIn.get_components()[i]->get_anode()]) - 
                ((CKTIn.get_voltages()[CKTIn.get_components()[i]->get_cathode()])) ;
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
    //std::cout<< "in setconductances" << std::endl;
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
bool IsLinear(Circuit & circuit){
    bool isLinear = true;
    for(Component* component: circuit.get_components()){
            //adjust all the diodes for the current voltage guess
        if(dynamic_cast<Diode*>(component)){
            isLinear = false; 
        }
        if(dynamic_cast<BJT*>(component)){
            //std::cout << "adjusting bjt" << std::endl;
            ((BJT*)component)->set_op(circuit.get_voltages());
            isLinear = false;
        }
    }
    return isLinear;
}
void TransientAnalysis(Circuit &CKTIn , double TimePeriod , int TimeStep){
    bool isLinear;
    isLinear = IsLinear(CKTIn);
    double CurrentTime = 0;
    std::cout <<"timestep is "<< TimeStep << std::endl;
    double deltaTime = (TimePeriod/TimeStep);
    remove("output_voltage.txt");
    fstream myfile ("output_voltage.txt");
    SetConductancesForSim(CKTIn,deltaTime); //SETS THE CONDUCTANCE FOR EACH INDUCTOR AND CAP THAT DEPENDS ON DELTA TIME (BUT REMAINS CONSTANT THROUGH SIM)
    for(double i = 0 ; i <= TimeStep ; i++){
        //outputs for vars in testing
        //std::cout << "timeperiod is "<< TimePeriod << std::endl;
        //std::cout << "deltatime is "<<deltaTime << std::endl;
        //std::cout << "current time is"<<CurrentTime << std::endl;
        //cout << TimeStep << endl;
        UpdateNodeVoltages(CKTIn , CurrentTime,isLinear); 
        NodeVoltagesToFile(CKTIn,CurrentTime);
        CurrentTime = CurrentTime + deltaTime;
        //POSSIBLE SHIFT ERROR MAY OCCUR. 
    }
}
