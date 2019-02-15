#pragma once

#include <iostream>
#include <mutex>
#include "mutex.h"

namespace { static inline fast_mutex kStdOutLock; }

template<typename... Ts>
inline void trace(Ts&&... args)
{
	std::scoped_lock lock(kStdOutLock);
	(std::cout << ... << args) << std::endl;
}
