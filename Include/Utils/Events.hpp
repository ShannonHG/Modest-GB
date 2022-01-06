#include <functional>

namespace SHG
{
	using SimpleCallback = std::function<void()>;
	using FileSelectionCallback = std::function<void(std::string)>;
}