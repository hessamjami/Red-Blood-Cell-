#include "Membrane.h"
#include "General_functions.hpp"
#include "OpenMM_structs.h"
#include "OpenMM_funcs.hpp"

const int EndOfList=-1;
using OpenMM::Vec3;
using std::vector;
using std::set;

void print_platform_info(ArgStruct_VCM userinputs){
    //    const string cbp_plugin_location="/scratch/alifarnudi/local/openmm/lib/plugins";
    if (!userinputs.platformPluginInput) {
        OpenMM::Platform::loadPluginsFromDirectory(OpenMM::Platform::getDefaultPluginsDirectory());
    } else {
        OpenMM::Platform::loadPluginsFromDirectory(userinputs.platforminfo.platformPluginPath);
    }
    
    PlatformInfo platforminfo = get_platform_info();
    cout<<"device flags:\n--platformID "<<platforminfo.platform_id<<" --platformDeviceID "<<platforminfo.platform_device_id<<endl;
    
}

void get_platform_device_properties(int index, int platform_id, string precision, PlatformInfo &platforminfo, bool print_to_screen, int &counter){
    int stepSizeInFs =1;
    OpenMM::Platform& platform = OpenMM::Platform::getPlatform(platform_id);
    std::string report;
    std::map<std::string, std::string> temp_device_properties;
    //                    temp_device_properties["CudaPlatformIndex"]=std::to_string(i);
    temp_device_properties["DeviceIndex"]=std::to_string(index);
    temp_device_properties["CudaPrecision"]=precision;
    OpenMM::System temp_system;
    temp_system.addParticle(1.0);
    OpenMM::VerletIntegrator temp_inegrator(stepSizeInFs * OpenMM::PsPerFs);
    OpenMM::Context temp_context(temp_system, temp_inegrator, platform, temp_device_properties);
    std::vector<std::string> platform_devices = platform.getPropertyNames();
    if (print_to_screen) {
        cout<<TBOLD<<TCUD<<counter<<TRESET<<" : ";
    }
    for (auto & name : platform_devices){
        if (name == "DeviceIndex") {
            continue;
        } else {
            report+="\t"+name+"\t"+platform.getPropertyValue(temp_context, name)+"\n";
            if (print_to_screen) {
                cout<<"\t"<<name<<"\t"<<TCUD<<platform.getPropertyValue(temp_context, name)<<TRESET<<endl;
            }
        }
    }
    if (print_to_screen) {
        cout<<TRESET<<"------------------------"<<endl;
    }
    counter++;
    platforminfo.device_properties.push_back(temp_device_properties);
    platforminfo.device_properties_report.push_back(report);
}

