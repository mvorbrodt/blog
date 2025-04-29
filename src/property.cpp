#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "property.hpp"

void foo(std::string& s) {}

void foo(const std::string& s) {}

void bar(std::string& s) {}

auto qaz(const property<std::string>& prop)
{
    auto tmp = property<std::string>{ prop };
    return tmp;
}

int main()
{
    using namespace std::string_literals;

    auto update_proc1 = [](const property<std::string>& p, void* ctx) { std::cout << &p << " / " << ctx << " updated with " << p << std::endl; };
    auto update_proc2 = [](const property<int>& p, void* ctx) { std::cout << &p << " / " << ctx << " updated with " << p << std::endl; };
    auto update_proc3 = [](const property<float>& p, void* ctx) { std::cout << &p << " / " << ctx << " updated with " << p << std::endl; };

    auto p1 = make_property<std::string>("C++11"); // property<std::string>{ "C++11" };
    auto p2 = make_property(p1); // property<std::string>{ p1 };
    auto p3 = make_property(std::move(p2)); // std::move(p2);

    p2 = qaz("C++11");
    p2 = qaz("C++11"s);

    auto s1 = "C++14"s;
    auto s2 = "C++17"s;
    auto p4 = make_property(s1); // property<std::string>{ s1 };
    auto p5 = make_property(std::move(s2)); // property<std::string>{ std::move(s2) };
    auto p6 = property<std::string>{ "C++20"s };
    auto p7 = property<std::string>{ (const std::string&)std::string{ "C++23" } };

    p1.set_update_proc(update_proc1, (void*)0x11111111);
    p2.set_update_proc(update_proc1, (void*)0x22222222);
    p3.set_update_proc(update_proc1, (void*)0x33333333);
    p4.set_update_proc(update_proc1, (void*)0x44444444);

    p1 = p6;
    p2 = std::move(p7);

    p3 = s1;
    p4 = std::move(s1);

    foo(p3); // OK! implicit user-defined type conversion returns CONST reference!
    foo(std::as_const(p3));
    foo(as_volatile(p3));
    foo(strip(std::as_const(p3)));
    foo(strip(as_volatile(p3)));

    foo(*p3); // WARNING! explicit call to operator* returns NON-const reference!
    foo(*std::as_const(p3));
    foo(*as_volatile(p3));

    foo(p4.get()); // WARNING! explicit call to get() returns NON-const reference!
    foo(std::as_const(p4).get());
    foo(as_volatile(p4).get());

    //bar(p4); // ERROR! implicit conversion to non-const reference is forbiden!
    bar(p4.get()); // OK! explicit call to get() to convert to non-const reference is allowed!
    bar(*p4); // OK! explicit call to operator* to convert to non-const reference is allowed!
    bar(strip(p4)); // OK! explicit call to strip is allowed!
    bar(static_cast<std::string&>(p4)); // OK! explicit conversion to non-const reference is allowed!

    std::string s3 = p3;
    std::string s4 = std::move(p3);

    [[maybe_unused]] auto tmp1 = p1->size();
    [[maybe_unused]] auto tmp2 = std::as_const(p2)->length();
    [[maybe_unused]] auto tmp3 = as_volatile(p3)->c_str();

    auto r1 = p1 == p2;
    auto r2 = p1 == p1;
    auto r3 = p1 == "C++20";
    auto r4 = "C++20" == p1;
    auto r5 = p1 == "C++20"s;
    auto r6 = "C++20"s == p1;

    auto p8 = make_property(1); // property<int>{ 1 };
    auto p8_2 = make_property<float>(1); // property<float>{ 1 };
    auto p9 = make_property(1.f); // property<float>{ 1.f };
    auto p9_2 = make_property<int>(1.f); // property<int>{ 1.f };

    p8.set_update_proc(update_proc2, (void*)0x88888888);
    p9.set_update_proc(update_proc3, (void*)0x99999999);

    //std::cin >> p8;
    //std::wcin >> p9;

    p8 = 2;
    p8 = 2.f;
    p9 = 3;
    p9 = 3.f;
    p8 = p9;
    p9 = p8;
    p8 = (int)p9;
    p9 = p9;

    p8 += p8;
    p9 += p8;
    p8 += p9;
    p8 += 1;

    auto r7 = p8 < p9;
    auto r8 = p8 == 2;
    auto r9 = 3 == p9;
    auto r10 = p1 == p2;
    auto r11 = p1 > p2;

    auto p10 = +p8;
    auto p11 = -p9;
    auto p12 = p8 + p8;
    auto p13 = p8 - p9;
    auto p14 = p8 * p9;
    auto p15 = p8 / p9;
    auto p16 = p8 % p8;

    auto p17 = ~p8;
    auto p19 = p8 & p8;
    auto p20 = p8 | p8;
    auto p21 = p8 ^ p8;
    auto p22 = p8 << p8;
    auto p23 = p8 >> p8;

    auto p24 = 10 + p10;
    auto p25 = p10 + 10;

    auto v1 = property<std::vector<int>>{ 1, 2, 3, 4, 5 };
    auto v2 = make_property(std::vector<int>{ 1, 2, 3, 4, 5 });
    [[maybe_unused]] auto tmp4 = v1->size();
    v1[0] = 666;
    auto i1 = v1[0];
    auto i2 = std::as_const(v1)[0];
    auto i3 = as_volatile(v1)[0];

    auto it1 = v1.begin();
    decltype(auto) it2 = v2.begin();

    for (auto i : v1) { std::cout << i << std::endl; }

    auto m1 = property<std::map<int, int>>(std::map<int, int>{ { 1, 1 }, { 2, 2 }, { 3, 3 } });
    auto m2 = make_property(std::map<int, int>{ { 1, 1 }, { 2, 2 }, { 3, 3 } });
    m1[1] = 11;
    m1[2] = 22;
    m1[3] = 33;

    for (auto i : m1) { std::cout << i.first << " -> " << i.second << std::endl; }

    auto ptr1 = make_property(std::make_shared<std::string>("C++23"));
    auto ptr2 = make_property(std::make_unique<std::string>("C++26"));
    auto ptr3 = make_property(std::make_shared<std::string>("C++1998"));
    auto ptr4 = make_property(std::make_unique<std::string>("C++2001"));
    auto ptr5 = make_property(std::make_shared<int[]>(3));
    auto ptr6 = make_property(std::make_shared<int[]>(3));

    ptr5[0] = ptr6[0] = 111;
    ptr5[1] = ptr6[1] = 222;
    ptr5[2] = ptr6[2] = 333;

    strip(ptr3).reset();
    delete strip(ptr4).release();

    strip(std::as_const(ptr3)).use_count();
    strip(as_volatile(ptr4)).get_deleter();

    strip(ptr5)[2] = strip(ptr6)[2] = -1;

    ptr1->length();
    (*ptr1).length();

    ptr2->size();
    (*ptr2).size();

    if (ptr1) std::cout << "not null" << std::endl;
    if (ptr2) std::cout << "not null" << std::endl;
    if (!ptr3) std::cout << "null" << std::endl;
    if (!ptr4) std::cout << "null" << std::endl;

    for (auto i = 0; i < 3; ++i) std::cout << ptr5[i] << ", " << ptr6[i] << std::endl;

    struct S { int x, y, z; };
    struct U : S {};

    auto ss = S{ 1, 2, 3 };
    auto uu = U{ 4, 5, 6 };

    auto ps1 = make_property<S>(1, 2, 3);
    auto ps2 = make_property(ss);
    auto ps3 = make_property(S{ 4, 5, 6 });

    auto ps5 = make_property<S>(uu);
    auto ps6 = make_property<S>(U{ 4, 5, 6 });

    auto ps8 = property<std::string>(10, '!');
    auto ps9 = make_property<std::string>(10, '!');

    struct SS { ~SS() { std::cout << "~SS\n"; } };

    auto dg1 = property{ "C++17" }; // property<std::string>
    auto dg2 = property{ L"C++17" }; // property<std::wstring>

    auto dg3 = make_property("C++17"); // property<std::string>
    auto dg4 = make_property(L"C++17"); // property<std::wstring>

    auto dg5 = property{ new int{ 17 } }; // property<std::unique_ptr<int>>
    auto dg6 = property{ new SS }; // property<std::unique_ptr<SS>>

    auto dg7 = make_property(new int{ 17 }); // property<std::unique_ptr<int>>
    auto dg8 = make_property(new SS); // property<std::unique_ptr<SS>>

    auto dg9 = property{ v1.begin(), v1.end() }; // property<std::vector<int>>
    auto dg10 = property{ m1.begin(), m1.end() }; // property<std::vector<std::pair<int, int>>>

    auto dg11 = make_property(v1.begin(), v1.end()); // property<std::vector<int>>
    auto dg12 = make_property(m1.begin(), m1.end()); // property<std::vector<std::pair<int, int>>>

    if (&dg1 != &*dg1) throw std::logic_error("Bad pointer!");
    if (&dg2 != &*dg2) throw std::logic_error("Bad pointer!");
    if (&dg3 != &dg3.get()) throw std::logic_error("Bad pointer!");
    if (&dg4 != &dg4.get()) throw std::logic_error("Bad pointer!");

    if (&dg5 != dg5.get().get()) throw std::logic_error("Bad pointer!");
    if (&dg6 != dg6.get().get()) throw std::logic_error("Bad pointer!");
    if (&dg7 != dg7.get().get()) throw std::logic_error("Bad pointer!");
    if (&dg8 != dg8.get().get()) throw std::logic_error("Bad pointer!");

    strip(dg6).reset();
    strip(dg8).reset();

    std::cout << "\nTHE END!\n";
}
