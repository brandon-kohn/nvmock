//
//! Copyright © 2015
//! Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#include <nvmock/mock.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

//! NVM works by instrumenting the types which are to be mocked. The instrumentation consists of two parts:
//! 1) a mechanism which communicates when an instance of the type is a mock.
//! 2) a system for redirecting calls to member functions into their mock equivalents.
//!
//! This instrumentation can be accomplished either via inheriting nvm::mockable publicly or by placing 
//! NVM_IMPLEMENTS_MOCKABLE() at the public scope of the type.
//! For simple types where no ancestor type is mockable, NVM_IMPLEMENTS_MOCKABLE() is probably the preferred strategy.
//! If the type has a complex ancestry where ancestor types are mockable in their own right, inheritance
//! of nvm::mockable is probably preferred.
//! 
class SomeTypeInheritsMockable : public virtual nvm::mockable //! Made virtual in case a type inherits from multiple mockable types.
{
public:
    
    //! Here is a simple example of a mock intercept. In general this case covers
    //! member functions which are not overloaded.
    void SomeMethod(int a, double b, float c)
    {
        NVM_MOCK_INTERCEPT(SomeTypeInheritsMockable::SomeMethod, a, b, c);
    }

    //! A version which returns a value.
    int SomeMethodWhichReturns()
    {
        NVM_MOCK_INTERCEPT(SomeTypeInheritsMockable::SomeMethodWhichReturns);
        return -1;
    }

    //! Here is the syntax for overloaded methods. In these cases the signature is required as an additional parameter.
    void SomeOverloadedMethod(int a, double b, float c) const
    {
        //! The const version of the macro is used when the member function is const.
        NVM_MOCK_OVERLOAD_CONST_INTERCEPT(SomeTypeInheritsMockable, SomeOverloadedMethod, void(int, double, float), a, b, c);
    }

    //! Another overload.
    int SomeOverloadedMethod(char a)
    {
        //! This one isn't const, so the non-const macro is used.
        NVM_MOCK_OVERLOAD_INTERCEPT(SomeTypeInheritsMockable, SomeOverloadedMethod, int(char), a);
        return -1;
    }
};

//! Next the mock type is defined. Google mock is used in this case, but I'm sure other mock libraries could be used instead.
//! The mock type must inherit from nvm::mock<T> where T is the type which is being mocked. 
struct MockSomeTypeInheritsMockable : nvm::mock < SomeTypeInheritsMockable >
{
    MockSomeTypeInheritsMockable()
    {
        //! These should only be run one time.
        NVM_ONCE_BLOCK()
        {
            //! Register each member function to be redirected to the mocked version.
            NVM_REGISTER_MOCK_MEMBER_FUNCTION(SomeTypeInheritsMockable, MockSomeTypeInheritsMockable, SomeMethod);
            NVM_REGISTER_MOCK_MEMBER_FUNCTION(SomeTypeInheritsMockable, MockSomeTypeInheritsMockable, SomeMethodWhichReturns);

            //! Overloaded member functions require a special macro.
            NVM_REGISTER_MOCK_OVERLOADED_CONST_MEMBER_FUNCTION(SomeTypeInheritsMockable, MockSomeTypeInheritsMockable, SomeOverloadedMethod, void(int, double, float));
            NVM_REGISTER_MOCK_OVERLOADED_MEMBER_FUNCTION(SomeTypeInheritsMockable, MockSomeTypeInheritsMockable, SomeOverloadedMethod, int(char));
        }
    }

    MOCK_CONST_METHOD3(SomeMethod, void(int a, double b, float c));
    MOCK_METHOD0(SomeMethodWhichReturns, int());
    MOCK_CONST_METHOD3(SomeOverloadedMethod, void(int, double, float));
    MOCK_METHOD1(SomeOverloadedMethod, int(char));
};

//! Test the above mock registrations.
TEST(simple_inherits_mockable_test_suite, test_registrations)
{
    using namespace ::testing;    

    MockSomeTypeInheritsMockable mst;
    SomeTypeInheritsMockable& sut = mst;

    EXPECT_CALL(mst, SomeMethod(_, _, _));    
    sut.SomeMethod(10, 20., 30.f);

    EXPECT_CALL(mst, SomeMethodWhichReturns()).WillOnce(Return(10));
    EXPECT_EQ(10, sut.SomeMethodWhichReturns());

    EXPECT_CALL(mst, SomeOverloadedMethod(_, _, _));
    sut.SomeOverloadedMethod(10, 20., 30.f);

    EXPECT_CALL(mst, SomeOverloadedMethod(_)).WillOnce(Return(10));
    EXPECT_EQ(10, sut.SomeOverloadedMethod('a'));
}

//! Entry point for tests.
int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
