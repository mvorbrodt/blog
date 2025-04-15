#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <concepts>
#include <functional>
#include <initializer_list>
#include <ostream>
#include <type_traits>
#include <unordered_map>
#include <utility>



template<typename T> concept non_pointer = not std::is_pointer_v<T>;
template<typename T> concept non_reference = not std::is_reference_v<T>;
template<typename T> concept basic_property_type = non_pointer<T> and non_reference<T>;



template<typename T>
requires basic_property_type<T>
struct basic_property_policy
{
    basic_property_policy() = default;
    basic_property_policy(const basic_property_policy& other) : value{ other.get() } { }
    basic_property_policy(basic_property_policy&& other) noexcept : value{ std::move(other.get()) } {}
    ~basic_property_policy() noexcept = default;

    basic_property_policy(const T& v) : value{ v } {}
    basic_property_policy(T&& v) : value{ std::move(v) } {}

    template<typename... Args>
    requires (std::constructible_from<T, Args...>
        and not std::is_same_v<T, std::remove_cvref_t<Args>...>
            and not std::is_same_v<basic_property_policy, std::remove_cvref_t<Args>...>)
    basic_property_policy(Args&&... args) : value{ std::forward<Args>(args)... } {}

    template<typename U>
    requires (std::constructible_from<T, std::initializer_list<U>>)
    basic_property_policy(std::initializer_list<U> l) : value{ l } {}

    [[nodiscard]] T& get() { return value; }
    [[nodiscard]] const T& get() const { return value; }
    [[nodiscard]] const T& get() const volatile { return const_cast<const T&>(value); }

    void set(const T& v) { value = v; }
    void set(T&& v) noexcept { value = std::move(v); }

private:
    T value = T{};
};



template<typename T, typename P>
requires basic_property_type<T>
class basic_property : private P
{
public:
    basic_property() = default;
    basic_property(const basic_property&) = default;
    basic_property(basic_property&&) noexcept = default;
    ~basic_property() noexcept { clear_update_proc(); }

    basic_property(const T& v) : P{ v } {}
    basic_property(T&& v) : P{ std::move(v) } {}

    template<typename... Args>
    requires (std::constructible_from<T, Args...>
        and not std::is_same_v<T, std::remove_cvref_t<Args>...>
            and not std::is_same_v<basic_property, std::remove_cvref_t<Args>...>)
    basic_property(Args&&... args) : P{ std::forward<Args>(args)... } {}

    template<typename U>
    requires (std::constructible_from<T, std::initializer_list<U>>)
    basic_property(std::initializer_list<U> l) : P{ l } {}

    basic_property& operator = (const basic_property& other)
    {
        P::set(other.get());
        dispatch_update();
        return *this;
    }

    basic_property& operator = (basic_property&& other) noexcept
    {
        P::set(std::move(other.get()));
        dispatch_update();
        return *this;
    }

    basic_property& operator = (const T& v)
    {
        P::set(v);
        dispatch_update();
        return *this;
    }

    basic_property& operator = (T&& v) noexcept
    {
        P::set(std::move(v));
        dispatch_update();
        return *this;
    }

    template<typename U>
    requires (std::constructible_from<T, U>
        and not std::is_same_v<T, std::remove_cvref_t<U>>
            and not std::is_same_v<basic_property, std::remove_cvref_t<U>>)
    basic_property& operator = (U&& v)
    {
        P::set(std::forward<U>(v));
        dispatch_update();
        return *this;
    }

    using P::get;

    auto operator <=> (const basic_property& other) const { return P::get() <=> other.get(); }
    bool operator == (const basic_property& other) const { return P::get() == other.get(); }

    template<typename U>
    requires (not std::is_same_v<basic_property, std::remove_cvref_t<U>>)
    auto operator <=> (const U& other) const { return P::get() <=> other; }

