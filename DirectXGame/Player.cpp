#include "Player.h"
#include "WinApp.h"

Player::Player() {}

Player::~Player() {
	// bullet_の解放
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}

	delete sprite2dReticle_;
}

void Player::Initialize(Model* model, uint32_t textureHandle,Vector3&position) {
	// NULLポインタチェック
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.translation_ = position;
	worldTransform_.Initialize();
	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
	//3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();
	//3Dレティクルのテクスチャ
	//ReticletextureHandle_ = TextureManager::Load("reticle.png");
	//レティクル用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("reticle.png");
	//スプライト生成
	sprite2dReticle_ = Sprite::Create(textureReticle, {worldTransform3DReticle_.translation_.x, worldTransform3DReticle_.translation_.y}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5, 0.5});
}

void Player::Update(const ViewProjection& viewProjection) {
	//デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};
	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;
	// 押した方向で移動ベクトルを変更(左右)
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}
	// 押した方向で移動ベクトルを変更(上下)
	if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	}

	// 座標移動()ベクトルの加算
	worldTransform_.translation_ = Add(worldTransform_.translation_, move);

	// 移動限界座標
	const float kMoveLimitX = 30.0f;
	const float kMoveLimitY = 15.0f;

	// 範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	// 旋回
	Rotate();

	// 攻撃
	Attack();

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	worldTransform_.UpdateMatrix();

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	
	// 自機の座標から3Dレティクルのワールド座標を計算
	// 自機から3Dレティクルへの距離
	const float kDistancePlayerTo3DReticle = 25.0f;
	// 自機から3Dレティクルへのオフセット(z+向き)
	Vector3 offset = {0, 0, 1.0f};
	// 自機のワールド行列の回転を反映
	offset = TransformNormal(offset, worldTransform_.matWorld_);
	// ベクトルの長さを整える
	offset = Normalize(offset) * kDistancePlayerTo3DReticle;
	// 3Dレティクルの座標を設定
	worldTransform3DReticle_.translation_ = worldTransform_.translation_ + offset;
	worldTransform3DReticle_.UpdateMatrix();

	//3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	Vector3 positionReticle;
	positionReticle.x = worldTransform3DReticle_.matWorld_.m[3][0];
	positionReticle.y = worldTransform3DReticle_.matWorld_.m[3][1];
	positionReticle.z = worldTransform3DReticle_.matWorld_.m[3][2];
	//ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	//ビュー行列とプロジェクション行列、ビューポート行列を合成
	Matrix4x4 matViewProjectionViewport = Multiply(Multiply(viewProjection.matView,viewProjection.matProjection), matViewport);
	//ワールド→スクリーン座標変換(ここで3Dから2Dになる)
	positionReticle = Transform(positionReticle, matViewProjectionViewport);
	//スプライトのレティクルに座標設定
	sprite2dReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));

	POINT mousePosition;
	// マウス座標(スクリーン座標)を取得する
	GetCursorPos(&mousePosition);
	// クライアントエリア座標に変換する
	HWND hwnd = WinApp::GetInstance()->GetHwnd();
	ScreenToClient(hwnd, &mousePosition);
	// マウス座標を2Dレティクルのスプライトに代入する
	sprite2dReticle_->SetPosition(Vector2(float(mousePosition.x), float(mousePosition.y)));
	// ビュー行列、射影行列、ビューポート行列の合成行列を計算する
	Matrix4x4 matVPV = Multiply(Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);
	// 合成行列の逆行列を計算する
	Matrix4x4 matInverseVPV = Inverse(matVPV);
	//スクリーン座標
	Vector3 posNear = Vector3(float(mousePosition.x), float(mousePosition.y), 0);
	Vector3 posFar = Vector3(float (mousePosition.x), float(mousePosition.y), 1);
    //スクリーン座標系からワールド座標系へ
	posNear = Transform(posNear, matInverseVPV);
	posFar = Transform(posFar, matInverseVPV);
	//マウスレイの方向
	Vector3 mouseDirection = posFar - posNear;
	mouseDirection = Normalize(mouseDirection);
	//カメラから照準オブジェクトの距離
	const float kDistanceTestObject = 100.0f;
	worldTransform3DReticle_.translation_ = posNear + mouseDirection * kDistanceTestObject;
	worldTransform3DReticle_.UpdateMatrix();

	// キャラクターの座標を画面表示する処理
	ImGui::Begin("Debug");
	ImGui::DragFloat3("Position", &worldTransform_.translation_.x, 0.1f);
	ImGui::Text("2DReticle:(%f,%f)", positionReticle.x, positionReticle.y);
	ImGui::Text("Near:(%+.2f,%+.2f,%+.2f)", posNear.x, posNear.y, posNear.z);
	ImGui::Text("Far:(%+.2f,%+.2f,%+.2f)", posFar.x, posFar.y, posFar.z);
	ImGui::Text("3DReticle:(%+.2f,%+.2f,%+.2f)", worldTransform3DReticle_.translation_.x, 
		worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);
	ImGui::End();
}

void Player::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	//3Dレティクルを描画
	model_->Draw(worldTransform3DReticle_, viewProjection);

	// 弾の描画
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}

// 旋回
void Player::Rotate() {
	// 回転速さ[ラジアン/frame]
	const float kRotSpeed = 0.02f;
	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += kRotSpeed;
	}
}

// 攻撃
void Player::Attack() {
	if (input_->TriggerKey(DIK_SPACE)) {
		//弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		//自機から照準オブジェクトへのベクトル
		velocity = Subtract(worldTransform3DReticle_.translation_, worldTransform_.translation_);
		velocity = Normalize(velocity) * kBulletSpeed;

		//速度ベクトルを自機の向きに合わせて回転させる
	   //  velocity = TransformNormal(velocity,worldTransform_.matWorld_);

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_,GetWorldPosition(),velocity);

		// 弾を登録する
		bullets_.push_back(newBullet);
	}
}

//衝突時コールバック

void Player::OnCollision() {

}


void Player::SetParent(const WorldTransform* parent) {
	//親子関係を結ぶ
	worldTransform_.parent_ = parent;
}

//UI描画
void Player::DrawUI() { sprite2dReticle_->Draw(); }

//ワールド座標を取得
Vector3 Player::GetWorldPosition() {
	//ワールド座標を入れる関数
	Vector3 worldPos;
	//ワールド行列の平行移動成分を取得
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}