PlatformInfo get_platform_info_forResume(string platformName, ArgStruct_VCM userinputs)
{
    int stepSizeInFs =1;
    int list_depth=20;
    if (!userinputs.platformPluginInput) {
        OpenMM::Platform::loadPluginsFromDirectory(OpenMM::Platform::getDefaultPluginsDirectory());
    } else {
        OpenMM::Platform::loadPluginsFromDirectory(userinputs.platforminfo.platformPluginPath);
    }
    
    PlatformInfo platforminfo;
    
    for (int i = 0; i < OpenMM::Platform::getNumPlatforms(); i++) {
        OpenMM::Platform& platform = OpenMM::Platform::getPlatform(i);
        if (platform.getName() == platformName) {
            platforminfo.platform_id=i;
            break;
        }
    }
    OpenMM::Platform& platform = OpenMM::Platform::getPlatform(platforminfo.platform_id);
    
    vector<vector<string> > checkpointDeviceProperties;
    string hardwarereport = generalParameters.Checkpoint_path;
    string namesize="_Checkpoint";
    int del = namesize.size();
    hardwarereport.erase(hardwarereport.end()-del, hardwarereport.end());
    hardwarereport+="_hardware_runtime.txt";
    ifstream hardwarefile(hardwarereport.c_str());
    vector<string> deviceProperties;
    string line;
    getline (hardwarefile,line);
    getline (hardwarefile,line);
    getline (hardwarefile,line);
    vector<string> temp_splitline = splitstring(line, ' ');
    int platofrm_ID = stoi(temp_splitline[1]);
    int platform_device_ID = stoi(temp_splitline[3]);
    getline (hardwarefile,line);
    while (getline (hardwarefile,line)) {
//        cout<<line<<endl;
        vector<string> splitline = splitstring(line, '\t');
        if (splitline.size()<3) {
            break;
        }
        deviceProperties.clear();
        deviceProperties.push_back(splitline[1]);
        deviceProperties.push_back(splitline[2]);
        checkpointDeviceProperties.push_back(deviceProperties);
//        cout<<checkpointDeviceProperties[checkpointDeviceProperties.size()-1][0]<<checkpointDeviceProperties[checkpointDeviceProperties.size()-1][1]<<endl;
    }
    
    int platformDeviceID = -1;
//
    if (platformName == "OpenCL") {
        int counter=-1;
        for (int i=0; i<list_depth; i++) {
            for (int j=0; j<list_depth; j++) {
                vector<string> data={"single","double", "mixed"};
                for (vector<string>::iterator precision=data.begin(); precision!=data.end(); ++precision) {
                    int hardwareCompatibility=0;
                    try {
                        std::string report;
                        std::map<std::string, std::string> temp_device_properties;
                        temp_device_properties["OpenCLPlatformIndex"]=std::to_string(i);
                        temp_device_properties["OpenCLDeviceIndex"]=std::to_string(j);
                        temp_device_properties["Precision"]=*precision;
                        OpenMM::System temp_system;
                        temp_system.addParticle(1.0);
                        OpenMM::VerletIntegrator temp_inegrator(stepSizeInFs * OpenMM::PsPerFs);
                        OpenMM::Context temp_context(temp_system, temp_inegrator, platform, temp_device_properties);
                        std::vector<std::string> platform_devices = platform.getPropertyNames();
//                        cout<<TBOLD<<TOCL<<counter<<TRESET<<" : ";
                        counter++;
                        if (counter==platform_device_ID) {
                            for (auto & name : platform_devices){
                                for (auto &prop: checkpointDeviceProperties) {
                                    if (name==prop[0] && platform.getPropertyValue(temp_context, name)==prop[1]) {
                                        hardwareCompatibility++;
//                                        cout<<hardwareCompatibility<<endl;
                                    }
                                }
                            }
                        }
                        platforminfo.device_properties.push_back(temp_device_properties);
                    } catch (const std::exception& e) {

                    }
                    if (hardwareCompatibility==checkpointDeviceProperties.size()) {
                        platformDeviceID=platform_device_ID;
                    }
                }
            }
        }
    } else if (platformName == "CUDA") {
        int counter=-1;
        for (int j=0; j<2*list_depth; j++) {
            vector<string> data={"single","double", "mixed"};
            for (vector<string>::iterator precision=data.begin(); precision!=data.end(); ++precision) {
                int hardwareCompatibility=0;
                try {
                    std::string report;
                    std::map<std::string, std::string> temp_device_properties;
//                    temp_device_properties["CudaPlatformIndex"]=std::to_string(i);
                    temp_device_properties["DeviceIndex"]=std::to_string(j);
                    temp_device_properties["CudaPrecision"]=*precision;
                    OpenMM::System temp_system;
                    temp_system.addParticle(1.0);
                    OpenMM::VerletIntegrator temp_inegrator(stepSizeInFs * OpenMM::PsPerFs);
                    OpenMM::Context temp_context(temp_system, temp_inegrator, platform, temp_device_properties);
                    std::vector<std::string> platform_devices = platform.getPropertyNames();
                    counter++;
                    if (counter == platform_device_ID){
                        for (auto & name : platform_devices){
                            for (auto &prop: checkpointDeviceProperties) {
                                if (name==prop[0] && platform.getPropertyValue(temp_context, name)==prop[1]) {
//                                    cout<<prop[0]<<"  "<<prop[1]<<"\t";
                                    hardwareCompatibility++;
//                                    cout<<hardwareCompatibility<<endl;
                                }
                            }
                            
                            

                        }
//                        cout<<TRESET<<"------------------------"<<endl;
                    }

//                    counter++;
                    platforminfo.device_properties.push_back(temp_device_properties);
//                    platforminfo.device_properties_report.push_back(report);
                } catch (const std::exception& e) {

                }
                if (hardwareCompatibility==checkpointDeviceProperties.size()) {
                    platformDeviceID=platform_device_ID;
                }
            }
        }
    } else if (platformName == "CPU") {
        int hardwareCompatibility=0;
        OpenMM::System temp_system;
        temp_system.addParticle(1.0);
        OpenMM::VerletIntegrator temp_inegrator(stepSizeInFs * OpenMM::PsPerFs);
        OpenMM::Context temp_context(temp_system, temp_inegrator, platform);
        std::vector<std::string> platform_devices = platform.getPropertyNames();

        for (auto & name : platform_devices){
            for (auto &prop: checkpointDeviceProperties) {
                if (name==prop[0] && platform.getPropertyValue(temp_context, name)==prop[1]) {
                    hardwareCompatibility++;
                }
            }
        }
        if (hardwareCompatibility==checkpointDeviceProperties.size()) {
            platformDeviceID=0;
        }
    }

//    exit(0);
    if (platformDeviceID<0) {
        string errorMessage = TWARN;
        errorMessage+="error platform compatibility: non of the available platform devices were compatible with the hardware runtime report. Please check if the machine's hardware has chamged since the simulation interruption.\n";
        errorMessage+= TRESET;
        throw std::runtime_error(errorMessage);
    } else {
        platforminfo.platform_device_id=platformDeviceID;
    }
    
    return platforminfo;
}

