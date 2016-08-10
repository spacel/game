/*
 * This file is part of Spacel game.
 *
 * Copyright 2016, Vincent Glize <vincent.glize@live.fr>
 *
 * Spacel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Spacel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Spacel.  If not, see <http://www.gnu.org/licenses/>.
 */


#pragma once

#include "inventory.h"
#include "unit.h"

namespace spacel {

namespace engine {

enum PlayerRace
{
	PLAYER_RACE_HUMAN,
	PLAYER_RACE_KILBOG,
	PLAYER_RACE_ARKYN,
	PLAYER_RACE_SPACEELF,
	PLAYER_RACE_MAX,
};

/*static const char *player_race_names[PLAYER_RACE_MAX] = {
	"Human",
	"Kilbog",
	"Arkyn",
	"Space Elf"
};*/

enum PlayerSex
{
	PLAYER_SEX_MALE,
	PLAYER_SEX_FEMALE,
	PLAYER_SEX_MAX,
};

/*static const char *player_sex_names[PLAYER_SEX_MAX] = {
	"Male",
	"Female"
};*/

enum PlayerStat
{
	PLAYER_STAT_STAMINA,
	PLAYER_STAT_MAX,
};

class Player : public Unit
{
public:
	Player(const std::string &username);
	~Player() {};

private:
	std::string m_username = "";

	PlayerSex m_sex = PLAYER_SEX_MALE;
	PlayerRace m_race = PLAYER_RACE_HUMAN;

	uint32_t m_player_stats[PLAYER_STAT_MAX];
	uint64_t m_xp = 0;
	uint16_t m_level = 1;

	std::unordered_map<uint32_t, InventoryPtr> m_inventories;
};
}
}
