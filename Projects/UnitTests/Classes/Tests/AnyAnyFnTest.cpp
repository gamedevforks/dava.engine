#include "Base/Any.h"
#include "Base/AnyFn.h"
#include "Math/Vector.h"
#include "UnitTests/UnitTests.h"

namespace DAVA
{
DAVA_TESTCLASS (AnyAnyFnTest)
{
    struct A
    {
        enum E1
        {
            E1_1,
            E1_2
        };

        enum class E2
        {
            E2_1,
            E2_2
        };

        int a = 1;

        Vector3 TestFn(Vector3 v1, Vector3 v2)
        {
            return (v1 * v2) * static_cast<float32>(a);
        }

        Vector3 TestFnConst(Vector3 v1, Vector3 v2) const
        {
            return (v1 * v2);
        }

        static int32 StaticTestFn(int32 a, int32 b)
        {
            return a + b;
        }
    };

    class A1
    {
        int a1;
    };

    class B : public A
    {
        int b;
    };

    class D : public A, public A1
    {
        int d;
    };

    class E : public D
    {
        int e;
    };

    template <typename T>
    void DoAutoStorageSimpleTest(const T& initialValue)
    {
        T simpleValue(initialValue);

        AutoStorage<> as;
        TEST_VERIFY(as.IsEmpty());

        as.SetSimple(simpleValue);
        TEST_VERIFY(!as.IsEmpty());
        TEST_VERIFY(as.IsSimple());
        TEST_VERIFY(as.GetSimple<T>() == simpleValue);
        TEST_VERIFY(as.GetSimple<const T&>() == simpleValue);

        const T& simpleRef = simpleValue;
        as.SetSimple(simpleRef);
        TEST_VERIFY(!as.IsEmpty());
        TEST_VERIFY(as.IsSimple());
        TEST_VERIFY(as.GetSimple<T>() == simpleRef);
        TEST_VERIFY(as.GetSimple<const T&>() == simpleRef);

        as.Clear();
        TEST_VERIFY(as.IsEmpty());
    }

    template <typename T>
    void DoAutoStorageSharedTest(const T& initialValue)
    {
        T sharedValue(initialValue);

        AutoStorage<> as;
        TEST_VERIFY(as.IsEmpty());

        as.SetShared(sharedValue);
        TEST_VERIFY(!as.IsEmpty());
        TEST_VERIFY(!as.IsSimple());
        TEST_VERIFY(as.GetShared<T>() == sharedValue);
        TEST_VERIFY(as.GetShared<const T&>() == sharedValue);

        const T& sharedRef = sharedValue;
        as.SetShared(sharedRef);
        TEST_VERIFY(!as.IsEmpty());
        TEST_VERIFY(!as.IsSimple());
        TEST_VERIFY(as.GetShared<T>() == sharedRef);
        TEST_VERIFY(as.GetShared<const T&>() == sharedRef);

        as.Clear();
        TEST_VERIFY(as.IsEmpty());
    }

    template <typename T>
    void DoAnyTest(const T& initialValue)
    {
        T value(initialValue);
        T& valueRef(value);
        const T& valueConstRef(value);

        Any a;
        TEST_VERIFY(a.IsEmpty());

        a.Set(value);
        TEST_VERIFY(!a.IsEmpty());
        TEST_VERIFY(value == a.Get<T>());
        TEST_VERIFY(value == a.Get<const T&>());

        a.Set(valueRef);
        TEST_VERIFY(!a.IsEmpty());
        TEST_VERIFY(value == a.Get<T>());
        TEST_VERIFY(value == a.Get<const T&>());

        a.Set(valueConstRef);
        TEST_VERIFY(!a.IsEmpty());
        TEST_VERIFY(value == a.Get<T>());
        TEST_VERIFY(value == a.Get<const T&>());
    }

    template <typename T1, typename T2>
    void DoAnyCastTest()
    {
        Any::RegisterDefaultCastOP<T1, T2>();

        int v = 123;
        T1 t1 = static_cast<T1>(v);

        Any a(t1);
        T2 t2 = a.Cast<T2>();

        Any b(t2);
        T1 t3 = a.Cast<T1>();

        TEST_VERIFY(t3 == t1);
    }

    DAVA_TEST (AutoStorageTest)
    {
        int32 v = 10203040;

        DoAutoStorageSimpleTest<int32>(v);
        DoAutoStorageSimpleTest<int64>(10203040506070);
        DoAutoStorageSimpleTest<float32>(0.123456f);
        DoAutoStorageSimpleTest<void*>(nullptr);
        DoAutoStorageSimpleTest<int32*>(&v);
        DoAutoStorageSimpleTest<const int32*>(&v);

        String s("10203040");

        DoAutoStorageSharedTest<int32>(v);
        DoAutoStorageSharedTest<int64>(10203040506070);
        DoAutoStorageSharedTest<float32>(0.123456f);
        DoAutoStorageSharedTest<String>(s);
        DoAutoStorageSharedTest<void*>(nullptr);
        DoAutoStorageSharedTest<String*>(&s);
        DoAutoStorageSharedTest<const String*>(&s);
    }

    DAVA_TEST (AnyTestSimple)
    {
        int32 v = 50607080;
        String s("50607080");

        DoAnyTest<int32>(v);
        DoAnyTest<int64>(80706050403020);
        DoAnyTest<float32>(0.123456f);
        DoAnyTest<void*>(nullptr);
        DoAnyTest<const void*>(nullptr);
        DoAnyTest<const int32&>(v);
        DoAnyTest<int32*>(&v);
        DoAnyTest<const int32*>(&v);
        DoAnyTest<String>(s);
        DoAnyTest<const String&>(s);
        DoAnyTest<String*>(&s);
        DoAnyTest<const String*>(&s);

        try
        {
            Any a(10);
            a.Get<String>();
            TEST_VERIFY(false && "Shouldn't be able to ge String");
        }
        catch (const Any::Exception& anyExp)
        {
            TEST_VERIFY(anyExp.errorCode == Any::Exception::BadGet);
        }
    }

    DAVA_TEST (AnyTestPtr)
    {
        B b;
        D d;
        E e;

        B* bPtr = &b;
        A* baPtr = static_cast<A*>(bPtr);

        D* dPtr = &d;
        A* daPtr = static_cast<A*>(dPtr);
        A1* da1Ptr = static_cast<A1*>(dPtr);

        E* ePtr = &e;
        A1* ea1Ptr = static_cast<A1*>(ePtr);

        Any a;
        a.Set(bPtr);

        Type::RegisterBases<B, A>();
        Type::RegisterBases<D, A, A1>();
        Type::RegisterBases<E, D>();

        // simple
        TEST_VERIFY(bPtr == a.Get<void*>());
        TEST_VERIFY(bPtr == a.Get<const void*>());
        TEST_VERIFY(bPtr == a.Get<B*>());
        TEST_VERIFY(bPtr == a.Get<const B*>());
        TEST_VERIFY(a.CanCast<A*>());
        TEST_VERIFY(a.CanCast<const A*>());
        TEST_VERIFY(baPtr == a.Cast<A*>());
        TEST_VERIFY(baPtr == a.Cast<const A*>());

        // multiple inheritance
        a.Set(dPtr);
        TEST_VERIFY(a.CanCast<A*>());
        TEST_VERIFY(a.CanCast<A1*>());
        TEST_VERIFY(dPtr == a.Get<void*>());
        TEST_VERIFY(daPtr == a.Cast<A*>());
        TEST_VERIFY(da1Ptr == a.Cast<A1*>());

        // hierarchy down cast
        a.Set(ePtr);
        TEST_VERIFY(a.CanCast<A1*>());
        TEST_VERIFY(ea1Ptr == a.Cast<A1*>());

        // hierarchy up cast
        a.Set(ea1Ptr);
        TEST_VERIFY(ePtr == a.Cast<E*>());

        try
        {
            a.Cast<int>();
            TEST_VERIFY(false && "Shouldn't be able to cast to int");
        }
        catch (const Any::Exception& anyExp)
        {
            TEST_VERIFY(anyExp.errorCode == Any::Exception::BadCast);
        }
    }

    DAVA_TEST (AnyTestEnum)
    {
        Any a;

        a.Set(A::E1_1);
        TEST_VERIFY(A::E1_1 == a.Get<A::E1>());
        TEST_VERIFY(A::E1_2 != a.Get<A::E1>());

        a.Set(A::E2::E2_2);
        TEST_VERIFY(A::E2::E2_1 != a.Get<A::E2>());
        TEST_VERIFY(A::E2::E2_2 == a.Get<A::E2>());
    }

    DAVA_TEST (AnyLoadStoreCompare)
    {
        int v1 = 11223344;
        int v2;

        int* iptr1 = &v1;
        int* iptr2 = nullptr;

        Any a;
        a.LoadValue(Type::Instance<int>(), &v1);
        TEST_VERIFY(a.Get<int>() == v1);

        a.StoreValue(&v2, sizeof(v2));
        TEST_VERIFY(v1 == v2);

        Any b(v1);
        Any s(String("str"));
        TEST_VERIFY(a == b);
        TEST_VERIFY(a != s);

        a.Set(&v1);
        b.Set(&v2);
        TEST_VERIFY(a != b);

        a.LoadValue(Type::Instance<int*>(), &iptr1);
        a.StoreValue(&iptr2, sizeof(iptr2));
        TEST_VERIFY(iptr1 == iptr2);

        Vector3 vec3(1.1f, 2.2f, 3.3f);
        Vector3 vec3_1;

        try
        {
            a.LoadValue(Type::Instance<Vector3>(), &vec3);
            TEST_VERIFY(false && "Load should be done for unregistred non-fundamental types");
        }
        catch (const Any::Exception& anyExp)
        {
            TEST_VERIFY(anyExp.errorCode == Any::Exception::BadOperation);
        }

        a.Set(vec3);
        b.Set(vec3);

        try
        {
            a.StoreValue(&vec3_1, sizeof(vec3_1));
            TEST_VERIFY(false && "Store should be done for unregistred non-fundamental types");
        }
        catch (const Any::Exception& anyExp)
        {
            TEST_VERIFY(anyExp.errorCode == Any::Exception::BadOperation);
        }

        // now register default operations
        Any::RegisterDefaultOPs<Vector3>();

        a.LoadValue(Type::Instance<Vector3>(), &vec3);
        TEST_VERIFY(a.Get<Vector3>() == vec3);

        a.StoreValue(&vec3_1, sizeof(vec3_1));
        TEST_VERIFY(vec3_1 == vec3);

        try
        {
            a.StoreValue(&vec3_1, sizeof(vec3_1) / 2);
            TEST_VERIFY(false && "Store should be done if destenation size is smaller then source type require");
        }
        catch (const Any::Exception& anyExp)
        {
            TEST_VERIFY(anyExp.errorCode == Any::Exception::BadSize);
        }
    }

    DAVA_TEST (AnyDefaultCastTest)
    {
        DoAnyCastTest<int8, int16>();
        DoAnyCastTest<int8, int32>();
        DoAnyCastTest<int8, int64>();
        DoAnyCastTest<int8, uint8>();
        DoAnyCastTest<int8, uint16>();
        DoAnyCastTest<int8, uint32>();
        DoAnyCastTest<int8, uint64>();
        DoAnyCastTest<int8, float32>();
        DoAnyCastTest<int8, float64>();
        DoAnyCastTest<int8, size_t>();

        DoAnyCastTest<int16, int32>();
        DoAnyCastTest<int16, int64>();
        DoAnyCastTest<int16, uint8>();
        DoAnyCastTest<int16, uint16>();
        DoAnyCastTest<int16, uint32>();
        DoAnyCastTest<int16, uint64>();
        DoAnyCastTest<int16, float32>();
        DoAnyCastTest<int16, float64>();
        DoAnyCastTest<int16, size_t>();

        DoAnyCastTest<int32, int64>();
        DoAnyCastTest<int32, uint8>();
        DoAnyCastTest<int32, uint16>();
        DoAnyCastTest<int32, uint32>();
        DoAnyCastTest<int32, uint64>();
        DoAnyCastTest<int32, float32>();
        DoAnyCastTest<int32, float64>();
        DoAnyCastTest<int32, size_t>();

        DoAnyCastTest<int64, uint8>();
        DoAnyCastTest<int64, uint16>();
        DoAnyCastTest<int64, uint32>();
        DoAnyCastTest<int64, uint64>();
        DoAnyCastTest<int64, float32>();
        DoAnyCastTest<int64, float64>();
        DoAnyCastTest<int64, size_t>();

        DoAnyCastTest<uint8, uint16>();
        DoAnyCastTest<uint8, uint32>();
        DoAnyCastTest<uint8, uint64>();
        DoAnyCastTest<uint8, float32>();
        DoAnyCastTest<uint8, float64>();
        DoAnyCastTest<uint8, size_t>();

        DoAnyCastTest<uint16, uint32>();
        DoAnyCastTest<uint16, uint64>();
        DoAnyCastTest<uint16, float32>();
        DoAnyCastTest<uint16, float64>();
        DoAnyCastTest<uint16, size_t>();

        DoAnyCastTest<uint32, uint64>();
        DoAnyCastTest<uint32, float32>();
        DoAnyCastTest<uint32, float64>();
        DoAnyCastTest<uint32, size_t>();

        DoAnyCastTest<uint64, float32>();
        DoAnyCastTest<uint64, float64>();
        DoAnyCastTest<uint64, size_t>();

        DoAnyCastTest<float32, float64>();
        DoAnyCastTest<float32, size_t>();

        DoAnyCastTest<float64, size_t>();
    }

    DAVA_TEST (AnCastTest)
    {
    }

    DAVA_TEST (AnyFnTest)
    {
        A a;

        AnyFn fn;
        TEST_VERIFY(!fn.IsValid())

        fn = AnyFn(&A::StaticTestFn);
        TEST_VERIFY(fn.IsValid());
        TEST_VERIFY(fn.IsStatic());
        TEST_VERIFY(A::StaticTestFn(10, 20) == fn.Invoke(Any(10), Any(20)).Get<int32>());

        try
        {
            fn.Invoke();
            TEST_VERIFY(false && "Shouldn't be invoked with bad arguments");
        }
        catch (const AnyFn::Exception& anyFnExp)
        {
            TEST_VERIFY(anyFnExp.errorCode == AnyFn::Exception::BadInvokeArguments);
        }

        try
        {
            fn.BindThis(&a);
            TEST_VERIFY(false && "This shouldn't be binded to static function");
        }
        catch (const AnyFn::Exception& anyFnExp)
        {
            TEST_VERIFY(anyFnExp.errorCode == AnyFn::Exception::BadBindThis);
        }

        fn = AnyFn(&A::TestFn);
        TEST_VERIFY(fn.IsValid());
        TEST_VERIFY(!fn.IsStatic());

        Vector3 op1(1.0f, 2.0f, 3.0f);
        Vector3 op2(7.0f, 0.7f, 0.0f);
        Vector3 op3(4.0f, 3.5f, 2.1f);

        Any res;

        fn.Invoke(Any(&a), Any(op1), Any(op2));
        res = fn.Invoke(Any(static_cast<const A*>(&a)), Any(op1), Any(op2));
        TEST_VERIFY(a.TestFn(op1, op2) == res.Get<Vector3>());

        // check compilation if pointer is const
        TEST_VERIFY(a.TestFn(op1, op2) == res.Get<Vector3>());

        fn.BindThis(&a);
        fn = fn.BindThis(static_cast<const A*>(&a));
        TEST_VERIFY(fn.IsValid());
        TEST_VERIFY(fn.IsStatic());

        a.a = 5;
        res = fn.Invoke(Any(op1), Any(op3));
        TEST_VERIFY(a.TestFn(op1, op3) == res.Get<Vector3>());

        fn = AnyFn(&A::TestFnConst);
        res = fn.Invoke(Any(&a), Any(op2), Any(op3));
        TEST_VERIFY(a.TestFnConst(op2, op3) == res.Get<Vector3>());
    }
};

} // namespace DAVA
