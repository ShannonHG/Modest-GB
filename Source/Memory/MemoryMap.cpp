#include <filesystem>
#include "Memory/MemoryMap.hpp"
#include "Logger.hpp"
#include "Utils/DataConversions.hpp"
#include "InvalidOpcodeException.hpp"

namespace SHG
{
	const std::string MEMORY_MAP_LOG_HEADER = "[Memory Map]";

	void MemoryMap::AssignDeviceToAddressRange(DataStorageDevice* device, uint16_t lowerBoundAddress, uint16_t upperBoundAddress)
	{
		if (device == nullptr)
		{
			Logger::WriteError("Cannot add NULL device to memory map.");
			return;
		}

		if (IsAddressRangeOccupied(lowerBoundAddress, upperBoundAddress))
		{
			Logger::WriteWarning("The address range " + ConvertToHexString(lowerBoundAddress, 4) + " - " + ConvertToHexString(upperBoundAddress, 4) + " is already occupied.", MEMORY_MAP_LOG_HEADER);
			return;
		}

		MemoryMapEntry* entry = GetEntryForDevice(device);

		if (entry != nullptr)
			entry->AddAddressRange(lowerBoundAddress, upperBoundAddress);
		else
			memoryMapEntries.push_back(MemoryMapEntry(device, { MemoryMapRange(lowerBoundAddress, upperBoundAddress) }));
	}

	bool MemoryMap::IsAddressRangeOccupied(uint16_t lowerBoundAddress, uint16_t upperBoundAddress)
	{
		for (MemoryMapEntry& entry : memoryMapEntries)
		{
			for (const MemoryMapRange& range : entry.GetAddressRanges())
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

	uint8_t MemoryMap::Read(uint16_t address)
	{
		MemoryMapEntry* entry = GetMemoryMapEntryWithAddress(address);
		
		if (entry == nullptr)
			return 0;

		return entry->GetDevice()->Read(GetNormalizedAddress(entry, address));
	}

	void MemoryMap::Write(uint16_t address, uint8_t value)
	{
		ProcessBlarggTestsOutput(address, value);

		MemoryMapEntry* memoryMapEntry = GetMemoryMapEntryWithAddress(address);

		if (memoryMapEntry != nullptr)
		{
			uint16_t normalizedAddress = GetNormalizedAddress(memoryMapEntry, address);

			// Some addresses have read-only bits. This prevents the bits in these 
			// addresses from being changed.
			if (readonlyBitMasks.find(address) != readonlyBitMasks.end())
			{
				uint8_t currentByte = memoryMapEntry->GetDevice()->Read(normalizedAddress) & readonlyBitMasks[address];
				value = (value & ~readonlyBitMasks[address]) | currentByte;
			}

			memoryMapEntry->GetDevice()->Write(normalizedAddress, value);
		}
	}

	MemoryMapEntry* MemoryMap::GetMemoryMapEntryWithAddress(uint16_t address)
	{
		for (MemoryMapEntry& entry : memoryMapEntries)
		{
			for (const MemoryMapRange& range : entry.GetAddressRanges())
			{
				if (address >= range.lowerBoundAddress && address <= range.upperBoundAddress)
				{
					return &entry;
				}
			}
		}

		return nullptr;
	}

	bool MemoryMap::IsAddressAvailable(uint16_t address)
	{
		MemoryMapEntry* entry = GetMemoryMapEntryWithAddress(address);
		return entry != nullptr && entry->GetDevice()->IsAddressAvailable(GetNormalizedAddress(entry, address));
	}

	void MemoryMap::Reset()
	{
		readonlyBitMasks.clear();

		for (MemoryMapEntry& entry : memoryMapEntries)
			entry.GetDevice()->Reset();
	}

	bool MemoryMap::IsDeviceMapped(DataStorageDevice* device)
	{
		return GetEntryForDevice(device) != nullptr;
	}

	MemoryMapEntry* MemoryMap::GetEntryForDevice(DataStorageDevice* device)
	{
		for (MemoryMapEntry& entry : memoryMapEntries)
		{
			if (entry.GetDevice() == device)
			{
				return &entry;
			}
		}
		
		return nullptr;
	}

	uint16_t MemoryMap::GetNormalizedAddress(MemoryMapEntry* memoryMapEntry, uint16_t address)
	{
		return address - memoryMapEntry->GetLowestBoundAddress();
	}

	void MemoryMap::SetReadonlyBitMask(uint16_t address, uint8_t bitMask)
	{
		readonlyBitMasks[address] = bitMask;
	}

	void MemoryMap::ProcessBlarggTestsOutput(uint16_t address, uint8_t value)
	{
		if (address == 0xFF02 && value == 0x81)
		{
			if (!blarggOutStream.is_open())
			{
				blarggOutStream = std::ofstream(std::filesystem::current_path().string() + "/BlarggTestResult.log", std::ios::out);
			}

			blarggOutStream << (char)Read(0xFF01);
			blarggOutStream.flush();
		}
	}
}