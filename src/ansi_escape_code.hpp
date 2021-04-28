#pragma once

#include <ostream>



#define ANSI_ESCAPE_CODE(name, code) \
	inline struct ansi_escape_code_##name \
	{ \
		inline friend std::ostream& operator << (std::ostream& os, ansi_escape_code_##name) \
		{ \
			os << "\x1B[" #code "m"; \
			return os; \
		} \
		inline friend std::wostream& operator << (std::wostream& wos, ansi_escape_code_##name) \
		{ \
			wos << L"\x1B[" #code L"m"; \
			return wos; \
		} \
	} name;



namespace ansi_escape_code
{
	ANSI_ESCAPE_CODE(reset, 0)
	ANSI_ESCAPE_CODE(bold, 1)
	ANSI_ESCAPE_CODE(faint, 2)
	ANSI_ESCAPE_CODE(italic, 3)
	ANSI_ESCAPE_CODE(underline, 4)
	ANSI_ESCAPE_CODE(slow_blink, 5)
	ANSI_ESCAPE_CODE(rapid_blink, 6)
	ANSI_ESCAPE_CODE(inverse, 7)
	ANSI_ESCAPE_CODE(conceal, 8)
	ANSI_ESCAPE_CODE(crossed_out, 9)
	ANSI_ESCAPE_CODE(default_font, 10)
	ANSI_ESCAPE_CODE(alt_font_1, 11)
	ANSI_ESCAPE_CODE(alt_font_2, 12)
	ANSI_ESCAPE_CODE(alt_font_3, 13)
	ANSI_ESCAPE_CODE(alt_font_4, 14)
	ANSI_ESCAPE_CODE(alt_font_5, 15)
	ANSI_ESCAPE_CODE(alt_font_6, 16)
	ANSI_ESCAPE_CODE(alt_font_7, 17)
	ANSI_ESCAPE_CODE(alt_font_8, 18)
	ANSI_ESCAPE_CODE(alt_font_9, 19)
	ANSI_ESCAPE_CODE(fraktur, 20)
	ANSI_ESCAPE_CODE(doubly_underline, 21)
	ANSI_ESCAPE_CODE(normal, 22)
	ANSI_ESCAPE_CODE(not_italic_not_raktur, 23)
	ANSI_ESCAPE_CODE(underline_off, 24)
	ANSI_ESCAPE_CODE(blink_off, 25)
	ANSI_ESCAPE_CODE(inverse_off, 27)
	ANSI_ESCAPE_CODE(reveal, 28)
	ANSI_ESCAPE_CODE(not_crossed_out, 29)
	ANSI_ESCAPE_CODE(black, 30)
	ANSI_ESCAPE_CODE(red, 31)
	ANSI_ESCAPE_CODE(green, 32)
	ANSI_ESCAPE_CODE(yellow, 33)
	ANSI_ESCAPE_CODE(blue, 34)
	ANSI_ESCAPE_CODE(magenta, 35)
	ANSI_ESCAPE_CODE(cyan, 36)
	ANSI_ESCAPE_CODE(white, 37)
	ANSI_ESCAPE_CODE(default_foreground_color, 39)
	ANSI_ESCAPE_CODE(black_bg, 40)
	ANSI_ESCAPE_CODE(red_bg, 41)
	ANSI_ESCAPE_CODE(green_bg, 42)
	ANSI_ESCAPE_CODE(yellow_bg, 43)
	ANSI_ESCAPE_CODE(blue_bg, 44)
	ANSI_ESCAPE_CODE(magenta_bg, 45)
	ANSI_ESCAPE_CODE(cyan_bg, 46)
	ANSI_ESCAPE_CODE(white_bg, 47)
	ANSI_ESCAPE_CODE(default_background_color, 49)
	ANSI_ESCAPE_CODE(framed, 51)
	ANSI_ESCAPE_CODE(encircled, 52)
	ANSI_ESCAPE_CODE(overlined, 53)
	ANSI_ESCAPE_CODE(not_framed_or_encircled, 54)
	ANSI_ESCAPE_CODE(not_overlined, 55)
	ANSI_ESCAPE_CODE(bright_black, 90)
	ANSI_ESCAPE_CODE(bright_red, 91)
	ANSI_ESCAPE_CODE(bright_green, 92)
	ANSI_ESCAPE_CODE(bright_yellow, 93)
	ANSI_ESCAPE_CODE(bright_blue, 94)
	ANSI_ESCAPE_CODE(bright_magenta, 95)
	ANSI_ESCAPE_CODE(bright_cyan, 96)
	ANSI_ESCAPE_CODE(bright_white, 97)
	ANSI_ESCAPE_CODE(bright_black_bg, 100)
	ANSI_ESCAPE_CODE(bright_red_bg, 101)
	ANSI_ESCAPE_CODE(bright_green_bg, 102)
	ANSI_ESCAPE_CODE(bright_yellow_bg, 103)
	ANSI_ESCAPE_CODE(bright_blue_bg, 104)
	ANSI_ESCAPE_CODE(bright_magenta_bg, 105)
	ANSI_ESCAPE_CODE(bright_cyan_bg, 106)
	ANSI_ESCAPE_CODE(bright_white_bg, 107)



	struct color_n
	{
		explicit color_n(unsigned char n) : m_n(n) {}

		inline friend std::ostream& operator << (std::ostream& os, color_n n)
		{
			os << "\x1B[38;5;" << (int)n.m_n << "m";
			return os;
		}

		inline friend std::wostream& operator << (std::wostream& wos, color_n n)
		{
			wos << L"\x1B[38;5;" << (int)n.m_n << L"m";
			return wos;
		}

	private:
		unsigned char m_n;
	};



	struct color_bg_n
	{
		explicit color_bg_n(unsigned char n) : m_n(n) {}

		inline friend std::ostream& operator << (std::ostream& os, color_bg_n n)
		{
			os << "\x1B[48;5;" << (int)n.m_n << "m";
			return os;
		}

		inline friend std::wostream& operator << (std::wostream& wos, color_bg_n n)
		{
			wos << L"\x1B[48;5;" << (int)n.m_n << L"m";
			return wos;
		}

	private:
		unsigned char m_n;
	};



	struct color_rgb
	{
		color_rgb(unsigned char r, unsigned char g, unsigned char b) : m_r(r), m_g(g), m_b(b) {}

		inline friend std::ostream& operator << (std::ostream& os, color_rgb c)
		{
			os << "\x1B[38;2;" << (int)c.m_r << ";" << (int)c.m_g << ";" << (int)c.m_b << "m";
			return os;
		}

		inline friend std::wostream& operator << (std::wostream& wos, color_rgb c)
		{
			wos << L"\x1B[38;2;" << (int)c.m_r << L";" << (int)c.m_g << L";" << (int)c.m_b << L"m";
			return wos;
		}

	private:
		unsigned char m_r, m_g, m_b;
	};



	struct color_bg_rgb
	{
		color_bg_rgb(unsigned char r, unsigned char g, unsigned char b) : m_r(r), m_g(g), m_b(b) {}

		inline friend std::ostream& operator << (std::ostream& os, color_bg_rgb c)
		{
			os << "\x1B[48;2;" << (int)c.m_r << ";" << (int)c.m_g << ";" << (int)c.m_b << "m";
			return os;
		}

		inline friend std::wostream& operator << (std::wostream& wos, color_bg_rgb c)
		{
			wos << L"\x1B[48;2;" << (int)c.m_r << L";" << (int)c.m_g << L";" << (int)c.m_b << L"m";
			return wos;
		}

	private:
		unsigned char m_r, m_g, m_b;
	};
}
