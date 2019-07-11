//
//  General_constants.h
//  Cell-Durotaxis
//
//  Created by Ali Farnudi on 26/08/2017.
//  Copyright © 2017 Ali Farnudi. All rights reserved.
//

#ifndef General_constants_h
#define General_constants_h
#include <string>


namespace GenConst {
    /**MD total number of steps. Default 10000*/
    extern int MD_num_of_steps;
    /**Simulation time (in picoseconds). If this parameter is not set in the general config file by the user, or the value is set to zero, it will be calculate during runtime by multiplying the 'step size' by the 'total number of steps'.*/
    extern double Simulation_Time_In_Ps;
    /**Trajectory saving rate in number of steps. Default 1000*/
    extern int MD_traj_save_step;
    /**Trajectory saving rate in femtoseconds. If this parameter is not set by the user in the general config file, or the value is set to zero, the interval will be calculated by multiplying the MD_traj_save_step by the Step_Size_In_Fs.*/
    extern double Report_Interval_In_Fs;
    /** Integration step size (fs).  Default 0.001*/
    extern double Step_Size_In_Fs;
    /**Tempreture. Default 1*/
    extern double MD_T;
    /**Boltzman's constant. Default 1*/
    extern double K;
    /**During every 'MD_thermo_step' the thermostat will be applied to the system. Default 200*/
    extern int MD_thrmo_step;
    /**During every 'MC_step' the MC step will be applied to the membrane mesh. Default 100*/
    extern int MC_step;
    extern double Mem_fluidity;
    extern double Lbox;
    extern bool Periodic_condtion_status;
    /**Number of membranes in the system followed by the directory of their respective configuration files. All membranes will be configured using the first configuration file if only one is provided. Default 0.*/
    extern int Num_of_Membranes;
    /**Number of chromatins in the system followed by the directory of their respective configuration files. All chromatins will be configured using the first configuration file if only one is provided. Default 0.*/
    extern int Num_of_Chromatins;
    /**Number of actins in the system followed by the directory of their respective configuration files. All actins will be configured using the first configuration file if only one is provided. Default 0.*/
    extern int Num_of_Actins;
    /**Number of ECMs in the system followed by the directory of their respective configuration files. All ECMs will be configured using the first configuration file if only one is provided. Default 0.*/
    extern int Num_of_ECMs;
    /**Name of the output file. Please note that the date and time the file is generated will be attached to this name.*/
    extern std::string trajectory_file_name;
    
    extern double Buffer_temperature;
    /**Bussi tau will controll the fluctuation of the kinetic energy in the Bussi et. al. thermostat. Default 10*/
    extern double Bussi_tau;
    /**Actin-membrane LJ-like interaction strength. Default 400*/
    extern double Actin_Membrane_Bond_Coefficient;
    extern int Num_of_pointparticles;
    /**Specifies if an interaction map is provided by the user or not. deafult false.*/
    extern bool Interaction_map;
    /**Path to the interaction map (including the file name). if set to zero no class instances will interact with one another. Set to 1 and provide a path to the "interaction_map.txt" file.  Default = "interaction_map.txt".*/
    extern std::string Interaction_map_file_name;
    /**Set the excluded volume interaction for nodes of class instances. 0 for no repulsion and 1 for excluded volume interaction. Default 1.*/
    extern bool Excluded_volume_interaction;
    /**Set to 1 for the programme to use the OpenMM engine or to 0 to use the internal simple engine. Default 1 (true).*/
    extern bool OpenMM;
}

//#define MD_num_of_steps  300000//35000// number of MD stps
//#define MD_traj_save_step    2000//The step on which the trajector of the membrane is saved.
//#define MD_Time_Step     0.001 // time length of steps in MD
//#define MD_KT     1.0  // KT the quanta of energy
//#define MD_thrmo_step   100 //
//#define MC_step 1

//#define Mem_fluidity       0.002 //Used in the MC step

//#define Lbox  1000.0///    (size of square periodic box-1)
//#define Periodic_condtion_status 0.0 //status 0.0 = off (The Periodic update will not be executed in the 'Main MD' loop). status = 1.0 = on

#endif /* General_constants_h */
