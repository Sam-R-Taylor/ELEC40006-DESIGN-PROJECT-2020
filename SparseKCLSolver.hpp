#ifndef KCLSOLVER2_HPP
#define KCLSOLVER2_HPP

#include "Circuit.hpp"
#include "Component.hpp"
#include <memory>
#include <cassert>
#include <vector>
#include <eigen3/Eigen/Sparse>

using namespace Eigen;

std::vector<double> set_voltages_eigen(Eigen::VectorXd& solution)
{
        std::vector<double> voltages{0};
        //std::cerr <<"setting voltages" << std::endl;
        
        for(int i = 0; i<solution.size(); i++)
        {
            //std::cerr <<"index " << solution(i) << std::endl;
            //voltages[i+1] = solution(i);
            voltages.push_back(solution(i));
        }
        return voltages;
}
    std::vector<double> set_voltages_eigen(Eigen::SparseVector<double> & solution)
    {
        std::vector<double> voltages{0};
        //std::cerr <<"setting voltages" << std::endl;
        for(int i = 0; i<solution.size(); i++)
        {
            //std::cerr <<"index " << solution(i) << std::endl;
            //voltages[i+1] = solution.coeffRef(i);
            voltages.push_back(solution.coeffRef(i));
        }
        return voltages;
    }
void Set_Zero(Eigen::SparseMatrix<double>& matrix, int row){
    int n = matrix.rows();
    for(int i = 0; i < n; i ++){
        matrix.coeffRef(row,i) = 0;
    }
    //for (Eigen::SparseMatrix<double, Eigen::RowMajor>::InnerIterator it(matrix, row); it; ++it)   
    //{it.valueRef() = 0;}
}