    template<typename U>
    requires (not std::is_same_v<basic_property, std::remove_cvref_t<U>>)
    bool operator == (const U& other) const { return P::get() == other; }

#define DEFINE_PROPERTY_OP(OP) \
    basic_property& operator OP (const basic_property& other) \
    { \
        P::get() OP other.get(); \
        dispatch_update(); \
        return *this; \
    } \
    template<typename U> \
    requires (not std::is_same_v<basic_property, std::remove_cvref_t<U>>) \
    basic_property& operator OP (const U& v) \
    { \
        P::get() OP v; \
        dispatch_update(); \
        return *this; \
    }

    DEFINE_PROPERTY_OP(+= );
    DEFINE_PROPERTY_OP(-= );
    DEFINE_PROPERTY_OP(*= );
    DEFINE_PROPERTY_OP(/= );
    DEFINE_PROPERTY_OP(%= );

    DEFINE_PROPERTY_OP(&= );
    DEFINE_PROPERTY_OP(|= );
    DEFINE_PROPERTY_OP(^= );
    DEFINE_PROPERTY_OP(<<= );
    DEFINE_PROPERTY_OP(>>= );

    explicit operator T& () { return P::get(); }
    operator const T& () const { return P::get(); }
    operator const T& () const volatile { return P::get(); }

    T* operator -> () { return &P::get(); }
    const T* operator -> () const { return &P::get(); }
    const T* operator -> () const volatile { return &P::get(); }

    template<typename U>
    decltype(auto) operator [] (U&& i) { return P::get()[std::forward<U>(i)]; }

    template<typename U>
    decltype(auto) operator [] (U&& i) const { return P::get()[std::forward<U>(i)]; }

    template<typename U>
    decltype(auto) operator [] (U&& i) const volatile { return P::get()[std::forward<U>(i)]; }

    template<typename M>
    requires (std::is_invocable_v<M, basic_property, void*>)
    void set_update_proc(M&& proc, void* ctx) { k_update_proc_map.insert_or_assign(this, std::make_pair(std::forward<M>(proc), ctx)); }

    void clear_update_proc() noexcept { k_update_proc_map.erase(this); }

    friend std::ostream& operator << (std::ostream& os, const basic_property& p)
    {
        os << p.get();
        return os;
    }

    friend std::wostream& operator << (std::wostream& os, const basic_property& p)
    {
        os << p.get();
        return os;
    }

private:
    using update_proc_t = std::function<void(const basic_property&, void*)>;
    using update_proc_map_key_t = void*;
    using update_proc_map_val_t = std::pair<update_proc_t, void*>;
    using update_proc_map_t = std::unordered_map<update_proc_map_key_t, update_proc_map_val_t>;

    inline static auto k_update_proc_map = update_proc_map_t{};

    void dispatch_update() noexcept
    {
        if (auto it = k_update_proc_map.find(this); it != std::end(k_update_proc_map))
        {
            auto& proc = it->second.first;
            auto& ctx = it->second.second;
            proc(*this, ctx);
        }
    }
};



template<typename T, typename P>
const volatile basic_property<T, P>& as_volatile(const basic_property<T, P>& p)
{
    return const_cast<const volatile basic_property<T, P>&>(p);
}



#define DEFINE_PROPERTY_OP1(PT, OP) \
    template<typename T> \
    auto operator OP (const PT<T>& p) -> \
        PT<std::invoke_result_t<decltype([](T v) { return OP v; }), T>> \
            { return { OP p.get() }; }

#define DEFINE_PROPERTY_OP2(PT, OP) \
    template<typename T1, typename T2> \
    auto operator OP (const PT<T1>& lhs, const PT<T2>& rhs) -> \
        PT<std::invoke_result_t<decltype([](T1 x, T2 y) { return x OP y; }), T1, T2>> \
            { return { lhs.get() OP rhs.get() }; } \
    template<typename T, typename U> \
    requires (not std::is_same_v<PT<T>, std::remove_cvref_t<U>>) \
    auto operator OP (const PT<T>& lhs, const U& rhs) -> \
        PT<std::invoke_result_t<decltype([](T x, U y) { return x OP y; }), T, U>> \
            { return { lhs.get() OP rhs }; } \
    template<typename U, typename T> \
    requires (not std::is_same_v<std::remove_cvref_t<U>, PT<T>> \
        and not std::is_same_v<std::remove_cvref_t<U>, std::ostream> \
            and not std::is_same_v<std::remove_cvref_t<U>, std::wostream>) \
    auto operator OP (const U& lhs, const PT<T>& rhs) -> \
        PT<std::invoke_result_t<decltype([](U x, T y) { return x OP y; }), U, T>> \
            { return { lhs OP rhs.get() }; }



