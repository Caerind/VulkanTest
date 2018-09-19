#ifndef NU_EVENT_HPP
#define NU_EVENT_HPP

namespace nu
{

enum EventType
{
	MouseClick,
	MouseMove,
	MouseWheel,
	Resize,
	Close
};

struct Event
{
	EventType type;
	int param1;
	int param2;
};

} // namespace nu

#endif // NU_EVENT_HPP