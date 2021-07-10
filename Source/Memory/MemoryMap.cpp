#include <filesystem>
#include "Memory/MemoryMap.hpp"
#include "Logger.hpp"
#include "Globals.hpp"
#include "InvalidOpcodeException.hpp"

namespace SHG
{
	void MemoryMap::AssignDeviceToAddressRange(DataStorageDevice& device, uint16_t lowerBoundAddress, uint16_t upperBoundAddress)
	{
		if (IsAddressRangeOccupied(lowerBoundAddress, upperBoundAddress))
		{
			Logger::WriteWarning("[MemoryMap] The address range " + ConvertToHexString(lowerBoundAddress, 4) + " - " + ConvertToHexString(upperBoundAddress, 4) + " is already occupied.");
			return;
		}

		MemoryMappedDevice* mappedDevice = GetMappedDevice(device);
		if (mappedDevice != NULL)
		{
			mappedDevice->ranges.push_back(MemoryMapRange(lowerBoundAddress, upperBoundAddress));
		}
		else
		{
			mappedDevices.push_back(MemoryMappedDevice(device, { MemoryMapRange(lowerBoundAddress, upperBoundAddress) }));
		}
	}

	bool MemoryMap::IsAddressRangeOccupied(uint16_t lowerBoundAddress, uint16_t upperBoundAddress)
	{
		for (MemoryMappedDevice d : mappedDevices)
		{
			for (MemoryMapRange range : d.ranges)
			{
				// Check if this device is already contained within the provided address range,
				// or the address range is inside of this device's address range.
				if ((range.lowerBoundAddress >= lowerBoundAddress && range.lowerBoundAddress <= upperBoundAddress) ||
					(range.upperBoundAddress >= lowerBoundAddress && range.upperBoundAddress <= upperBoundAddress) ||
					(lowerBoundAddress >= range.lowerBoundAddress && lowerBoundAddress <= range.upperBoundAddress) ||
					(upperBoundAddress >= range.lowerBoundAddress && upperBoundAddress <= range.upperBoundAddress))
				{
					return true;
				}
			}
		}

		return false;
	}

	uint8_t MemoryMap::GetByte(uint16_t address)
	{
		MemoryMappedDevice* mappedDevice = GetMemoryMappedDeviceForRange(address);
		
		if (mappedDevice == NULL) return 0;

		/*Logger::Write("[MemoryMap] Getting byte at address: " + ConvertToHexString(address, 4));
		Logger::Write("[MemoryMap] Lower bound: " + ConvertToHexString(mappedDevice->GetLowestBound(), 4));
		Logger::Write("[MemoryMap] Upper bound: " + ConvertToHexString(mappedDevice->GetHighestBound(), 4));*/

		return mappedDevice->device.GetByte(address - mappedDevice->GetLowestBound());
	}

	void MemoryMap::SetByte(uint16_t address, uint8_t value)
	{
		ProcessBlarggTestsOutput(address, value);

		MemoryMappedDevice* mappedDevice = GetMemoryMappedDeviceForRange(address);

		if (mappedDevice != NULL)
		{
			/*Logger::Write("[MemoryMap] Setting byte at address: " + ConvertToHexString(address, 4));
			Logger::Write("[MemoryMap] Lower bound: " + ConvertToHexString(mappedDevice->GetLowestBound(), 4));
			Logger::Write("[MemoryMap] Upper bound: " + ConvertToHexString(mappedDevice->GetHighestBound(), 4));*/

			mappedDevice->device.SetByte(address - mappedDevice->GetLowestBound(), value);
		}
	}

	bool MemoryMap::IsAddressAvailable(uint16_t address)
	{
		MemoryMappedDevice* device = GetMemoryMappedDeviceForRange(address);
		return device != NULL && device->device.IsAddressAvailable(address - device->GetLowestBound());
	}

	MemoryMappedDevice* MemoryMap::GetMemoryMappedDeviceForRange(uint16_t address)
	{
		for (int i = 0; i < mappedDevices.size(); i++)
		{
			for (MemoryMapRange range : mappedDevices[i].ranges)
			{
				if (address >= range.lowerBoundAddress && address <= range.upperBoundAddress)
				{
					return &mappedDevices[i];
				}
			}
		}

		return NULL;
	}

	bool MemoryMap::IsDeviceMapped(DataStorageDevice& device)
	{
		return GetMappedDevice(device) != NULL;
	}

	MemoryMappedDevice* MemoryMap::GetMappedDevice(DataStorageDevice& device)
	{
		for (int i = 0; i < mappedDevices.size(); i++)
		{
			if (&mappedDevices[i].device == &device)
			{
				return &mappedDevices[i];
			}
		}

		return NULL;
	}

	void MemoryMap::ProcessBlarggTestsOutput(uint16_t address, uint8_t value)
	{
		if (address == 0xFF02 && value == 0x81)
		{
			if (!blarggOutStream.is_open())
			{
				blarggOutStream = std::ofstream(std::filesystem::current_path().string() + "/BlarggTestResult.log", std::ios::out);
			}
			blarggOutStream << std::to_string(GetByte(0xFF01));
			//blarggOutStream << (char)GetByte(0xFF01);
			blarggOutStream.flush();
		}
	}
}