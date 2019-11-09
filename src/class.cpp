#include <iostream>
#include <utility>
#include <cstdlib>
#include <ctime>

using namespace std;

#define VIRTUAL_FUNCTION_1 0
#define VIRTUAL_FUNCTION_2 1

typedef void(*VirtualFunctionPtr)(void*, int arg);

struct Base
{
	static void BaseConstructor(void* _this_)
	{
		((Base*)_this_)->VirtualTablePtr = BaseVirtualTable;
		((Base*)_this_)->Member1 = rand() % 100;
		cout << "BaseConstructor(" << _this_ <<
		", Member1 = " << ((Base*)_this_)->Member1 << ")" << endl;
	}
	
	static void BaseDestructor(void* _this_)
	{
		cout << "BaseDestructor(" << _this_ << ")" << endl;
	}
	
	static void BaseVirtualFunction_1(void* _this_, int arg)
	{
		cout << "Base(Member1 = " << ((Base*)_this_)->Member1 <<
		")::BaseVirtualFunction_1(" << arg << ")" << endl;
	}
	
	static void BaseVirtualFunction_2(void* _this_, int arg)
	{
		cout << "Base(Member1 = " << ((Base*)_this_)->Member1 <<
		")::BaseVirtualFunction_2(" << arg << ")"  << endl;
	}
	
	VirtualFunctionPtr* VirtualTablePtr;
	int Member1;
	static VirtualFunctionPtr BaseVirtualTable[3];
};

VirtualFunctionPtr Base::BaseVirtualTable[3] =
{
	&Base::BaseVirtualFunction_1,
	&Base::BaseVirtualFunction_2
};

struct Derived
{
	static void DerivedConstructor(void* _this_)
	{
		Base::BaseConstructor(_this_);
		((Derived*)_this_)->VirtualTablePtr = DerivedVirtualTable;
		((Derived*)_this_)->Member2 = rand() % 100;
		cout << "DerivedConstructor(" << _this_ <<
		", Member2 = " << ((Derived*)_this_)->Member2 << ")" << endl;
	}
	
	static void DerivedDestructor(void* _this_)
	{
		cout << "DerivedDestructor(" << _this_ << ")" << endl;
		Base::BaseDestructor(_this_);
	}
	
	static void DerivedVirtualFunction_1(void* _this_, int arg)
	{
		cout << "Derived(Member1 = " << ((Base*)_this_)->Member1 <<
		", Member2 = " << ((Derived*)_this_)->Member2 <<
		")::DerivedVirtualFunction_1(" << arg << ")"  << endl;
	}
	
	static void DerivedVirtualFunction_2(void* _this_, int arg)
	{
		cout << "Derived(Member1 = " << ((Base*)_this_)->Member1 <<
		", Member2 = " << ((Derived*)_this_)->Member2 <<
		")::DerivedVirtualFunction_2(" << arg << ")"  << endl;
	}
	
	VirtualFunctionPtr* VirtualTablePtr;
	int __Space_for_Base_Member1__;
	int Member2;
	static VirtualFunctionPtr DerivedVirtualTable[3];
};

VirtualFunctionPtr Derived::DerivedVirtualTable[3] =
{
	&Derived::DerivedVirtualFunction_1,
	&Derived::DerivedVirtualFunction_2
};

template<typename T, typename... A>
void VirtualDispatch(T* _this_, int VirtualFuncNum, A&&... args)
{
	_this_->VirtualTablePtr[VirtualFuncNum](_this_, forward<A>(args)...);
}

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));
	
	cout << "===> Base start <===" << endl;
	Base* base = (Base*)operator new(sizeof(Base));
	Base::BaseConstructor(base);
	
	VirtualDispatch(base, VIRTUAL_FUNCTION_1, rand() % 100);
	VirtualDispatch(base, VIRTUAL_FUNCTION_2, rand() % 100);
	
	Base::BaseDestructor(base);
	operator delete(base);
	cout << "===> Base end <===" << endl << endl;
	
	cout << "===> Derived start <===" << endl;
	Base* derived = (Base*)operator new(sizeof(Derived));
	Derived::DerivedConstructor(derived);
	
	VirtualDispatch(derived, VIRTUAL_FUNCTION_1, rand() % 100);
	VirtualDispatch(derived, VIRTUAL_FUNCTION_2, rand() % 100);
	
	Derived::DerivedDestructor(derived);
	operator delete(derived);
	cout << "===> Derived end <===" << endl << endl;
	
	return 1;
}
