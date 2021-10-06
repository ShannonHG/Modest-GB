#pragma once
#include <fstream>
#include <vector>
#include <unordered_map>
#include <functional>
#include "Memory/DataStorageDevice.hpp"
#include "Memory/MemoryMappedDevice.hpp"

namespace SHG
{
	using MemoryMapWriteCallback = std::function<void(uint16_t, uint8_t)>;

	// TODO: Create a more efficient way of storing and accessing memory map data.
	class MemoryMap : public DataStorageDevice
	{
	public:
		void RegisterMemoryWriteCallback(MemoryMapWriteCallback callback);
		void AssignDeviceToAddressRange(DataStorageDevice& device, uint16_t lowerBoundAddress, uint16_t upperBoundAddress);
		bool IsAddressRangeOccupied(uint16_t lowerBoundAddress, uint16_t upperBoundAddress);
		bool IsDeviceMapped(DataStorageDevice& device);
		MemoryMappedDevice* GetMappedDevice(DataStorageDevice& device);
		void SetReadonlyBitMask(uint16_t address, uint8_t bitMask);
		uint8_t GetByte(uint16_t address) override;
		void SetByte(uint16_t address, uint8_t value) override;
		bool IsAddressAvailable(uint16_t address) override;
	private:
		std::vector<MemoryMapWriteCallback> memoryWriteCallbacks;
		std::vector<MemoryMappedDevice> mappedDevices;
		std::ofstream blarggOutStream;
		std::unordered_map<uint16_t, uint8_t> readonlyBitMasks;

		MemoryMappedDevice* GetMemoryMappedDeviceForRange(uint16_t address);
		uint16_t GetNormalizedAddress(MemoryMappedDevice* mappedDevice, uint16_t address);

		void ProcessBlarggTestsOutput(uint16_t address, uint8_t value);
	};
}