void TransientSolver(Circuit &circuit){
    bool incomplete = true;
    int current_iteration = 0;
    //iterate adjusting voltages each time
    while(incomplete){
        current_iteration++;
        //cout << "Iteration " << current_iteration << endl;
        //loop through the components
        for(Component* component: circuit.get_components()){
            //adjust all the diodes for the current voltage guess
            if(dynamic_cast<Diode*>(component)){
                ((Diode*)component)->set_vd(current_voltages[component->get_anode()] - current_voltages[component->get_cathode()]);
                ((Diode*)component)->set_id0(((Diode*)component)->get_current(current_voltages));
            }
        }
        //set the voltages to the output of the KCL with the components
        vector<double> new_voltages = NodeVoltageSolver(components,_nodesptr);
        //check the error
        for(int i=0; i<voltages.size(); i++){
            incomplete = (abs(current_voltages[i] - new_voltages[i])>max_error);
        }
        //set the stored voltages to the new voltages
        if(incomplete){
            current_voltages = new_voltages;
        }
        //check that max iterations haven't occured
        if(current_iteration >= max_iterations){
            cout << "Hit maximum iterations";
            incomplete = false;
        }
    }
}