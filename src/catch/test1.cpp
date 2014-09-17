//=======================================================================
// Copyright (c) 2014 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE( "stupid/1=2", "Prove that one equals 2" ){
    int one = 1;
    REQUIRE( one == 2 );
}