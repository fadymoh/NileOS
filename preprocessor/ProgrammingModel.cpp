// Programming Model.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <algorithm>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
using namespace std;

struct function_desc
{
    string return_type;
    string function_pointer;
    string parameters;
    string body;

    char type;
};

struct serviceDetails
{
    string fileName;
    string serviceName;
    vector<pair<string, string>> functionPointers;
};

vector<function_desc> functions;
vector<serviceDetails> serviceDetailsArr;

void map_function_to_struct(string function, char type, serviceDetails&);
string create_service_enum_values(string, string);
bool createLibraryFile(ostream &);
string create_services_enum(ostream &);
bool generateLinkerFile();

#define kernelFunctionTokenDef "(?:\/\/STL_K)([\\s\\S]*?)(?:\/\/END_STL_K)"
#define userFunctionTokenDef "(?:\/\/STL_U)([\\s\\S]*?)(?:\/\/END_STL_U)"
#define serviceObjectNameTokenDef "(?:typedef struct \\{)(?:\\s*)([\\s\\S]*)(?:\\s*\\})([\\S]*)(?:\\s?\\;)"
#define p(x) cout << x << "\n";
const int BUFFER_SIZE = 1000;

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        puts("wrong number of arguments!");
        return EXIT_FAILURE;
    }
    string inputDirectory = argv[1];
    string outputDirectory = argv[2];
    string libraryFileName = "Library.h";
    string libraryFilePath = outputDirectory + libraryFileName;
    ofstream outputStreamLibraryFile(libraryFilePath);

    for (const auto &entry : fs::directory_iterator(inputDirectory))
    {
        string path = entry.path();
        serviceDetails tempServiceDetails;
        if (path.find(".c") == string::npos)
            continue;

        string fileName = path.substr(inputDirectory.length(), (path.length() - inputDirectory.length() - 2));
        tempServiceDetails.fileName = fileName;
        
        string kernelOutputCFile = outputDirectory + fileName + "_K.c";
        string userOutputCFile = outputDirectory + fileName + "_U.c";

        printf("fileName: %s\nkernelOutputCFile: %s\n userOutputCFile: %s\n", fileName.c_str(), kernelOutputCFile.c_str(), userOutputCFile.c_str());
        ofstream outputStreamKernelCFile(kernelOutputCFile);
        ofstream outputStreamUserCFile(userOutputCFile);
        ifstream inputStreamCFile(inputDirectory + fileName + ".c");
        std::experimental::filesystem::copy_file(inputDirectory + fileName + ".h", outputDirectory + fileName + ".h", fs::copy_options::overwrite_existing);
        fstream inOutStreamHeaderFile(outputDirectory + fileName + ".h", ios::out | ios::app | ios::in);

        string streamIteratorCFile((std::istreambuf_iterator<char>(inputStreamCFile)), istreambuf_iterator<char>());

        regex kernelFunctionToken(kernelFunctionTokenDef);
        regex userFunctionToken(userFunctionTokenDef);
        regex serviceObjectNameToken(serviceObjectNameTokenDef);

        std::regex_iterator<std::string::iterator> kernelFunctionTokenIteratorBegin(streamIteratorCFile.begin(), streamIteratorCFile.end(), kernelFunctionToken);
        std::regex_iterator<std::string::iterator> kernelFunctionTokenIteratorEnd;

        std::regex_iterator<std::string::iterator> userFunctionTokenIteratorBegin(streamIteratorCFile.begin(), streamIteratorCFile.end(), userFunctionToken);
        std::regex_iterator<std::string::iterator> userFunctionTokenIteratorEnd;

        string streamIteratorHeaderFile((std::istreambuf_iterator<char>(inOutStreamHeaderFile)), istreambuf_iterator<char>());
        std::regex_iterator<std::string::iterator> headerTokenIteratorBegin(streamIteratorHeaderFile.begin(), streamIteratorHeaderFile.end(), serviceObjectNameToken);
        std::regex_iterator<std::string::iterator> headerTokenIteratorEnd;

        string serviceObjectName = (*headerTokenIteratorBegin)[2];
        tempServiceDetails.serviceName = serviceObjectName;
        char buffer[BUFFER_SIZE];
        sprintf(buffer, "#include \"%s.h\"\n", fileName.c_str());

        outputStreamKernelCFile << buffer;
        outputStreamUserCFile << buffer;

        sprintf(buffer, "#include \"%s.h\"\n", "Console");
        
        outputStreamKernelCFile << buffer;
        outputStreamUserCFile << buffer;

        while (kernelFunctionTokenIteratorBegin != kernelFunctionTokenIteratorEnd)
        {
            string str_temp = ((*kernelFunctionTokenIteratorBegin)[1]);
            outputStreamKernelCFile << str_temp;

            map_function_to_struct(str_temp, 'K', tempServiceDetails);
            ++kernelFunctionTokenIteratorBegin;
        }

        while (userFunctionTokenIteratorBegin != userFunctionTokenIteratorEnd)
        {
            string str_temp = ((*userFunctionTokenIteratorBegin)[1]);
            outputStreamUserCFile << str_temp;

            map_function_to_struct(str_temp, 'U', tempServiceDetails);
            ++userFunctionTokenIteratorBegin;
        }

        string enums = create_service_enum_values(fileName, serviceObjectName);
        functions.clear();

        serviceDetailsArr.push_back(tempServiceDetails);
        inOutStreamHeaderFile << enums << endl;

        inOutStreamHeaderFile.close();
        outputStreamKernelCFile.close();
        outputStreamUserCFile.close();
    }

    createLibraryFile(outputStreamLibraryFile);
    generateLinkerFile();
    outputStreamLibraryFile.close();
    return 0;
}

