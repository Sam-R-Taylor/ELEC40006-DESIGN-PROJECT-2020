#ifndef LEO_KCLSOLVER_HPP
#define LEO_KCLSOLVER_HPP

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


Eigen::VectorXd Matrix_solver(const Circuit& input_circuit)
{ 
    std::cerr<< "in Matrix_solver" << std::endl;
    //initializing a matrix of the right size 
    int Mat_size = input_circuit.get_number_of_nodes();     
    Eigen::MatrixXd Mat(Mat_size,Mat_size);

    //initializing a vector of the right size
    Eigen::VectorXd Vec(Mat_size);

    //Fills up the matrix with 0s
    Mat.setZero(Mat_size,Mat_size);
    std::cerr<<"initialized Mat is "<<std::endl;
    std::cerr<< Mat <<std::endl;

    //Fills up the vector with 0s
    Vec.setZero(Mat_size);
    std::cerr<<"initialized vec is "<<std::endl;
    std::cerr<< Vec <<std::endl;

    //nodes needed to set up the KCL equations
    std::vector<Node> nodes = input_circuit.get_nodes();            //possible optimization here

    //needed to store voltage components
    std::vector<Voltage_Component*> voltage_components;


    std::cerr<<"initialized Matrix" << std::endl;


    //initializing each element of the conductance matrix
    //sum of currents out of a node = 0
    for (int row = 0; row < Mat_size; row++)
    {
        int node_index = row;

        //iterating through each componenet of the node considered
        for(Component* component_attached: nodes[node_index].components_attached)
        {
            std::cerr<<"node index is "  << node_index << std::endl;
            std::cerr<<"in loop considering "  << component_attached->get_name() << std::endl;
            //std::cerr<<"ptr is " << component_attached << std::endl;
            //std::cerr<<"dynamic cast gives " << dynamic_cast<Resistor*>(component_attached) << std::endl;

            //checking type of each component and act accordingly
            if(dynamic_cast<Resistor*>(component_attached))
            {
                int anode = component_attached->get_anode();
                int cathode = component_attached->get_cathode();

                Resistor* Rptr = dynamic_cast<Resistor*>(component_attached);

                //debug
                double conductance = Rptr->get_conductance();
                std::cerr<<"R "<< anode<< " "<< cathode <<" " << conductance << std::endl;

                if(anode == node_index)
                {
                    Mat(node_index,anode)+= Rptr->get_conductance();
                    Mat(node_index,cathode)-= Rptr->get_conductance();
                }
                else if (cathode == node_index)
                {
                    Mat(node_index,cathode)+= Rptr->get_conductance();
                    Mat(node_index,anode)-= Rptr->get_conductance();
                }
            }
            if(dynamic_cast<Current_source*>(component_attached))
            {
                int anode = component_attached->get_anode();
                int cathode = component_attached->get_cathode();
                Current_source* Currentptr = dynamic_cast<Current_source*>(component_attached);

                //debug
                double current = Currentptr->get_current();
                std::cerr<<"I "<< anode<< " "<< cathode <<" " << current << std::endl;

                if(anode == node_index)
                {
                    Vec(node_index) -= Currentptr->get_current();
                }
                else if (cathode == node_index)
                {
                    Vec(node_index) += Currentptr->get_current();
                }
            }
            if(dynamic_cast<Voltage_Component*>(component_attached))
            {
                int anode = component_attached->get_anode();
                int cathode = component_attached->get_cathode();
                Voltage_Component* Vptr = dynamic_cast<Voltage_Component*>(component_attached);

                //needs to be processed at the end
                voltage_components.push_back(Vptr);
                std::cerr<<"voltage component added is "<<component_attached->get_name()  << std::endl;
            }
        }
    }

    std::cerr<<"constructed Matrix" << std::endl;
    std::cerr<<"Mat is " << std::endl;
    std::cerr << Mat << std::endl;

        //processing voltage sources
    for(Voltage_Component* i : voltage_components)
    {
        std::cerr<<"no of voltage components is "<<voltage_components.size()  << std::endl;
        std::cerr<<"voltage component is "<<i->get_name()  << std::endl;
        int anode = i->get_anode() ;
        int cathode = i->get_cathode() ;

        if(dynamic_cast<Voltage_Source*>(i))
        {
            std::cerr<<"VS" << std::endl;
            Voltage_Source* Vptr = dynamic_cast<Voltage_Source*>(i);
            double voltage = Vptr->get_voltage();
            
            assert(anode != cathode);

            if(cathode == 0)
            {   
                Mat.row(anode).setZero();
                Mat(anode,anode) = 1;                   
                Vec(anode) = voltage;
                //std::cout << "Voltage " << Vec(anode) << std::endl;
            }
            else if (anode == 0)
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
        else if (dynamic_cast<Voltage_Controlled_Voltage_Source*>(i))
        {
            Voltage_Controlled_Voltage_Source* Vptr = dynamic_cast<Voltage_Controlled_Voltage_Source*>(i);
            std::cerr<<"VCVS " << std::endl;
            double gain = Vptr->get_gain();
            int control_anode = Vptr->get_control_anode() ;
            int control_cathode = Vptr->get_control_cathode();

            if(anode == 0)
            {
                //sets defining eq of VCVS at row cathode: V_anode - V_cathode = gain(V_control_anode - V_control_cathode)
                Mat.row(cathode).setZero();
                Vec(cathode) = 0;

                Mat(cathode,anode)+= 1;
                Mat(cathode,cathode)+= -1;
                Mat(cathode,control_anode)+= -gain;
                Mat(cathode,control_cathode)+= gain;
            }
            else if (cathode == 0)
            {
                //sets defining eq of VCVS at row anode: V_anode - V_cathode = gain(V_control_anode - V_control_cathode)
                Mat.row(anode).setZero();
                Vec(anode) = 0;

                Mat(anode,anode)+= 1;
                Mat(anode,cathode)+= -1;
                Mat(anode,control_anode)+= -gain;
                Mat(anode,control_cathode)+= gain;
            }
            else
            {
                //combines KCL equations
                Mat.row(cathode) += Mat.row(anode);
                Vec(cathode) += Vec(anode);
                
                //sets defining eq of VCVS at row anode: V_anode - V_cathode = gain(V_control_anode - V_control_cathode)
                Mat.row(anode).setZero();
                Vec(anode) = 0;
                
                Mat(anode,anode)+= 1;
                Mat(anode,cathode)+= -1;
                std::cerr<<"hello"<<std::endl;              
                Mat(anode,control_anode)+= -gain;
                Mat(anode,control_cathode)+= gain;
                
                
                
            }
            
             
        
        }
    }
    

    std::cerr<<"added voltage sources to Matrix" << std::endl;
    std::cerr<<"Mat is " << std::endl;
    std::cerr << Mat << std::endl;
    std::cerr<<"Vec is " << std::endl;
    std::cerr<< Vec << std::endl;
    //setting V0 to GND and removing corresponding row and column
    remove_Row(Mat,0);
    remove_Column(Mat,0);
    //std:: cout << "vec " << Vec(1);
    Vec = Vec.tail(Mat_size-1);
    //std::cerr<<"removed V0" << std::endl;

    std::cerr<<"Mat resized is " << std::endl;
    std::cerr << Mat << std::endl;
    std::cerr<<"Vec resized is " << std::endl;
    std::cerr<< Vec << std::endl;
    
    //finding the inverse matrix
    Eigen::VectorXd solution(Mat_size -1);
    solution = Mat.colPivHouseholderQr().solve(Vec);
    std::cerr<< "solution is " <<std::endl;
    std::cout << solution << std::endl;

    std::cerr<< "end of Matrix solver" << std::endl;
    return solution;
  
}


#endif
