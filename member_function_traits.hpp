//
//! Copyright © 2015
//! Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NVM_MEMBERFUNCTIONTRAITS_HPP
#define NVM_MEMBERFUNCTIONTRAITS_HPP
#pragma once

#include "detail/member_function_traits.hpp"
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/function_types/is_member_function_pointer.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/typeof/typeof.hpp>

template <typename F>
struct signature_of_mem_fn
{
    typedef typename boost::function_types::result_type<F>::type result_type;
    typedef typename boost::function_types::parameter_types<F>::type parameter_types;
    typedef typename boost::mpl::pop_front<parameter_types>::type base;
    typedef typename boost::mpl::push_front<base, result_type>::type L;
    typedef typename boost::function_types::function_type<L>::type type;
};

#endif // NVM_MEMBERFUNCTIONTRAITS_HPP
