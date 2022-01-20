#pragma once
#include <map>

namespace ModestGB
{
	enum class GBButton
	{
		A,
		B, 
		RIGHT, 
		LEFT, 
		UP, 
		DOWN, 
		START,
		SELECT
	};

	const std::map<GBButton, std::string> GBBUTTON_STRINGS
	{
		{ GBButton::A, "A" },
		{ GBButton::B, "B" },
		{ GBButton::RIGHT, "RIGHT" },
		{ GBButton::LEFT, "LEFT" },
		{ GBButton::UP, "UP" },
		{ GBButton::DOWN, "DOWN" },
		{ GBButton::START, "START" },
		{ GBButton::SELECT, "SELECT" },
	};
}