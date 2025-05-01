#pragma once

#include <concepts>
#include <functional>
#include <initializer_list>
#include <istream>
#include <iterator>
#include <memory>
#include <ostream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>



template<typename PT> concept std_smart_pointer = requires (PT p)
{
    typename PT::element_type;
    { static_cast<bool>(p) };
}
and std::disjunction_v<
    std::is_same<PT, std::shared_ptr<typename PT::element_type>>,
    std::is_same<PT, std::unique_ptr<typename PT::element_type>>,
    std::is_same<PT, std::shared_ptr<typename PT::element_type[]>>,
    std::is_same<PT, std::unique_ptr<typename PT::element_type[]>>>;



template<typename CT> concept std_container = requires(CT a, const CT b)
{
    requires std::regular<CT>;
    requires std::swappable<CT>;
    requires std::destructible<typename CT::value_type>;
    requires std::same_as<typename CT::reference, typename CT::value_type&>;
    requires std::same_as<typename CT::const_reference, const typename CT::value_type&>;
    requires std::forward_iterator<typename CT::iterator>;
    requires std::forward_iterator<typename CT::const_iterator>;
    requires std::signed_integral<typename CT::difference_type>;
    requires std::same_as<typename CT::difference_type, typename std::iterator_traits<typename CT::iterator>::difference_type>;
    requires std::same_as<typename CT::difference_type, typename std::iterator_traits<typename CT::const_iterator>::difference_type>;
    { a.begin() } -> std::same_as<typename CT::iterator>;
    { a.end() } -> std::same_as<typename CT::iterator>;
    { b.begin() } -> std::same_as<typename CT::const_iterator>;
    { b.end() } -> std::same_as<typename CT::const_iterator>;
    { a.cbegin() } -> std::same_as<typename CT::const_iterator>;
    { a.cend() } -> std::same_as<typename CT::const_iterator>;
    { a.size() } -> std::same_as<typename CT::size_type>;
    { a.max_size() } -> std::same_as<typename CT::size_type>;
    { a.empty() } -> std::same_as<bool>;
};



template<typename T> concept non_pointer = (not std::is_pointer_v<T>);
template<typename T> concept non_reference = (not std::is_reference_v<T>);
template<typename T> concept basic_property_type = ((non_pointer<T> and non_reference<T>) or std_smart_pointer<T> or std_container<T>);



template<basic_property_type T>
struct basic_property_policy
{
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using rvalue_reference = value_type&&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    basic_property_policy() = default;
    basic_property_policy(const basic_property_policy& other) : m_value(other.get()) {}
    basic_property_policy(basic_property_policy&& other) noexcept : m_value(std::move(other.get())) {}
    ~basic_property_policy() noexcept = default;

    basic_property_policy(const_reference value) : m_value(value) {}
    basic_property_policy(rvalue_reference value) : m_value(std::move(value)) {}

    template<typename U> requires (std::convertible_to<U, T>)
    basic_property_policy(U&& value) : m_value(static_cast<T>(std::forward<U>(value))) {}

    template<typename... Args> requires (std::constructible_from<T, Args...>)
    basic_property_policy(Args&&... args) : m_value(std::forward<Args>(args)...) {}

    template<typename U> requires (std::constructible_from<T, std::initializer_list<U>>)
    basic_property_policy(std::initializer_list<U> l) : m_value(l) {}

    [[nodiscard]] reference get() { return m_value; }
    [[nodiscard]] const_reference get() const { return m_value; }
    [[nodiscard]] const_reference get() const volatile { return const_cast<const_reference>(m_value); }

    void set(const_reference value) { m_value = value; }
    void set(rvalue_reference value) noexcept { m_value = std::move(value); }

private:
    T m_value;
};



template<basic_property_type T, typename P>
class basic_property : private P
{
public:
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using rvalue_reference = value_type&&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    using P::P;
    using P::get;

    basic_property() = default;
    basic_property(const basic_property&) = default;
    basic_property(basic_property&&) noexcept = default;
    ~basic_property() noexcept { clear_update_proc(); }

    basic_property(const_reference value) : P(value) {}
    basic_property(rvalue_reference value) : P(std::move(value)) {}

    template<typename U> requires (std::convertible_to<U, T>)
    basic_property(U&& value) : P(std::forward<U>(value)) {}

    template<typename... Args> requires (std::constructible_from<T, Args...>)
    basic_property(Args&&... args) : P(std::forward<Args>(args)...) {}

    template<typename U> requires (std::constructible_from<T, std::initializer_list<U>>)
    basic_property(std::initializer_list<U> l) : P(l) {}

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

    basic_property& operator = (const_reference value)
    {
        P::set(value);
        dispatch_update();
        return *this;
    }

