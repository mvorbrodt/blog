#include <iostream>
#include <thread>
#include <cstdlib>
#include <liboath/oath.h>
#include "ascii_escape_codes.hpp"

using namespace std;
using namespace ascii_escape_codes;

int main(int argc, char** argv)
{
	auto result = oath_init();
	if(result != OATH_OK)
	{
		cerr << oath_strerror(result) << endl;
		return -1;
	}

	const char secret[] = "00112233445566778899";
	const auto time_step = OATH_TOTP_DEFAULT_TIME_STEP_SIZE;
	const auto digits = 6;

	while(true)
	{
		auto t = time(NULL);
		auto left = time_step - (t % time_step);

		char otp[digits + 1] = {};
		result = oath_totp_generate(
			secret,
			sizeof(secret),
			t,
			time_step,
			OATH_TOTP_DEFAULT_START_TIME,
			digits,
			otp);

		if(result != OATH_OK)
		{
			cerr << oath_strerror(result) << endl;
			return -1;
		}

		cout << bold << "OTP: " << reset
			<< bright_red << otp << reset
			<< slow_blink << " (" << left << ") \r" << reset;
		cout.flush();

		this_thread::sleep_for(1s);
	}

	oath_done();

	return 1;
}
