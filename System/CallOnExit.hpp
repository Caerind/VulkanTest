#ifndef NU_CALLONEXIT_HPP
#define NU_CALLONEXIT_HPP

#include <functional>

#include "Prerequisites.hpp"

namespace nu
{

class CallOnExit
{
	public:
		using Func = std::function<void()>;
		
		CallOnExit(Func func = nullptr);
		CallOnExit(const CallOnExit&) = delete;
		CallOnExit(CallOnExit&&) = delete;
		~CallOnExit();

		void call();
		void callAndReset(Func func = nullptr);
		void reset(Func func = nullptr);

		CallOnExit& operator=(const CallOnExit&) = delete;
		CallOnExit& operator=(CallOnExit&&) = delete;

	private:
		Func mFunc;
};

inline CallOnExit::CallOnExit(Func func)
	: mFunc(func)
{
}

inline CallOnExit::~CallOnExit()
{
	call();
}

inline void CallOnExit::call()
{
	if (mFunc)
	{
		mFunc();
	}
}
	
inline void CallOnExit::callAndReset(Func func)
{
	call();
	reset(func);
}

inline void CallOnExit::reset(Func func)
{
	mFunc = func;
}

} // namespace nu

#endif // NU_CALLONEXIT_HPP_HPP
