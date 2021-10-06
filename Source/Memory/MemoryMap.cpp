#include <filesystem>
#include "Memory/MemoryMap.hpp"
#include "Logger.hpp"
#include "Common/DataConversions.hpp"
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

		if (mappedDevice != nullptr)
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
				// or the provided address range is inside of this device's address range.
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
		
		if (mappedDevice == nullptr) return 0;

		return mappedDevice->device.GetByte(GetNormalizedAddress(mappedDevice, address));
	}

	void MemoryMap::SetByte(uint16_t address, uint8_t value)
	{
		ProcessBlarggTestsOutput(address, value);

		MemoryMappedDevice* mappedDevice = GetMemoryMappedDeviceForRange(address);

		if (mappedDevice != nullptr)
		{
			uint16_t normalizedAddress = GetNormalizedAddress(mappedDevice, address);

			// Some addresses have read-only bits. This prevents the bits in these 
			// addresses from being changed.
			if (readonlyBitMasks.find(address) != readonlyBitMasks.end())
			{
				uint8_t currentByte = mappedDevice->device.GetByte(normalizedAddress) & readonlyBitMasks[address];
				value = (value & ~readonlyBitMasks[address]) | currentByte;
			}

			mappedDevice->device.SetByte(normalizedAddress, value);

			for (MemoryMapWriteCallback c : memoryWriteCallbacks)
				c(address, value);
		}
	}

	bool MemoryMap::IsAddressAvailable(uint16_t address)
	{
		MemoryMappedDevice* device = GetMemoryMappedDeviceForRange(address);
		return device != NULL && device->device.IsAddressAvailable(GetNormalizedAddress(device, address));
	}

	void MemoryMap::RegisterMemoryWriteCallback(MemoryMapWriteCallback callback)
	{
		memoryWriteCallbacks.push_back(callback);
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
		return GetMappedDevice(device) != nullptr;
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

	uint16_t MemoryMap::GetNormalizedAddress(MemoryMappedDevice* mappedDevice, uint16_t address)
	{
		return address - mappedDevice->GetLowestBound();
	}

	void MemoryMap::ProcessBlarggTestsOutput(uint16_t address, uint8_t value)
	{
		if (address == 0xFF02 && value == 0x81)
		{
			if (!blarggOutStream.is_open())
			{
				blarggOutStream = std::ofstream(std::filesystem::current_path().string() + "/BlarggTestResult.log", std::ios::out);
			}

			blarggOutStream << (char)GetByte(0xFF01);
			blarggOutStream.flush();
		}
	}

	void MemoryMap::SetReadonlyBitMask(uint16_t address, uint8_t bitMask)
	{
		readonlyBitMasks[address] = bitMask;
	}
}