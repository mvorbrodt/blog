#include <iostream>
#include <thread>
#include <cstdlib>
#include <liboath/oath.h>
#include "ansi_escape_code.hpp"

using namespace std;
using namespace ansi_escape_code;

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
			<< slow_blink << " (" << left << ")" << reset;
		cout.flush();

		this_thread::sleep_for(1s);
		cout << "\r";
	}

	oath_done();

	return 1;
}
