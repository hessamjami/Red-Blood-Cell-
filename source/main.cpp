/** @file doxygen_example.cpp
 @author Lastname:Firstname:A00123456:cscxxxxx
 @version Revision 1.1
 @brief Illustrates doxygen-style comments for documenting a C++
 program file and the functions in that file.
 @details If you want to add any further detailed description of
 what is in the file, then place it here (after the first statement)
 and it will appear in the detailed description section of the HTML
 output description for the file.
 @date Monday, September 19, 2011
 */

/// \file

#include <stdio.h>
#include <ctime>
#include <sstream>
#include <map>
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <random>
#include <string>
#include <math.h>

#include "Membrane.h"
#include "Chromatin.h"
#include "Actin.h"
#include "ECM.h"

#include "General_functions.hpp"
#include "write_functions.hpp"
#include "interaction.hpp"
#include "maps.hpp"
#include "Global_functions.hpp"
#include "OpenMM_structs.h"
#include "OpenMM_funcs.hpp"


/** -----------------------------------------------------------------------------
 *                           OpenMM-USING CODE
 * -----------------------------------------------------------------------------
 * The OpenMM API is visible only at this point and below. Normally this would
 * be in a separate compilation module; we're including it here for simplicity.
 * -----------------------------------------------------------------------------
 */

// Suppress irrelevant warnings from Microsoft's compiler.
#ifdef _MSC_VER
#pragma warning(disable:4996)   // sprintf is unsafe
#endif

#include "OpenMM.h"


namespace GenConst {
    int MD_num_of_steps;
    double Simulation_Time_In_Ps;
    int MD_traj_save_step;
    double Report_Interval_In_Fs;
    double Step_Size_In_Fs;
    double MD_T;
    double K;
    int MD_thrmo_step;
    int MC_step;
    double Mem_fluidity;
    double Lbox;
    bool Periodic_condtion_status;
    int Num_of_Membranes;
    int Num_of_Chromatins;
    int Num_of_Actins;
    int Num_of_ECMs;
    int Num_of_pointparticles;
    string trajectory_file_name;;
    double Buffer_temperature;
    double Bussi_tau;
    double Actin_Membrane_Bond_Coefficient;
    
}



static const bool   WantEnergy   = true;




const int EndOfList=-1;






