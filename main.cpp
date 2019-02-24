/*#include <stdio.h>
 #include <stdlib.h>
 #include <time.h>
 
 int main()
 {
 srand((unsigned int)time(NULL));
 for(int n = 0; n < 10; ++n)
 printf("%d ", rand() % 1000);
 printf("\n");
 }*/
#include <iostream>
#include <random>
#include <algorithm>
#include <vector>
#include <chrono>
using namespace std;
using namespace chrono;

const int COUNT = 1'000'000;

int main()
{
	/* random_device MAY BE CRYPTOGRAPHICALLY STRONG */
	random_device rd;
	/* SEED mt19937 and mt19937_64 ENGINE WITH STRONG RANDOM NUMBER */
	mt19937 mt(rd());
	mt19937_64 mt64(rd());
	/* CREATE RANDOM DISTRIBUTION OBJECTS FOR INTS AND DOUBLES */
	uniform_int_distribution int_dist(-10, 10);
	uniform_real_distribution real_dist(1.0, 10.0);

	/* PRINT MIN AND MAX OF EACH RANDOM ENGINE */
	cout << "random_device min = " << rd.min() << ", max = " << rd.max() << endl;
	cout << "mt19937 min = " << mt.min() << ", max = " << mt.max() << endl;
	cout << "mt19937_64 min = " << mt64.min() << ", max = " << mt64.max() << endl;

	/* GENERATE 10 INTEGERS IN RANGE -10 TO 10 USING mt19937 GENERATOR */
	for(int n = 0; n < 10; ++n)
		cout << int_dist(mt) << " ";
	cout << endl;

	/* GENERATE 10 DOUBLES IN RANGE 1.0 TO 10.0 USING mt19937 GENERATOR */
	for(int n = 0; n < 5; ++n)
		cout << real_dist(mt) << " ";
	cout << endl;

	/* GENERATE A VECTOR OF CONSECUTIVE INTEGERS */
	vector<int> v;
	for(int n = 0; n < 10; ++n)
		v.push_back(n);

	/* PRINT IT */
	cout << "vector of ints: ";
	for(auto it : v) cout << it << " ";
	cout << endl;

	/* RANDOMLY SHUFFLE THE VECTOR OF INTEGERS USING mt19937 GENERATOR */
	shuffle(begin(v), end(v), mt);

	/* PRINT IT */
	cout << "shuffled to   : ";
	for(auto it : v) cout << it << " ";
	cout << endl;
	
	/* ********************* */
	/* BENCHMARK STARTS HERE */
	/* ********************* */
	cout << "generating " << COUNT << " random numbers..." << endl;

	/* TEST PERFORMANCE OF random_device */
	auto start = high_resolution_clock::now();
	int result{0};
	for(int i = 0; i < COUNT; ++i)
		result += rd();
	auto end = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(end - start).count() / 1000.0f;
	cout << "random_device duration in ms = " << duration << endl;

	/* TEST PERFORMANCE OF mt19937 */
	start = high_resolution_clock::now();
	result = 0;
	for(int i = 0; i < COUNT; ++i)
		result += mt();
	end = high_resolution_clock::now();
	duration = duration_cast<microseconds>(end - start).count() / 1000.0f;
	cout << "mt19937 duration in ms = " << duration << endl;

	/* TEST PERFORMANCE OF uniform_int_distribution */
	start = high_resolution_clock::now();
	result = 0;
	for(int i = 0; i < COUNT; ++i)
		result += int_dist(mt);
	end = high_resolution_clock::now();
	duration = duration_cast<microseconds>(end - start).count() / 1000.0f;
	cout << "uniform_int_distribution duration in ms = " << duration << endl;

	/* TEST PERFORMANCE OF uniform_real_distribution */
	start = high_resolution_clock::now();
	result = 0;
	for(int i = 0; i < COUNT; ++i)
		result += real_dist(mt);
	end = high_resolution_clock::now();
	duration = duration_cast<microseconds>(end - start).count() / 1000.0f;
	cout << "uniform_real_distribution duration in ms = " << duration << endl;
}
