//
//! Copyright © 2015
//! Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NVM_MOCKBASE_HPP
#define NVM_MOCKBASE_HPP
#pragma once

#include "mockable.hpp"
#include <boost/function_types/function_arity.hpp>
#include <boost/container/flat_map.hpp>

namespace boost { class function_base; }

namespace nvm
{
    struct mock_base
    {
        struct mocker
        {
            virtual boost::shared_ptr<boost::function_base> operator()(void* pThis) const = 0;
        };
    private:

        typedef boost::container::flat_map < std::string, boost::shared_ptr<mocker> > mocker_map;
        
        template <typename T, typename Original, typename Mocked>
        struct mocker_impl : mocker
        {
            mocker_impl(Original o, Mocked m)
                : o(o)
                , m(m)
            {}

            Original o;
            Mocked m;

            boost::shared_ptr<boost::function_base> operator()(void* pThis) const
            {
                return nvm::mock_base::make_mocked(o, m, static_cast<T*>(pThis));
            }
        };

		static mocker_map& get_mocker_map_instance()
        {
			//! FIXME:? Not thread safe... may bloat as well due to multiple modules.
			static mocker_map s_mockers;
			return s_mockers;
		}

    protected:

        static boost::shared_ptr<mocker> get_mocker(const std::string& signature)
        {
			mocker_map& s_mockers = get_mocker_map_instance();
            mocker_map::const_iterator it(s_mockers.find(signature));
            if (it == s_mockers.end())
                return boost::shared_ptr<mocker>();
            else
                return it->second;
        }

        template <typename T, typename OriginalMFN, typename MockMFN>
        static void register_mocker(OriginalMFN o, MockMFN m, const std::string& mfName)
        {
			mocker_map& s_mockers = get_mocker_map_instance();
            s_mockers[get_mock_mem_fn_key(o, mfName)] = boost::make_shared< mocker_impl<T, OriginalMFN, MockMFN> >(o, m);
        }

        template <typename OriginalMFN, typename MockMFN, typename T>
        static boost::shared_ptr<boost::function_base> make_mocked(OriginalMFN opMFN, MockMFN mpMFN, T* pThis)
        {
            return mock_function_factory< boost::function_types::function_arity<OriginalMFN>::value >::create(opMFN, mpMFN, pThis);
        }
    };

}//! namespace nvm;

#define NVM_REGISTER_NONVIRTUAL_MOCK_MEMBER_FUNCTION(OriginalType, MockType, MemberFn) \
    register_mocker<MockType>(&OriginalType::MemberFn, &MockType::MemberFn, BOOST_PP_STRINGIZE(OriginalType::MemberFn))\
/***/

//! \def NVM_REGISTER_NONVIRTUAL_MOCK_OVERLOADED_MEMBER_FUNCTION
//! \brief This macros is to be used in the case of overloaded member functions.
//! Example usage:
//! \code
//! NVM_REGISTER_NONVIRTUAL_MOCK_OVERLOADED_MEMBER_FUNCTION(A, MockA, OverloadedFn, void(int, double));
//! NVM_REGISTER_NONVIRTUAL_MOCK_OVERLOADED_MEMBER_FUNCTION(A, MockA, OverloadedFn, bool(char, double));
//! \endcode
#define NVM_REGISTER_NONVIRTUAL_MOCK_OVERLOADED_MEMBER_FUNCTION(OriginalType, MockType, MemberFn, Signature)                    \
    register_mocker<MockType>                                                                                                   \
    (                                                                                                                           \
        static_cast<nvm::mem_fn_ptr_gen<Signature>::template apply<OriginalType>::type>(&OriginalType::MemberFn)                \
      , static_cast<nvm::mem_fn_ptr_gen<Signature>::template apply<MockType>::type>(&MockType::MemberFn)                        \
      , BOOST_PP_STRINGIZE(OriginalType::MemberFn)                                                                              \
    )                                                                                                                           \
/***/

//! \def NVM_REGISTER_NONVIRTUAL_MOCK_OVERLOADED_MEMBER_FUNCTION
//! \brief This macros is to be used in the case of overloaded member functions.
//! Example usage:
//! \code
//! NVM_REGISTER_NONVIRTUAL_MOCK_OVERLOADED_MEMBER_FUNCTION(A, MockA, OverloadedFn, void(int, double));
//! NVM_REGISTER_NONVIRTUAL_MOCK_OVERLOADED_MEMBER_FUNCTION(A, MockA, OverloadedFn, bool(char, double));
//! \endcode
#define NVM_REGISTER_NONVIRTUAL_MOCK_OVERLOADED_CONST_MEMBER_FUNCTION(OriginalType, MockType, MemberFn, Signature)              \
    register_mocker<MockType>                                                                                                   \
    (                                                                                                                           \
        static_cast<nvm::mem_fn_ptr_gen<Signature>::template apply<OriginalType>::const_type>(&OriginalType::MemberFn)          \
      , static_cast<nvm::mem_fn_ptr_gen<Signature>::template apply<MockType>::const_type>(&MockType::MemberFn)                  \
      , BOOST_PP_STRINGIZE(OriginalType::MemberFn)                                                                              \
    )                                                                                                                           \
/***/

#endif // NVM_MOCKBASE_HPP
