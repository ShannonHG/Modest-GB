#pragma once
#include <fstream>
#include <vector>
#include "Memory/DataStorageDevice.hpp"
#include "Memory/MemoryMappedDevice.hpp"

namespace SHG
{
	class MemoryMap : public DataStorageDevice
	{
	public:
		void AssignDeviceToAddressRange(DataStorageDevice& device, uint16_t lowerBoundAddress, uint16_t upperBoundAddress);
		bool IsAddressRangeOccupied(uint16_t lowerBoundAddress, uint16_t upperBoundAddress);
		bool IsDeviceMapped(DataStorageDevice& device);
		MemoryMappedDevice* GetMappedDevice(DataStorageDevice& device);
		uint8_t GetByte(uint16_t address) override;
		void SetByte(uint16_t address, uint8_t value) override;
		bool IsAddressAvailable(uint16_t address) override;
	private:
		std::vector<MemoryMappedDevice> mappedDevices;
		MemoryMappedDevice* GetMemoryMappedDeviceForRange(uint16_t address);
		uint16_t GetNormalizedAddress(MemoryMappedDevice* mappedDevice, uint16_t address);
		std::ofstream blarggOutStream;
		void ProcessBlarggTestsOutput(uint16_t address, uint8_t value);
	};
}