template<typename T> using property = basic_property<T, basic_property_policy<T>>;

DEFINE_PROPERTY_OP1(property, +);
DEFINE_PROPERTY_OP1(property, -);
DEFINE_PROPERTY_OP2(property, +);
DEFINE_PROPERTY_OP2(property, -);
DEFINE_PROPERTY_OP2(property, *);
DEFINE_PROPERTY_OP2(property, /);
DEFINE_PROPERTY_OP2(property, %);

DEFINE_PROPERTY_OP1(property, ~);
DEFINE_PROPERTY_OP2(property, &);
DEFINE_PROPERTY_OP2(property, |);
DEFINE_PROPERTY_OP2(property, ^);
DEFINE_PROPERTY_OP2(property, <<);
DEFINE_PROPERTY_OP2(property, >>);



void foo(std::string& ps) {}

void foo(const std::string& ps) {}

auto bar(const property<std::string>& p)
{
    auto tmp = property<std::string>{ p };
    return tmp;
}



int main()
{
    using namespace std::string_literals;

    auto update_proc1 = [](const property<std::string>& p, void* ctx) { std::cout << &p << " / " << ctx << " updated with " << p << std::endl; };
    auto update_proc2 = [](const property<int>& p, void* ctx) { std::cout << &p << " / " << ctx << " updated with " << p << std::endl; };
    auto update_proc3 = [](const property<float>& p, void* ctx) { std::cout << &p << " / " << ctx << " updated with " << p << std::endl; };

    auto p1 = property<std::string>{ "C++11" };
    auto p2 = property<std::string>{ p1 };
    auto p3 = std::move(p2);

    p2 = bar("C++11");
    p2 = bar("C++11"s);

    auto s1 = "C++14"s;
    auto s2 = "C++17"s;
    auto p4 = property<std::string>{ s1 };
    auto p5 = property<std::string>{ std::move(s2) };
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

    foo(p3);
    foo(as_volatile(p3));

    //foo(p4); // ERROR! implicit conversion to non-const reference is forbiden!
    foo(static_cast<std::string&>(p4)); // OK! explicit conversion to non-const reference is allowed!
    foo(p4.get());
    foo(std::as_const(p4).get());
    foo(as_volatile(p4).get());

    std::string s3 = p3;
    std::string s4 = as_volatile(p3);

    [[maybe_unused]] auto tmp1 = p1->size();
    [[maybe_unused]] auto tmp2 = std::as_const(p2)->length();
    [[maybe_unused]] auto tmp3 = as_volatile(p3)->c_str();

    auto r1 = p1 == p2;
    auto r2 = p1 == p1;
    auto r3 = p1 == "C++20";
    auto r4 = "C++20" == p1;
    auto r5 = p1 == "C++20"s;
    auto r6 = "C++20"s == p1;

    auto p8 = property<int>{ 1 };
    auto p9 = property<float>{ 1.f };

    p8.set_update_proc(update_proc2, (void*)0x88888888);
    p9.set_update_proc(update_proc3, (void*)0x99999999);

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

    10 + p10;
    p10 + 10;

    auto v1 = property<std::vector<int>>{ 1, 2, 3, 4, 5 };
    // auto v2 = property{ std::vector<int>{ 1, 2, 3, 4, 5 } };
    [[maybe_unused]] auto tmp4 = v1->size();
    v1[0] = 666;
    auto i1 = v1[0];
    auto i2 = std::as_const(v1)[0];
    auto i3 = as_volatile(v1)[0];

    auto m1 = property<std::map<int, int>>(std::map<int, int>{ { 1, 1 }, { 2, 2 }, { 3, 3 } });
    m1[1] = 11;
    m1[2] = 22;
    m1[3] = 33;
}
