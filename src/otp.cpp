#include <iostream>
#include <string>
#include <thread>
#include <cstdlib>
#include <liboath/oath.h>

using namespace std;

int main(int argc, char** argv)
{
	const char secret[] = "00112233445566778899";

	while(true)
	{
		auto t = time(NULL);
		auto left = 30 - (t % 30);

		char otp[7] = {};
		auto result = oath_totp_generate(
			secret,
			sizeof(secret),
			t,
			OATH_TOTP_DEFAULT_TIME_STEP_SIZE,
			OATH_TOTP_DEFAULT_START_TIME,
			6,
			otp);

		if(result == OATH_OK)
			cout << "OTP: " << otp << " (" << left << ") \r";
		cout.flush();

		this_thread::sleep_for(1s);
	}

	return 1;
}
