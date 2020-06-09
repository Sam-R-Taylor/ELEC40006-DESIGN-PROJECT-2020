#ifndef LEO_KCLSOLVER2_HPP
#define LEO_KCLSOLVER2_HPP

#include "Circuit.hpp"
#include "Component.hpp"

#include<Eigen/Dense>
#include<cassert>
#include<chrono>





void Matrix_solver(Circuit& input_circuit)
{ 
    //std::cerr<< "in Matrix_solver" << std::endl;
    //initializing a matrix of the right size 
    int Mat_size = input_circuit.get_number_of_nodes() - 1;     
    Eigen::MatrixXd Mat(Mat_size,Mat_size);

    //initializing a vector of the right size
    Eigen::VectorXd Vec(Mat_size);

    //Fills up the matrix with 0s
    Mat.setZero(Mat_size,Mat_size);
    //std::cerr<<"initialized Mat is "<<std::endl;
    //std::cerr<< Mat <<std::endl;

    //Fills up the vector with 0s
    Vec.setZero(Mat_size);
    //std::cerr<<"initialized vec is "<<std::endl;
    //std::cerr<< Vec <<std::endl;

    //std::cerr<<"initialized Matrix" << std::endl;

    //needed to store voltage components
    std::vector<Voltage_Source*> voltage_sources;

    //initializing each element of the conductance matrix
    //sum of currents out of a node = 0
    for(Component* i: input_circuit.get_components())
    {
        int anode = i->get_anode() -1;
        int cathode = i->get_cathode() -1;

        if(dynamic_cast<Resistor*>(i))
        {
            Resistor* Rptr = dynamic_cast<Resistor*>(i);
            double conductance = Rptr->get_conductance();

            //add coefficients for anode and cathode
            if(anode!=-1 && cathode!=-1)
            {
                Mat(anode,cathode)-=conductance;
                Mat(cathode,anode)-=conductance;
                Mat(anode,anode)+=conductance;
                Mat(cathode,cathode)+=conductance;
            }
            else if(anode!=-1)
            {
                Mat(anode,anode)+=conductance;
            }
            else
            {
                Mat(cathode,cathode)+=conductance;
            }
        }
        else if(dynamic_cast<Current_source*>(i))
        {
            Current_source* Currentptr = dynamic_cast<Current_source*>(i);
            double current = Currentptr->get_current();

            //add coefficients for anode
            if(anode!=-1){Vec(anode)-=current;}
            
            //add coefficients for cathode
            if(cathode!=-1){Vec(cathode)+=current;}
        }
        else if(dynamic_cast<Voltage_Source*>(i))
        {
            Voltage_Source* Vptr = dynamic_cast<Voltage_Source*>(i);

            //needs to be processed at the end
            voltage_sources.push_back(Vptr);
        }
        else if(dynamic_cast<Inductor*>(i))
        {
            Inductor* Iptr = dynamic_cast<Inductor*>(i);
            double current = Iptr->get_linear_current();
            double conductance = Iptr->get_conductance();

            //for Inductor current source and resistor
            
            //add coefficients for anode and cathode
            if(anode!=-1 && cathode!=-1)
            {
                
                Mat(anode,cathode)-=conductance;
                Mat(cathode,anode)-=conductance;
                Mat(anode,anode)+=conductance;
                Mat(cathode,cathode)+=conductance;

                Vec(anode)-=current;
                Vec(cathode)+=current;
            }
            else if(anode!=-1)
            {
                Mat(anode,anode)+=conductance;
                Vec(anode)-=current;
            }
            else
            {
                Mat(cathode,cathode)+=conductance;
                Vec(cathode)+=current;
            }
        }
        else if(dynamic_cast<Capacitor*>(i))
        {
            Capacitor* Cptr = dynamic_cast<Capacitor*>(i);
            double current = Cptr->get_linear_current();
            double conductance = Cptr->get_conductance();


            //for Capacitor current source and resistor
            
            //add coefficients for anode and cathode
            if(anode!=-1 && cathode!=-1)
            {
                
                Mat(anode,cathode)-=conductance;
                Mat(cathode,anode)-=conductance;
                Mat(anode,anode)+=conductance;
                Mat(cathode,cathode)+=conductance;

                Vec(anode)+=current;
                Vec(cathode)-=current;
            }
            else if(anode!=-1)
            {
                Mat(anode,anode)+=conductance;
                Vec(anode)+=current;
            }
            else
            {
                Mat(cathode,cathode)+=conductance;
                Vec(cathode)-=current;
            }
        }
        else if(dynamic_cast<Diode*>(i))
        {
            Diode* Dptr = dynamic_cast<Diode*>(i);
            double anode_coeff = Dptr->get_anode_coefficient();
            double cathode_coeff = Dptr->get_cathode_coefficient();
            double current = Dptr->get_constant_coefficient();
            std::cout << anode_coeff << " " << cathode_coeff << " " << current << std::endl;
            if(anode!=-1 && cathode!=-1)
            {             
                //add diode coefficients
                Mat(anode,anode)+=anode_coeff;
                Mat(anode,cathode)+=cathode_coeff;

                Mat(cathode,anode)-=anode_coeff;
                Mat(cathode,cathode)-=cathode_coeff;
                //for Diode current source
                Vec(anode)-=current;          
                Vec(cathode)+=current;
            }
            else if(anode!=-1)
            {
                Mat(anode,anode)+=anode_coeff;
                Vec(anode)-=current;
            }
            else
            {
                Mat(cathode,cathode)-=cathode_coeff;
                Vec(cathode)+=current;
            }
        }
        else if(dynamic_cast<BJT*>(i))
        {
            BJT* BJTptr = dynamic_cast<BJT*>(i);
            int base = BJTptr->get_base() -1;
            //std::cout << anode_coeff << " " << cathode_coeff << " " << current << std::endl;
            if(anode!=-1)
            {    
                //add collector coefficients
                Mat(anode,anode)+=BJTptr->get_collector_coefficient(anode+1);;
                if(cathode!=-1){
                    //collector emmitter
                    Mat(anode,cathode)+=BJTptr->get_emmitter_coefficient(anode+1);
                    Mat(cathode,anode)+=BJTptr->get_collector_coefficient(cathode+1);
                }
                if(base!=-1){
                    //collector base
                    Mat(anode,base)+=BJTptr->get_base_coefficient(anode+1);
                    Mat(base,anode)+=BJTptr->get_collector_coefficient(base+1);
                }
                Vec(anode)-=BJTptr->get_constant_coefficient(anode+1);
            }
            if(cathode!=-1)
            { 
                //add emmitter coefficients
                Mat(cathode,cathode)+=BJTptr->get_emmitter_coefficient(cathode+1);
                if(base!=-1){
                    //emmitter base 
                    Mat(cathode,base)+=BJTptr->get_base_coefficient(cathode+1);
                    Mat(base,cathode)+=BJTptr->get_emmitter_coefficient(base+1);
                }
                Vec(cathode)-=BJTptr->get_constant_coefficient(cathode+1);
            }
            if(base!=-1)
            { 
                //add base coefficients
                Mat(base,base)+=BJTptr->get_base_coefficient(base+1);
                Vec(base)-=BJTptr->get_constant_coefficient(base+1);
            }
        }
    }

    //std::cerr<<"constructed Matrix" << std::endl;

    //std::cout<<"Mat is " << std::endl;
    //std::cout << Mat << std::endl;
    //std::cout<<"Vec is " << std::endl;
    //std::cout << Vec << std::endl;

    //processing voltage sources
    for(Voltage_Source* voltage_source : voltage_sources)
    {
        int anode = voltage_source->get_anode() -1;
        int cathode = voltage_source->get_cathode() -1;
        double voltage = voltage_source->get_voltage();
        
        assert(anode != cathode);

        if(cathode == -1)
        {   
            Mat.row(anode).setZero();
            Mat(anode,anode) = 1;                   
            Vec(anode) = voltage;
            //std::cout << "Voltage " << Vec(anode) << std::endl;
        }
        else if (anode == -1)
        {
            Mat.row(cathode).setZero();
            Mat(cathode,cathode) = -1;
            Vec(cathode) = voltage;
            //std::cout << "Voltage cathode " << Vec(cathode) << std::endl;
        }
        else
        {   
            //combines KCL equations
            Mat.row(cathode) += Mat.row(anode);
            Vec(cathode) += Vec(anode);
            //sets anode node to V_anode - V_cathode = deltaV
            Mat.row(anode).setZero();
            Mat(anode,anode) = 1;
            Mat(anode,cathode) = -1;
            Vec(anode) = voltage;
        }
    }

    //std::cerr<<"added voltage sources to Matrix" << std::endl;
    //std::cerr<<"Mat is " << std::endl;
    //std::cerr << Mat << std::endl;
    //std::cerr<<"Vec is " << std::endl;
    //std::cerr<< Vec << std::endl;
    //setting V0 to GND and removing corresponding row and column
    
    
    //remove_Row(Mat,0);
    //remove_Column(Mat,0);
    
    //Vec = Vec.tail(Mat_size-1);

    //std::cerr<<"removed V0" << std::endl;

    //std::cerr<<"Mat resized is " << std::endl;
    //std::cerr << Mat << std::endl;
    //std::cerr<<"Vec resized is " << std::endl;
    //std::cerr<< Vec << std::endl;
    
    //finding the inverse matrix
    Eigen::VectorXd solution(Mat_size);
    Mat = Mat.inverse();
    //solution = Mat.colPivHouseholderQr().solve(Vec);
    solution = Mat * Vec;
    //std::cerr<< "solution is " <<std::endl;
    //std::cerr << solution << std::endl;


    input_circuit.set_voltages_eigen(solution);

    //std::cerr<< "end of Matrix solver" << std::endl;
    return;
  
}


#endif