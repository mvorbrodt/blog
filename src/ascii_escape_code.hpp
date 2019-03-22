#pragma once

#include <ostream>

#define ASCII_ESCAPE_CODE(name, code) \
	static inline struct name##_ascii_escape_code {} name; \
	inline std::ostream& operator << (std::ostream& os, const name##_ascii_escape_code&) \
	{ os << "\x1b[" #code "m"; return os; }

namespace ascii_escape_code
{
	ASCII_ESCAPE_CODE(reset, 0)
	ASCII_ESCAPE_CODE(bold, 1)
	ASCII_ESCAPE_CODE(faint, 2)
	ASCII_ESCAPE_CODE(italic, 3)
	ASCII_ESCAPE_CODE(underline, 4)
	ASCII_ESCAPE_CODE(slow_blink, 5)
	ASCII_ESCAPE_CODE(rapid_blink, 6)
	ASCII_ESCAPE_CODE(inverse, 7)
	ASCII_ESCAPE_CODE(conceal, 8)
	ASCII_ESCAPE_CODE(crossed_out, 9)
	ASCII_ESCAPE_CODE(default_font, 10)
	ASCII_ESCAPE_CODE(alt_font_1, 11)
	ASCII_ESCAPE_CODE(alt_font_2, 12)
	ASCII_ESCAPE_CODE(alt_font_3, 13)
	ASCII_ESCAPE_CODE(alt_font_4, 14)
	ASCII_ESCAPE_CODE(alt_font_5, 15)
	ASCII_ESCAPE_CODE(alt_font_6, 16)
	ASCII_ESCAPE_CODE(alt_font_7, 17)
	ASCII_ESCAPE_CODE(alt_font_8, 18)
	ASCII_ESCAPE_CODE(alt_font_9, 19)
	ASCII_ESCAPE_CODE(fraktur, 20)
	ASCII_ESCAPE_CODE(doubly_underline, 21)
	ASCII_ESCAPE_CODE(normal, 22)
	ASCII_ESCAPE_CODE(not_italic_not_raktur, 23)
	ASCII_ESCAPE_CODE(underline_off, 24)
	ASCII_ESCAPE_CODE(blink_off, 25)
	ASCII_ESCAPE_CODE(inverse_off, 27)
	ASCII_ESCAPE_CODE(reveal, 28)
	ASCII_ESCAPE_CODE(not_crossed_out, 29)
	ASCII_ESCAPE_CODE(default_foreground_color, 39)
	ASCII_ESCAPE_CODE(default_background_color, 49)
	ASCII_ESCAPE_CODE(framed, 51)
	ASCII_ESCAPE_CODE(encircled, 52)
	ASCII_ESCAPE_CODE(overlined, 53)
	ASCII_ESCAPE_CODE(not_framed_or_encircled, 54)
	ASCII_ESCAPE_CODE(not_overlined, 55)

	ASCII_ESCAPE_CODE(black, 30)
	ASCII_ESCAPE_CODE(red, 31)
	ASCII_ESCAPE_CODE(green, 32)
	ASCII_ESCAPE_CODE(yellow, 33)
	ASCII_ESCAPE_CODE(blue, 34)
	ASCII_ESCAPE_CODE(magenta, 35)
	ASCII_ESCAPE_CODE(cyan, 36)
	ASCII_ESCAPE_CODE(white, 37)

	ASCII_ESCAPE_CODE(black_bg, 40)
	ASCII_ESCAPE_CODE(red_bg, 41)
	ASCII_ESCAPE_CODE(green_bg, 42)
	ASCII_ESCAPE_CODE(yellow_bg, 43)
	ASCII_ESCAPE_CODE(blue_bg, 44)
	ASCII_ESCAPE_CODE(magenta_bg, 45)
	ASCII_ESCAPE_CODE(cyan_bg, 46)
	ASCII_ESCAPE_CODE(white_bg, 47)

	ASCII_ESCAPE_CODE(bright_black, 90)
	ASCII_ESCAPE_CODE(bright_red, 91)
	ASCII_ESCAPE_CODE(bright_green, 92)
	ASCII_ESCAPE_CODE(bright_yellow, 93)
	ASCII_ESCAPE_CODE(bright_blue, 94)
	ASCII_ESCAPE_CODE(bright_magenta, 95)
	ASCII_ESCAPE_CODE(bright_cyan, 96)
	ASCII_ESCAPE_CODE(bright_white, 97)

	ASCII_ESCAPE_CODE(bright_black_bg, 100)
	ASCII_ESCAPE_CODE(bright_red_bg, 101)
	ASCII_ESCAPE_CODE(bright_green_bg, 102)
	ASCII_ESCAPE_CODE(bright_yellow_bg, 103)
	ASCII_ESCAPE_CODE(bright_blue_bg, 104)
	ASCII_ESCAPE_CODE(bright_magenta_bg, 105)
	ASCII_ESCAPE_CODE(bright_cyan_bg, 106)
	ASCII_ESCAPE_CODE(bright_white_bg, 107)

	struct color_n
	{
		explicit color_n(unsigned char n) : m_n(n) {}
		friend std::ostream& operator << (std::ostream& os, const color_n& n);
	private:
		unsigned char m_n;
	};

	struct color_bg_n
	{
		explicit color_bg_n(unsigned char n) : m_n(n) {}
		friend std::ostream& operator << (std::ostream& os, const color_bg_n& n);
	private:
		unsigned char m_n;
	};

	inline std::ostream& operator << (std::ostream& os, const color_n& n)
	{ os << "\x1b[38;5;" << (int)n.m_n << "m"; return os; }

	inline std::ostream& operator << (std::ostream& os, const color_bg_n& n)
	{ os << "\x1b[48;5;" << (int)n.m_n << "m"; return os; }

	struct color_rgb
	{
		color_rgb(unsigned char r, unsigned char g, unsigned char b) : m_r(r), m_g(g), m_b(b) {}
		friend std::ostream& operator << (std::ostream& os, const color_rgb& c);
	private:
		unsigned char m_r, m_g, m_b;
	};

	struct color_bg_rgb
	{
		color_bg_rgb(unsigned char r, unsigned char g, unsigned char b) : m_r(r), m_g(g), m_b(b) {}
		friend std::ostream& operator << (std::ostream& os, const color_bg_rgb& c);
	private:
		unsigned char m_r, m_g, m_b;
	};

	inline std::ostream& operator << (std::ostream& os, const color_rgb& c)
	{ os << "\x1b[38;2;" << (int)c.m_r << ";" << (int)c.m_g << ";" << (int)c.m_b << "m"; return os; }

	inline std::ostream& operator << (std::ostream& os, const color_bg_rgb& c)
	{ os << "\x1b[48;2;" << (int)c.m_r << ";" << (int)c.m_g << ";" << (int)c.m_b << "m"; return os; }
}