void map_function_to_struct(string function, char type, serviceDetails& tempServiceDetails)
{
    regex expression("([a-zA-Z].+?)\\s(.+?)\\s{0,}([(].+[)])\\s([\\s\\S]+)");

    std::regex_iterator<std::string::iterator> start(function.begin(), function.end(), expression);
    std::regex_iterator<std::string::iterator> end;

    function_desc temp_fn;

    temp_fn.return_type = (*start)[1];
    temp_fn.function_pointer = (*start)[2];
    temp_fn.parameters = (*start)[3];
    temp_fn.body = (*start)[4];
    temp_fn.type = type;

    tempServiceDetails.functionPointers.push_back({temp_fn.function_pointer, temp_fn.return_type + "_" + temp_fn.function_pointer });
    functions.push_back(temp_fn);
}

string create_service_enum_values(string fileName, string serviceObjectName)
{
    if (functions.size() == 0)
        return NULL;
    else
    {
        char buffer[BUFFER_SIZE];
        sprintf(buffer, "\nenum %s_exposed_functions { ", serviceObjectName.c_str());

        string temp(buffer);

        for (int i = 0; i < functions.size() - 1; i++)
        {
            temp = temp + functions[i].return_type + "_" + functions[i].function_pointer + ", ";
        }

        temp = temp + functions[functions.size() - 1].return_type + "_" + functions[functions.size() - 1].function_pointer + " };\n";

        return temp;
    }
}

bool createLibraryFile(ostream &libraryFileStream)
{
    libraryFileStream << "\n#include \"service.h\"\n";
    libraryFileStream << "\n#include \"Kernel.h\"\n";
    for (auto serviceObject : serviceDetailsArr)
    {
        char buffer[BUFFER_SIZE];
        sprintf(buffer, "#include \"%s.h\"\n", serviceObject.fileName.c_str());
        libraryFileStream << buffer;
    }

    string serviceEnums = create_services_enum(libraryFileStream);
    libraryFileStream << serviceEnums << endl;

    libraryFileStream << "\nextern Kernel kernel;\n\n";

    for (auto serviceObject : serviceDetailsArr)
    {
        char buffer[BUFFER_SIZE];
        sprintf(buffer, "%s %s_object;\nService %sService;\n", serviceObject.serviceName.c_str(), serviceObject.serviceName.c_str(), serviceObject.serviceName.c_str());
        libraryFileStream << buffer;
    }

    libraryFileStream << "\nbool generateServices()\n{";

    int counter = 0;
    for (auto serviceObject : serviceDetailsArr)
    {
        char buffer[BUFFER_SIZE];
        sprintf(buffer, "\n\tkernel.void_pointers[%d] = (void*) &%s_object;\n", counter++, serviceObject.serviceName.c_str());
        libraryFileStream << buffer;

        sprintf(buffer, "\tservice_init(&%sService, (void *)&%s_object, %s_t);\n", serviceObject.serviceName.c_str(), serviceObject.serviceName.c_str(), serviceObject.serviceName.c_str());
        libraryFileStream << buffer;
        
        for (auto serviceFunction : serviceObject.functionPointers)
        {
            sprintf(buffer, "\t%sService.add_service(&%sService, %s, %s);\n", serviceObject.serviceName.c_str(), serviceObject.serviceName.c_str(), serviceFunction.first.c_str(), serviceFunction.second.c_str());
            libraryFileStream << buffer;
        }
        sprintf(buffer, "\tRegisterServiceToKernel(&kernel.service_transporter, &%sService, %s_t);\n", serviceObject.serviceName.c_str(), serviceObject.serviceName.c_str());
        
        libraryFileStream << buffer << "";
    }
    libraryFileStream << "}\n";

    return false;
}

