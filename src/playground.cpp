#include <iostream>
#include <cstdint>

#include <stdexcept>
#include <ostream>
#include <bitset>

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

	explicit BitMask(MaskT val = 0) noexcept : value(val) {}

	template<std::size_t Pos, typename... Bits>
	explicit BitMask(const Bit<Pos>& first, Bits&&... next) noexcept { SetBit(first); (SetBit(next),...); }

	template<std::size_t Pos>
	bool Get() const noexcept
	{
		static_assert(Pos < Bits, "Bit out of range!");
		return (value >> Pos) & 1;
	}

	template<std::size_t Pos>
	bool Set(bool val = 1) noexcept
	{
		bool old_val = Get<Pos>();
		SetBit(Bit<Pos>(val));
		return old_val;
	}

	template<std::size_t Pos>
	bool Flip() noexcept
	{
		static_assert(Pos < Bits, "Bit out of range!");
		bool old_val = Get<Pos>();
		value ^= MaskT(1) << Pos;
		return old_val;
	}

	auto Get() const noexcept { return value; }

	void Set(bool val = 1) noexcept { value = val ? ~MaskT(0) : MaskT(0); }

	void Flip() noexcept { value = ~value; }

	void Clear() noexcept { Set(0); }

	auto At(std::size_t pos)
	{
		if(pos >= Bits)
			throw std::out_of_range("Bit out of range!");
		return StackProxy(*this, pos);
	}

	bool At(std::size_t pos) const
	{
		if(pos >= Bits)
			throw std::out_of_range("Bit out of range!");
		return (value >> pos) & 1;
	}

	explicit operator MaskT () const noexcept { return value; }

	bool operator ! () const noexcept { return !value; }
	explicit operator bool () const noexcept { return value; }

	auto operator [] (std::size_t pos) noexcept { return StackProxy(*this, pos); }
	bool operator [] (std::size_t pos) const noexcept { return (value >> pos) & 1; }

	auto operator ~ () const noexcept { return BitMask(~value); }

	auto& operator &= (const BitMask& rhs) noexcept { value &= rhs.value; return *this; }
	auto& operator |= (const BitMask& rhs) noexcept { value |= rhs.value; return *this; }
	auto& operator ^= (const BitMask& rhs) noexcept { value ^= rhs.value; return *this; }

	auto& operator <<= (std::size_t n) noexcept { value <<= n; return *this; }
	auto& operator >>= (std::size_t n) noexcept { value >>= n; return *this; }

	friend auto operator & (const BitMask& lhs, const BitMask& rhs) noexcept { return BitMask(lhs.value & rhs.value); }
	friend auto operator | (const BitMask& lhs, const BitMask& rhs) noexcept { return BitMask(lhs.value | rhs.value); }
	friend auto operator ^ (const BitMask& lhs, const BitMask& rhs) noexcept { return BitMask(lhs.value ^ rhs.value); }

	friend auto operator << (const BitMask& lhs, std::size_t n) noexcept { return BitMask(lhs.value << n); }
	friend auto operator >> (const BitMask& lhs, std::size_t n) noexcept { return BitMask(lhs.value >> n); }

	friend bool operator == (const BitMask& lhs, const BitMask& rhs) noexcept { return lhs.value == rhs.value; }
	friend bool operator != (const BitMask& lhs, const BitMask& rhs) noexcept { return lhs.value != rhs.value; }

	template<std::size_t Pos> auto& operator &= (const Bit<Pos>& rhs) noexcept { SetBit(Bit<Pos>(Get<Pos>() & rhs)); return *this; }
	template<std::size_t Pos> auto& operator |= (const Bit<Pos>& rhs) noexcept { SetBit(Bit<Pos>(Get<Pos>() | rhs)); return *this; }
	template<std::size_t Pos> auto& operator ^= (const Bit<Pos>& rhs) noexcept { SetBit(Bit<Pos>(Get<Pos>() ^ rhs)); return *this; }

	template<std::size_t Pos> friend auto operator & (const BitMask& lhs, const Bit<Pos>& rhs) noexcept { return BitMask(lhs) &= rhs; }
	template<std::size_t Pos> friend auto operator | (const BitMask& lhs, const Bit<Pos>& rhs) noexcept { return BitMask(lhs) |= rhs; }
	template<std::size_t Pos> friend auto operator ^ (const BitMask& lhs, const Bit<Pos>& rhs) noexcept { return BitMask(lhs) ^= rhs; }

	friend std::ostream& operator << (std::ostream& os, const BitMask& rhs)
	{
		os << std::bitset<Bits>(rhs.value);
		return os;
	}