    basic_property& operator = (rvalue_reference value) noexcept
    {
        P::set(std::move(value));
        dispatch_update();
        return *this;
    }

    template<typename U> requires (std::constructible_from<T, U>
    and not std::same_as<T, std::remove_cvref_t<U>> and not std::same_as<basic_property, std::remove_cvref_t<U>>)
    basic_property& operator = (U&& value)
    {
        P::set(std::forward<U>(value));
        dispatch_update();
        return *this;
    }

    [[nodiscard]] auto operator <=> (const basic_property& other) const { return P::get() <=> other.get(); }
    [[nodiscard]] bool operator == (const basic_property& other) const { return P::get() == other.get(); }

    template<typename U> requires (not std::same_as<basic_property, std::remove_cvref_t<U>>)
    [[nodiscard]] auto operator <=> (const U& other) const { return P::get() <=> other; }

    template<typename U> requires (not std::same_as<basic_property, std::remove_cvref_t<U>>)
    [[nodiscard]] bool operator == (const U& other) const { return P::get() == other; }

#if defined(DEFINE_PROPERTY_OPERATOR)
#undef DEFINE_PROPERTY_OPERATOR
#endif

#define DEFINE_PROPERTY_OPERATOR(OP) \
    basic_property& operator OP (const basic_property& other) \
    { \
        P::get() OP other.get(); \
        dispatch_update(); \
        return *this; \
    } \
    \
    template<typename U> requires (not std::same_as<basic_property, std::remove_cvref_t<U>>) \
    basic_property& operator OP (const U& value) \
    { \
        P::get() OP value; \
        dispatch_update(); \
        return *this; \
    }

    DEFINE_PROPERTY_OPERATOR(+=);
    DEFINE_PROPERTY_OPERATOR(-=);
    DEFINE_PROPERTY_OPERATOR(*=);
    DEFINE_PROPERTY_OPERATOR(/=);
    DEFINE_PROPERTY_OPERATOR(%=);

    DEFINE_PROPERTY_OPERATOR(&=);
    DEFINE_PROPERTY_OPERATOR(|=);
    DEFINE_PROPERTY_OPERATOR(^=);
    DEFINE_PROPERTY_OPERATOR(<<=);
    DEFINE_PROPERTY_OPERATOR(>>=);

#undef DEFINE_PROPERTY_OPERATOR

    explicit operator reference () { return P::get(); }
    operator const_reference () const { return P::get(); }
    operator const_reference () const volatile { return P::get(); }



    template<typename PT = T> requires (not std_smart_pointer<PT>)
    [[nodiscard]] pointer operator & () { return &P::get(); }

    template<typename PT = T> requires (not std_smart_pointer<PT>)
    [[nodiscard]] const_pointer operator & () const { return &P::get(); }

    template<typename PT = T> requires (not std_smart_pointer<PT>)
    [[nodiscard]] const_pointer operator & () const volatile { return &P::get(); }

    template<typename PT = T> requires (not std_smart_pointer<PT>)
    [[nodiscard]] reference operator * () { return P::get(); }

    template<typename PT = T> requires (not std_smart_pointer<PT>)
    [[nodiscard]] const_reference operator * () const { return P::get(); }

    template<typename PT = T> requires (not std_smart_pointer<PT>)
    [[nodiscard]] const_reference operator * () const volatile { return P::get(); }

    template<typename PT = T> requires (not std_smart_pointer<PT>)
    pointer operator -> () { return &P::get(); }

    template<typename PT = T> requires (not std_smart_pointer<PT>)
    const_pointer operator -> () const { return &P::get(); }

    template<typename PT = T> requires (not std_smart_pointer<PT>)
    const_pointer operator -> () const volatile { return &P::get(); }



    template<typename PT = T> requires (std_smart_pointer<PT>)
    explicit operator bool() const noexcept { return static_cast<bool>(P::get()); }

    template<typename PT = T> requires (std_smart_pointer<PT>)
    [[nodiscard]] PT::element_type* operator & () { return P::get().get(); }

    template<typename PT = T> requires (std_smart_pointer<PT>)
    [[nodiscard]] const PT::element_type* operator & () const { return P::get().get(); }

    template<typename PT = T> requires (std_smart_pointer<PT>)
    [[nodiscard]] const PT::element_type* operator & () const volatile { return P::get().get(); }

    template<typename PT = T> requires (std_smart_pointer<PT>)
    [[nodiscard]] PT::element_type& operator * () { return *P::get(); }

    template<typename PT = T> requires (std_smart_pointer<PT>)
    [[nodiscard]] const PT::element_type& operator * () const { return *P::get(); }

    template<typename PT = T> requires (std_smart_pointer<PT>)
    [[nodiscard]] const PT::element_type& operator * () const volatile { return *P::get(); }

