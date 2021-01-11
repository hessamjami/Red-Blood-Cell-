//
//  General_functions.cpp
//  Cell-Durotaxis
//
//  Created by Ali Farnudi on 27/08/2017.
//  Copyright © 2017 Ali Farnudi. All rights reserved.
//
#include <iostream>
#include <fstream>
#include "General_constants.h"
//#include <stdlib.h>

#include "Arg_pars.hpp"
#include "cxxopts.hpp"
#include "General_functions.hpp"

using namespace std;

void consistency_check(ArgStruct_Analysis &args);
void build_output_names(ArgStruct_Analysis &args);
void check_membrane_labels(ArgStruct_Analysis &args);


ArgStruct_Analysis cxxparser_analysis(int argc, char **argv){
    ArgStruct_Analysis args;
    string programme_name =argv[0];
    try
    {
        
        cxxopts::Options options(programme_name,
                                 "\n Welcome to VCM.\n\n"
                                 " If the configuration files are ready you can run the simulation by running the"
                                 " programme without any option arguments by following this example:"
                                 " \n"
                                 " "+programme_name+"\n"
                                 " \n"
                                 " How to utilise VCM's analysis options:\nThere are currently 3 features: 3D, 2D, and Energy analysis\n"
                                 " 3D analysis of the Membrane surface undulations:\n"
                                 " \n "+programme_name+
                                 " --analysis 3"
                                 " --pdbpath <path to pdb>"
                                 " --lmax <maximum mode number>"
                                 " --angavg <number of random rotations>"
                                 " --align_axes <node on the Z axis>,<node on the Z-Y plane>"
                                 " --ext <output file extension>"
                                 " --framelimits <first frame>,<last frame>"
                                 " --memlabels <firstlabel>,<secondlabel>,..."
                                 " --minimisedState"
                                 " --meshfile Path/to/meshfile"
                                 "\n\n 2D analysis of the Membrane contour undulations:\n"
                                 " \n "+programme_name+
                                 " --analysis 2"
                                 " --pdbpath <path to pdb>"
                                 " --lmax <maximum mode number>"
                                 " --angavg <number of random rotations>"
                                 " --align_axes <node on the Z axis>,<node on the Z-Y plane>"
                                 " --ext <output file extension>"
                                 " --framelimits <first frame>,<last frame>"
                                 " --memlabels <firstlabel>,<secondlabel>,..."
                                 " --meshfile Path/to/meshfile"
                                 "\n\n Mechanical energy of the Membrane calculator:\n"
                                 " \n "+programme_name+
                                 " --analysis E"
                                 " --pdbpath <path to pdb>"
                                 " --ext <output file extension>"
                                 " --framelimits <first frame>,<last frame>"
                                 " --memlabels <firstlabel>,<secondlabel>,..."
                                 " --meshfile Path/to/meshfile");
        options
        .positional_help("")
        .show_positional_help();
        
        options
        //                .allow_unrecognised_options()
        .add_options()
        ("h,help", "Print help")
        ("analysis", "3 for 3D analysis and 2 for 2D", cxxopts::value<string>())
        ("pdbfile", "[2D + 3D] Path to Membrane pdb file that contain ONLY a single Membrane. Example: path/to/my/pdbfile.pdb", cxxopts::value<std::vector<std::string>>(),"Path+file")
        
        ("lmax", "[3D]The maximum mode number (l) the RSH amplitudes are measured for. This is a very expensive analysis since the number of angular modes (m) grow very rapidly (2*l+1) with l. Default 20.", cxxopts::value<int>(), "int")
        ("qmax", "[2D]The maximum wave number (q) the FFT amplitudes are measured for. . Default 70.", cxxopts::value<int>(), "int")
        
        ("angavg", "[optional][3D] Determin the number of random orientations the amplitude of a U_lm will be measured for. For each pdb frame, the Membrane will be randomly rotated (with respect to the Membrane's centre of mass) and the average amplitude of the measured U_lms will be written to the output file. Default 0. Note: This option cannot be selected simultaniusly with \"--align_axes\".", cxxopts::value<int>(), "int")
        
        ("alignaxes", "[optional][3D] Takes two integers, \"first index\" and \"second index\" to specify the orientation of the material frame with respect to which the RSH analysis will be done. During the analysis, the origin of the reference frame will be at the Membrane centre of mass and the frame of reference will be rotated so that the Z axis goes through the first node (first index) and the second node (second index) lies on the Z-Y plane. Note: This option cannot be selected simultaniusly with \"--angavg\".", cxxopts::value<std::vector<int>>(),"int,int")
        
        ("ext", "[optional][3D] The Ulm amplitudes will be saved in a file with the same name as the pdb file provided in the \"pdbpath\" + what you put in after this flag. Example (Note: irelevant flags are omited):   ./VCM --pdbpath mydirectory/mypdb.pdb --ext _myUlms.myextension --memlabels mem0                 Output files: mydirectory/mypdb_mem0_myUlms.myextension", cxxopts::value<std::string>(),"example.txt")
        
        ("framelimits", "[optional][3D] Takes the start and end frame of the pdb. The analysis will be performed only for these frames and all pdb frames in between. A 0 value for the first or last frame will be interpreted as the biginning frame and the las frame respectfully. Default is 0 for both limits. Example (Note: irelevant flags are omited):"
         "                                             "
         "\n  1)  ./VCM --framelimits 0,0""                 "
         "\n  2)  ./VCM --framelimits 3,0""                 "
         "\n  3)  ./VCM --framelimits 0,293""               "
         "\n  4)  ./VCM --framelimits 11,73""               "
         "\n  interpretation:""                             "
         "\n  1)  All frames.""                             "
         "\n  2)  From frame 3 to the end.""                "
         "\n  3)  From the beginning upto frame 293         "
         "(Not including 293)                               "
         "\n  4)  From frame 11 upto frame 73 (Not including"
         "73)                                               "
         , cxxopts::value<std::vector<int> >(),"int,int")
        ("memlabels", "[3D] The label(s) used to represent the memebrane(s) in the pdb file. The label(s) will also be used to distinguish between output files. Example (Note: irelevant flags are omited):               ./VCM --pdbpath mydirectory/mypdb.pdb --ext _myUlms.myextension --memlabels mem0,mem1            Output files: mydirectory/mypdb_mem0_myUlms.myextension and mydirectory/mypdb_mem1_myUlms.myextension", cxxopts::value<std::vector<std::string>>(),"mem0")
        
        ("m,minimisedState", "[3D] Calculates the difference between the mode amplitudes in the frames, and the amplitudes of the minimised state. Takes two options, 'Mesh' to use the mesh coordinates, or the frame number you wish to set as the minimised state.", cxxopts::value<string>(), "string or int")
        ("meshfile", "[2D and 3D] path to the mesh file.", cxxopts::value<string>(), "Path+file")
        ("reportindecies", "[E] The indecies (beginning from 0) corresponding to each membrane label. This option can be used for different combinations. A) If there is only one membrane in the reportfile/configfile you can exclude this option from the inputs. B) If there are multiple membranes in the reportfile/configfile and you are using the default option for the \"memlabels\" option (that uses the first atom label in the pdb) or one membrane label is provided, you should provide an index (integer beginning from 0) that corresponds to the memebrane configuration appearing in the reportfile/config file. For example if 3 membranes are configured in the reportfile/configfile the first one is identified with index 0, the second one with index 1, etc. If multiple membranes exist in the reportfile/configfile and multiple lables are provided using the \"memlabels\" option, the indecies corresponding to the membrane labels that appear in the reportfile/configfile must be provided. Example: 4 membranes configured in the reportfile/configfile, and the label (label is identified using the pdb file) of the third and first file is set using memlabels:\n ./VCM --analysis E .... --memlabels thirdlabel,firstlabel --reportindecies 2,0", cxxopts::value<std::vector<int> >(),"0,1")
        ;
        //      I have put this here so that parser throuws an exception when an extra argument is put in the command line that is not associated with any flags
        options.parse_positional({""});
        
        auto result = options.parse(argc, argv);
        
        if (result.count("help"))
        {
            std::cout << options.help() << std::endl;
            exit(0);
        }
        
        if (result.count("analysis"))
        {
            args.analysis=result["analysis"].as<string>();
        }
        
        if (result.count("pdbfile"))
        {
            auto& ff = result["pdbfile"].as<std::vector<std::string>>();
            string filename;
            for (const auto& f : ff)
            {
                filename+= f ;
            }
            args.analysis_filename = filename;
        }
        if (result.count("lmax"))
        {
            args.ell_max = result["lmax"].as<int>();
        }
        if (result.count("qmax"))
        {
            args.q_max = result["qmax"].as<int>();
        }
        
        if (result.count("angavg"))
        {
            args.analysis_averaging_option = 1;
            args.num_ang_avg = result["angavg"].as<int>();
        }
        if (result.count("alignaxes"))
        {
            args.analysis_averaging_option=2;
            const auto values = result["alignaxes"].as<std::vector<int>>();
            if (values.size()!=2) {
                string errorMessage = TWARN;
                errorMessage+="Error: "; errorMessage+=TRESET;
                errorMessage+="Expected 2 int arguments for \"alignaxes\", got "; errorMessage+=TFILE;
                errorMessage+=values.size(); errorMessage+=TRESET;
                errorMessage+="\n Input format example: --alignaxes 2,4\nRun help (-h, --help) for more information.\n";
                throw std::runtime_error(errorMessage);
            }
            args.z_node = values[0];
            args.zy_node= values[1];
        }
        if (result.count("ext"))
        {
            args.extension = result["ext"].as<std::string>();
        }
        if (result.count("framelimits"))
        {
            const auto values = result["framelimits"].as<std::vector<int>>();
            if (values.size()!=2) {
                
                string errorMessage = TWARN;
                errorMessage+="Error: "; errorMessage+=TRESET;
                errorMessage+="Expected 2 int arguments for \"framelimits\", got "; errorMessage+=TFILE;
                errorMessage+=values.size(); errorMessage+=TRESET;
                errorMessage+="\n Run help (-h, --help) for more information.\n";   errorMessage+=TRESET;
                throw std::runtime_error(errorMessage);
                
            } else if (values[0] > values[1] && values[1]!=0){
                
                string errorMessage = TWARN;
                errorMessage+="Error: "; errorMessage+=TRESET;
                errorMessage+="Got ";    errorMessage+=TFILE;
                errorMessage+=values[0]; errorMessage+=TRESET;
                errorMessage+=" as the beginning frame and ";  errorMessage+=TFILE;
                errorMessage+=values[1]; errorMessage+=TRESET;
                errorMessage+=" as the end frame. Beginning frame cannot be larger than end frame.\n Run help (-h, --help) for more information.\n";
                throw std::runtime_error(errorMessage);
                
            } else if (values[0]!=0 && values[1]!=0 ){
                if (values[0]==values[1]) {
                    
                    string errorMessage = TWARN;
                    errorMessage+="Error: "; errorMessage+=TRESET;
                    errorMessage+="Beginning frame ("; errorMessage+=TFILE;
                    errorMessage+=values[0]; errorMessage+=TRESET;
                    errorMessage+=") and end frame ("; errorMessage+=TFILE;
                    errorMessage+=values[1]; errorMessage+=TRESET;
                    errorMessage+=") cannot point to the same frame number. If you wish to analyse frame number N, set the beginning frame to N and end frame to N+1.\n Run help (-h, --help) for more information.\n";
                    throw std::runtime_error(errorMessage);
                }
            }
            args.framelimits_beg = values[0];
            args.framelimits_end = values[1];
        }
        if (result.count("memlabels"))
        {
            auto& ff = result["memlabels"].as<std::vector<std::string>>();
            for (const auto& f : ff)
            {
                args.membane_labels.push_back(f) ;
            }
        }
        if (result.count("reportindecies"))
        {
            const auto values = result["reportindecies"].as<std::vector<int>>();
            for (auto &ind: values) {
                args.membane_label_indecies.push_back(ind);
            }
        } else {
            args.membane_label_indecies.push_back(0);
        }
        if (result.count("m"))
        {
            string input = result["minimisedState"].as<string>();
            
            if (input == "Mesh" || input == "mesh") {
                args.MeshMinimisation = true;
            } else {
                try {
                    args.FrameMinimisation = stoi(input);
                } catch (...) {
                    string errorMessage = TWARN;
                    errorMessage+="Error: "; errorMessage+=TRESET;
                    errorMessage+="Invalid input for the \"minimisedState\" ("; errorMessage+=TFILE;
                    errorMessage+=input; errorMessage+=TRESET;
                    errorMessage+="). Please try again.\nExample inputs: Mesh, mesh, or an integer (example 2).";
                    throw std::runtime_error(errorMessage);
                }
                
            }
        }
        if (result.count("meshfile"))
        {
            args.meshpathinput = result["meshfile"].as<string>();
        }
        
        consistency_check(args);
        return args;
        
    } catch (const cxxopts::OptionException& e)
    {
        std::cout << "error parsing options: " << e.what() << std::endl;
        exit(6);
    }
    
    
}

