module;
#include <type_traits>

export module ZeroMem;

export template <typename T>
void ZeroMem(T* obj)
{
	static_assert(!std::is_pointer_v<T>);
	memset(obj, 0, sizeof(T));
}