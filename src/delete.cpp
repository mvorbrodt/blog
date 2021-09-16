#include <iostream>
#include <ios>
using namespace std;

void any_type(void* p) { cout << "any_type(void* p = " << hex << p << ")\n"; }

void void_only(std::nullptr_t) { cout << "void_only(std::nullptr_t)\n"; }
void void_only(void* p) { cout << "void_only(void* p = " << hex << p << ")\n"; }

#if __cplusplus >= 202002L
void void_only(auto*) = delete; // C++20 and newer...
#else
template<typename T> void void_only(T*) = delete; // prior to C++20...
#endif

int main()
{
    any_type(new char);
    any_type(new short);
    any_type(new int);

    void_only(nullptr);      // 1st overload
    void_only((void*)0xABC); // 2nd overload, type must be void*

    // void_only(0);         // ERROR, ambiguous
    // void_only(NULL);      // ERROR, ambiguous
    // void_only((long*)0);  // ERROR, explicitly deleted
}