string create_services_enum(ostream &libraryFileStream) //this function creates the service_t enum for all the serivces and adds it to library.h
{
    string temp;  //typedef enum user_services_enum_t {service1_t = 10, hamadaStruct_t};
    libraryFileStream << "typedef enum user_services_enum_t { ";

    for (int i = 0; i < serviceDetailsArr.size() - 1; i++)
        temp += serviceDetailsArr[i].serviceName + "_t" + (i==0? " = 10":" ") + ", ";

     temp += serviceDetailsArr[serviceDetailsArr.size() - 1].serviceName + "_t" + " } user_service_enum;\n";


    return temp;
}

bool generateLinkerFile()
{
    string part1 = "ENTRY(_start)\n \
\n\
SECTIONS\n \
{\n\
	.text BLOCK(4K) : ALIGN(4K)\n\
	{\
		KernelWrapper.o (.text) /* This needs to be the first file for\n\
                                            stage1 to be able to switch to\n\
                                            stage2\n */\
\tgdt.o (.text)\n\
\tidt.o (.text)\n\
\tisr.o (.text)\n\
\tpit.o (.text)\n\
\tPhysicalMemory.o (.text)\n\
\tMemoryUtilities_asm.o (.text)\n\
\tClockUtils.o (.text)\n\
\tMemoryUtilities.o (.text)\n\
\tmemcpy_asm.o (.text)\n\  
\tmemcpy.o (.text)\n\
\tmemswap_asm.o (.text)\n\    
\tchecksum_asm.o (.text)\n\
\tpci_device.o (.text)\n\
\tpci_service.o (.text)\n\    
\tmemswap.o (.text)\n\
\tipi.o (.text)\n\
\tTerminal.o (.text)\n\
\tConsole.o (.text)\n\
\tACPI.o (.text)\n\
\tAPIC.o (.text)\n\
\tATADisk.o (.text)\n\
\tATAManager.o (.text)\n\
\tTSSDescriptor.o (.text)\n\
\tGlobalDescriptorTableEntry.o (.text)\n\
\tGlobalDescriptorTable.o (.text)\n\
\tInterruptManager.o (.text)\n\
\tInterruptDescriptorTable.o (.text)\n\
\tisr_stub.o (.text)\n\
\tAPICManager.o (.text)\n\
\tPorts.o (.text)\n\
\tCoreUtils.o (.text)\n\
\tNetUtils.o (.text)\n\
\tMMIOUtils.o (.text)\n\
\tstdio.o (.text)\n\
\tprintk.o (.text)\n\
\tservice1_K.o (.text)\n\
\tchecksum.o (.text)\n\
\tMemoryPageMap.o (.text)\n\
\tSharedMemory.o (.text)\n\
\tBlockService.o (.text)\n\
\tService.o (.text)\n\
\tServiceTransport.o (.text)\n\
\tnumbers.o (.text)\n\
\trtl8139.o (.text)\n\
\tSpinlock.o (.text)\n\
\tKMalloc.o (.text)\n\
\tE1000.o (.text)\n\
\tARP.o (.text)\n\
\tIP.o (.text)\n\
\tEthernet.o (.text)\n\
\tNetworkDriver.o (.text)\n\
\tkeyboard.o (.text)\n\
\tvoid_ptr_stack.o (.text)\n\
\tXMLParser.o (.text)\n\
\tXQuery.o (.text)\n\
\tXMLService.o (.text)\n\
\
";


string part2 = "\n\
\tKernel.o (.text)\n\
\t\t. = ALIGN(4K);\n\
\t}\n\
\t\t.rodata BLOCK(4K) : ALIGN(4K)\n\
\t{\n\
\t*(.rodata)\n\
\t\t. = ALIGN(4K);\n\
\t}\n\
\t\t.data BLOCK(4K) : ALIGN(4K)\n\
\t{\n\
\t*(.data)\n\
\t\t. = ALIGN(4K);\n\
\t}\n\
\t\t.bss BLOCK(4K) : ALIGN(4K)\n\
\t{\n\
\t*(COMMON)\n\
\t*(.bss)\n\
\t*(.bootstrap_stack)\n\
\t\t. = ALIGN(4K);\n\
\t}\n\
\n\
\tend = .;\n\
}";

string middle;

for(int i=0; i<serviceDetailsArr.size(); i++)
{
    middle += "\t"+serviceDetailsArr[i].fileName +"_K.o (.text)\n";
    middle += "\t"+serviceDetailsArr[i].fileName +"_U.o (.text)\n";
}

ofstream linkerOutputStream("output/linker.ld");
linkerOutputStream << (part1 + middle + part2);
linkerOutputStream.close();

}