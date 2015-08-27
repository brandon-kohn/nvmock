//
//! Copyright © 2015
//! Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#include "mock_base.hpp"

namespace nvm
{
    //! Static instance of mock registered non-virtual functions.
    mock_base::mocker_map mock_base::s_mockers;
}//! namespace nvm;
