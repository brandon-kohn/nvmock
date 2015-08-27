//
//! Copyright © 2015
//! Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NVM_MOCKABLE_HPP
#define NVM_MOCKABLE_HPP
#pragma once

#include "member_function_traits.hpp"
#include "mock_function_factory.hpp"

#include <boost/function.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/empty.hpp>
#include <boost/type_traits.hpp>

namespace nvm
{
    /////////////////////////////////////////////////////////////////////////////
    //
    //! \class mockable
    //! \brief Inherit from this class to allow non-virtual member functions to be mockable.
    //! This type can be used as a base type for classes and structs which have non-virtual
    //! member functions which need to be mocked. See LegionUtility/Test/TestRunnerHook.cpp
    //! for the example usage.
    class mockable
    {
    public:
        mockable(){}
        virtual ~mockable(){}

        virtual bool is_mocked() const { return false; }
        virtual boost::shared_ptr<boost::function_base> get_mock_mem_fn(const std::string& signature) const { return boost::shared_ptr<boost::function_base>(); }
    };

    template <typename MFN>
    inline std::string get_mock_mem_fn_key(MFN, const std::string& methodName)
    {
        return methodName + typeid(MFN).name();
    }
    
}//! namespace nvm;

#if !defined(NVM_NO_NONVIRTUAL_MOCK_INTERCEPT)
    //! \def NVM_MOCK_NONVIRTUAL_INTERCEPT( Method, ... )
    //! \brief Macro to implement a non-virtual mock function intercept.
    //!
    //! Preconditions:
    //! Method is a fully qualified member function name (i.e. MyClass::MemberFunction).
    //! ... (variadic preprocessor args) are the parameter names of the arguments passed into the
    //!     member function.
    //!
    //! Example usage:
    //! \code
    //! bool MyClass::MemberFunction(int a, double b)
    //! {
    //!     NVM_MOCK_NONVIRTUAL_INTERCEPT(MyClass::MemberFunction, a, b);
    //!     return a < b;
    //! }
    //! \endcode
    #define NVM_MOCK_NONVIRTUAL_INTERCEPT(Method, ...)                                   \
        if( is_mocked() )                                                                \
        {                                                                                \
            using namespace nvm;                                                         \
            typedef signature_of_mem_fn<BOOST_TYPEOF(&Method)>::type sig_type;           \
            boost::shared_ptr< boost::function<sig_type> > pMockFn =                     \
            boost::static_pointer_cast< boost::function<sig_type> >                      \
            (get_mock_mem_fn(get_mock_mem_fn_key(&Method                                 \
              , BOOST_PP_STRINGIZE(Method))));                                           \
            if (pMockFn)                                                                 \
                return (*pMockFn)(__VA_ARGS__);                                          \
        }                                                                                \
    /***/
    //! \def NVM_MOCK_NONVIRTUAL_INTERCEPT_SIG( Method, Signature, ... )
    //! \brief Macro to implement a non-virtual mock function intercept.
    //! This signature version can be used in cases when the boost::function facility
    //! fails to deduce the signature automatically.
    //! Preconditions:
    //! Method is a fully qualified member function name (i.e. MyClass::MemberFunction).
    //! Signature is the signature of the member function.
    //! ... (variadic preprocessor args) are the parameter names of the arguments passed into the
    //!     member function.    
    //!
    //! Example usage:
    //! \code
    //! bool MyClass::MemberFunction(int a, double b)
    //! {
    //!     NVM_MOCK_NONVIRTUAL_INTERCEPT_SIG(MyClass::MemberFunction, bool(int, double), a, b);
    //!     return a < b;
    //! }
    //! \endcode
    #define NVM_MOCK_NONVIRTUAL_INTERCEPT_SIG(Method, Signature, ...)                    \
        if( is_mocked() )                                                                \
        {                                                                                \
            using namespace nvm;                                                         \
            boost::shared_ptr< boost::function<Signature> > pMockFn =                    \
                boost::static_pointer_cast< boost::function<Signature> >                 \
                (get_mock_mem_fn(get_mock_mem_fn_key(&Method                             \
                  , BOOST_PP_STRINGIZE(Method))));                                       \
            if (pMockFn)                                                                 \
                return (*pMockFn)(__VA_ARGS__);                                          \
        }                                                                                \
    /***/
    //! \def NVM_MOCK_NONVIRTUAL_OVERLOAD_INTERCEPT( Type, Method, Signature, ... )
    //! \brief Macro to implement a non-virtual mock function intercept for overloaded non-const member functions.
    //!
    //! Preconditions:
    //! Type is the class name.
    //! Method is the unqualified member function name (i.e. MemberFunction).
    //! Signature is the signature of the member function.
    //! ... (variadic preprocessor args) are the parameter names of the arguments passed into the
    //!     member function.
    //!
    //! Example usage:
    //! \code
    //! bool MyClass::MemberFunction(int a, double b)
    //! {
    //!     NVM_MOCK_NONVIRTUAL_OVERLOAD_INTERCEPT(MyClass, MemberFunction, bool(int, double), a, b);
    //!     return a < b;
    //! }
    //! \endcode
    #define NVM_MOCK_NONVIRTUAL_OVERLOAD_INTERCEPT(T, Method, Sig, ...)                  \
        if (is_mocked())                                                                 \
        {                                                                                \
            using namespace nvm;                                                         \
            boost::shared_ptr< boost::function<Sig> > pMockFn =                          \
                boost::static_pointer_cast<boost::function<Sig> >                        \
                (                                                                        \
                    get_mock_mem_fn                                                      \
                    (                                                                    \
                        get_mock_mem_fn_key                                              \
                        (                                                                \
                            mem_fn_ptr_gen<Sig>::template apply<T>::type()               \
                          , BOOST_PP_STRINGIZE(BOOST_PP_CAT(T, BOOST_PP_CAT(::, Method)))\
                        )                                                                \
                    )                                                                    \
                );                                                                       \
            if (pMockFn)                                                                 \
                return (*pMockFn)(__VA_ARGS__);                                          \
        }                                                                                \
    /***/
    //! \def NVM_MOCK_NONVIRTUAL_OVERLOAD_CONST_INTERCEPT( Type, Method, Signature, ... )
    //! \brief Macro to implement a non-virtual mock function intercept for overloaded const member functions.
    //!
    //! Preconditions:
    //! Type is the class name.
    //! Method is the unqualified member function name (i.e. MemberFunction).
    //! Signature is the signature of the member function.
    //! ... (variadic preprocessor args) are the parameter names of the arguments passed into the
    //!     member function.
    //!
    //! Example usage:
    //! \code
    //! bool MyClass::MemberFunction(int a, double b) const
    //! {
    //!     NVM_MOCK_NONVIRTUAL_OVERLOAD_CONST_INTERCEPT(MyClass, MemberFunction, bool(int, double), a, b);
    //!     return a < b;
    //! }
    //! \endcode
    #define NVM_MOCK_NONVIRTUAL_OVERLOAD_CONST_INTERCEPT(T, Method, Sig, ...)            \
        if (is_mocked())                                                                 \
        {                                                                                \
            using namespace nvm;                                                         \
            boost::shared_ptr< boost::function<Sig> > pMockFn =                          \
                boost::static_pointer_cast<boost::function<Sig> >                        \
                (                                                                        \
                    get_mock_mem_fn                                                      \
                    (                                                                    \
                        get_mock_mem_fn_key                                              \
                        (                                                                \
                            mem_fn_ptr_gen<Sig>::template apply<T>::const_type()         \
                          , BOOST_PP_STRINGIZE(BOOST_PP_CAT(T, BOOST_PP_CAT(::, Method)))\
                        )                                                                \
                    )                                                                    \
                );                                                                       \
            if (pMockFn)                                                                 \
                return (*pMockFn)(__VA_ARGS__);                                          \
        }                                                                                \
    /***/

    //! \def NVM_IMPLEMENT_MOCKABLE
    //! \brief This macro can be used instead of inheriting from mockable to provide a non-virtual mechanism for checking if a type is mocked.
    //! Simply place this macro in the class definition instead of inheriting from mockable.
    //! NOTE: get_mock_mem_fn is a virtual member function and so adding this macro will
    //! render a type as polymorphic. This will break POD-ness.
    //! Example usage:
    //! \code
    //! class MyClass
    //! {
    //! public:
    //!     MyClass();
    //!     ~MyClass();
    //! protected:
    //!     NVM_IMPLEMENT_MOCKABLE();
    //! };
    //! \endcode
    #define NVM_IMPLEMENT_MOCKABLE()                                           \
        typedef void ImplementsMockable;                                       \
        struct mockable                                                        \
        {                                                                      \
            mockable()                                                         \
            : is_mocked(false)                                                 \
            {}                                                                 \
            bool is_mocked;                                                    \
        } m_mockState;                                                         \
        bool is_mocked() const                                                 \
        {                                                                      \
            return m_mockState.is_mocked;                                      \
        }                                                                      \
        void set_is_mocked(bool v)                                             \
        {                                                                      \
            m_mockState.is_mocked = v;                                         \
        }                                                                      \
        virtual boost::shared_ptr<boost::function_base> get_mock_mem_fn        \
        (const std::string& signature) const                                   \
        { return boost::shared_ptr<boost::function_base>(); }                  \
    /***/
#else
    #define NVM_MOCK_NONVIRTUAL_INTERCEPT(Method, Signature, ...)
    #define NVM_MOCK_NONVIRTUAL_INTERCEPT_SIG(Method, Signature, ...)  
    #define NVM_MOCK_NONVIRTUAL_OVERLOAD_INTERCEPT(T, Method, Sig, ...)  
    #define NVM_MOCK_NONVIRTUAL_OVERLOAD_CONST_INTERCEPT(T, Method, Sig, ...) 
    #define NVM_IMPLEMENT_MOCKABLE()
#endif

#endif // NVM_MOCKABLE_HPP
