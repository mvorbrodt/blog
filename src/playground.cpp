#include <iostream>
#include <bitset>

#include <stdexcept>
#include <cstddef>
#include <cstdint>

template<std::size_t Pos>
class Bit
{
public:
	inline static constexpr std::size_t Position = Pos;

	Bit(bool val) noexcept : value(val) {}

	operator bool () const noexcept { return value; }

private:
	bool value;
};

template<typename MaskT>
class BitMask
{
public:
	inline static constexpr std::size_t Bits = sizeof(MaskT) * 8;

	BitMask(MaskT val = 0) noexcept : value(val) {}

	template<std::size_t Pos, typename... Bits>
	BitMask(Bit<Pos> first, Bits&&... next) noexcept
	{ SetBit(first); (SetBit(next),...); }

	template<std::size_t Pos>
	bool Get() const noexcept
	{
		static_assert(Pos < Bits, "Bit out of range!");
		return (value >> Pos) & 1;
	}

	template<std::size_t Pos>
	bool Set(bool val) noexcept
	{
		auto old_val = Get<Pos>();
		SetBit(Bit<Pos>(val));
		return old_val;
	}

	template<std::size_t Pos>
	bool Flip() noexcept
	{
		static_assert(Pos < Bits, "Bit out of range!");
		auto old_val = Get<Pos>();
		value ^= MaskT(1) << Pos;
		return old_val;
	}

	void ClearAll() noexcept { SetAll(0); }

	void SetAll(bool val) noexcept { value = val ? ~MaskT(0) : MaskT(0); }

	void FlipAll() noexcept { value ^= ~MaskT(0); }

	auto At(std::size_t pos)
	{
		if(pos >= Bits)
			throw std::out_of_range("Bit out of range!");
		return Proxy{ *this, pos };
	}

	bool At(std::size_t pos) const
	{
		if(pos >= Bits)
			throw std::out_of_range("Bit out of range!");
		return (value >> pos) & 1;
	}

	auto operator [] (std::size_t pos) noexcept { return Proxy{ *this, pos }; }

	bool operator [] (std::size_t pos) const noexcept { return (value >> pos) & 1; }

	operator MaskT () const noexcept { return value; }

private:
	template<std::size_t Pos>
	void SetBit(const Bit<Pos>& bit) noexcept
	{
		static_assert(Pos < Bits, "Bit out of range!");
		value &= ~(MaskT(1) << Pos);
		value |= (MaskT(bit) << Pos);
	}

	struct Proxy
	{
		BitMask& mask;
		std::size_t pos;

		operator bool () const noexcept { return (mask.value >> pos) & 1; }

		Proxy& operator = (bool val) noexcept
		{
			mask.value &= ~(MaskT(1) << pos);
			mask.value |= (MaskT(val) << pos);
			return *this;
		}
	};

	MaskT value = 0;
};

int main()
{
	using namespace std;

	auto m8  = BitMask<uint8_t>(Bit<0>(1), Bit<7>(1));
	auto m16 = BitMask<uint16_t>(Bit<0>(1), Bit<2>(1), Bit<4>(1), Bit<5>(1), Bit<14>(1), Bit<15>(1));
	auto m32 = BitMask<uint32_t>(Bit<0>(1), Bit<4>(1), Bit<8>(1), Bit<15>(1), Bit<29>(1), Bit<30>(1));
	auto m64 = BitMask<uint64_t>(Bit<0>(1), Bit<63>(1));

	cout << bitset<decltype(m8)::Bits>(m8) << endl;
	cout << bitset<decltype(m16)::Bits>(m16) << endl;
	cout << bitset<decltype(m32)::Bits>(m32) << endl;
	cout << bitset<decltype(m64)::Bits>(m64) << endl << endl;

	m64.Flip<0>();
	m64.Flip<1>();
	cout << bitset<decltype(m64)::Bits>(m64) << endl;
	m64.FlipAll();
	cout << bitset<decltype(m64)::Bits>(m64) << endl;
	m64.SetAll(0);
	cout << bitset<decltype(m64)::Bits>(m64) << endl;
	m64.SetAll(1);
	cout << bitset<decltype(m64)::Bits>(m64) << endl;
	m64.ClearAll();
	cout << bitset<decltype(m64)::Bits>(m64) << endl << endl;

	auto m = BitMask<uint8_t>(0b10101010);
	m.Set<0>(1);
	m.Set<1>(1);
	m.Set<7>(1);
	cout << m.Get<7>() << " " << m.Get<6>() << " " << m.Get<5>() << " " << m.Get<4>() << " " << m.Get<3>() << " " << m.Get<2>() << " " << m.Get<1>() << " " << m.Get<0>() << endl;
	cout << m[7] << " " << m[6] << " " << m[5] << " " << m[4] << " " << m[3] << " " << m[2] << " " << m[1] << " " << m[0] << endl << endl;

	m = 0b11111111;
	cout << m.At(7) << " " << m[6] << " " << m[5] << " " << m[4] << " " << m[3] << " " << m[2] << " " << m[1] << " " << m[0] << endl;
	m.At(7) = 0;
	cout << m.At(7) << " " << m[6] << " " << m[5] << " " << m[4] << " " << m[3] << " " << m[2] << " " << m[1] << " " << m[0] << endl << endl;

	const auto n = m;
	m = n;
	[[maybe_unused]] auto bbb = n[0];
	[[maybe_unused]] auto ccc = n.At(0);
	(m[0] = 1) = 0;
	cout << bitset<decltype(m)::Bits>(m) << endl;
	cout << bitset<decltype(n)::Bits>(n) << endl;

	auto b1 = Bit<123>(0);
	auto b2 = Bit<321>(1);
	[[maybe_unused]] auto x = b1 && b2;
	[[maybe_unused]] auto eq = b1 == b2;
	[[maybe_unused]] auto ne = b1 != b2;

	b1 = 0;
	if(!b1) cout << "!b1" << endl;
	if(b2) cout << "b2" << endl;
}