PlatformInfo get_platform_info(void)
{
    int stepSizeInFs =1;
    int list_depth=20;
    PlatformInfo platforminfo;
    
    
    //cout<<"platform default directory path = "<<OpenMM::Platform::getDefaultPluginsDirectory()<<endl;
    //Listing the names of all available platforms.
//    cout<<"Here";
    cout<<TOMM<<"\nOpenMM available platforms:\n"<<TGRAY<<"Index Name \t  Speed (Estimated)\n"<<TRESET;
    for (int i = 0; i < OpenMM::Platform::getNumPlatforms(); i++) {
        OpenMM::Platform& platform = OpenMM::Platform::getPlatform(i);
        cout<<" ("<<TBOLD<<std::to_string(i)<<TRESET<<")  "<<platform.getName().c_str()<<
        "\t   "<<platform.getSpeed()<<endl;
    }
    platforminfo.platform_id=0;
    cout<<"Please choose a pltform (index): \n"<<TFILE;
    std::cin>>platforminfo.platform_id;
    cout<<TRESET;
    OpenMM::Platform& platform = OpenMM::Platform::getPlatform(platforminfo.platform_id);
    
    //    std::vector<std::string> device_properties_report;
    //    std::vector<std::map<std::string, std::string> > device_properties;
    if (platform.getName() == "OpenCL") {
        cout<<"Available devices on the "<<TOCL<<platform.getName()<<TRESET<<" platform:\n";
        int counter=0;
        for (int i=0; i<list_depth; i++) {
            for (int j=0; j<list_depth; j++) {
                vector<string> data={"single","double", "mixed"};
                for (vector<string>::iterator precision=data.begin(); precision!=data.end(); ++precision) {
                    try {
                        std::string report;
                        std::map<std::string, std::string> temp_device_properties;
                        temp_device_properties["OpenCLPlatformIndex"]=std::to_string(i);
                        temp_device_properties["OpenCLDeviceIndex"]=std::to_string(j);
                        temp_device_properties["Precision"]=*precision;
                        OpenMM::System temp_system;
                        temp_system.addParticle(1.0);
                        OpenMM::VerletIntegrator temp_inegrator(stepSizeInFs * OpenMM::PsPerFs);
                        OpenMM::Context temp_context(temp_system, temp_inegrator, platform, temp_device_properties);
                        std::vector<std::string> platform_devices = platform.getPropertyNames();
                        cout<<TBOLD<<TOCL<<counter<<TRESET<<" : ";
                        for (auto & name : platform_devices){
                            if (name == "DeviceIndex" || name == "OpenCLPlatformIndex") {
                                continue;
                            } else {
                                report+="\t"+name+"\t"+platform.getPropertyValue(temp_context, name)+"\n";
                                cout<<"\t"<<name<<"\t"<<TOCL<<platform.getPropertyValue(temp_context, name)<<TRESET<<endl;
                            }
                        }
                        cout<<TRESET<<"------------------------"<<endl;
                        counter++;
                        platforminfo.device_properties.push_back(temp_device_properties);
                        platforminfo.device_properties_report.push_back(report);
                    } catch (const std::exception& e) {
                        
                    }
                }
            }
        }
    } else if (platform.getName() == "CUDA") {
        cout<<"Available devices on the "<<TCUD<<platform.getName()<<TRESET<<" platform:\n";
        int counter=0;
        for (int j=0; j<2*list_depth; j++) {
            vector<string> data={"single","double", "mixed"};
            for (vector<string>::iterator precision=data.begin(); precision!=data.end(); ++precision) {
                try {
                    get_platform_device_properties(j, platforminfo.platform_id, *precision, platforminfo, true, counter);
//                    std::string report;
//                    std::map<std::string, std::string> temp_device_properties;
//                    //                    temp_device_properties["CudaPlatformIndex"]=std::to_string(i);
//                    temp_device_properties["DeviceIndex"]=std::to_string(j);
//                    temp_device_properties["CudaPrecision"]=*precision;
//                    OpenMM::System temp_system;
//                    temp_system.addParticle(1.0);
//                    OpenMM::VerletIntegrator temp_inegrator(stepSizeInFs * OpenMM::PsPerFs);
//                    OpenMM::Context temp_context(temp_system, temp_inegrator, platform, temp_device_properties);
//                    std::vector<std::string> platform_devices = platform.getPropertyNames();
//                    cout<<TBOLD<<TCUD<<counter<<TRESET<<" : ";
//                    for (auto & name : platform_devices){
//                        if (name == "DeviceIndex") {
//                            continue;
//                        } else {
//                            report+="\t"+name+"\t"+platform.getPropertyValue(temp_context, name)+"\n";
//                            cout<<"\t"<<name<<"\t"<<TCUD<<platform.getPropertyValue(temp_context, name)<<TRESET<<endl;
//                        }
//                    }
//                    cout<<TRESET<<"------------------------"<<endl;
//                    counter++;
//                    platforminfo.device_properties.push_back(temp_device_properties);
//                    platforminfo.device_properties_report.push_back(report);
                } catch (const std::exception& e) {
                    
                }
            }
        }
    } else if (platform.getName() == "CPU") {
        OpenMM::System temp_system;
        temp_system.addParticle(1.0);
        OpenMM::VerletIntegrator temp_inegrator(stepSizeInFs * OpenMM::PsPerFs);
        OpenMM::Context temp_context(temp_system, temp_inegrator, platform);
        std::vector<std::string> platform_devices = platform.getPropertyNames();
        cout<<TCPU<<"CPU"<<TRESET<<" properties:\n";
        for (auto & name : platform_devices){
            cout<<"\t"<<name<<"\t"<<TCPU<<platform.getPropertyValue(temp_context, name)<<TRESET<<endl;
        }
        cout<<TRESET<<"------------------------"<<endl;
    }
    
    platforminfo.platform_device_id=0;
    if (platforminfo.device_properties.size()>1) {
        cout<<"Please choose a device (index): \n"<<TFILE;
        std::cin>>platforminfo.platform_device_id;
        cout<<TRESET;
    }
    
    return platforminfo;
}

