#include <iostream>
#include <string>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/chrono.h>
#include <fmt/compile.h>
#include <fmt/locale.h>
#include <fmt/ostream.h>
#include <fmt/posix.h>
#include <fmt/printf.h>

using namespace std;
using namespace fmt;

int main()
{
	// Format into a std::string
	auto msg1 = fmt::format("The answer is {}", 42);
	auto msg2 = "{0}{1}"_format("The answer is ", 42);

	// Print std::string
	fmt::print("{}\n", msg1);
	fmt::print("{}\n", msg2);

	// Format into memory buffer and print
	fmt::memory_buffer out;
	format_to(out, "The answer is {0}", "42");
	auto msg3 = string(out.begin(), out.end());
	fmt::print("{}\n", msg3);

	// Reverse order of parameters,
	// print to various outputs
	fmt::print("{1} {0}\n", 42, "The answer is");
	fmt::print(cout, "{1} {0}\n", 42, "The answer is");
	fmt::print(stdout, "{1} {0}\n", 42, "The answer is");

	// Named arguments
	fmt::print("{first} {second}\n", fmt::arg("first", "The answer is"), fmt::arg("second", 42));
	fmt::print("{second} {first}\n", "second"_a="The answer is", "first"_a=42);

	// printf style formatting
	fmt::printf("The answer is %.2f\n", 42.f);
	fmt::fprintf(cout, "The answer is %.2f\n", 42.f);
	fmt::fprintf(stdout, "The answer is %.2f\n", 42.f);

	// printf style formatting into a std::string
	auto msg4 = fmt::sprintf("The answer is %.2f\n", 42.f);
	fmt::printf("%s", msg4);

	// Text color and style manipulation
	fmt::print(fmt::emphasis::bold, "The text is bold\n");
	fmt::print(fmt::fg(fmt::color::red) | fmt::bg(fmt::color::green), "The color is red and green\n");

	// Date and time formatting
	std::time_t t = std::time(nullptr);
	fmt::print("The date and time is {:%Y-%m-%d %H:%M:%S}\n", *std::localtime(&t));

	// Alignment
	fmt::print("{:-<30}\n", "left aligned");
	fmt::print("{:->30}\n", "right aligned");
	fmt::print("{:-^30}\n", "centered");
}
