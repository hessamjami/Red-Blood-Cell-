#include "OpenMM_funcs.hpp"

using std::set;
const int EndOfList=-1;

void set_bonded_forces(Bonds*                                 bonds,
                       OpenMM::HarmonicBondForce*            &HarmonicBond,
//                       OpenMM::HarmonicBondForce*            &Kelvin_VoigtBond,
//                       vector<OpenMM::CustomBondForce*>      &X4harmonics,
                       vector<OpenMM::CustomBondForce*>      &KremerGrests,
//                       vector<OpenMM::CustomBondForce*>      &Gompperbond,
//                       vector<OpenMM::CustomBondForce*>      &Gompperrep,
//                       vector<OpenMM::CustomBondForce*>      &Contractiles,
//                       vector<OpenMM::CustomBondForce*>      &KFs,
//                       vector<OpenMM::CustomBondForce*>      &hill_bonds,
//                       vector<OpenMM::CustomBondForce*>      &Harmonic_minmax,
                       vector<OpenMM::CustomBondForce*>      &abrahams,
//                       TimeDependantData*                    &time_dependant_data,
                       OpenMM::System                        &system){
    
    bool HarmonicBondForce=false;
    
    for (int i=0; bonds[i].type != EndOfList; ++i) {
        
        const int*      atom = bonds[i].atoms;
        
        if (bonds[i].type == potentialModelIndex.Model["KremerGrest"])
        {
            
            if (KremerGrests.size() == 0) {
                string K_FENE = to_string(30*generalParameters.BoltzmannKJpermolkelvin*generalParameters.temperature);
                string epsilon_WCA = to_string(generalParameters.BoltzmannKJpermolkelvin*generalParameters.temperature);
                string sigma = "sigmaKG";
                string potential = "-0.5*"+K_FENE+"*2.25*log(1-(r*r/(2.25*"+sigma+"*"+sigma+")))*step(0.99*1.5*"+sigma+"-r)+4*"+epsilon_WCA+"*(("+sigma+"/r)^12-("+sigma+"/r)^6+0.25)*step(2^(1./6.)*"+sigma+"-r)";
                
                KremerGrests.push_back(new OpenMM::CustomBondForce(potential));
                
                KremerGrests[0]->addPerBondParameter(sigma);
                
                system.addForce(KremerGrests[0]);
            }
            vector<double> parameters={bonds[i].nominalLengthInNm
                                       };
            
            KremerGrests[0]->addBond(atom[0], atom[1], parameters);
    
        }
        else if (bonds[i].type == potentialModelIndex.Model["Harmonic"])
        {
            HarmonicBondForce=true;
            HarmonicBond->addBond(atom[0], atom[1],
                                  bonds[i].nominalLengthInNm,
                                  bonds[i].stiffnessInKJPerNm2);
        }
        else if (bonds[i].type == potentialModelIndex.Model["Abraham1989"])
        {
            
            if (abrahams.size() == 0) {
                string sigma   = "sigmaAbraham";
                string epsilon = to_string(4*generalParameters.BoltzmannKJpermolkelvin*generalParameters.temperature);
                string potential = epsilon+"*(("+sigma+"/r)^12-("+sigma+"/r)^6+0.25)*step("+sigma+"*2^(1/6)-r)+" + epsilon+"*(("+sigma+"/(lbond-r))^12-("+sigma+"/(lbond-r))^6+0.25)*step(r-(lbond-" +sigma+"*2^(1/6)))";
                
                abrahams.push_back( new OpenMM::CustomBondForce(potential));
                
                abrahams[0]->addPerBondParameter(sigma);
                abrahams[0]->addPerBondParameter("lbond");
                system.addForce(abrahams[0]);
            }
            double sigmaAbraham =bonds[i].nominalLengthInNm/5;
            double lbond =bonds[i].nominalLengthInNm + sigmaAbraham * pow(2,1./6);
            
            vector<double> parameters;
            parameters.resize(2);
            parameters[0]=sigmaAbraham;
            parameters[1]=lbond;
            
            abrahams[0]->addBond(atom[0], atom[1], parameters);
        }
    }
    
    if (HarmonicBondForce) {
        system.addForce(HarmonicBond);
    }
        
}
                      



