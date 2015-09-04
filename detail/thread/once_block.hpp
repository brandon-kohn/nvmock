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

#include <boost/preprocessor/cat.hpp>

#ifndef NVM_NO_CXX11_THREAD_SAFE_STATIC_LOCAL_VARIABLES
#include <atomic>
//! \def NVM_ONCE_BLOCK()
//! \brief This macro can be used to define the entry to a block which will only be run once even in the context of multiple threads.
#define NVM_ONCE_BLOCK()                                                             \
    static std::atomic<bool> BOOST_PP_CAT(nvm_once_block_sentinel,__LINE__)(false);  \
    bool BOOST_PP_CAT(nvm_once_block_expected,__LINE__) = false;                     \
    if(BOOST_PP_CAT(nvm_once_block_sentinel,__LINE__).compare_exchange_strong(       \
                                   BOOST_PP_CAT(nvm_once_block_expected,__LINE__)    \
                                 , true, std::memory_order_seq_cst))                 \
/***/

#else
#include <boost/atomic.hpp>
#include <boost/thread/once.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/ref.hpp>

#define NVM_STATIC_ATOMIC_BOOL(name)                                                                             \
    struct BOOST_PP_CAT(nvm_local_static, __LINE__)                                                              \
    {                                                                                                            \
        static void nvm_create_instance(boost::atomic<bool>*& value) { value = new boost::atomic<bool>(false); };\
        static boost::atomic<bool>& get()                                                                        \
        {                                                                                                        \
            static boost::once_flag once = BOOST_ONCE_INIT;                                                      \
            static boost::atomic<bool>* value = 0;                                                               \
            boost::call_once(once, nvm_create_instance, boost::ref(value));                                      \
            return *value;                                                                                       \
        }                                                                                                        \
    };                                                                                                           \
    boost::atomic<bool>& name = BOOST_PP_CAT(nvm_local_static, __LINE__)::get(); \
/***/

//! \def NVM_ONCE_BLOCK()
//! \brief This macro can be used to define the entry to a block which will only be run once even in the context of multiple threads.
#define NVM_ONCE_BLOCK()                                                                              \
    NVM_STATIC_ATOMIC_BOOL(BOOST_PP_CAT(nvm_once_block_sentinel, __LINE__));                          \
    bool BOOST_PP_CAT(nvm_once_block_expected,__LINE__) = false;                                      \
    if (BOOST_PP_CAT(nvm_once_block_sentinel, __LINE__).compare_exchange_strong(                      \
        BOOST_PP_CAT(nvm_once_block_expected, __LINE__)                                               \
        , true, boost::memory_order_seq_cst))                                                         \
/***/

#endif

#endif // NVM_THREAD_ONCEBLOCK_HPP
