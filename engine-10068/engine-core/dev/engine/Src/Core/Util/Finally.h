#pragma once
#include "Std/type_traits.h"

namespace LORD
{
	// FinalAction allows you to ensure something gets run at the end of a scope
	template <class Func>
	class FinalAction
	{
	public:
		explicit FinalAction(Func f) noexcept 
			: func(std::move(f)) {}

		FinalAction(FinalAction&& other) noexcept 
			: func(std::move(other.func))
			, invoke(other.invoke)
		{
			other.invoke = false;
		}

		FinalAction(const FinalAction&) = delete;
		FinalAction& operator=(const FinalAction&) = delete;
		FinalAction& operator=(FinalAction&&) = delete;

		~FinalAction() noexcept
		{
			if (invoke)
			{
				func();
			}
		}

	private:
		Func func;
		bool invoke = true;
	};

	template <typename Func>
	inline FinalAction<std::decay_t<Func>> finally(const Func& f) noexcept
	{
		return FinalAction<std::decay_t<Func>>(f);
	}

	template <typename Func>
	inline FinalAction<std::decay_t<Func>> finally(Func&& f) noexcept
	{
		return FinalAction<std::decay_t<Func>>(std::forward<Func>(f));
	}
}

using LORD::finally;

#define FINALLY(code) auto __final_action_##__LINE__ = ::LORD::finally([&] { code; });