void get_platform_info(PlatformInfo &platforminfo)
{
    int stepSizeInFs =1;
    int list_depth=20;
    OpenMM::Platform& platform = OpenMM::Platform::getPlatform(platforminfo.platform_id);
    
    if (platform.getName() == "OpenCL") {
//        int counter=0;
        for (int i=0; i<list_depth; i++) {
            for (int j=0; j<list_depth; j++) {
                vector<string> data={"single","double", "mixed"};
                for (vector<string>::iterator precision=data.begin(); precision!=data.end(); ++precision) {
                    try {
                        std::string report;
                        std::map<std::string, std::string> temp_device_properties;
                        temp_device_properties["OpenCLPlatformIndex"]=std::to_string(i);
                        temp_device_properties["OpenCLDeviceIndex"]=std::to_string(j);
                        temp_device_properties["Precision"]=*precision;
                        OpenMM::System temp_system;
                        temp_system.addParticle(1.0);
                        OpenMM::VerletIntegrator temp_inegrator(stepSizeInFs * OpenMM::PsPerFs);
                        OpenMM::Context temp_context(temp_system, temp_inegrator, platform, temp_device_properties);
                        std::vector<std::string> platform_devices = platform.getPropertyNames();
                        for (auto & name : platform_devices){
                            if (name == "DeviceIndex" || name == "OpenCLPlatformIndex") {
                                continue;
                            } else {
                                report+="\t"+name+"\t"+platform.getPropertyValue(temp_context, name)+"\n";
                            }
                        }
//                        counter++;
                        platforminfo.device_properties.push_back(temp_device_properties);
                        platforminfo.device_properties_report.push_back(report);
                    } catch (const std::exception& e) {
                        
                    }
                }
            }
        }
    } else if (platform.getName() == "CUDA") {
        int counter=0;
        for (int j=0; j<2*list_depth; j++) {
            vector<string> data={"single","double", "mixed"};
            for (vector<string>::iterator precision=data.begin(); precision!=data.end(); ++precision) {
                try {
                    get_platform_device_properties(j, platforminfo.platform_id, *precision, platforminfo, false, counter);
//                    std::string report;
//                    std::map<std::string, std::string> temp_device_properties;
//                    //                    temp_device_properties["CudaPlatformIndex"]=std::to_string(i);
//                    temp_device_properties["DeviceIndex"]=std::to_string(j);
//                    temp_device_properties["CudaPrecision"]=*precision;
//                    OpenMM::System temp_system;
//                    temp_system.addParticle(1.0);
//                    OpenMM::VerletIntegrator temp_inegrator(stepSizeInFs * OpenMM::PsPerFs);
//                    OpenMM::Context temp_context(temp_system, temp_inegrator, platform, temp_device_properties);
//                    std::vector<std::string> platform_devices = platform.getPropertyNames();
//                    for (auto & name : platform_devices){
//                        if (name == "DeviceIndex") {
//                            continue;
//                        } else {
//                            report+="\t"+name+"\t"+platform.getPropertyValue(temp_context, name)+"\n";
//                        }
//                    }
////                    counter++;
//                    platforminfo.device_properties.push_back(temp_device_properties);
//                    platforminfo.device_properties_report.push_back(report);
                } catch (const std::exception& e) {
                    
                }
            }
        }
    } else if (platform.getName() == "CPU") {
        OpenMM::System temp_system;
        temp_system.addParticle(1.0);
        OpenMM::VerletIntegrator temp_inegrator(stepSizeInFs * OpenMM::PsPerFs);
        OpenMM::Context temp_context(temp_system, temp_inegrator, platform);
        std::vector<std::string> platform_devices = platform.getPropertyNames();
    }
}



void generateHardwareReport (PlatformInfo platforminfo){
    OpenMM::Platform& platform = OpenMM::Platform::getPlatform(platforminfo.platform_id);
    int stepSizeInFs =1;
    generalParameters.hardwareReport  = exec("hostname");
    generalParameters.hardwareReport += "Running on the "+platform.getName()+" platform:\nPlatformID "+to_string(platforminfo.platform_id)+" platformDeviceID "+to_string(platforminfo.platform_device_id)+"\nDefault properties:\n";
    if (platform.getName() != "CPU") {
        generalParameters.hardwareReport+=platforminfo.device_properties_report[platforminfo.platform_device_id]+"\n";
    } else {
        OpenMM::System tsystem;
        tsystem.addParticle(1.0);
        OpenMM::VerletIntegrator tinegrator(stepSizeInFs * OpenMM::PsPerFs);
        OpenMM::Context tcontext(tsystem, tinegrator, platform);
        std::vector<std::string> tplatform_devices = platform.getPropertyNames();
        for (auto & name : tplatform_devices){
            generalParameters.hardwareReport+="\t"+name+"\t"+platform.getPropertyValue(tcontext, name)+"\n";
        }
    }
    generalParameters.hardwareReport+="------------------------\n\n";
}
