#pragma once

#include <iostream>
#include <mutex>

namespace { static inline std::mutex kStdOutLock; }

template<typename... Ts>
inline void trace(Ts&&... args)
{
	std::scoped_lock lock(kStdOutLock);
	((std::cout << args << " "), ...) << std::endl;
}
