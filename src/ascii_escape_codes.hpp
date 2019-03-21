#pragma once

#include <ostream>

namespace ascii_escape_codes
{
	#define ENTRY(name, code) \
		static inline struct name##_opcode {} name; \
		inline std::ostream& operator << (std::ostream& os, const name##_opcode&) \
		{ os << "\x1b[" #code "m"; return os; }

	ENTRY(reset, 0)
	ENTRY(bold, 1)
	ENTRY(faint, 2)
	ENTRY(italic, 3)
	ENTRY(underline, 4)
	ENTRY(slow_blink, 5)
	ENTRY(rapid_blink, 6)
	ENTRY(inverse, 7)
	ENTRY(conceal, 8)
	ENTRY(crossed_out, 9)
	ENTRY(default_font, 10)
	ENTRY(alt_font_1, 11)
	ENTRY(alt_font_2, 12)
	ENTRY(alt_font_3, 13)
	ENTRY(alt_font_4, 14)
	ENTRY(alt_font_5, 15)
	ENTRY(alt_font_6, 16)
	ENTRY(alt_font_7, 17)
	ENTRY(alt_font_8, 18)
	ENTRY(alt_font_9, 19)
	ENTRY(fraktur, 20)
	ENTRY(doubly_underline, 21)
	ENTRY(normal, 22)
	ENTRY(not_italic_not_raktur, 23)
	ENTRY(underline_off, 24)
	ENTRY(blink_off, 25)
	ENTRY(inverse_off, 27)
	ENTRY(reveal, 28)
	ENTRY(not_crossed_out, 29)
	ENTRY(default_foreground_color, 39)
	ENTRY(default_background_color, 49)
	ENTRY(framed, 51)
	ENTRY(encircled, 52)
	ENTRY(overlined, 53)
	ENTRY(not_framed_or_encircled, 54)
	ENTRY(not_overlined, 55)

	#define COLOR_ENTRY(name, code) \
		static inline struct name##_color {} name; \
		inline std::ostream& operator << (std::ostream& os, const name##_color&) \
		{ os << "\x1b[" #code "m"; return os; }

	COLOR_ENTRY(black,   30)
	COLOR_ENTRY(red,     31)
	COLOR_ENTRY(green,   32)
	COLOR_ENTRY(yellow,  33)
	COLOR_ENTRY(blue,    34)
	COLOR_ENTRY(magenta, 35)
	COLOR_ENTRY(cyan,    36)
	COLOR_ENTRY(white,   37)

	COLOR_ENTRY(black_bg,   40)
	COLOR_ENTRY(red_bg,     41)
	COLOR_ENTRY(green_bg,   42)
	COLOR_ENTRY(yellow_bg,  43)
	COLOR_ENTRY(blue_bg,    44)
	COLOR_ENTRY(magenta_bg, 45)
	COLOR_ENTRY(cyan_bg,    46)
	COLOR_ENTRY(white_bg,   47)

	COLOR_ENTRY(bright_black,   90)
	COLOR_ENTRY(bright_red,     91)
	COLOR_ENTRY(bright_green,   92)
	COLOR_ENTRY(bright_yellow,  93)
	COLOR_ENTRY(bright_blue,    94)
	COLOR_ENTRY(bright_magenta, 95)
	COLOR_ENTRY(bright_cyan,    96)
	COLOR_ENTRY(bright_white,   97)

	COLOR_ENTRY(bright_black_bg,   100)
	COLOR_ENTRY(bright_red_bg,     101)
	COLOR_ENTRY(bright_green_bg,   102)
	COLOR_ENTRY(bright_yellow_bg,  103)
	COLOR_ENTRY(bright_blue_bg,    104)
	COLOR_ENTRY(bright_magenta_bg, 105)
	COLOR_ENTRY(bright_cyan_bg,    106)
	COLOR_ENTRY(bright_white_bg,   107)

	struct color_n
	{
		color_n(unsigned char n) : m_n(n) {}
		friend std::ostream& operator << (std::ostream& os, const color_n& n);
	private:
		unsigned char m_n;
	};

	struct color_bg_n
	{
		color_bg_n(unsigned char n) : m_n(n) {}
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
