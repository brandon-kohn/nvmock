//
//! Copyright © 2015
//! Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#include "mockable.hpp"
#include "mock_base.hpp"
#include "LegionUtility/Thread/OnceBlock.hpp"

/////////////////////////////////////////////////////////////////////////////
//
// Anonymous Namespace
//
/////////////////////////////////////////////////////////////////////////////
namespace 
{

}// Anonymous

/////////////////////////////////////////////////////////////////////////////
//
// Class mockable Member Functions
//
/////////////////////////////////////////////////////////////////////////////
namespace Legion
{
    namespace Test
    {

    }//! namespace Test;
}//!namespace Legion'

namespace
{
    struct SomeTypeInheritsMockable : virtual nvm::mockable
    {
        void SomeMethod(int a, double b, float c) const
        {
            NVM_MOCK_NONVIRTUAL_INTERCEPT(SomeTypeInheritsMockable::SomeMethod, a, b, c);
            TRACE("Called SomeMethod");
        }

        int SomeMethod2()
        {
            NVM_MOCK_NONVIRTUAL_INTERCEPT(SomeTypeInheritsMockable::SomeMethod2);
            TRACE("Called SomeMethod2");
            return -1;
        }

        int SomeMethod3()
        {
            NVM_MOCK_NONVIRTUAL_INTERCEPT(SomeTypeInheritsMockable::SomeMethod3);
            TRACE("Called SomeMethod3");
            return -2;
        }
    };

    struct mockSomeTypeInheritsMockable : nvm::mock < SomeTypeInheritsMockable >
    {
        mockSomeTypeInheritsMockable()
            : mockMethodCalled(false)
            , IsmockedCalled(false)
        {
            NVM_ONCE_BLOCK()
            {
                NVM_REGISTER_NONVIRTUAL_MOCK_MEMBER_FUNCTION(SomeTypeInheritsMockable, mockSomeTypeInheritsMockable, SomeMethod);
                NVM_REGISTER_NONVIRTUAL_MOCK_MEMBER_FUNCTION(SomeTypeInheritsMockable, mockSomeTypeInheritsMockable, SomeMethod2);                
                NVM_REGISTER_NONVIRTUAL_MOCK_MEMBER_FUNCTION(SomeTypeInheritsMockable, mockSomeTypeInheritsMockable, SomeMethod3);
            }
        }

        void SomeMethod(int a, double b, float c) const
        {
            TRACE("Called mockSomeMethod");
            mockMethodCalled = true;
        }

        MOCK_METHOD0(SomeMethod2, int());
        MOCK_METHOD0(SomeMethod3, int());

        virtual bool Ismocked() const
        {
            IsmockedCalled = true;
            return true;
        }

        mutable bool mockMethodCalled;
        mutable bool IsmockedCalled;
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
        mockSomeTypeInheritsMockable mst;
        CallSomeMethod(mst);
        EXPECT_TRUE(mst.mockMethodCalled);
        EXPECT_TRUE(mst.IsmockedCalled);
        ON_CALL(mst, SomeMethod2()).WillByDefault(Return(42));
        EXPECT_CALL(mst, SomeMethod2()).Times(1);
        EXPECT_EQ(CallSomeMethod2(mst), 42);

        //! Methods with the same signature but different names should work.
        EXPECT_CALL(mst, SomeMethod3()).WillRepeatedly(Return(24));
        EXPECT_EQ(CallSomeMethod3(mst), 24);
    }

    //! This type implements mockable without using inheritance or virtuals. Best used
    //! in cases where no base type may be mockable.
    struct SomeTypeImplementsmockable
    {
    protected:
        NVM_IMPLEMENT_MOCKABLE();
    public:
        void SomeMethod(int a, double b, float c)
        {
            NVM_MOCK_NONVIRTUAL_INTERCEPT(SomeTypeImplementsmockable::SomeMethod, a, b, c);
            TRACE("Called SomeMethod");
        }

        int SomeMethod2()
        {
            NVM_MOCK_NONVIRTUAL_INTERCEPT_SIG(SomeTypeImplementsmockable::SomeMethod2, int());
            TRACE("Called SomeMethod2");
            return -1;
        }
    };

    struct mockSomeTypeImplementsmockable : nvm::mock < SomeTypeImplementsmockable >
    {
        mockSomeTypeImplementsmockable()
            : mockMethodCalled(false)
            , IsmockedCalled(false)
        {        
            NVM_ONCE_BLOCK()
            {
                NVM_REGISTER_NONVIRTUAL_MOCK_MEMBER_FUNCTION(SomeTypeImplementsmockable, mockSomeTypeImplementsmockable, SomeMethod);
                NVM_REGISTER_NONVIRTUAL_MOCK_MEMBER_FUNCTION(SomeTypeImplementsmockable, mockSomeTypeImplementsmockable, SomeMethod2);
            }
        }

