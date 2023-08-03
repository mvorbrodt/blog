#include <iostream>
#include <memory>
#include <utility>
#include <typeinfo>
#include <type_traits>



class i_boxed
{
public:
	virtual ~i_boxed() = default;

	virtual std::size_t hash() const = 0;

	virtual std::unique_ptr<i_boxed> clone() const = 0;
};



template<typename T>
class boxed : public i_boxed
{
public:
	template<typename U = T>
	explicit boxed(U&& v) : m_v { std::forward<U>(v) } {}

	virtual std::size_t hash() const final override { return typeid(T).hash_code(); }

	virtual std::unique_ptr<i_boxed> clone() const final override { return std::make_unique<boxed<T>>(*this); }

	T& value() { return m_v; }

	const T& value() const { return m_v; }

private:
	T m_v;
};



class box
{
public:
	template<typename T>
	explicit box(T&& v) : m_v{ std::make_unique<boxed<T>>(std::forward<T>(v)) } {}

	box(const box& o) : m_v{ o.m_v->clone() } {}

	box(box&&) = default;

	box& operator = (const box& o)
	{
		if (this == &o)
			return *this;

		m_v = o.m_v->clone();
		return *this;
	}

	box& operator = (box&&) = default;

	operator bool() const { return !!m_v; }

	bool operator ! () const { return !m_v; }

	template<typename T>
	[[nodiscard]] friend T& unbox(box& a)
	{
		if (typeid(T).hash_code() != a.m_v->hash())
			throw std::bad_cast();

		return static_cast<boxed<T>*>(a.m_v.get())->value();
	}

	template<typename T>
	[[nodiscard]] friend const T& unbox(const box& a)
	{
		if (typeid(T).hash_code() != a.m_v->hash())
			throw std::bad_cast();

		return static_cast<const boxed<T>*>(a.m_v.get())->value();
	}

private:
	std::unique_ptr<i_boxed> m_v;
};



int main()
{
	using namespace std;

	try
	{
		box x(1);
		box y(2);
		const box z(3);

		cout << unbox<int>(x) << endl;
		cout << unbox<int>(y) << endl;
		cout << unbox<int>(z) << endl;

		box a = x;
		box b = std::move(y);

		y = b;
		x = std::move(y);

		if (x) cout << "x has value!" << endl;
		if (!y) cout << "y has NO value!" << endl;

		[[maybe_unused]] auto q = unbox<float>(x);
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
	}
}
