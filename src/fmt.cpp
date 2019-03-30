#include <iostream>
#include <string>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/time.h>
#include <fmt/color.h>
#include <fmt/printf.h>

using namespace std;
using namespace fmt;

int main()
{
	auto msg1 = fmt::format("The answer is {}", 42);
	auto msg2 = "{0}{1}"_format("The answer is ", 42);

	fmt::print("{}\n", msg1);
	fmt::print("{}\n", msg2);

	fmt::memory_buffer out;
	format_to(out, "The answer is {0}", "42");
	fmt::print("{}\n", out.data());
	fmt::print(cout, "{}\n", out.data());
	fmt::print(stdout, "{}\n", out.data());

	fmt::print("{1} {0}\n", 42, "The answer is");

	fmt::print("{first} {second}\n", fmt::arg("first", "The answer is"), fmt::arg("second", 42));
	fmt::print("{second} {first}\n", "second"_a="The answer is", "first"_a=42);

	fmt::printf("The answer is %.2f\n", 42.f);
	fmt::fprintf(cout, "The answer is %.2f\n", 42.f);
	fmt::fprintf(stdout, "The answer is %.2f\n", 42.f);
	auto msg3 = fmt::sprintf("The answer is %.2f\n", 42.f);
	fmt::printf("%s", msg3);

	fmt::print(fmt::emphasis::bold, "The text is bold\n");
	fmt::print(fmt::fg(fmt::color::red) | fmt::bg(fmt::color::green), "The color is red and green\n");

	std::time_t t = std::time(nullptr);
	fmt::print("The date is {:%Y-%m-%d}.\n", *std::localtime(&t));

	print("{:-<30}\n", "left aligned");
	print("{:->30}\n", "right aligned");
	print("{:-^30}\n", "centered");
}