    template<typename PT = T> requires (std_smart_pointer<PT>)
    reference operator -> () { return P::get(); }

    template<typename PT = T> requires (std_smart_pointer<PT>)
    const_reference operator -> () const { return P::get(); }

    template<typename PT = T> requires (std_smart_pointer<PT>)
    const_reference operator -> () const volatile { return P::get(); }



    template<typename U> requires (std_smart_pointer<T> or std_container<T>)
    [[nodiscard]] decltype(auto) operator [] (U&& index) { return P::get()[std::forward<U>(index)]; }

    template<typename U> requires (std_smart_pointer<T> or std_container<T>)
    [[nodiscard]] decltype(auto) operator [] (U&& index) const { return P::get()[std::forward<U>(index)]; }

    template<typename U> requires (std_smart_pointer<T> or std_container<T>)
    [[nodiscard]] decltype(auto) operator [] (U&& index) const volatile { return P::get()[std::forward<U>(index)]; }



    template<typename C = T> requires (std_container<C>)
    [[nodiscard]] decltype(auto) begin() { return P::get().begin(); }

    template<typename C = T> requires (std_container<C>)
    [[nodiscard]] decltype(auto) end() { return P::get().end(); }

    template<typename C = T> requires (std_container<C>)
    [[nodiscard]] decltype(auto) begin() const { return P::get().begin(); }

    template<typename C = T> requires (std_container<C>)
    [[nodiscard]] decltype(auto) end() const { return P::get().end(); }

    template<typename C = T> requires (std_container<C>)
    [[nodiscard]] decltype(auto) cbegin() { return P::get().cbegin(); }

    template<typename C = T> requires (std_container<C>)
    [[nodiscard]] decltype(auto) cend() { return P::get().cend(); }



    template<typename M> requires (std::invocable<M, basic_property, void*>)
    void set_update_proc(M&& proc, void* ctx) { k_update_proc_map.insert_or_assign(this, std::make_pair(std::forward<M>(proc), ctx)); }

    void clear_update_proc() noexcept { k_update_proc_map.erase(this); }

    friend std::ostream& operator << (std::ostream& output, const basic_property& prop)
    {
        output << prop.get();
        return output;
    }

    friend std::wostream& operator << (std::wostream& output, const basic_property& prop)
    {
        output << prop.get();
        return output;
    }

    friend std::istream& operator >> (std::istream& input, basic_property& prop)
    {
        input >> prop.get();
        prop.dispatch_update();
        return input;
    }