void consistency_check(ArgStruct_Analysis &args){
    cout<<endl;
    
    if (args.analysis != "2" && args.analysis != "3" && args.analysis != "E") {
        string errorMessage = TBLINK;
        errorMessage += TRED;
        errorMessage+="Error: "; errorMessage+=TRESET;
        errorMessage+="Analysis dimension option not supported.\nUse -h for more information."; errorMessage+=TRESET;
        throw std::runtime_error(errorMessage);
    } else {
        if (args.analysis_filename == "" ) {
            string errorMessage = TBLINK;
            errorMessage += TRED;
            errorMessage += "Analysis file (path+file) not provided. Use -h for more information.";
            throw std::runtime_error(errorMessage);
        } else {
            args.analysis_filename = check_if_file_exists(args.analysis_filename);
            cout<<"Will analyse \""<<TFILE<<args.analysis_filename<<TRESET<<"\""<<endl;
        }
        
        if (args.analysis == "3") {
            cout<<TWARN<<"3D"<<TRESET" analysis mode: "<<TON<<"ON\n"<<TRESET;
            cout<<"Real Spherical harmonics, U_lm, will go up to l="<<args.ell_max<<endl;
            
            
            if (args.MeshMinimisation) {
                args.analysis_averaging_option = 2;
            }
            if (args.FrameMinimisation>0) {
                args.analysis_averaging_option = 2;
            }
            
        } else if (args.analysis == "2") {
            cout<<TWARN<<"2D"<<TRESET" analysis mode: "<<TON<<"ON\n"<<TRESET;
            cout<<TWWARN<<"This option is under development. Prceed with the trial version."<<TRESET<<endl;
            
            cout<<"Wave number, q, will go up to q_max="<<args.q_max<<endl;
            
        }
        
        if (args.num_ang_avg !=1 && args.z_node !=-1) {
            cout<<"\"angavg\" and \"alignaxes\" "<<TWWARN<<"cannot be used simultaneously"<<TRESET<<". Run help (-h, --help) for more inforamtion."<<endl;
            exit(4);
        }else {
            if (args.analysis_averaging_option == 1) {
                cout<<"Average over random rotations:"<<TON<<" ON"<<TRESET<<endl;
                cout<<"Number of random rotations: "<<args.num_ang_avg<<endl;
            }else if (args.analysis_averaging_option == 2) {
                cout<<"Material frame reference: "<<TON<<"ON"<<TRESET<<endl;
                cout<<"Node index on the Z axis: "<<args.z_node<<endl;
                cout<<"Node inedx on the Z-Y plane: "<<args.zy_node<<endl;
                
            }
        }
        
        
        if (args.membane_labels.size()==0) {
            cout<<TWARN<<"\n!!!Warning"<<TRESET<<TBOLD<<", no membrane labels assigned. If the pdb strictly containes one Membrane class (and no other classes), ignore this warning. If it contains multiple membranes or other classes as well, assign the membrane label to be analysed. By default VCM analyser will use the first ATOM label appearing in the pdb file.\n"<<endl;
        } else {
            check_membrane_labels(args);
        }
        build_output_names(args);
        
        cout<<"Output file(s):\n";
        for (int i=0; i<args.output_filename.size(); i++) {
            cout<<"\t"<<TFILE<<args.output_filename[i]<<TRESET<<endl;
        }
        
        if (args.analysis!="E") {
            if (args.meshpathinput == "None") {
                string meshpath;
                cout<<"Mesh information is "<<TWARN<<"required"<<TRESET<<" for analysis."<<endl;
                if (args.membane_labels.size()<2) {
                    cout<<"Please enter the path+file for the Membrane mesh.\nExample:\n\tpath/to/my/mesh.ply"<<endl;
                    cout<<TFILE;
                    cin>>meshpath;
                    cout<<TRESET;
                    
                    meshpath = check_if_file_exists(meshpath);
                    args.Mesh_files.push_back(meshpath);
                    
                    //                bool valid_answer = false;
                    //                string yn_answer;
                    //                cout<<"Real Spherical harmonics amplitutes are calculated relative to the ground state of the system. The default ground state is a sphere with the same surface area as the Membrane. Do you want to change the ground state to the shape of the Mesh? (y,n)"<<endl;
                    //                cout<<TFILE;
                    //                cin>>yn_answer;
                    //                cout<<TRESET;
                    //                if (yn_answer=="y") {
                    //                    args.MeshMinimisation.push_back(true);
                    //                    valid_answer=true;
                    //                } else if (yn_answer=="n") {
                    //                    args.MeshMinimisation.push_back(false);
                    //                    valid_answer=true;
                    //                }
                    //                while (!valid_answer) {
                    //                    cout<<"Please enter y or n"<<endl;
                    //                    cout<<TFILE;
                    //                    cin>>yn_answer;
                    //                    cout<<TRESET;
                    //                    if (yn_answer=="y") {
                    //                        args.MeshMinimisation.push_back(true);
                    //                        valid_answer=true;
                    //                    } else if (yn_answer=="n") {
                    //                        args.MeshMinimisation.push_back(false);
                    //                        valid_answer=true;
                    //                    }
                    //                }
                    
                } else {
                    cout<<"Please enter the path+file for the following Membranes.\nExample:\n\tpath/to/my/mesh.ply"<<endl;
                    for (int i=0; i<args.membane_labels.size(); i++) {
                        cout<<"For "<<TWARN<<args.membane_labels[i]<<TRESET<<":"<<endl;
                        cout<<TFILE;
                        cin>>meshpath;
                        cout<<TRESET;
                        std::ifstream read_mesh;
                        read_mesh.open(meshpath.c_str());
                        if (!read_mesh) {
                            std::cout << TWWARN<<"Unable to read"<<TRESET<<" \""<<TFILE<<meshpath<<TRESET<<"\" or it does not exist.\nPlease try again."<<std::endl;
                            i--;
                        } else {
                            args.Mesh_files.push_back(meshpath);
                        }
                    }
                }
            } else {
                string meshpath = check_if_file_exists(args.meshpathinput);
                args.Mesh_files.push_back(meshpath);
            }
            
            
        } else {
            if (args.membane_labels.size()!=0 && args.membane_labels.size()!=args.membane_label_indecies.size()) {
                string errorMessage = TWARN;
                errorMessage +="Error: Arg Parser: The number of mambrane labels and the corresponding indecies don't match. Check the number of arguments in the \"memlabels\" and \"reportindecies\" options.\n";
                errorMessage +=TRESET;
                throw std::runtime_error(errorMessage);
            }
            if (args.membane_labels.size()==0) {
                cout<<TWARN<<"\n!!!Warning"<<TRESET<<TBOLD<<", no membrane labels assigned. By default VCM analyser will use the first Membrane class settings in the report file to initilise the membrane. For more options use the \"memlabel\" and \"reportindecies\" flags or use -h for more options.\n"<<endl;
            }
        }
        
        if (args.framelimits_beg==0 && args.framelimits_end==0) {
            cout<<"All frames will be analysied."<<endl;
        } else if (args.framelimits_beg==0) {
            cout<<"Frames from the beginning to frame "<<args.framelimits_end<<" will be analysed."<<endl;
            
        } else if (args.framelimits_end==0) {
            cout<<"Frames from "<<args.framelimits_beg<<" to the end will be analysed."<<endl;
        } else if (args.framelimits_end == args.framelimits_beg+1){
            cout<<"Frame "<<args.framelimits_beg<<" will be analysed."<<endl;
        } else {
            cout<<"Frames from "<<args.framelimits_beg<<" to "<<args.framelimits_end<<" will be analysed."<<endl;
        }
    }
    cout<<endl;
    
    
}



