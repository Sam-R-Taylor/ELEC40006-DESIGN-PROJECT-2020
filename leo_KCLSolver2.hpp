#ifndef LEO_KCLSOLVER2_HPP
#define LEO_KCLSOLVER2_HPP

#include "Circuit.hpp"
#include "Component.hpp"

#include<Eigen/Dense>
#include<cassert>


//helper functions for Matrix_solver
void remove_Row(Eigen::MatrixXd& matrix, unsigned int rowToRemove)
{
    unsigned int numRows = matrix.rows()-1;
    unsigned int numCols = matrix.cols();

    if( rowToRemove < numRows )
        matrix.block(rowToRemove,0,numRows-rowToRemove,numCols) = matrix.block(rowToRemove+1,0,numRows-rowToRemove,numCols);

    matrix.conservativeResize(numRows,numCols);
}

void remove_Column(Eigen::MatrixXd& matrix, unsigned int colToRemove)
{
    unsigned int numRows = matrix.rows();
    unsigned int numCols = matrix.cols()-1;

    if( colToRemove < numCols )
        matrix.block(0,colToRemove,numRows,numCols-colToRemove) = matrix.block(0,colToRemove+1,numRows,numCols-colToRemove);

    matrix.conservativeResize(numRows,numCols);
}


void Matrix_solver(Circuit& input_circuit)
{ 
    //std::cerr<< "in Matrix_solver" << std::endl;
    //initializing a matrix of the right size 
    int Mat_size = input_circuit.get_number_of_nodes();     
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
        int anode = i->get_anode();
        int cathode = i->get_cathode();

        if(dynamic_cast<Resistor*>(i))
        {
            Resistor* Rptr = dynamic_cast<Resistor*>(i);
            double conductance = Rptr->get_conductance();

            //add coefficients for anode
            Mat(anode,anode)+=conductance;
            Mat(anode,cathode)-=conductance;

            //add coefficients for cathode
            Mat(cathode,cathode)+=conductance;
            Mat(cathode,anode)-=conductance;
        }
        else if(dynamic_cast<Current_source*>(i))
        {
            Current_source* Currentptr = dynamic_cast<Current_source*>(i);
            double current = Currentptr->get_current();

            //add coefficients for anode
            Vec(anode)-=current;

            //add coefficients for cathode
            Vec(cathode)+=current;
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

            //for Inductor current source

            //add coefficients for anode
            Vec(anode)-=current;
            //add coefficients for cathode
            Vec(cathode)+=current;

            //for Inductor resistor

            //add coefficients for anode
            Mat(anode,anode)+=conductance;
            Mat(anode,cathode)-=conductance;

            //add coefficients for cathode
            Mat(cathode,cathode)+=conductance;
            Mat(cathode,anode)-=conductance;
        }
        else if(dynamic_cast<Capacitor*>(i))
        {
            Capacitor* Cptr = dynamic_cast<Capacitor*>(i);
            double current = Cptr->get_linear_current();
            double conductance = Cptr->get_conductance();

            //for Capacitor current source

            //add coefficients for anode
            Vec(anode)-=current;
            //add coefficients for cathode
            Vec(cathode)+=current;

            //for Capacitor resistor

            //add coefficients for anode
            Mat(anode,anode)+=conductance;
            Mat(anode,cathode)-=conductance;

            //add coefficients for cathode
            Mat(cathode,cathode)+=conductance;
            Mat(cathode,anode)-=conductance;
        }
        else if(dynamic_cast<Diode*>(i))
        {
            Diode* Dptr = dynamic_cast<Diode*>(i);
            double anode_coeff = Dptr->get_anode_coefficient();
            double cathode_coeff = Dptr->get_cathode_coefficient();
            double current = Dptr->get_constant_coefficient();

            //add diode coefficients
            Mat(anode,anode)+=anode_coeff;
            Mat(cathode,anode)+=anode_coeff;

            Mat(cathode,cathode)-=cathode_coeff;
            Mat(anode,cathode)-=cathode_coeff;

            //for Diode current source
            Vec(anode)-=current;          
            Vec(cathode)+=current;
        }
    }

    //std::cerr<<"constructed Matrix" << std::endl;

    std::cerr<<"Mat is " << std::endl;
    std::cerr << Mat << std::endl;

    //processing voltage sources
    for(Voltage_Source* voltage_source : voltage_sources)
    {
        int anode = voltage_source->get_anode();
        int cathode = voltage_source->get_cathode();
        double voltage = voltage_source->get_voltage();
        
        assert(anode != cathode);

        if(cathode == 0)
        {   
            Mat.row(anode).setZero();
            Mat(anode,anode) = 1;                   
            Vec(anode) = voltage;
        }
        else if (anode == 0)
        {
            Mat.row(cathode).setZero();
            Mat(cathode,cathode) = -1;
            Vec(cathode) = voltage;
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
    remove_Row(Mat,0);
    remove_Column(Mat,0);
    
    Vec = Vec.tail(Mat_size-1);
    //std::cerr<<"removed V0" << std::endl;

    //std::cerr<<"Mat resized is " << std::endl;
    //std::cerr << Mat << std::endl;
    //std::cerr<<"Vec resized is " << std::endl;
    //std::cerr<< Vec << std::endl;
    
    //finding the inverse matrix
    Eigen::VectorXd solution(Mat_size -1);
    solution = Mat.colPivHouseholderQr().solve(Vec);
    //std::cerr<< "solution is " <<std::endl;
    //std::cerr << solution << std::endl;


    input_circuit.set_voltages_eigen(solution);

    //std::cerr<< "end of Matrix solver" << std::endl;
    return;
  
}


#endif