#pragma once
#include <fstream>
#include <vector>
#include <unordered_map>
#include <functional>
#include "Memory/DataStorageDevice.hpp"
#include "Memory/MemoryMapEntry.hpp"
#include "Graphics/DMATransferRegister.hpp"

namespace SHG
{
	using MemoryMapWriteCallback = std::function<void(uint16_t address, uint8_t value)>;

	// TODO: Create a more efficient way of storing and accessing memory map data.
	class MemoryMap : public DataStorageDevice
	{
	public:
		void AssignDeviceToAddressRange(DataStorageDevice* device, uint16_t lowerBoundAddress, uint16_t upperBoundAddress);
		bool IsAddressRangeOccupied(uint16_t lowerBoundAddress, uint16_t upperBoundAddress);
		void SetReadonlyBitMask(uint16_t address, uint8_t bitMask);
		uint8_t Read(uint16_t address) const override;
		void Write(uint16_t address, uint8_t value) override;
		bool IsAddressAvailable(uint16_t address) const override;
		void Reset() override;
	private:
		std::vector<MemoryMapWriteCallback> memoryWriteCallbacks;
		std::vector<MemoryMapEntry> memoryMapEntries;
		std::ofstream blarggOutStream;
		std::unordered_map<uint16_t, uint8_t> readonlyBitMasks;

		bool IsDeviceMapped(DataStorageDevice* device) const;
		const MemoryMapEntry* GetEntryForDevice(DataStorageDevice* device) const;
		const MemoryMapEntry* GetMemoryMapEntryWithAddress(uint16_t address) const;
		uint16_t GetNormalizedAddress(const MemoryMapEntry* memoryMapEntry, uint16_t address) const;
	};
}