void Sparse_Matrix_solver(Circuit& input_circuit, bool OP = false)
{ 
    //initializing a matrix of the right size 
    int Mat_size = input_circuit.get_number_of_nodes() - 1;     
    Eigen::SparseMatrix<double> Mat(Mat_size,Mat_size);

    //initializing a vector of the right size
    Eigen::SparseVector<double> Vec(Mat_size);

    //Fills up the matrix with 0s
    //Mat.setZero(Mat_size,Mat_size);

    //Fills up the vector with 0s
    //Vec.setZero(Mat_size);

    //needed to store voltage components
    std::vector<Voltage_Component*> voltage_components;

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
                Mat.coeffRef(anode,cathode)-=conductance;
                Mat.coeffRef(cathode,anode)-=conductance;
                Mat.coeffRef(anode,anode)+=conductance;
                Mat.coeffRef(cathode,cathode)+=conductance;
            }
            else if(anode!=-1)
            {
                Mat.coeffRef(anode,anode)+=conductance;
            }
            else
            {
                Mat.coeffRef(cathode,cathode)+=conductance;
            }
        }
        else if(dynamic_cast<Current_source*>(i))
        {
            Current_source* Currentptr = dynamic_cast<Current_source*>(i);
            double current = Currentptr->get_current();

            //add coefficients for anode
            if(anode!=-1){Vec.coeffRef(anode)-=current;}
            
            //add coefficients for cathode
            if(cathode!=-1){Vec.coeffRef(cathode)+=current;}
        }
        else if(dynamic_cast<Voltage_Component*>(i))
        {
            Voltage_Component* Vptr = dynamic_cast<Voltage_Component*>(i);

            //needs to be processed at the end
            voltage_components.push_back(Vptr);
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
                
                Mat.coeffRef(anode,cathode)-=conductance;
                Mat.coeffRef(cathode,anode)-=conductance;
                Mat.coeffRef(anode,anode)+=conductance;
                Mat.coeffRef(cathode,cathode)+=conductance;

                Vec.coeffRef(anode)-=current;
                Vec.coeffRef(cathode)+=current;
            }
            else if(anode!=-1)
            {
                Mat.coeffRef(anode,anode)+=conductance;
                Vec.coeffRef(anode)-=current;
            }
            else
            {
                Mat.coeffRef(cathode,cathode)+=conductance;
                Vec.coeffRef(cathode)+=current;
            }
        }
        else if(dynamic_cast<Voltage_Controlled_Current_Source*>(i))
        {
            Voltage_Controlled_Current_Source* VCCSptr = dynamic_cast<Voltage_Controlled_Current_Source*>(i);
            double gain = VCCSptr->get_gain();
            double control_anode = VCCSptr->get_control_anode() -1;
            double control_cathode = VCCSptr->get_control_cathode() -1;

            //add coefficients for anode and cathode
            if(anode!=-1 && cathode!=-1)
            {   
                if(control_cathode != -1)
                {
                    Mat.coeffRef(anode,control_cathode)-=gain;
                    Mat.coeffRef(cathode,control_cathode)+=gain;
                }
                if(control_anode != -1)
                {
                    Mat.coeffRef(cathode,control_anode)-=gain;
                    Mat.coeffRef(anode,control_anode)+=gain;
                }
            }
            else if(anode!=-1)
            {
                if(control_cathode != -1)
                {
                    Mat.coeffRef(anode,control_cathode)-=gain;
                }
                if(control_anode != -1)
                {
                    Mat.coeffRef(anode,control_anode)+=gain;
                }
            }
            else
            {
                if(control_cathode != -1)
                {
                    Mat.coeffRef(cathode,control_cathode)+=gain;
                }
                if(control_anode != -1)
                {
                    Mat.coeffRef(cathode,control_anode)-=gain;
                }
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
                
                Mat.coeffRef(anode,cathode)-=conductance;
                Mat.coeffRef(cathode,anode)-=conductance;
                Mat.coeffRef(anode,anode)+=conductance;
                Mat.coeffRef(cathode,cathode)+=conductance;

                Vec.coeffRef(anode)+=current;
                Vec.coeffRef(cathode)-=current;
            }
            else if(anode!=-1)
            {
                Mat.coeffRef(anode,anode)+=conductance;
                Vec.coeffRef(anode)+=current;
            }
            else
            {
                Mat.coeffRef(cathode,cathode)+=conductance;
                Vec.coeffRef(cathode)-=current;
            }
        }
        else if(dynamic_cast<Diode*>(i))
        {
            Diode* Dptr = dynamic_cast<Diode*>(i);
            double anode_coeff = Dptr->get_anode_coefficient();
            double cathode_coeff = Dptr->get_cathode_coefficient();
            double current = Dptr->get_constant_coefficient();
            //std::cout << anode_coeff << " " << cathode_coeff << " " << current << std::endl;
            if(anode!=-1 && cathode!=-1)
            {             
                //add diode coefficients
                Mat.coeffRef(anode,anode)+=anode_coeff;
                Mat.coeffRef(anode,cathode)+=cathode_coeff;

                Mat.coeffRef(cathode,anode)-=anode_coeff;
                Mat.coeffRef(cathode,cathode)-=cathode_coeff;

                Vec.coeffRef(anode)-=current;          
                Vec.coeffRef(cathode)+=current;
            }
            else if(anode!=-1)
            {
                Mat.coeffRef(anode,anode)+=anode_coeff;
                Vec.coeffRef(anode)-=current;
            }
            else
            {
                Mat.coeffRef(cathode,cathode)-=cathode_coeff;
                Vec.coeffRef(cathode)+=current;
            }
        }
        else if(dynamic_cast<BJT*>(i))
        {
            BJT* BJTptr = dynamic_cast<BJT*>(i);
            int base = BJTptr->get_base() -1;
            if(anode!=-1)
            {    
                //add collector coefficients
                Mat.coeffRef(anode,anode)+=BJTptr->get_collector_coefficient(anode+1);;
                if(cathode!=-1){
                    //collector emmitter
                    Mat.coeffRef(anode,cathode)+=BJTptr->get_emmitter_coefficient(anode+1);
                    Mat.coeffRef(cathode,anode)+=BJTptr->get_collector_coefficient(cathode+1);
                }
                if(base!=-1){
                    //collector base
                    Mat.coeffRef(anode,base)+=BJTptr->get_base_coefficient(anode+1);
                    Mat.coeffRef(base,anode)+=BJTptr->get_collector_coefficient(base+1);
                }
                Vec.coeffRef(anode)-=BJTptr->get_constant_coefficient(anode+1);
            }
            if(cathode!=-1)
            { 
                //add emmitter coefficients
                Mat.coeffRef(cathode,cathode)+=BJTptr->get_emmitter_coefficient(cathode+1);
                if(base!=-1){
                    //emmitter base 
                    Mat.coeffRef(cathode,base)+=BJTptr->get_base_coefficient(cathode+1);
                    Mat.coeffRef(base,cathode)+=BJTptr->get_emmitter_coefficient(base+1);
                }
                Vec.coeffRef(cathode)-=BJTptr->get_constant_coefficient(cathode+1);
            }
            if(base!=-1)
            { 
                //add base coefficients
                Mat.coeffRef(base,base)+=BJTptr->get_base_coefficient(base+1);
                Vec.coeffRef(base)-=BJTptr->get_constant_coefficient(base+1);
            }
        }
    }

    //processing voltage sources
    for(Voltage_Component* i : voltage_components)
    {
        int anode = i->get_anode() -1;
        int cathode = i->get_cathode() -1;

        if(dynamic_cast<Voltage_Source*>(i) || dynamic_cast<AC_Voltage_Source*>(i))
        {            
            double voltage = i->get_voltage();
            
            assert(anode != cathode);

            if(cathode == -1)
            {   
                Set_Zero(Mat,anode);
                //Mat.row(anode).setZero();
                Mat.coeffRef(anode,anode) = 1;                   
                Vec.coeffRef(anode) = voltage;
            }
            else if (anode == -1)
            {
                Set_Zero(Mat,cathode);
                //Mat.row(cathode).setZero();
                Mat.coeffRef(cathode,cathode) = -1;
                Vec.coeffRef(cathode) = voltage;
            }
            else
            {   
                //combines KCL equations
                for(int i = 0; i < input_circuit.get_number_of_nodes() -1; i++){
                    Mat.coeffRef(cathode,i) += Mat.coeffRef(anode,i);
                }
//                Mat.row(cathode) += Mat.row(anode);
                Vec.coeffRef(cathode) += Vec.coeffRef(anode);
                //sets anode node to V_anode - V_cathode = deltaV
                Set_Zero(Mat,anode);
                //Mat.row(anode).setZero();
                Mat.coeffRef(anode,anode) = 1;
                Mat.coeffRef(anode,cathode) = -1;
                Vec.coeffRef(anode) = voltage;
            }
        }
        else if (dynamic_cast<Voltage_Controlled_Voltage_Source*>(i))
        {
            Voltage_Controlled_Voltage_Source* Vptr = dynamic_cast<Voltage_Controlled_Voltage_Source*>(i);
            double gain = Vptr->get_gain();
            int control_anode = Vptr->get_control_anode() -1;
            int control_cathode = Vptr->get_control_cathode() -1;

            assert(anode != cathode);

            if(anode == -1)
            {
                //sets defining eq of VCVS at row cathode: V_anode - V_cathode = gain(V_control_anode - V_control_cathode)
                Set_Zero(Mat,cathode); 
                //Mat.row(cathode).setZero();
                Vec.coeffRef(cathode) = 0;

                //Mat(cathode,anode)+= 1;
                Mat.coeffRef(cathode,cathode)+= -1;
                if(control_anode != -1) Mat.coeffRef(cathode,control_anode)+= -gain;
                if(control_cathode!=-1) Mat.coeffRef(cathode,control_cathode)+= gain;
            }
            else if (cathode == -1)
            {
                //sets defining eq of VCVS at row anode: V_anode - V_cathode = gain(V_control_anode - V_control_cathode)
                Set_Zero(Mat,anode);
                //Mat.row(anode).setZero();
                Vec.coeffRef(anode) = 0;

                Mat.coeffRef(anode,anode)+= 1;
                //Mat(anode,cathode)+= -1;
                if(control_anode != -1) Mat.coeffRef(anode,control_anode)+= -gain;
                if(control_cathode!=-1) Mat.coeffRef(anode,control_cathode)+= gain;
            }
            else
            {
                //combines KCL equations
                for(int i = 0; i < input_circuit.get_number_of_nodes() -1; i++){
                    Mat.coeffRef(cathode,i) += Mat.coeffRef(anode,i);
                }
                //Mat.row(cathode) += Mat.row(anode);
                Vec.coeffRef(cathode) += Vec.coeffRef(anode);
                
                //sets defining eq of VCVS at row anode: V_anode - V_cathode = gain(V_control_anode - V_control_cathode)
                Set_Zero(Mat,anode);
                //Mat.row(anode).setZero();
                Vec.coeffRef(anode) = 0;
                
                Mat.coeffRef(anode,anode)+= 1;
                Mat.coeffRef(anode,cathode)+= -1;              
                if(control_anode != -1) Mat.coeffRef(anode,control_anode)+= -gain;
                if(control_cathode!=-1) Mat.coeffRef(anode,control_cathode)+= gain;             
            }       
        }
    }
    //if OP is true replace all inductors with short circuits
    if(OP){
        //std::cout << "IN OP" << std::endl;
        for(Component* component: input_circuit.get_components()){
            if(dynamic_cast<Inductor*>(component)){
                //std::cout << "IN Inductor" << std::endl;
                if(component->get_cathode()!=0&&component->get_anode()!=0){
                    //std::cout << "IN cathode" << std::endl;
                    for(int i = 0; i < input_circuit.get_number_of_nodes() -1; i++){
                        Mat.coeffRef(component->get_cathode()-1,i) += Mat.coeffRef(component->get_anode()-1,i);
                        Mat.coeffRef(component->get_anode()-1,i) = 0;
                    }
                    Mat.coeffRef(component->get_anode()-1,component->get_anode()-1) = 1;
                    Mat.coeffRef(component->get_anode()-1,component->get_cathode()-1) = -1;
                    Vec.coeffRef(component->get_anode()-1) = 0;

                }else if(component->get_cathode()!=0){
                    //std::cout << "IN anode" << std::endl;
                    for(int i = 0; i < input_circuit.get_number_of_nodes() -1; i++){
                        Mat.coeffRef(component->get_cathode()-1,i) = 0;
                    }
                    Mat.coeffRef(component->get_cathode()-1,component->get_cathode()-1) = -1;
                    Vec.coeffRef(component->get_cathode()-1) = 0;
                    
                }else{
                    //std::cout << "IN ELSE" << std::endl;
                    //std::cout << input_circuit.get_number_of_nodes() -1 << std::endl;
                    for(int i = 0; i < input_circuit.get_number_of_nodes() -1; i++){
                        Mat.coeffRef(component->get_anode()-1,i) = 0;
                    }
                    Mat.coeffRef(component->get_anode()-1,component->get_anode()-1) = 1;
                    Vec.coeffRef(component->get_anode()-1) = 0;
                    
                }   
            }        
        }
    }
    //finding the inverse matrix
    Mat.makeCompressed();
    Eigen::SparseVector<double> solution(Mat_size);
    Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
    solver.compute(Mat);
    solution = solver.solve(Vec);
    //std::cout << Vec << std::endl;
    //Mat = Mat.inverse();
    //solution = Mat * Vec;
    std::vector<double> voltages;
    voltages = set_voltages_eigen(solution);
    input_circuit.set_voltages(voltages);
    return;
}


#endif
