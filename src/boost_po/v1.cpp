#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

namespace po = boost::program_options;

int main(int argc, const char* argv[]){
    po::options_description description("MyTool Usage");

    description.add_options()
        ("help", "Display this help message")
        ("version", "Display the version number");

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
    po::notify(vm);

    if(vm.count("help")){
        std::cout << description;
    }

    return 0;
}
