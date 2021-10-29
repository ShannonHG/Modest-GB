#include <functional>

namespace SHG
{
	using SimpleEvent = std::function<void()>;
	using FileSelectionEvent = std::function<void(std::string)>;
}