        void SomeMethod(int a, double b, float c)
        {
            TRACE("Called mockSomeMethod");
            mockMethodCalled = true;
        }

        MOCK_METHOD0(SomeMethod2, int());

        virtual bool Ismocked() const
        {
            IsmockedCalled = true;
            return true;
        }

        bool mockMethodCalled;
        mutable bool IsmockedCalled;
    };

    void CallSomeMethod(SomeTypeImplementsmockable& t)
    {
        t.SomeMethod(1, 2, 3);
    }

    int CallSomeMethod2(SomeTypeImplementsmockable& t)
    {
        return t.SomeMethod2();
    }

    TEST(mockTests, TestNonVirtualMethodmockInterceptionImplemented)
    {
        using namespace ::testing;
        mockSomeTypeImplementsmockable mst;
        CallSomeMethod(mst);
        EXPECT_TRUE(mst.mockMethodCalled);
        EXPECT_FALSE(mst.IsmockedCalled);
        ON_CALL(mst, SomeMethod2()).WillByDefault(Return(42));
        EXPECT_CALL(mst, SomeMethod2()).Times(1);
        EXPECT_EQ(CallSomeMethod2(mst), 42);
    }

    //! Test mocking object which inherits from mockable multiple times.
    struct AnotherTypeInheritsMockable : SomeTypeInheritsMockable, virtual nvm::mockable
    {
        void SomeMethod(int a, double b, float c) const
        {
            NVM_MOCK_NONVIRTUAL_INTERCEPT(AnotherTypeInheritsMockable::SomeMethod, a, b, c);
            TRACE("Called AnotherTypeInheritsMockable::SomeMethod");
        }

        int SomeMethod2()
        {
            NVM_MOCK_NONVIRTUAL_INTERCEPT(AnotherTypeInheritsMockable::SomeMethod2);
            TRACE("Called AnotherTypeInheritsMockable::SomeMethod2");
            return -1;
        }
    };

    struct mockAnotherTypeInheritsMockable : nvm::mock < AnotherTypeInheritsMockable >
    {
        mockAnotherTypeInheritsMockable()
            : mockMethodCalled(false)
            , IsmockedCalled(false)
        {
            NVM_ONCE_BLOCK()
            {
                NVM_REGISTER_NONVIRTUAL_MOCK_MEMBER_FUNCTION(AnotherTypeInheritsMockable, mockAnotherTypeInheritsMockable, SomeMethod);
                NVM_REGISTER_NONVIRTUAL_MOCK_MEMBER_FUNCTION(AnotherTypeInheritsMockable, mockAnotherTypeInheritsMockable, SomeMethod2);
            }
        }

        void SomeMethod(int a, double b, float c) const
        {
            TRACE("Called AnotherTypeInheritsMockable::mockSomeMethod");
            mockMethodCalled = true;
        }

        MOCK_METHOD0(SomeMethod2, int());

        virtual bool Ismocked() const
        {
            IsmockedCalled = true;
            return true;
        }

        mutable bool mockMethodCalled;
        mutable bool IsmockedCalled;
    };

    void CallSomeMethod(AnotherTypeInheritsMockable& t)
    {
        t.SomeMethod(1, 2, 3);
    }

    int CallSomeMethod2(AnotherTypeInheritsMockable& t)
    {
        return t.SomeMethod2();
    }
    
    TEST(mockTests, TestmockAnotherTypeInheritsMockable)
    {
        using namespace ::testing;
        mockAnotherTypeInheritsMockable mst;
        CallSomeMethod(mst);
        EXPECT_TRUE(mst.mockMethodCalled);
        EXPECT_TRUE(mst.IsmockedCalled);
        ON_CALL(mst, SomeMethod2()).WillByDefault(Return(42));
        EXPECT_CALL(mst, SomeMethod2()).Times(1);
        EXPECT_EQ(CallSomeMethod2(mst), 42);
    }

    //! Test inheritence when two instance implement mockable rather than inherit.
    struct AnotherTypeImplementsmockable : SomeTypeImplementsmockable
    {
    protected:
        NVM_IMPLEMENT_MOCKABLE();
    public:
        void SomeMethod(int a, double b, float c)
        {
            NVM_MOCK_NONVIRTUAL_INTERCEPT(AnotherTypeImplementsmockable::SomeMethod, a, b, c);
            TRACE("Called SomeMethod");
        }

        int SomeMethod2()
        {
            NVM_MOCK_NONVIRTUAL_INTERCEPT_SIG(AnotherTypeImplementsmockable::SomeMethod2, int());
            TRACE("Called SomeMethod2");
            return -1;
        }
    };