private:
	template<std::size_t Pos>
	void SetBit(const Bit<Pos>& bit) noexcept
	{
		static_assert(Pos < Bits, "Bit out of range!");
		value &= ~(MaskT(1) << Pos);
		value |= (MaskT(bool(bit)) << Pos);
	}

	class StackProxy
	{
	public:
		StackProxy(BitMask& m, std::size_t p) noexcept : mask(m), pos(p) {}
		StackProxy(const StackProxy&) = delete;
		StackProxy(StackProxy&&) = delete;

		StackProxy& operator = (const StackProxy& rhs) noexcept
		{
			if(mask != rhs.mask or pos != rhs.pos)
			{
				mask.value &= ~(MaskT(1) << pos);
				mask.value |= (MaskT(bool(rhs)) << pos);
			}
			return *this;
		}

		StackProxy& operator = (bool val) noexcept
		{
			mask.value &= ~(MaskT(1) << pos);
			mask.value |= (MaskT(val) << pos);
			return *this;
		}

		explicit operator bool () const noexcept { return (mask.value >> pos) & 1; }

		friend std::ostream& operator << (std::ostream& os, const StackProxy& rhs)
		{
			os << bool(rhs);
			return os;
		}

	private:
		BitMask& mask;
		std::size_t pos;
	};

	MaskT value = 0;
};

int main()
{
	using namespace std;

	auto m1 = BitMask<uint8_t>(0b11110000);
	auto m2 = BitMask<uint8_t>(Bit<0>(1), Bit<1>(1), Bit<2>(1), Bit<3>(1));
	cout << m1 << endl;
	cout << m2 << endl << endl;

	m1.Flip<0>();
	m2.Flip<0>();
	cout << m1 << endl;
	cout << m2 << endl << endl;

	m1.Flip();
	m2.Flip();
	cout << m1[7] << m1[6] << m1[5] << m1[4] << m1[3] << m1[2] << m1[1] << m1[0] << endl;
	cout << m2.Get<7>() << m2.Get<6>() << m2.Get<5>() << m2.Get<4>() << m2.Get<3>() << m2.Get<2>() << m2.Get<1>() << m2.Get<0>() << endl << endl;

	m1.Clear();
	m2.Set(0);
	m1[0] = m1[1] = m1[2] = m1[3] = m1[4] = m1[5] = m1[6] = m1[7] = 1;
	m1[0] = m1[0] = m1[1] = m1[1] = m1[2] = m1[2] = m1[3] = m1[3] = m1[4] = m1[4] = m1[5] = m1[5] = m1[6] = m1[6] = m1[7] = m1[7] = 1;
	if(!m2) cout << "if(!m2)..." << endl;
	m2 = m1;
	if(m2) cout << "if(m2)..." << endl;
	m2[0] = 0;
	m2.At(1) = 0;
	m2.Set<2>(0);

	cout << m1 << endl;
	cout << m2 << endl << endl;

	auto m3 = ~m2;
	cout << m3 << endl;
	m3.Flip();
	cout << m3 << endl;
	m3.Clear();
	if(!m3[1]) cout << "if(!m3[1])" << endl;
	cout << m3 << endl;
	m3.Set();
	if(m3[1]) cout << "if(m3[1])" << endl;
	cout << m3 << endl << endl;

	auto b1 = Bit<123>(1);
	auto b2 = Bit<321>(0);
	if(b1) cout << "if(b1)" << endl;
	if(!b2) cout << "if(!b2)" << endl;
}