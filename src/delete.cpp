#include <iostream>
#include <ios>
using namespace std;

void void_only(std::nullptr_t) { cout << "api(std::nullptr_t)\n"; }
void void_only(void* p) { cout << "api(void* p = " << hex << p << ")\n"; }

#if __cplusplus >= 202002L
void void_only(auto*) = delete; // C++20 and newer...
#else
template<typename T> void void_only(T*) = delete; // prior to C++20...
#endif

int main()
{
    void_only(nullptr);      // 1st overload
    void_only((void*)0xABC); // 2nd overload
    // void_only(NULL);      // ERROR, ambiguous
    // void_only((long*)0);  // ERROR, type not accepted
}
