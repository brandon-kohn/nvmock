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

namespace
{
    struct SomeTypeInheritsMockable : virtual nvm::mockable
    {
        void SomeMethod(int a, double b, float c) const
        {
            NVM_MOCK_INTERCEPT(SomeTypeInheritsMockable::SomeMethod, a, b, c);
        }

        int SomeMethod2()
        {
            NVM_MOCK_INTERCEPT(SomeTypeInheritsMockable::SomeMethod2);
            return -1;
        }

        int SomeMethod3()
        {
            NVM_MOCK_INTERCEPT(SomeTypeInheritsMockable::SomeMethod3);
            return -2;
        }
    };

    struct MockSomeTypeInheritsMockable : nvm::mock < SomeTypeInheritsMockable >
    {
        MockSomeTypeInheritsMockable()
            : IsMockedCalled(false)
        {
            NVM_ONCE_BLOCK()
            {
                NVM_REGISTER_MOCK_MEMBER_FUNCTION(SomeTypeInheritsMockable, MockSomeTypeInheritsMockable, SomeMethod);
                NVM_REGISTER_MOCK_MEMBER_FUNCTION(SomeTypeInheritsMockable, MockSomeTypeInheritsMockable, SomeMethod2);                
                NVM_REGISTER_MOCK_MEMBER_FUNCTION(SomeTypeInheritsMockable, MockSomeTypeInheritsMockable, SomeMethod3);
            }
        }

        MOCK_CONST_METHOD3(SomeMethod, void(int a, double b, float c));
        MOCK_METHOD0(SomeMethod2, int());
        MOCK_METHOD0(SomeMethod3, int());

        virtual bool is_mocked() const
        {
            IsMockedCalled = true;
            return true;
        }

        mutable bool IsMockedCalled;
    };

    void CallSomeMethod(SomeTypeInheritsMockable& t)
    {
        t.SomeMethod(1, 2, 3);
    }

    int CallSomeMethod2(SomeTypeInheritsMockable& t)
    {
        return t.SomeMethod2();
    }

    int CallSomeMethod3(SomeTypeInheritsMockable& t)
    {
        return t.SomeMethod3();
    }

    TEST(mockTests, TestNonVirtualMethodmockInterceptionInherited)
    {
        using namespace ::testing;
        MockSomeTypeInheritsMockable mst;
		EXPECT_CALL(mst, SomeMethod(_,_,_));
        CallSomeMethod(mst);
        EXPECT_TRUE(mst.IsMockedCalled);
        ON_CALL(mst, SomeMethod2()).WillByDefault(Return(42));
        EXPECT_CALL(mst, SomeMethod2()).Times(1);
        EXPECT_EQ(42, CallSomeMethod2(mst));

        //! Methods with the same signature but different names should work.
        EXPECT_CALL(mst, SomeMethod3()).WillRepeatedly(Return(24));
        EXPECT_EQ(CallSomeMethod3(mst), 24);
    }

    //! This type implements mockable without using inheritance or virtuals. Best used
    //! in cases where no base type may be mockable.
    struct SomeTypeImplementsMockable
    {
        NVM_IMPLEMENT_MOCKABLE();

        void SomeMethod(int a, double b, float c)
        {
            NVM_MOCK_INTERCEPT(SomeTypeImplementsMockable::SomeMethod, a, b, c);
        }

        int SomeMethod2()
        {
            NVM_MOCK_INTERCEPT_SIG(SomeTypeImplementsMockable::SomeMethod2, int());
            return -1;
        }
    };

    struct MockSomeTypeImplementsMockable : nvm::mock < SomeTypeImplementsMockable >
    {
        MockSomeTypeImplementsMockable()
            : IsMockedCalled(false)
        {        
            NVM_ONCE_BLOCK()
            {
                NVM_REGISTER_MOCK_MEMBER_FUNCTION(SomeTypeImplementsMockable, MockSomeTypeImplementsMockable, SomeMethod);
                NVM_REGISTER_MOCK_MEMBER_FUNCTION(SomeTypeImplementsMockable, MockSomeTypeImplementsMockable, SomeMethod2);
            }
        }

        MOCK_CONST_METHOD3(SomeMethod, void(int a, double b, float c));
        MOCK_METHOD0(SomeMethod2, int());

        virtual bool is_mocked() const
        {
            IsMockedCalled = true;
            return true;
        }

