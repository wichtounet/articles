//=======================================================================
// Copyright (c) 2014 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

namespace po = boost::program_options;

int main(int argc, const char* argv[]){
    po::options_description description("MyTool Usage");

    description.add_options()
        ("help,h", "Display this help message")
        ("compression,c", po::value<int>()->default_value(5)->implicit_value(10),"Compression level")
        ("input-files", po::value<std::vector<std::string>>(), "Input files")
        ("version,v", "Display the version number");

    po::positional_options_description p;
    p.add("input-files", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(description).positional(p).run(), vm);
    po::notify(vm);

    if(vm.count("help")){
        std::cout << description;

        return 0;
    }

    if(vm.count("version")){
        std::cout << "MyTool Version 1.0" << std::endl;

        return 0;
    }

    if(vm.count("compression")){
        std::cout << "Compression level " << vm["compression"].as<int>() << std::endl;
    }

    if(vm.count("input-files")){
        std::vector<std::string> files = vm["input-files"].as<std::vector<std::string>>();
        for(std::string file : files){
            std::cout << "Input file " << file << std::endl;
        }
    }

    return 0;
}
