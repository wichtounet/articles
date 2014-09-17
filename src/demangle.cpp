//=======================================================================
// Copyright (c) 2014 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <memory>
#include <stdlib.h>

#include "demangle.hpp"

#ifdef __GNUC__

#include <cxxabi.h>

struct deleter_free {
    template<class T>
    void operator()(T *p) const {
        free(p);
    }
};

std::string demangle(const char *name){
    int status = 0;
    std::unique_ptr<char, deleter_free> demName(abi::__cxa_demangle(name, nullptr, nullptr, &status));
    return (status==0) ? demName.get() : name;
}

#else

std::string demangle(const char *name){
    return name;
}

#endif
