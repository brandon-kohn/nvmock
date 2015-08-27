//
//! Copyright © 2015
//! Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NVM_THREAD_ONCEBLOCK_HPP
#define NVM_THREAD_ONCEBLOCK_HPP
#pragma once

#include <boost/atomic.hpp>
#include <boost/preprocessor/cat.hpp>

//! \def NVM_ONCE_BLOCK()
//! \brief This macro can be used to define the entry to a block which will only be run once even in the context of multiple threads.
#define NVM_ONCE_BLOCK()                                                             \
    static boost::atomic<bool> BOOST_PP_CAT(nvm_once_block_sentinel,__LINE__)(false);\
    bool BOOST_PP_CAT(nvm_once_block_expected,__LINE__) = false;                     \
    if(BOOST_PP_CAT(nvm_once_block_sentinel,__LINE__).compare_exchange_strong(       \
                                   BOOST_PP_CAT(nvm_once_block_expected,__LINE__)    \
                                 , true, boost::memory_order_seq_cst))               \
/***/

#endif // NVM_THREAD_ONCEBLOCK_HPP
