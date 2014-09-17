//=======================================================================
// Copyright (c) 2014 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#ifndef ARTICLES_GRAPHS
#define ARTICLES_GRAPHS

#include <memory>
#include <string>
#include <vector>

namespace graphs {

struct result {
    std::string serie;
    std::string group;
    std::size_t value;
};

struct graph {
    std::string name;
    std::string title;
    std::string unit;
    std::vector<result> results;

    graph(const std::string& name, const std::string& title, const std::string& unit) : name(name), title(title), unit(unit) {}
};

enum class Output : unsigned int {
    GOOGLE,
    PLUGIN
};

void new_graph(const std::string& graph_name, const std::string& graph_title, const std::string& unit);
void new_result(const std::string& serie, const std::string& group, std::size_t value);
void output(Output output);

}

#endif