void set_bonded_forces(Bonds*                                 bonds,
                       OpenMM::HarmonicBondForce*            &HarmonicBond,
                       OpenMM::HarmonicBondForce*            &Kelvin_VoigtBond,
                       vector<OpenMM::CustomBondForce*>      &X4harmonics,
                       vector<OpenMM::CustomCompoundBondForce*>      &ellipsoid,
                       vector<OpenMM::CustomCompoundBondForce*>      &ulm_potential,
                       vector<OpenMM::CustomBondForce*>      &KremerGrests,
                       vector<OpenMM::CustomBondForce*>      &Gompper,
//                       vector<OpenMM::CustomBondForce*>      &Gompperrep,
                       vector<OpenMM::CustomBondForce*>      &Contractiles,
                       vector<OpenMM::CustomBondForce*>      &KFs,
                       vector<OpenMM::CustomBondForce*>      &HillBonds,
                       vector<OpenMM::CustomBondForce*>      &Harmonic_minmax,
                       vector<OpenMM::CustomBondForce*>      &abrahams,
                       TimeDependantData*                    &time_dependant_data,
                       OpenMM::System                        &system
                       ){
    
    bool KremerGrestBondForce=false;
    bool gompperBondForce=false;
    bool HarmonicBondForce=false;
    bool Kelvin_VoigtBondForce=false;
    bool Hill_Force=false;
    bool k_Force=false;
    
    set <std::string> KremerGrest_classes;
    set <std::string> Gompper_classes;
    set <std::string> X4harmonic_classes;
    set <std::string> ellipsoid_classes;
    set <std::string> ulm_classes;
    set <std::string> Contractile_classes;
    set <std::string> Hill_classes;
    set <std::string> KF_classes;
    set <std::string> Harmonic_minmax_classes;
    set <std::string> abraham_classes;
    
    int KremerGrest_index = -1;
    int Gompper_index = -1;
    int X4harmonic_index = -1;
    int ellipsoid_index = -1;
    int ulm_index = -1;
    int Contractile_index = -1;
    int Hill_index = -1;
    int KF_index = -1;
    int Harmonic_minmax_index = -1;
    int abraham_index = -1;
    
    for (int i=0; bonds[i].type != EndOfList; ++i) {
        
        const int*      atom = bonds[i].atoms;
        
        if (bonds[i].type == potentialModelIndex.Model["KremerGrest"])
        {
            if (bonds[i].globalStat) {
                auto KremerGrest_item = KremerGrest_classes.find(bonds[i].class_label);
                if (KremerGrest_item == KremerGrest_classes.end()) {
//                    cout<<"Ok, I'm here ...."<<endl;
                    KremerGrest_classes.insert(bonds[i].class_label);
                    KremerGrest_index++;
                    
                    string K_fene = "K_fene"+bonds[i].class_label;
                    string R0 = "R0"+bonds[i].class_label;
                    string epsilon_fene = "epsilon_fene"+bonds[i].class_label;
                    string sigma_fene = "sigma_fene"+bonds[i].class_label;
                    string cut_fene = "cut_fene"+bonds[i].class_label;
                    string potential = "-0.5*"+K_fene+"*"+R0+"*"+R0+"*log(1-(r*r/("+R0+"*"+R0+")))*step(0.99*"+R0+"-r)+4*"+epsilon_fene+"*(("+sigma_fene+"/r)^12-("+sigma_fene+"/r)^6+0.25)*step("+cut_fene+"-r)";
                    
                    KremerGrests.push_back(new OpenMM::CustomBondForce(potential));
                    
                    KremerGrests[KremerGrest_index]->addGlobalParameter(K_fene,bonds[i].k_FENE_inKJpermol);
                    KremerGrests[KremerGrest_index]->addGlobalParameter(R0,bonds[i].FENER0inNm);
                    KremerGrests[KremerGrest_index]->addGlobalParameter(epsilon_fene,bonds[i].epsilon_WCA_inKJpermol);
                    KremerGrests[KremerGrest_index]->addGlobalParameter(sigma_fene,bonds[i].nominalLengthInNm);
                    KremerGrests[KremerGrest_index]->addGlobalParameter(cut_fene,bonds[i].nominalLengthInNm*pow(2,1./6.) );
                    
                    system.addForce(KremerGrests[KremerGrest_index]);
                }
                
                KremerGrests[KremerGrest_index]->addBond(atom[0], atom[1]);
                
            } else {
                auto KremerGrest_item = KremerGrest_classes.find(bonds[i].class_label);
                if (KremerGrest_item == KremerGrest_classes.end()) {
                    
                    KremerGrest_classes.insert(bonds[i].class_label);
                    KremerGrest_index++;
                    
                    KremerGrests.push_back(new OpenMM::CustomBondForce("-0.5*K_fene*R0*R0*log(1-(r*r/(R0*R0)))*step(0.99*R0-r)+4*epsilon_fene*((sigma_fene/r)^12-(sigma_fene/r)^6+0.25)*step(cut_fene-r)"));
                    
                    KremerGrests[KremerGrest_index]->addPerBondParameter("K_fene");
                    KremerGrests[KremerGrest_index]->addPerBondParameter("R0");
                    KremerGrests[KremerGrest_index]->addPerBondParameter("epsilon_fene");
                    KremerGrests[KremerGrest_index]->addPerBondParameter("sigma_fene");
                    KremerGrests[KremerGrest_index]->addPerBondParameter("cut_fene");
                    
                    system.addForce(KremerGrests[KremerGrest_index]);
                }
                vector<double> parameters={bonds[i].k_FENE_inKJpermol,
                                           bonds[i].FENER0inNm,
                                           bonds[i].epsilon_WCA_inKJpermol,
                                           bonds[i].nominalLengthInNm,
                                           bonds[i].nominalLengthInNm*(pow(2,1./6.))
                                           };
                
                KremerGrests[KremerGrest_index]->addBond(atom[0], atom[1], parameters);
    //            if (GenConst::Periodic_box) {
    //                FENEs[FENE_index]->setUsesPeriodicBoundaryConditions(true);
    //            }
            }
        }
        else if (bonds[i].type == potentialModelIndex.Model["Gompper"])
        {
            auto Gompper_item = Gompper_classes.find(bonds[i].class_label);
            if (Gompper_item == Gompper_classes.end()) {
                
                Gompper_classes.insert(bonds[i].class_label);
                Gompper_index++;
                string label = bonds[i].class_label;

                string potential = to_string(80*generalParameters.BoltzmannKJpermolkelvin*generalParameters.temperature)+"*(";
                potential += "exp(1/(1.15*edge-r))/(1.33*edge-r)*step(r-1.15*edge)";
                potential += "+";
                potential += "exp(1/(r-0.85*edge))/(r-0.67*edge)*step(0.85*edge-r)";
                potential += ")";
                
                Gompper.push_back(new OpenMM::CustomBondForce(potential));
                Gompper[Gompper_index]->addPerBondParameter("edge");
        
                system.addForce(Gompper[Gompper_index]);
            }
            vector<double> parameters;
            parameters.push_back(bonds[i].nominalLengthInNm);
            
            Gompper[Gompper_index]->addBond(atom[0], atom[1], parameters);

        }
        else if (bonds[i].type == potentialModelIndex.Model["Harmonic"])
        {
            HarmonicBondForce=true;
            // Note factor of 2 for stiffness below because Amber specifies the constant
            // as it is used in the harmonic energy term kx^2 with force 2kx; OpenMM wants
            // it as used in the force term kx, with energy kx^2/2.
            HarmonicBond->addBond(atom[0], atom[1],
                                  bonds[i].nominalLengthInNm,
                                  bonds[i].stiffnessInKJPerNm2);
//            if (GenConst::Periodic_box) {
//                HarmonicBond->setUsesPeriodicBoundaryConditions(true);
//            }
        }
        else if (bonds[i].type == potentialModelIndex.Model["LockOnEllipsoid"])
        {
            auto ellipsoid_item = ellipsoid_classes.find(bonds[i].class_label);
            if (ellipsoid_item == ellipsoid_classes.end()) {
                
                ellipsoid_classes.insert(bonds[i].class_label);
                ellipsoid_index++;
                
                ellipsoid.push_back( new OpenMM::CustomCompoundBondForce(2,"0.5*"+to_string(bonds[i].stiffnessInKJPerNm2)+"*"+to_string(bonds[i].nominalLengthInNm*bonds[i].nominalLengthInNm)+"*( sqrt( (x2/"+to_string(bonds[i].ellipsoidLockXscale)+")^2 +(y2/"+to_string(bonds[i].ellipsoidLockYscale)+")^2 + (z2/"+to_string(bonds[i].ellipsoidLockZscale)+")^2)-1)^2"));
                system.addForce(ellipsoid[ellipsoid_index]);
            }
            vector<int> bond_atoms={atom[0],atom[1]};
            ellipsoid[ellipsoid_index]->addBond(bond_atoms);
        }
        else if (bonds[i].type == potentialModelIndex.Model["LockOnULM2_0"])
        {
            auto ulm_item = ulm_classes.find(bonds[i].class_label);
            if (ulm_item == ulm_classes.end()) {
                
                ulm_classes.insert(bonds[i].class_label);
                ulm_index++;
                string potential = "0.5*"+to_string(bonds[i].stiffnessInKJPerNm2)+"*(sqrt(x2^2+y2^2+z2^2)-"+to_string(bonds[i].nominalLengthInNm)+"*("+to_string(bonds[i].LockOnULM_amplitude)+"*0.3153915653*(3*z2^2/(x2^2 +y2^2 + z2^2)-1)+1))^2";
                ulm_potential.push_back( new OpenMM::CustomCompoundBondForce(2,potential));
//                cout<<potential<<endl;exit(0);
                system.addForce(ulm_potential[ulm_index]);
            }
            vector<int> bond_atoms={atom[0],atom[1]};
            ulm_potential[ulm_index]->addBond(bond_atoms);
        }
        else if (bonds[i].type == potentialModelIndex.Model["HarmonicX4"])
        {
            auto X4harmonic_item = X4harmonic_classes.find(bonds[i].class_label);
            if (X4harmonic_item == X4harmonic_classes.end()) {
                
                X4harmonic_classes.insert(bonds[i].class_label);
                X4harmonic_index++;
                
                X4harmonics.push_back( new OpenMM::CustomBondForce("k_bond*((r/r_rest)-1)^4"));
                
                X4harmonics[X4harmonic_index]->addPerBondParameter("r_rest");
                X4harmonics[X4harmonic_index]->addPerBondParameter("k_bond");
                system.addForce(X4harmonics[X4harmonic_index]);
            }
            double r_rest =bonds[i].nominalLengthInNm;
            double k_bond=bonds[i].stiffnessInKJPerNm4;
            vector<double> parameters;
            parameters.resize(2);
            parameters[0]=r_rest;
            parameters[1]=k_bond;
            X4harmonics[X4harmonic_index]->addBond(atom[0], atom[1], parameters);
//            if (GenConst::Periodic_box) {
//                X4harmonics[X4harmonic_index]->setUsesPeriodicBoundaryConditions(true);
//            }
        }
        else if (bonds[i].type == potentialModelIndex.Model["Kelvin-Voigt"])
        {
            Kelvin_VoigtBondForce=true;
            // Note factor of 2 for stiffness below because Amber specifies the constant
            // as it is used in the harmonic energy term kx^2 with force 2kx; OpenMM wants
            // it as used in the force term kx, with energy kx^2/2.
            Kelvin_VoigtBond->addBond(atom[0], atom[1],
                                      bonds[i].nominalLengthInNm,
                                      bonds[i].stiffnessInKJPerNm2);
            
            time_dependant_data->Kelvin_Voigt_damp.push_back(bonds[i].dampInKJPsPerNm2);
//            if (GenConst::Periodic_box) {
//                Kelvin_VoigtBond->setUsesPeriodicBoundaryConditions(true);
//            }
        }
        else if (bonds[i].type == potentialModelIndex.Model["Contractile"])
        {
            auto Contractile_item = Contractile_classes.find(bonds[i].class_label);
            if (Contractile_item == Contractile_classes.end()) {
                
                Contractile_classes.insert(bonds[i].class_label);
                Contractile_index++;
                
                Contractiles.push_back( new OpenMM::CustomBondForce("(step(r-r_min) - step(r-r_max)) * F0 * (r - r0) + step(r-r_max) * F0 * (r_max - r0) + step(r_min - r) * F0 * (r_min - r0)"));
                
                Contractiles[Contractile_index]->addPerBondParameter("r0");
                Contractiles[Contractile_index]->addPerBondParameter("F0");
                Contractiles[Contractile_index]->addPerBondParameter("r_min");
                Contractiles[Contractile_index]->addPerBondParameter("r_max");
                system.addForce(Contractiles[Contractile_index]);
            }
            double r0 =bonds[i].nominalLengthInNm;
            double F0=bonds[i].F0;
            double r_min = bonds[i].r_min;
            double r_max = bonds[i].r_max;
            vector<double> parameters;
            parameters.resize(4);
            parameters[0]=r0;
            parameters[1]=F0;
            parameters[2]=r_min;
            parameters[3]=r_max;
            Contractiles[Contractile_index]->addBond(atom[0], atom[1], parameters);
            
//            if(GenConst::Periodic_box)
//            {
//                Contractiles[Contractile_index]->setUsesPeriodicBoundaryConditions(true);
//            }
            
            //time_dependant_data->hill_coefficient = bonds[i].hill_co;
            //time_dependant_data->Contractile_force = bonds[i].F0;
        }
        else if (bonds[i].type == potentialModelIndex.Model["Harmonic_minmax"])
        {
            auto Harmonic_minmax_item = Harmonic_minmax_classes.find(bonds[i].class_label);
            if (Harmonic_minmax_item == Harmonic_minmax_classes.end()) {
                
                Harmonic_minmax_classes.insert(bonds[i].class_label);
                Harmonic_minmax_index++;
                
                Harmonic_minmax.push_back( new OpenMM::CustomBondForce("(step(r-r_min) - step(r-r_max)) * (0.5 * k_bond *((r - r0)^2)) + step(r-r_max) * (0.5 * k_bond *((r_max - r0)^2)) + step(r_min - r) * (0.5 * k_bond *((r_min - r0)^2))"));
                
                Harmonic_minmax[Harmonic_minmax_index]->addPerBondParameter("r0");
                Harmonic_minmax[Harmonic_minmax_index]->addPerBondParameter("k_bond");
                Harmonic_minmax[Harmonic_minmax_index]->addPerBondParameter("r_min");
                Harmonic_minmax[Harmonic_minmax_index]->addPerBondParameter("r_max");
                system.addForce(Harmonic_minmax[Harmonic_minmax_index]);
            }
            double r0 =bonds[i].nominalLengthInNm;
            double k_bond=bonds[i].stiffnessInKJPerNm2;
            double r_min = bonds[i].r_min;
            double r_max = bonds[i].r_max;
            vector<double> parameters;
            parameters.resize(4);
            parameters[0]=r0;
            parameters[1]=k_bond;
            parameters[2]=r_min;
            parameters[3]=r_max;
            Harmonic_minmax[Harmonic_minmax_index]->addBond(atom[0], atom[1], parameters);
            
//            if(GenConst::Periodic_box)
//                      {
//                          Harmonic_minmax[Harmonic_minmax_index]->setUsesPeriodicBoundaryConditions(true);
//                      }
        } else if (bonds[i].type == potentialModelIndex.Model["hill"]){
            Hill_Force = true;
            auto Hill_item = Hill_classes.find(bonds[i].class_label);
            if (Hill_item == Hill_classes.end()) {
                
                Hill_classes.insert(bonds[i].class_label);
                Hill_index++;
                
                HillBonds.push_back( new OpenMM::CustomBondForce("(step(r-r_min) - step(r-r_max)) * F0 * (r - r0) + step(r-r_max) * F0 * (r_max - r0) + step(r_min - r) * F0 * (r_min - r0)"));
                
                HillBonds[Hill_index]->addPerBondParameter("r0");
                HillBonds[Hill_index]->addPerBondParameter("F0");
                HillBonds[Hill_index]->addPerBondParameter("r_min");
                HillBonds[Hill_index]->addPerBondParameter("r_max");
                HillBonds[Hill_index]->addPerBondParameter("b");
                system.addForce(HillBonds[Hill_index]);
            }
            double r0 =bonds[i].nominalLengthInNm;
            double F0=bonds[i].F0;
            double r_min = bonds[i].r_min;
            double r_max = bonds[i].r_max;
            double b = bonds[i].hill_co;
            vector<double> parameters;
            parameters.resize(5);
            parameters[0]=r0;
            parameters[1]=F0;
            parameters[2]=r_min;
            parameters[3]=r_max;
            parameters[4]=b;
            HillBonds[Hill_index]->addBond(atom[0], atom[1], parameters);
            
            time_dependant_data->hill_coefficient.push_back(b);
            time_dependant_data->hill_const_force.push_back(F0);
            
//            if(GenConst::Periodic_box)
//                      {
//                          HillBonds[Hill_index]->setUsesPeriodicBoundaryConditions(true);
//                      }
        }
        else if (bonds[i].type == potentialModelIndex.Model["KFs"]){
            k_Force = true;
            auto KF_item = KF_classes.find(bonds[i].class_label);
            if (KF_item == KF_classes.end()) {
                
                KF_classes.insert(bonds[i].class_label);
                KF_index++;
                
                KFs.push_back( new OpenMM::CustomBondForce("(step(r-r_min) - step(r-r_max)) * F0 * (r - r0) + step(r-r_max) * F0 * (r_max - r0) + step(r_min - r) * F0 * (r_min - r0)"));
                
                KFs[KF_index]->addPerBondParameter("r0");
                KFs[KF_index]->addPerBondParameter("F0");
                KFs[KF_index]->addPerBondParameter("r_min");
                KFs[KF_index]->addPerBondParameter("r_max");
                KFs[KF_index]->addPerBondParameter("n");
                KFs[KF_index]->addPerBondParameter("Fc");
                system.addForce(KFs[KF_index]);
            }
            double r0 =bonds[i].nominalLengthInNm;
            double F0=bonds[i].F0;
            double r_min = bonds[i].r_min;
            double r_max = bonds[i].r_max;
            double n = bonds[i].k_F0;
            double Fc=bonds[i].F0;
            vector<double> parameters;
            parameters.resize(6);
            parameters[0]=r0;
            parameters[1]=F0;
            parameters[2]=r_min;
            parameters[3]=r_max;
            parameters[4]=n;
            parameters[5]=Fc;
            KFs[KF_index]->addBond(atom[0], atom[1], parameters);
            
            //time_dependant_data->hill_coefficient.push_back(b);
            //time_dependant_data->hill_const_force.push_back(F0);
        }
        else if (bonds[i].type == potentialModelIndex.Model["Abraham1989"])
        {
            auto abraham_item = abraham_classes.find(bonds[i].class_label);
            if (abraham_item == abraham_classes.end()) {
                
                abraham_classes.insert(bonds[i].class_label);
                abraham_index++;
                
//                string sigma   = "sigmaAbraham";
                string epsilon = to_string(4*generalParameters.BoltzmannKJpermolkelvin*generalParameters.temperature);
                
//                string potential = epsilon+"*(("+sigma+"/r)^12-("+sigma+"/r)^6+0.25)*step("+sigma+"*2^(1/6)-r)+" + epsilon+"*(("+sigma+"/(lbond-r))^12-("+sigma+"/(lbond-r))^6+0.25)*step(r-(lbond-" +sigma+"*2^(1/6)))";
                string potential = epsilon+"*(";
                potential +="(";
                potential +="0.1*lbond/(0.67*lbond-r)";
                potential +=")^12";
                potential +="-(";
                potential +="0.1*lbond/(0.67*lbond-r)";
                potential +=")^6";
                potential +="+0.25)*";
                potential +="step(lbond*(0.67+0.1*2^(1/6))-r)";
                potential +="+" + epsilon+"*(";
                potential +="(";
                potential +="0.1*lbond/(1.33*lbond-r)";
                potential +=")^12";
                potential +="-(";
                potential +="0.1*lbond/(1.33*lbond-r)";
                potential +=")^6";
                potential +="+0.25)*";
                potential +="step(r-lbond*(1.33-0.1*2^(1/6)))";
                
                abrahams.push_back( new OpenMM::CustomBondForce(potential));
                
//                abrahams[abraham_index]->addPerBondParameter(sigma);
                abrahams[abraham_index]->addPerBondParameter("lbond");
                system.addForce(abrahams[abraham_index]);
            }
//            double sigmaAbraham =bonds[i].nominalLengthInNm/5;
            double lbond =bonds[i].nominalLengthInNm;// + sigmaAbraham * pow(2,1./6);
            
            vector<double> parameters;
            parameters.resize(1);
//            parameters[0]=sigmaAbraham;
            parameters[1]=lbond;
            
            abrahams[abraham_index]->addBond(atom[0], atom[1], parameters);
//            if (GenConst::Periodic_box) {
//                X4harmonics[X4harmonic_index]->setUsesPeriodicBoundaryConditions(true);
//            }
        }
        
        
    }
    
    
    if (HarmonicBondForce) {
        system.addForce(HarmonicBond);
    }
    
    if (Kelvin_VoigtBondForce) {
        system.addForce(Kelvin_VoigtBond);
        time_dependant_data->Kelvin_Voigt = true;
    }
    
    if (Hill_Force) {
        time_dependant_data->HillForce = true;
    }
    
    if (k_Force) {
        time_dependant_data->kForce = true;
    }
}