    friend std::wistream& operator >> (std::wistream& input, basic_property& prop)
    {
        input >> prop.get();
        prop.dispatch_update();
        return input;
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



basic_property(const char*) -> basic_property<std::string, basic_property_policy<std::string>>;
basic_property(const wchar_t*) -> basic_property<std::wstring, basic_property_policy<std::wstring>>;

template<typename T> basic_property(T*) -> basic_property<std::unique_ptr<T>, basic_property_policy<std::unique_ptr<T>>>;

template<typename Iterator> basic_property(Iterator, Iterator) ->
    basic_property<std::vector<typename std::iterator_traits<Iterator>::value_type>,
    basic_property_policy<std::vector<typename std::iterator_traits<Iterator>::value_type>>>;



template<basic_property_type T, typename P = basic_property_policy<T>>
[[nodiscard]] auto make_property(const basic_property<T, P>& prop)
{
    using U = std::remove_cvref_t<T>;
    return basic_property<U, P>(prop);
}

template<basic_property_type T, typename P = basic_property_policy<T>>
[[nodiscard]] auto make_property(basic_property<T, P>&& prop)
{
    using U = std::remove_cvref_t<T>;
    return basic_property<U, P>(std::move(prop));
}

template<basic_property_type T, typename P = basic_property_policy<T>>
[[nodiscard]] auto make_property(const T& value)
{
    using U = std::remove_cvref_t<T>;
    return basic_property<U, P>(value);
}

template<basic_property_type T, typename P = basic_property_policy<T>>
[[nodiscard]] auto make_property(T&& value)
{
    using U = std::remove_cvref_t<T>;
    return basic_property<U, P>(std::move(value));
}

template<basic_property_type T, typename U, typename P = basic_property_policy<T>>
requires (std::convertible_to<U, T>)
[[nodiscard]] auto make_property(U&& value)
{
    using V = std::remove_cvref_t<T>;
    return basic_property<V, P>(std::forward<U>(value));
}

template<basic_property_type T, typename... Args, typename P = basic_property_policy<T>>
requires (std::constructible_from<T, Args...>)
[[nodiscard]] auto make_property(Args&&... args)
{
    using U = std::remove_cvref_t<T>;
    return basic_property<U, P>(std::forward<Args>(args)...);
}

template<basic_property_type T, typename U, typename P = basic_property_policy<T>>
requires (std::constructible_from<T, std::initializer_list<U>>)
[[nodiscard]] auto make_property(std::initializer_list<U> l)
{
    using V = std::remove_cvref_t<T>;
    return basic_property<V, P>(l);
}

[[nodiscard]] auto make_property(const char* str)
{
    return basic_property(str);
}

[[nodiscard]] auto make_property(const wchar_t* str)
{
    return basic_property(str);
}

template<typename T>
requires (std::is_pointer_v<T>)
[[nodiscard]] auto make_property(T ptr)
{
    return basic_property(ptr);
}

template<typename Iterator>
requires (std::input_iterator<Iterator>)
[[nodiscard]] auto make_property(Iterator begin, Iterator end)
{
    return basic_property(begin, end);
}



template<typename T, typename P>
[[nodiscard]] decltype(auto) strip(basic_property<T, P>& prop)
{
    return static_cast<T&>(prop);
}

template<typename T, typename P>
[[nodiscard]] decltype(auto) strip(const basic_property<T, P>& prop)
{
    return static_cast<const T&>(prop);
}

template<typename T, typename P>
[[nodiscard]] decltype(auto) strip(const volatile basic_property<T, P>& prop)
{
    return static_cast<const T&>(prop);
}



template<typename T, typename P>
[[nodiscard]] const volatile basic_property<T, P>& as_volatile(const basic_property<T, P>& prop)
{
    return const_cast<const volatile basic_property<T, P>&>(prop);
}



#if defined(DEFINE_PROPERTY_OPERATOR_UNARY)
#undef DEFINE_PROPERTY_OPERATOR_UNARY
#endif

#define DEFINE_PROPERTY_OPERATOR_UNARY(PT, OP) \
    template<typename T> \
    [[nodiscard]] auto operator OP (const PT<T>& prop) -> \
        PT<decltype(OP std::declval<T>())> \
            { return { OP prop.get() }; }

#if defined(DEFINE_PROPERTY_OPERATOR_BINARY)
#undef DEFINE_PROPERTY_OPERATOR_BINARY
#endif

#define DEFINE_PROPERTY_OPERATOR_BINARY(PT, OP) \
    template<typename T1, typename T2> \
    [[nodiscard]] auto operator OP (const PT<T1>& lhs, const PT<T2>& rhs) -> \
        PT<decltype(std::declval<T1>() OP std::declval<T2>())> \
            { return { lhs.get() OP rhs.get() }; } \
    \
    template<typename T, typename U> requires (not std::same_as<PT<T>, std::remove_cvref_t<U>>) \
    [[nodiscard]] auto operator OP (const PT<T>& lhs, const U& rhs) -> \
        PT<decltype(std::declval<T>() OP std::declval<U>())> \
            { return { lhs.get() OP rhs }; } \
    \
    template<typename U, typename T> requires (not std::same_as<std::remove_cvref_t<U>, PT<T>> \
    and not std::same_as<std::remove_cvref_t<U>, std::ostream> and not std::same_as<std::remove_cvref_t<U>, std::wostream> \
    and not std::same_as<std::remove_cvref_t<U>, std::istream> and not std::same_as<std::remove_cvref_t<U>, std::wistream>) \
    [[nodiscard]] auto operator OP (const U& lhs, const PT<T>& rhs) -> \
        PT<decltype(std::declval<U>() OP std::declval<T>())> \
            { return { lhs OP rhs.get() }; }



template<basic_property_type T> using property = basic_property<T, basic_property_policy<T>>;

DEFINE_PROPERTY_OPERATOR_UNARY(property, +);
DEFINE_PROPERTY_OPERATOR_UNARY(property, -);

DEFINE_PROPERTY_OPERATOR_BINARY(property, +);
DEFINE_PROPERTY_OPERATOR_BINARY(property, -);
DEFINE_PROPERTY_OPERATOR_BINARY(property, *);
DEFINE_PROPERTY_OPERATOR_BINARY(property, /);
DEFINE_PROPERTY_OPERATOR_BINARY(property, %)

DEFINE_PROPERTY_OPERATOR_UNARY(property, ~);

DEFINE_PROPERTY_OPERATOR_BINARY(property, &);
DEFINE_PROPERTY_OPERATOR_BINARY(property, |);
DEFINE_PROPERTY_OPERATOR_BINARY(property, ^);
DEFINE_PROPERTY_OPERATOR_BINARY(property, <<);
DEFINE_PROPERTY_OPERATOR_BINARY(property, >>);

#undef DEFINE_PROPERTY_OPERATOR_UNARY
#undef DEFINE_PROPERTY_OPERATOR_BINARY