int main(int argc, char **argv)
{
    // get the current time.
    time_t t = time(0);
    struct tm * now = localtime( & t );
    char buffer [80];
    strftime (buffer,80,"%Y_%m_%d_time_%H_%M",now);
    
    string general_file_name="general-config.txt";
    cout<<"Please enter the path (relative to the binary file) + name of the config file:\n";
    cin>>general_file_name;
    clock_t tStart = clock();//Time the programme
    vector<string> membrane_config_list;
    vector<string> chromatin_config_list;
    vector<string> actin_config_list;
    vector<string> ecm_config_list;
    vector<string> pointparticle_config_list;
    
    read_general_parameters(general_file_name, membrane_config_list, chromatin_config_list, actin_config_list, ecm_config_list, pointparticle_config_list);
    
    ofstream Trajectory;
    string traj_file_name="Results/"+GenConst::trajectory_file_name+buffer+".xyz";
    
    
    vector<Membrane> Membranes;
    vector<Chromatin> Chromatins;
    vector<Actin> Actins;
    vector<ECM> ECMs;
    vector<point_particle> pointparticles;
    
    bool Include_Membrane  = false;
    bool Include_Chromatin = false;
    bool Include_Actin     = false;
    bool Include_ECM       = false;
    bool Include_pointparticle = false;
    
    if (GenConst::Num_of_Membranes!=0) {
        Include_Membrane = true;
        
        Membranes.resize(GenConst::Num_of_Membranes);
        for (int i=0; i<GenConst::Num_of_Membranes; i++) {
            string label="mem"+to_string(i);
            Membranes[i].set_label(label);
            Membranes[i].set_file_time(buffer);
            Membranes[i].set_index(i);
            Membranes[i].import_config(membrane_config_list[i]);
            Membranes[i].generate_report();
        }
    }
    
    
    if (GenConst::Num_of_Chromatins!=0) {
        Include_Chromatin = true;
        Chromatins.resize(GenConst::Num_of_Chromatins);
        for (int i=0; i<GenConst::Num_of_Chromatins; i++) {
            Chromatins[i].set_file_time(buffer);
            Chromatins[i].set_index(i);
            if (GenConst::Num_of_Membranes == GenConst::Num_of_Chromatins) {
                ///put a flag for chromatin inside membrane
                Chromatins[i].import_config(chromatin_config_list[i], Membranes[i].get_min_radius_after_relaxation());
            } else {
                Chromatins[i].import_config(chromatin_config_list[i]);
            }
            
            Chromatins[i].generate_report();
        }
    }
    
    if (GenConst::Num_of_Actins!=0) {
        Include_Actin = true;
        Actins.resize(GenConst::Num_of_Actins);
        for (int i=0; i<GenConst::Num_of_Actins; i++) {
            Actins[i].set_file_time(buffer);
            Actins[i].set_index(i);
            Actins[i].import_config(actin_config_list[i]);
            //            Actins[i].generate_report();
        }
    }
    
    if (GenConst::Num_of_ECMs!=0){
        Include_ECM=true;
        ECMs.resize(GenConst::Num_of_ECMs);
        for (int i=0; i<GenConst::Num_of_ECMs; i++) {
            ECMs[i].set_file_time(buffer);
            ECMs[i].set_index(i);
            ECMs[i].import_config(ecm_config_list[i]);
        }
        
    }
    
    if (GenConst::Num_of_pointparticles!=0){
        Include_pointparticle=true;
        pointparticles.resize(GenConst::Num_of_pointparticles);
        for (int i=0; i<GenConst::Num_of_pointparticles; i++) {
            pointparticles[i].set_file_time(buffer);
            pointparticles[i].set_index(i);
            pointparticles[i].import_config(pointparticle_config_list[i]);
            pointparticles[i].generate_report();
        }
        
    }
    
    if (Include_Membrane && Include_ECM) {
        for (int i=0; i<GenConst::Num_of_Membranes; i++) {
            for (int j=0; j<GenConst::Num_of_ECMs; j++) {
                Membrane_ECM_neighbour_finder(ECMs[j], Membranes[i]);
            }
        }
    }
    
    
    
    int num_of_atoms=0;
    int num_of_bonds=0;
    int num_of_dihedrals=0;
    if (Include_Membrane) {
        for (int i=0; i<Membranes.size(); i++) {
            num_of_atoms  += Membranes[i].get_num_of_nodes();
            num_of_bonds     += Membranes[i].get_num_of_node_pairs();
            num_of_dihedrals += Membranes[i].get_num_of_triangle_pairs();
        }
    }
    if (Include_Chromatin) {
        for (int i=0; i<Chromatins.size(); i++) {
            num_of_atoms+=Chromatins[i].return_num_of_nodes();
        }
    }
    if (Include_Actin) {
        for (int i=0; i<Actins.size(); i++) {
            num_of_atoms+=Actins[i].return_num_of_nodes();
        }
    }
    if (Include_ECM) {
        for (int i=0; i<ECMs.size(); i++) {
            num_of_atoms+=ECMs[i].return_num_of_nodes();
        }
    }
    if (Include_pointparticle){
        num_of_atoms+=GenConst::Num_of_pointparticles;
    }
    
    if (Include_Membrane){
        if (Include_Actin){
            for (int i=0; i<GenConst::Num_of_Actins; i++) {
                for (int j=0; j<Membranes.size(); j++) {
                    Actin_Membrane_shared_Node_Identifier(Actins[i], Membranes[j] , j);
                    if (Membranes[j].get_relax_with_actin_flag()) {
                        Membranes[j].Relax_1();
                    }
                }
                
            } //for (int i=0; i<GenConst::Num_of_Actins; i++)
        } //if (Include_Actin)
        else{
            for (int i=0; i<Membranes.size(); i++){
                Membranes[i].Relax_1();
            }// End of for (int i=0; i<Membranes.size(); i++)
        }//end else
    } // End of if (Include_Membrane)
    int progress=0;
    bool openmm_sim=true;
    //openmm**
    if (openmm_sim) {
        cout<<"\nBeginnig the OpenMM section:\n";
        std::string   platformName;
        int atom_count=0;
        int bond_count=0;
        int dihe_count=0;
        
        
        MyAtomInfo* all_atoms    = new MyAtomInfo[num_of_atoms+1];
        Bonds* all_bonds         = new Bonds[num_of_bonds+1];
        Dihedrals* all_dihedrals = new Dihedrals[num_of_dihedrals+1];
        
        all_atoms[num_of_atoms].type         =EndOfList;
        all_bonds[num_of_bonds].type         =EndOfList;
        all_dihedrals[num_of_dihedrals].type =EndOfList;
        
        if (Include_Membrane) {
            for (int i=0; i<Membranes.size(); i++) {
                
                MyAtomInfo* atoms = convert_membrane_position_to_openmm(Membranes[i]);
                for (int j=0;j<Membranes[i].get_num_of_nodes(); j++) {
                    all_atoms[j+atom_count]=atoms[j];
                }
                
                
                
                Bonds* bonds = convert_membrane_bond_info_to_openmm(Membranes[i]);
                for (int j=0; j<Membranes[i].get_num_of_node_pairs(); j++) {
                    all_bonds[j+bond_count]=bonds[j];
                    all_bonds[j+bond_count].atoms[0]=bonds[j].atoms[0]+atom_count;
                    all_bonds[j+bond_count].atoms[1]=bonds[j].atoms[1]+atom_count;
                    
                }
                
                
                Dihedrals* dihedrals = convert_membrane_dihedral_info_to_openmm(Membranes[i]);
                for (int j=0; j<Membranes[i].get_num_of_triangle_pairs(); j++) {
                    all_dihedrals[j+dihe_count]=dihedrals[j];
                    all_dihedrals[j+dihe_count].atoms[0]=dihedrals[j].atoms[0]+atom_count;
                    all_dihedrals[j+dihe_count].atoms[1]=dihedrals[j].atoms[1]+atom_count;
                    all_dihedrals[j+dihe_count].atoms[2]=dihedrals[j].atoms[2]+atom_count;
                    all_dihedrals[j+dihe_count].atoms[3]=dihedrals[j].atoms[3]+atom_count;
                }
                
                //These parameters are used to shift the index of the atoms/bonds/dihedrals.
                atom_count += Membranes[i].get_num_of_nodes();
                bond_count += Membranes[i].get_num_of_node_pairs();
                dihe_count += Membranes[i].get_num_of_triangle_pairs();
            }
        }
        // ALWAYS enclose all OpenMM calls with a try/catch block to make sure that
        // usage and runtime errors are caught and reported.
        
        try {
            
            MyOpenMMData* omm = myInitializeOpenMM(all_atoms, GenConst::Step_Size_In_Fs, platformName, all_bonds, all_dihedrals);
            // Run the simulation:
            //  (1) Write the first line of the PDB file and the initial configuration.
            //  (2) Run silently entirely within OpenMM between reporting intervals.
            //  (3) Write a PDB frame when the time comes.
            printf("REMARK  Using OpenMM platform %s\n", platformName.c_str());
            std::string traj_name="Results/"+GenConst::trajectory_file_name+buffer+".pdb";
            
            const int NumSilentSteps = (int)(GenConst::Report_Interval_In_Fs / GenConst::Step_Size_In_Fs + 0.5);
            for (int frame=1; ; ++frame) {
                double time, energy;
                myGetOpenMMState(omm, WantEnergy, time, energy, all_atoms);
                myWritePDBFrame(frame, time, energy, all_atoms, traj_name);
                
                if (time >= GenConst::Simulation_Time_In_Ps)
                    break;
                
                myStepWithOpenMM(omm, NumSilentSteps);
                if (int(100*time/GenConst::Simulation_Time_In_Ps)>progress){
                    cout<<"[ "<<progress<<"% ]\t time: "<<time<<" Ps\r" << std::flush;
                    progress+=5;
                }
            }
            
            // Clean up OpenMM data structures.
            myTerminateOpenMM(omm);
            cout<<"[ 100% ]\t time: "<<GenConst::Simulation_Time_In_Ps<<" Ps\n";
            cout<<"\nDone!"<<endl;
            printf("Wall clock time of the simulation: %.2f Minutes\n", (double)((clock() - tStart)/CLOCKS_PER_SEC)/60.0);
            return 0; // Normal return from main.
        }
        
        // Catch and report usage and runtime errors detected by OpenMM and fail.
        catch(const std::exception& e) {
            printf("EXCEPTION: %s\n", e.what());
            return 1;
        }
    }
    
    
    Trajectory.open(traj_file_name.c_str(), ios::app);
    Trajectory << std:: fixed;
    progress=0;
    cout<<"\nBeginnig the MD\nProgress:\n";
    for(int MD_Step=0 ;MD_Step<=GenConst::MD_num_of_steps ; MD_Step++){
//        cout<<Membranes[0].return_node_position(0, 0);
        
        //Thermostat step first step
        if (GenConst::MD_thrmo_step!=0 && MD_Step%GenConst::MD_thrmo_step==0 && MD_Step>1000) {
            if (Include_Membrane) {
                for (int i=0; i<Membranes.size(); i++) {
                    Membranes[i].Thermostat_Bussi(GenConst::Buffer_temperature);
                }
            }
            if (Include_Actin) {
                for (int i=0; i<Actins.size(); i++) {
                    //                    Actins[i].Thermostat_Bussi(GenConst::MD_T);
                }
            }
            if (Include_Chromatin) {
                for (int i=0; i<Chromatins.size(); i++) {
                    Chromatins[i].Thermostat_Bussi(GenConst::MD_T*0.01);
                }
            }
            if (Include_ECM) {
                for (int i=0; i<ECMs.size(); i++) {
                    //                    ECMs[i].Thermostat_Bussi(GenConst::MD_T);
                }
            }
        }
        
        
        //Velocity Verlet first step
        if (Include_Membrane)
        {
            for (int i=0; i<Membranes.size(); i++) {
                Membranes[i].MD_Evolution_beginning(GenConst::Step_Size_In_Fs);
            }
        }
        if (Include_Chromatin)
        {
            for (int i=0; i<Chromatins.size(); i++) {
                Chromatins[i].MD_Evolution_beginning(GenConst::Step_Size_In_Fs);
            }
        }
        if (Include_Actin)
        {
            for (int i=0; i<Actins.size(); i++) {
                Actins[i].MD_Evolution_beginning(GenConst::Step_Size_In_Fs);
            }
        }
        if (Include_ECM)
        {
            for (int i=0; i<ECMs.size(); i++) {
                ECMs[i].MD_Evolution_beginning(GenConst::Step_Size_In_Fs);
            }
        }
        
        if (Include_pointparticle)
        {   
            for (int i=0; i<pointparticles.size(); i++) {
                if (pointparticles[i].on_or_off_MD_evolution){
                    pointparticles[i].MD_Evolution_beginning(GenConst::Step_Size_In_Fs);
                }
            }
        }
        
        //force implamentation
        if (Include_Membrane)
        {
            for (int i=0; i<Membranes.size(); i++) {
                Membranes[i].Elastic_Force_Calculator(0);
            }
        }
        if (Include_Chromatin)
        {
            for (int i=0; i<Chromatins.size(); i++) {
                Chromatins[i].Force_Calculator_2();
            }
        }
        if (Include_Actin)
        {
            for (int i=0; i<Actins.size(); i++) {
                Actins[i].Elastic_Force_Calculator();
            }
        }
        if (Include_ECM)
        {
            for (int i=0; i<ECMs.size(); i++) {
                //                ECMs[i].Elastic_Force_Calculator();
            }
        }
        
        //Shared Forces
        if (Include_Chromatin && Include_Membrane) {
            if (MD_Step%2000==0) {
                for (int i=0; i<Chromatins.size(); i++) {
                    Chromatin_Membrane_neighbour_finder(Chromatins[i], Membranes[i]);
                    Chromatin_Membrane_hard_sphere(Chromatins[i], Membranes[i]);
                }
            }
            for (int i=0; i<Chromatins.size(); i++) {
                Chromatin_Membrane_hard_sphere(Chromatins[i], Membranes[i]);
            }
            
            
        }
        
        if (Include_Membrane && Include_Actin) {
            for (int i=0; i<Actins.size(); i++) {
                for (int j=0; j<Membranes.size(); j++) {
                    Actin_Membrane_shared_Node_Force_calculator(Actins[i], Membranes[j], j);
                }
            }
        }
        
        if (Include_Membrane && Include_ECM) {
            for (int i=0; i<Membranes.size(); i++) {
                for (int j=0; j<ECMs.size(); j++) {
                    Membrane_ECM_shared_node_force (ECMs[j], Membranes[i]);
                    if (MD_Step%2000==0) {
                        update_ecm_mem_neighbour_list (ECMs[j], Membranes[i]);
                    }
                }
            }
        }
        
        
        if (Include_Membrane && Include_pointparticle) {
            for (int i=0; i<Membranes.size(); i++) {
                for (int j=0; j<pointparticles.size(); j++){
                    Vesicle_pointparticle_neighbour_finder (pointparticles[j], Membranes[i]);
                    pointparticle_vesicle_shared_node_force (pointparticles[j], Membranes[i]);
                }
            }
        }
        
        if(Include_pointparticle && pointparticles.size()>1){
            for (int i=0; i<pointparticles.size(); i++){
                for (int j=0; j<pointparticles.size(); j++){
                    if(i != j){
                        pointparticle_pointparticle_interaction(pointparticles[i],pointparticles[j]);
                    }
                }
            }
        }
        
        //Velocity Verlet second step
        if (Include_Membrane) {
            for (int i=0; i<Membranes.size(); i++) {
                Membranes[i].MD_Evolution_end(GenConst::Step_Size_In_Fs);
            }
        }
        if (Include_Chromatin) {
            for (int i=0; i<Chromatins.size(); i++) {
                Chromatins[i].MD_Evolution_end(GenConst::Step_Size_In_Fs);
            }
        }
        if (Include_Actin) {
            for (int i=0; i<Actins.size(); i++) {
                Actins[i].MD_Evolution_end(GenConst::Step_Size_In_Fs);
            }
        }
        if (Include_ECM) {
            for (int i=0; i<ECMs.size(); i++) {
                ECMs[i].MD_Evolution_end(GenConst::Step_Size_In_Fs);
            }
        }
        
        if (Include_pointparticle)
        {
            for (int i=0; i<pointparticles.size(); i++) {
                if (pointparticles[i].on_or_off_MD_evolution){
                    pointparticles[i].MD_Evolution_end(GenConst::Step_Size_In_Fs);
                    
                }
            }
        }
        
        
        //Thermostat second step
        if (GenConst::MD_thrmo_step!=0 && MD_Step%GenConst::MD_thrmo_step==0 && MD_Step>1000) {
            if (Include_Membrane) {
                for (int i=0; i<Membranes.size(); i++) {
                    Membranes[i].Thermostat_Bussi(GenConst::Buffer_temperature);
                }
            }
            if (Include_Actin) {
                for (int i=0; i<Actins.size(); i++) {
                    //                    Actins[i].Thermostat_Bussi(GenConst::MD_T);
                }
            }
            if (Include_Chromatin) {
                for (int i=0; i<Chromatins.size(); i++) {
                    Chromatins[i].Thermostat_Bussi(GenConst::MD_T*0.01);
                }
            }
            if (Include_ECM) {
                for (int i=0; i<ECMs.size(); i++) {
                    //                    ECMs[i].Thermostat_Bussi(GenConst::MD_T);
                }
            }
        }
        
        //saving Results
        if (MD_Step%GenConst::MD_traj_save_step == 0)
        {
            Trajectory << num_of_atoms<<endl;
            Trajectory << " nodes  "<<endl;
            
            
            if (Include_Membrane) {
                for (int i=0; i<Membranes.size(); i++) {
//                    string label="Membrane_"+to_string(i);
                    Membranes[i].write_traj(traj_file_name);
                    Membranes[i].export_for_resume(MD_Step);
                }
            }
            
            if (Include_Chromatin) {
                for (int i=0; i<Chromatins.size(); i++) {
                    string label="Chromatin_"+to_string(i);
                    Chromatins[i].write_traj(traj_file_name, label);
                    Chromatins[i].export_for_resume(MD_Step);
                }
            }
            if (Include_Actin) {
                for (int i=0; i<Actins.size(); i++) {
                    string label="Actin_"+to_string(i);
                    Actins[i].write_traj(traj_file_name, label);
                    //                    Actins[i].export_for_resume(MD_Step);
                }
            }
            if (Include_ECM) {
                for (int i=0; i<ECMs.size(); i++) {
                    string label="ECM_"+to_string(i);
                    ECMs[i].write_traj(traj_file_name, label);
                    //                    Actins[i].export_for_resume(MD_Step);
                }
            }
            
            if (Include_pointparticle) {
                for (int i=0; i<pointparticles.size(); i++) {
                    
                    pointparticles[i].write_traj(traj_file_name);
                 
                }
            }
        }// End of if (MD_Step%100==0)
        
        
        if (int(100*MD_Step/GenConst::MD_num_of_steps)>progress){
            cout<<"[ "<<progress<<"% ]\t step: "<<MD_Step<<"\r" << std::flush;
            progress+=5;
        }
        
    } //End of for (int MD_Step=0 ;MD_Step<=MD_num_of_steps ; MD_Step++)
    cout<<"[ 100% ]\t step: "<<GenConst::MD_num_of_steps<<"\n";
    cout<<"\nDone!"<<endl;
    printf("Time taken: %.2f Minutes\n", (double)((clock() - tStart)/CLOCKS_PER_SEC)/60.0);
    return 0;
}





