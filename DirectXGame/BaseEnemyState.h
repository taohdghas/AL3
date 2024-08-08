#pragma once
#include <string>
#include "Enemy.h"

class Enemy;

//敵の状態基底
class BaseEnemyState {
public:
	BaseEnemyState(const std::string& name, Enemy* enemy) : name_(name), enemy_(enemy){};

	//毎フレーム処理(純粋仮想関数)
	virtual void Update() = 0;

	//デバックログ出力
	virtual void DebugLog();

	protected:
		//状態名
	    std::string name_;
		//操作対象の敵
	    Enemy* enemy_ = nullptr;
};
