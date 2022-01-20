#include <functional>

namespace ModestGB
{
	using SimpleCallback = std::function<void()>;
	using FileSelectionCallback = std::function<void(std::string)>;
}