    struct mockAnotherTypeImplementsmockable : nvm::mock < AnotherTypeImplementsmockable >
    {
        mockAnotherTypeImplementsmockable()
            : mockMethodCalled(false)
            , IsmockedCalled(false)
        {
            NVM_ONCE_BLOCK()
            {
                NVM_REGISTER_NONVIRTUAL_MOCK_MEMBER_FUNCTION(AnotherTypeImplementsmockable, mockAnotherTypeImplementsmockable, SomeMethod);
                NVM_REGISTER_NONVIRTUAL_MOCK_MEMBER_FUNCTION(AnotherTypeImplementsmockable, mockAnotherTypeImplementsmockable, SomeMethod2);
            }
        }

        void SomeMethod(int a, double b, float c)
        {
            TRACE("Called mockSomeMethod");
            mockMethodCalled = true;
        }

        MOCK_METHOD0(SomeMethod2, int());

        virtual bool Ismocked() const
        {
            IsmockedCalled = true;
            return true;
        }

        bool mockMethodCalled;
        mutable bool IsmockedCalled;
    };

    void CallSomeMethod(AnotherTypeImplementsmockable& t)
    {
        t.SomeMethod(1, 2, 3);
    }

    int CallSomeMethod2(AnotherTypeImplementsmockable& t)
    {
        return t.SomeMethod2();
    }

    TEST(mockTests, TestAnotherTypeImplementsmockable)
    {
        using namespace ::testing;
        //! NOTE, only the most derived class will be mocked for non-virtuals in this case.

        mockAnotherTypeImplementsmockable mst;
        CallSomeMethod(mst);
        EXPECT_TRUE(mst.mockMethodCalled);
        EXPECT_FALSE(mst.IsmockedCalled);
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
            NVM_MOCK_NONVIRTUAL_OVERLOAD_CONST_INTERCEPT(SomeTypeWithOverloadsInheritsMockable, SomeMethod, void(int, double, float), a, b, c);
            TRACE("Called SomeMethod");
        }

        int SomeMethod()
        {
            NVM_MOCK_NONVIRTUAL_OVERLOAD_INTERCEPT(SomeTypeWithOverloadsInheritsMockable, SomeMethod, int());
            TRACE("Called SomeMethod int()");
            return -1;
        }

        int SomeMethod2()
        {
            NVM_MOCK_NONVIRTUAL_OVERLOAD_INTERCEPT(SomeTypeWithOverloadsInheritsMockable, SomeMethod2, int());
            TRACE("Called SomeMethod2 int()");
            return -1;
        }
    };

    struct mockSomeTypeWithOverloadsInheritsMockable : nvm::mock < SomeTypeWithOverloadsInheritsMockable >
    {
        mockSomeTypeWithOverloadsInheritsMockable()
            : IsmockedCalled(false)
        {
            NVM_ONCE_BLOCK()
            {
                NVM_REGISTER_NONVIRTUAL_MOCK_OVERLOADED_CONST_MEMBER_FUNCTION(SomeTypeWithOverloadsInheritsMockable, mockSomeTypeWithOverloadsInheritsMockable, SomeMethod, void(int, double, float));
                NVM_REGISTER_NONVIRTUAL_MOCK_MEMBER_FUNCTION(SomeTypeWithOverloadsInheritsMockable, mockSomeTypeWithOverloadsInheritsMockable, SomeMethod2);
                NVM_REGISTER_NONVIRTUAL_MOCK_OVERLOADED_MEMBER_FUNCTION(SomeTypeWithOverloadsInheritsMockable, mockSomeTypeWithOverloadsInheritsMockable, SomeMethod, int());
            }
        }
        
        MOCK_CONST_METHOD3(SomeMethod, void(int a, double b, float c));        
        MOCK_METHOD0(SomeMethod, int());
        MOCK_METHOD0(SomeMethod2, int());

        virtual bool Ismocked() const
        {
            IsmockedCalled = true;
            return true;
        }

        mutable bool IsmockedCalled;
    };

    int CallSomeMethod2(SomeTypeWithOverloadsInheritsMockable& st)
    {
        return st.SomeMethod2();
    }

    TEST(mockTests, TestmockingOverloads)
    {
        using namespace ::testing;
        
        mockSomeTypeWithOverloadsInheritsMockable mst;
        SomeTypeWithOverloadsInheritsMockable& st = mst;
        
        EXPECT_CALL(mst, SomeMethod(_, _, _));
        st.SomeMethod(0, 5., 10.f);
        EXPECT_TRUE(mst.IsmockedCalled);
        EXPECT_CALL(mst, SomeMethod()).WillOnce(Return(42));
        EXPECT_CALL(mst, SomeMethod2()).WillOnce(Return(24));
        EXPECT_EQ(st.SomeMethod(), 42);        
        EXPECT_EQ(CallSomeMethod2(mst), 24);
    }

}//! anonymous