        mutable bool IsMockedCalled;
    };

    void CallSomeMethod(SomeTypeImplementsMockable& t)
    {
        t.SomeMethod(1, 2, 3);
    }

    int CallSomeMethod2(SomeTypeImplementsMockable& t)
    {
        return t.SomeMethod2();
    }

    TEST(mockTests, TestNonVirtualMethodmockInterceptionImplemented)
    {
        using namespace ::testing;
        MockSomeTypeImplementsMockable mst;
		EXPECT_CALL(mst, SomeMethod(_,_,_));
        CallSomeMethod(mst);
        EXPECT_FALSE(mst.IsMockedCalled);
        ON_CALL(mst, SomeMethod2()).WillByDefault(Return(42));
        EXPECT_CALL(mst, SomeMethod2());
        EXPECT_EQ(CallSomeMethod2(mst), 42);
    }

    //! Test mocking object which inherits from mockable multiple times.
    struct AnotherTypeInheritsMockable : SomeTypeInheritsMockable, virtual nvm::mockable
    {
        void SomeMethod(int a, double b, float c) const
        {
            NVM_MOCK_INTERCEPT(AnotherTypeInheritsMockable::SomeMethod, a, b, c);
        }

        int SomeMethod2()
        {
            NVM_MOCK_INTERCEPT(AnotherTypeInheritsMockable::SomeMethod2);
            return -1;
        }
    };

    struct MockAnotherTypeInheritsMockable : nvm::mock < AnotherTypeInheritsMockable >
    {
        MockAnotherTypeInheritsMockable()
            : IsMockedCalled(false)
        {
            NVM_ONCE_BLOCK()
            {
                NVM_REGISTER_MOCK_MEMBER_FUNCTION(AnotherTypeInheritsMockable, MockAnotherTypeInheritsMockable, SomeMethod);
                NVM_REGISTER_MOCK_MEMBER_FUNCTION(AnotherTypeInheritsMockable, MockAnotherTypeInheritsMockable, SomeMethod2);
            }
        }

        MOCK_CONST_METHOD3(SomeMethod, void(int a, double b, float c));
        MOCK_METHOD0(SomeMethod2, int());

        virtual bool is_mocked() const
        {
            IsMockedCalled = true;
            return true;
        }

        mutable bool MockMethodCalled;
        mutable bool IsMockedCalled;
    };

    void CallSomeMethod(AnotherTypeInheritsMockable& t)
    {
        t.SomeMethod(1, 2, 3);
    }

    int CallSomeMethod2(AnotherTypeInheritsMockable& t)
    {
        return t.SomeMethod2();
    }
    
    TEST(mockTests, TestMockAnotherTypeInheritsMockable)
    {
        using namespace ::testing;
        MockAnotherTypeInheritsMockable mst;
		EXPECT_CALL(mst, SomeMethod(_,_,_));
        CallSomeMethod(mst);
        EXPECT_TRUE(mst.IsMockedCalled);
        ON_CALL(mst, SomeMethod2()).WillByDefault(Return(42));
        EXPECT_CALL(mst, SomeMethod2()).Times(1);
        EXPECT_EQ(CallSomeMethod2(mst), 42);
    }

    //! Test inheritence when two instance implement mockable rather than inherit.
    struct AnotherTypeImplementsMockable : SomeTypeImplementsMockable
    {
        NVM_IMPLEMENT_MOCKABLE();

        void SomeMethod(int a, double b, float c)
        {
            NVM_MOCK_INTERCEPT(AnotherTypeImplementsMockable::SomeMethod, a, b, c);
        }

        int SomeMethod2()
        {
            NVM_MOCK_INTERCEPT_SIG(AnotherTypeImplementsMockable::SomeMethod2, int());
            return -1;
        }
    };

    struct MockAnotherTypeImplementsMockable : nvm::mock < AnotherTypeImplementsMockable >
    {
        MockAnotherTypeImplementsMockable()
            : IsMockedCalled(false)
        {
            NVM_ONCE_BLOCK()
            {
                NVM_REGISTER_MOCK_MEMBER_FUNCTION(AnotherTypeImplementsMockable, MockAnotherTypeImplementsMockable, SomeMethod);
                NVM_REGISTER_MOCK_MEMBER_FUNCTION(AnotherTypeImplementsMockable, MockAnotherTypeImplementsMockable, SomeMethod2);
            }
        }

