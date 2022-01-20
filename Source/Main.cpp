#include <string>
#include "Logger.hpp"
#include "Emulator.hpp"
#include "Utils/Arithmetic.hpp"

int main(int argc, char* argv[])
{
	return ModestGB::Emulator().Run();
}