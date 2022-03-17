#include <iostream>
#include <memory>
#include <vector>
#include <utility>
#include <concepts>
#include <type_traits>

struct T1 { int x = 11; };
struct T2 { int y = 14; };
struct T3 { int z = 17; };

void foo(const T1& t) { std::cout << "foo T1: " << t.x << std::endl; }
void foo(const T2& t) { std::cout << "foo T2: " << t.y << std::endl; }
void foo(const T3& t) { std::cout << "foo T3: " << t.z << std::endl; }

void bar(const T1& t) { std::cout << "bar T1: " << t.x << std::endl; }
void bar(const T2& t) { std::cout << "bar T2: " << t.y << std::endl; }
void bar(const T3& t) { std::cout << "bar T3: " << t.z << std::endl; }

class TypeErased
{
public:
	template<typename T> using ByValue = std::remove_cvref_t<T>;

	template<typename T> requires(not std::same_as<TypeErased, ByValue<T>>)
	TypeErased(T&& v) : value_ptr(std::make_unique<Model<ByValue<T>>>(std::forward<T>(v))) {}

	TypeErased(const TypeErased& other) : value_ptr(other.value_ptr->Clone()) {}
	TypeErased(TypeErased&&) = default;

	TypeErased& operator = (TypeErased other) noexcept
	{
		swap(other);
		return *this;
	}

	void swap(TypeErased& other) noexcept { std::swap(value_ptr, other.value_ptr); }
	friend void swap(TypeErased& first, TypeErased& second) noexcept { first.swap(second); }

	friend void foo(const TypeErased& erased) { erased.value_ptr->foo(); }
	friend void bar(const TypeErased& erased) { erased.value_ptr->bar(); }

private:
	class Concept;
	using ConceptPtr = std::unique_ptr<Concept>;

	class Concept
	{
	public:
		virtual ~Concept() = default;
		virtual ConceptPtr Clone() const = 0;

		virtual void foo() const = 0;
		virtual void bar() const = 0;
	};

	template<typename T>
	class Model : public Concept
	{
	public:
		Model(const T& value) : my_value(value) {}
		Model(T&& value) : my_value(std::move(value)) {}

		virtual ConceptPtr Clone() const override { return std::make_unique<Model>(*this); }

		virtual void foo() const override { ::foo(my_value); }
		virtual void bar() const override { ::bar(my_value); }

	private:
		T my_value;
	};

	ConceptPtr value_ptr;
};

using TypeErasedList = std::vector<TypeErased>;

void foo_list(const TypeErasedList& erased_list) { for(const auto& erased : erased_list) foo(erased); }
void bar_list(const TypeErasedList& erased_list) { for(const auto& erased : erased_list) bar(erased); }

int main()
{
	auto e1 = TypeErased{ T1{1} };
	auto e2 = TypeErased{ T2{2} };
	auto e3 = TypeErased{ T3{3} };
	auto e4 = TypeErased{ e1 };
	auto e5 = TypeErased{ std::move(e2) };

	foo(e3);
	foo(e4);
	foo(e5);

	bar(e3);
	bar(e4);
	bar(e5);

	e1 = e4;
	e2 = std::move(e5);
	e3 = T3{3};

	auto l = TypeErasedList{ e1, e2, e3 };

	l.emplace_back(T1{});
	l.emplace_back(T2{});
	l.emplace_back(T3{});

	foo_list(l);
	bar_list(l);
}
