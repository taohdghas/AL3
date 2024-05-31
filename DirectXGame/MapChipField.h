#pragma once
#include "Vector3.h"
#include <stdint.h>
#include <string>
#include <vector>

enum class MapChipType { kBlank, kBlock };

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {
public:
	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
	uint32_t GetNumBlockHorizontal();
	uint32_t GetNumBlockVirtical();

private:
	static inline const float kBlockWidth = 2.0f;
	static inline const float kBlockHeight = 2.0f;
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;
	MapChipData mapChipData_;
};
