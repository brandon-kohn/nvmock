//
//! Copyright © 2015
//! Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NVM_MOCK_HPP
#defineNVM_MOCK_HPP
#pragma once

#include "mock_base.hpp"
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/iteration/local.hpp>

#if !defined(NVM_MAX_MOCK_PARAMS)
    #define NVM_MAX_MOCK_PARAMS 10
#endif

/////////////////////////////////////////////////////////////////////////////
//
// FORWARD DECLARATION
//
/////////////////////////////////////////////////////////////////////////////
namespace nvm
{
    template <typename T, typename EnableIf = void>
    class mock;

    template <typename T>
    class mock< T, typename boost::enable_if_c<boost::is_base_and_derived<mockable, T>::value>::type >
        : public T, public mock_base
    {
    public:

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)
        template <typename... Args>
        mock(Args... args)
            : T(args...)
        {}
#else
        mock()
        {}

        #define BOOST_PP_LOCAL_MACRO(n)                   \
        template <BOOST_PP_ENUM_PARAMS(n, typename A)>    \
        mock(BOOST_PP_ENUM_BINARY_PARAMS(n, const A, &a)) \
        : T(BOOST_PP_ENUM_PARAMS(n,a))                    \
        {}                                                \
        /***/

        #define BOOST_PP_LOCAL_LIMITS (1, NVM_MAX_MOCK_PARAMS)
        #include BOOST_PP_LOCAL_ITERATE()            
#endif//Use old preprocessor if no variadic templates.
            
        virtual ~mock(){}

        virtual boost::shared_ptr<boost::function_base> get_mocked_mem_fn(const std::string& signature) const
        {
            boost::shared_ptr<mocker> pMocker = mock_base::get_mocker(signature);
            if (!pMocker)
                return boost::shared_ptr<boost::function_base>();
            else
                return pMocker->operator()((void*)this);
        }
    };

    template <typename T>
    class mock< T, typename T::ImplementsMockable >
        : public T, public mock_base
    {
    public:

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)
        template <typename... Args>
        mock(Args... args)
            : T(args...)
        {
            T::set_is_mocked(true);
        }
#else
        mock()
        {
            T::set_is_mocked(true);
        }

        #define BOOST_PP_LOCAL_MACRO(n)                   \
        template <BOOST_PP_ENUM_PARAMS(n, typename A)>    \
        mock(BOOST_PP_ENUM_BINARY_PARAMS(n, const A, &a)) \
        : T(BOOST_PP_ENUM_PARAMS(n,a))                    \
        {                                                 \
            T::set_is_mocked(true);                       \
        }                                                 \
        /***/

        #define BOOST_PP_LOCAL_LIMITS (1, NVM_MAX_MOCK_PARAMS)
        #include BOOST_PP_LOCAL_ITERATE()            
#endif//Use old preprocessor if no variadic templates.

        virtual ~mock(){}

        virtual boost::shared_ptr<boost::function_base> get_mocked_mem_fn(const std::string& signature) const
        {
            boost::shared_ptr<mocker> pMocker = mock_base::get_mocker(signature);
            if (!pMocker)
                return boost::shared_ptr<boost::function_base>();
            else
                return pMocker->operator()((void*)this);
        }
    };

}//! namespace nvm;

#endif // NVM_MOCK_HPP
