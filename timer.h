#pragma once

#include <thread>
#include <memory>
#include "event.h"

namespace timer
{
	template<typename D, typename F, typename... Args>
	std::shared_ptr<manual_event> set_timeout(D d, F f, Args&&... args)
	{
		auto event = std::make_shared<manual_event>();
		std::thread([=]()
		{
			if(event->wait_for(d)) return;
			f(args...);
		}).detach();
		return event;
	}

	template<typename D, typename F, typename... Args>
	std::shared_ptr<manual_event> set_interval(D d, F f, Args&&... args)
	{
		auto event = std::make_shared<manual_event>();
		std::thread([=]()
		{
			while(true)
			{
				if(event->wait_for(d)) return;
				f(args...);
			}
		}).detach();
		return event;
	}
}