void build_output_names(ArgStruct_Analysis &args){
    if (args.analysis == "3") {
        if (args.extension=="") {
            args.extension="_ulmt.txt";
        }
    } else if (args.analysis == "2") {
        if (args.extension=="") {
            args.extension="_f2d.txt";
        }
    }
    if (args.membane_labels.size()==0) {
        string filename = args.analysis_filename;
        filename.erase(filename.end()-4,filename.end() );
        args.output_filename.push_back(filename + args.extension);
    } else {
        string filename = args.analysis_filename;
        filename.erase(filename.end()-4,filename.end() );
        for (int i=0; i<args.membane_labels.size(); i++) {
            args.output_filename.push_back(filename +"_"+args.membane_labels[i]+args.extension);
        }
    }
    
}

void check_membrane_labels(ArgStruct_Analysis &args){
    if (args.membane_labels.size()!=0) {
        for (int i=0; i<args.membane_labels.size()-1; i++) {
            string label = args.membane_labels[i];
            for (int j=i+1; j<args.membane_labels.size(); j++) {
                if (label==args.membane_labels[j]) {
                    args.membane_labels.erase(args.membane_labels.begin()+j);
                    j--;
                }
            }
            
        }
        for (int i=0; i<args.membane_labels.size(); i++) {
            if (args.membane_labels[i].size()!=4) {
                cout<<TWWARN<<"\n!!!Warning"<<TRESET<<", pdb labels are 4 charachters long. The Membrane label \""<<TWARN<<args.membane_labels[i]<<TRESET<<"\" is "<<args.membane_labels[i].size()<<" long. If there is a typo, end the programme and start again.\n\n";
            }
        }
    }
}
