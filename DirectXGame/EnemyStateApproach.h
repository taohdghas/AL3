#pragma once
#include "BaseEnemyState.h"
#include "EnemyStateLeave.h"

//接近フェーズ
class EnemyStateApproach:public BaseEnemyState {
public:
	//コンストラクタ
	EnemyStateApproach(Enemy* enemy);
	//更新
	void Update();
};