        MOCK_CONST_METHOD3(SomeMethod, void(int a, double b, float c));
        MOCK_METHOD0(SomeMethod2, int());

        virtual bool is_mocked() const
        {
            IsMockedCalled = true;
            return true;
        }

        bool MockMethodCalled;
        mutable bool IsMockedCalled;
    };

    void CallSomeMethod(AnotherTypeImplementsMockable& t)
    {
        t.SomeMethod(1, 2, 3);
    }

    int CallSomeMethod2(AnotherTypeImplementsMockable& t)
    {
        return t.SomeMethod2();
    }

    TEST(mockTests, TestAnotherTypeImplementsMockable)
    {
        using namespace ::testing;
        //! NOTE, only the most derived class will be mocked for non-virtuals in this case.

        MockAnotherTypeImplementsMockable mst;
		EXPECT_CALL(mst, SomeMethod(_,_,_));
        CallSomeMethod(mst);
        EXPECT_FALSE(mst.IsMockedCalled);
        ON_CALL(mst, SomeMethod2()).WillByDefault(Return(42));
        EXPECT_CALL(mst, SomeMethod2()).Times(1);
        EXPECT_EQ(CallSomeMethod2(mst), 42);
    }

    //////////////////////////////////////////////////////////////////////////
    //!
    //! Test overloaded member functions.
    //!
    struct SomeTypeWithOverloadsInheritsMockable : virtual nvm::mockable
    {
        void SomeMethod(int a, double b, float c) const
        {
            NVM_MOCK_OVERLOAD_CONST_INTERCEPT(SomeTypeWithOverloadsInheritsMockable, SomeMethod, void(int, double, float), a, b, c);
        }

        int SomeMethod()
        {
            NVM_MOCK_OVERLOAD_INTERCEPT(SomeTypeWithOverloadsInheritsMockable, SomeMethod, int());
            return -1;
        }

        int SomeMethod2()
        {
            NVM_MOCK_OVERLOAD_INTERCEPT(SomeTypeWithOverloadsInheritsMockable, SomeMethod2, int());
            return -1;
        }
    };

    struct MockSomeTypeWithOverloadsInheritsMockable : nvm::mock < SomeTypeWithOverloadsInheritsMockable >
    {
        MockSomeTypeWithOverloadsInheritsMockable()
            : IsMockedCalled(false)
        {
            NVM_ONCE_BLOCK()
            {
                NVM_REGISTER_MOCK_OVERLOADED_CONST_MEMBER_FUNCTION(SomeTypeWithOverloadsInheritsMockable, MockSomeTypeWithOverloadsInheritsMockable, SomeMethod, void(int, double, float));
                NVM_REGISTER_MOCK_MEMBER_FUNCTION(SomeTypeWithOverloadsInheritsMockable, MockSomeTypeWithOverloadsInheritsMockable, SomeMethod2);
                NVM_REGISTER_MOCK_OVERLOADED_MEMBER_FUNCTION(SomeTypeWithOverloadsInheritsMockable, MockSomeTypeWithOverloadsInheritsMockable, SomeMethod, int());
            }
        }
        
        MOCK_CONST_METHOD3(SomeMethod, void(int a, double b, float c));        
        MOCK_METHOD0(SomeMethod, int());
        MOCK_METHOD0(SomeMethod2, int());

        virtual bool is_mocked() const
        {
            IsMockedCalled = true;
            return true;
        }

        mutable bool IsMockedCalled;
    };

    int CallSomeMethod2(SomeTypeWithOverloadsInheritsMockable& st)
    {
        return st.SomeMethod2();
    }

    TEST(mockTests, TestMockingOverloads)
    {
        using namespace ::testing;
        
        MockSomeTypeWithOverloadsInheritsMockable mst;
        SomeTypeWithOverloadsInheritsMockable& st = mst;
        
        EXPECT_CALL(mst, SomeMethod(_, _, _));
        st.SomeMethod(0, 5., 10.f);
        EXPECT_TRUE(mst.IsMockedCalled);
        EXPECT_CALL(mst, SomeMethod()).WillOnce(Return(42));
        EXPECT_CALL(mst, SomeMethod2()).WillOnce(Return(24));
        EXPECT_EQ(st.SomeMethod(), 42);        
        EXPECT_EQ(CallSomeMethod2(mst), 24);
    }

}//! anonymous

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
