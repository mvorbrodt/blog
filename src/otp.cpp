#include <iostream>
#include <thread>
#include <cstdlib>
#include <liboath/oath.h>

using namespace std;

int main(int argc, char** argv)
{
	const char secret[] = "00112233445566778899";
	const auto time_step = OATH_TOTP_DEFAULT_TIME_STEP_SIZE;
	const auto digits = 6;

	while(true)
	{
		auto t = time(NULL);
		auto left = time_step - (t % time_step);

		char otp[digits + 1] = {};
		auto result = oath_totp_generate(
			secret,
			sizeof(secret),
			t,
			time_step,
			OATH_TOTP_DEFAULT_START_TIME,
			digits,
			otp);

		if(result == OATH_OK)
		{
			cout << "OTP: " << otp << " (" << left << ") \r";
			cout.flush();
		}

		this_thread::sleep_for(1s);
	}

	return 1;
}
