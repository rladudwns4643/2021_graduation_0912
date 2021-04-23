#include "pch.h"
#include "Player.h"

Player::Player() {
	Initialize();
}

Player::Player(const Player& rhs) {
	if (this != &rhs) {
		m_id = rhs.m_id;
		m_isEmpty = rhs.m_isEmpty;
		m_isReady = rhs.m_isReady;
		m_isDead = rhs.m_isDead;
		memcpy(m_idStr, rhs.m_idStr, sizeof(char) * 10);
		m_mmr = rhs.m_mmr;
	}
}

Player& Player::operator=(const Player& rhs) noexcept {
	if (this != &rhs) {
		delete m_cur;
		m_cur = rhs.m_cur;
		m_id = rhs.m_id;
		m_isEmpty = rhs.m_isEmpty;
		m_isReady = rhs.m_isReady;
		m_isDead = rhs.m_isDead;
		memcpy(m_idStr, rhs.m_idStr, sizeof(char) * 10);
		m_mmr = rhs.m_mmr;
		delete& rhs;
	}
	return *this;
}

Player::~Player() {
	delete m_cur;
	m_cur = nullptr;
}

void Player::Initialize() {
	m_id = -1;
	m_cur = nullptr;
	m_isEmpty = true;
	m_isReady = false;
	m_isDead = false;
	ZeroMemory(&m_idStr, sizeof(char) * 10);
	m_mmr = 0;
	m_prevAnimType = ANIM_IDLE;
	m_keyW = false;
	m_keyA = false;
	m_keyS = false;
	m_keyD = false;
	m_keyJump = false;
	m_animJump = false;
}

void Player::Reset() {
	m_cur = nullptr;
	m_isReady = false;
	m_isDead = false;
	m_isMoveable = false;
	m_keyW = false;
	m_keyA = false;
	m_keyS = false;
	m_keyD = false;
	m_keyJump = false;
}

void Player::SetCurObject(const int& spawn_pos_idx) {
	m_cur = new Character(spawn_pos_idx);
}

Object* Player::GetCurObject() {
	return m_cur;
}

void Player::Enter() {
	m_isEmpty = false;
}

void Player::Leave(){
	m_isEmpty = true;
}

bool Player::GetSlotStatus() {
	return m_isEmpty;
}

void Player::SetID(const int& id) {
	m_id = id;
}

int Player::GetID() {
	return m_id;
}

void Player::SetReady(const bool& ready) {
	m_isReady = ready;
}

bool Player::GetReady() {
	return m_isReady;
}

void Player::SetID_STR(char* name) {
	memcpy(&m_idStr, name, sizeof(char) * MAX_STR_LEN);
}

char* Player::GetID_STR() {
	return m_idStr;
}

void Player::SetMMR(int mmr) {
	m_mmr = mmr;
}

int Player::GetMMR() {
	return m_mmr;
}

void Player::SetIsMoveable(const bool& moveable) {
	m_isMoveable = moveable;
}

bool Player::IsMoveable() {
	return m_isMoveable;
}

void Player::SetDead(const bool& dead) {
	m_isDead = dead;
}

bool Player::IsDead() {
	return m_isDead;
}

int Player::GetPrevAnimType() {
	return m_prevAnimType;
}

void Player::SetPrevAnimType(int animType) {
	m_prevAnimType = animType;
}

void Player::SetKeyW(bool input) {
	m_keyW = input;
}

void Player::SetKeyS(bool input) {
	m_keyS = input;
}

void Player::SetKeyA(bool input) {
	m_keyA = input;
}

void Player::SetKeyD(bool input) {
	m_keyD = input;
}

void Player::SetKeyJump(bool input) {
	m_keyJump = input;
}

bool Player::GetKeyW() {
	return m_keyW;
}

bool Player::GetKeyA() {
	return m_keyA;
}

bool Player::GetKeyS() {
	return m_keyS;
}

bool Player::GetKeyD() {
	return m_keyD;
}

bool Player::GetKeyJump() {
	return m_keyJump;
}