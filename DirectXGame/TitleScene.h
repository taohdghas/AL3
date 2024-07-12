#pragma once
#include "Input.h"
    /// <summary>
/// 
/// </summary>
class TitleScene {
public:
	void Initialize();

	void Update();

	void Draw();

	//デスグラグのgetter
	bool IsFinished() const { return finished_; }

private:
	//終了フラグ
	bool finished_ = false;
};
