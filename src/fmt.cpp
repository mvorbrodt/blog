#include <string>
#include <fmt/core.h>
#include <fmt/time.h>
#include <fmt/color.h>

using namespace std;
using namespace fmt;

int main()
{
	auto msg = fmt::format("The answer is {}.", 42);
	fmt::print("{}\n", msg);

	fmt::memory_buffer out;
	format_to(out, "For a moment, {} happened.", "nothing");
	fmt::print("{}\n", out.data());

	fmt::print(stderr, "Don't {}\n", "panic");

	fmt::print("I'd rather be {1} than {0}.\n", "right", "happy");

	fmt::print("Hello, {name}! The answer is {number}.\n", fmt::arg("name", "Martin"), fmt::arg("number", 42));
	fmt::print("Hello, {name}! The answer is {number}.\n", "name"_a="Martin", "number"_a=42);

	msg = "{0}{1}{0}"_format("abra", "cad");
	fmt::print("{}\n", msg);

	try
	{
		fmt::format("The answer is {:d}", "forty-two");
	}
	catch(fmt::format_error& e)
	{
		fmt::print("{}\n", e.what());
	}

	std::time_t t = std::time(nullptr);
	fmt::print(fmt::fg(color::red) | fmt::bg(fmt::color::green), "The date is {:%Y-%m-%d}.\n", *std::localtime(&t));
}
