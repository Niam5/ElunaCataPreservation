/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRINITYCORE_GROUP_H
#define TRINITYCORE_GROUP_H

#include "DBCEnums.h"
#include "DatabaseEnvFwd.h"
#include "GroupRefManager.h"
#include "Loot.h"
#include "SharedDefines.h"
#include <map>

class Battlefield;
class Battleground;
class Creature;
class DynamicObject;
class InstanceSave;
class Map;
class Player;
class Unit;
class WorldObject;
class WorldPacket;
class WorldSession;

struct MapEntry;

#define MAX_GROUP_SIZE 5
#define MAXRAIDSIZE 40
#define MAX_RAID_SUBGROUPS MAXRAIDSIZE/MAX_GROUP_SIZE
#define TARGET_ICONS_COUNT 8
#define SPELL_RAID_MARKER 84996

enum RollVote
{
    PASS              = 0,
    NEED              = 1,
    GREED             = 2,
    DISENCHANT        = 3,
    NOT_EMITED_YET    = 4,
    NOT_VALID         = 5
};

enum GroupMemberOnlineStatus
{
    MEMBER_STATUS_OFFLINE   = 0x0000,
    MEMBER_STATUS_ONLINE    = 0x0001,                       // Lua_UnitIsConnected
    MEMBER_STATUS_PVP       = 0x0002,                       // Lua_UnitIsPVP
    MEMBER_STATUS_DEAD      = 0x0004,                       // Lua_UnitIsDead
    MEMBER_STATUS_GHOST     = 0x0008,                       // Lua_UnitIsGhost
    MEMBER_STATUS_PVP_FFA   = 0x0010,                       // Lua_UnitIsPVPFreeForAll
    MEMBER_STATUS_UNK3      = 0x0020,                       // used in calls from Lua_GetPlayerMapPosition/Lua_GetBattlefieldFlagPosition
    MEMBER_STATUS_AFK       = 0x0040,                       // Lua_UnitIsAFK
    MEMBER_STATUS_DND       = 0x0080                        // Lua_UnitIsDND
};

enum GroupMemberFlags
{
    MEMBER_FLAG_ASSISTANT   = 0x01,
    MEMBER_FLAG_MAINTANK    = 0x02,
    MEMBER_FLAG_MAINASSIST  = 0x04
};

enum GroupMemberAssignment
{
    GROUP_ASSIGN_MAINTANK   = 0,
    GROUP_ASSIGN_MAINASSIST = 1
};

enum GroupFlags
{
    GROUP_FLAG_NONE                 = 0x000,
    GROUP_FLAG_FAKE_RAID            = 0x001,
    GROUP_FLAG_RAID                 = 0x002,
    GROUP_FLAG_LFG_RESTRICTED       = 0x004, // Script_HasLFGRestrictions()
    GROUP_FLAG_LFG                  = 0x008,
    GROUP_FLAG_DESTROYED            = 0x010,
    GROUP_FLAG_ONE_PERSON_PARTY     = 0x020, // Script_IsOnePersonParty()
    GROUP_FLAG_EVERYONE_ASSISTANT   = 0x040, // Script_IsEveryoneAssistant()

    GROUP_MASK_BGRAID                = GROUP_FLAG_FAKE_RAID | GROUP_FLAG_RAID,
};

enum GroupUpdateFlags
{
    GROUP_UPDATE_FLAG_NONE              = 0x00000000,       // nothing
    GROUP_UPDATE_FLAG_STATUS            = 0x00000001,       // uint16 (GroupMemberStatusFlag)
    GROUP_UPDATE_FLAG_CUR_HP            = 0x00000002,       // uint32 (HP)
    GROUP_UPDATE_FLAG_MAX_HP            = 0x00000004,       // uint32 (HP)
    GROUP_UPDATE_FLAG_POWER_TYPE        = 0x00000008,       // uint8 (PowerType)
    GROUP_UPDATE_FLAG_CUR_POWER         = 0x00000010,       // int16 (power value)
    GROUP_UPDATE_FLAG_MAX_POWER         = 0x00000020,       // int16 (power value)
    GROUP_UPDATE_FLAG_LEVEL             = 0x00000040,       // uint16 (level value)
    GROUP_UPDATE_FLAG_ZONE              = 0x00000080,       // uint16 (zone id)
    GROUP_UPDATE_FLAG_WMO_GROUP_ID      = 0x00000100,       // int16 (WMOGroupID)
    GROUP_UPDATE_FLAG_POSITION          = 0x00000200,       // uint16 (x), uint16 (y), uint16 (z)
    GROUP_UPDATE_FLAG_AURAS             = 0x00000400,       // uint8 (unk), uint64 (mask), uint32 (count), for each bit set: uint32 (spell id) + uint16 (AuraFlags)  (if has flags Scalable -> 3x int32 (bps))
    GROUP_UPDATE_FLAG_PET_GUID          = 0x00000800,       // uint64 (pet guid)
    GROUP_UPDATE_FLAG_PET_NAME          = 0x00001000,       // cstring (name, nullptr terminated string)
    GROUP_UPDATE_FLAG_PET_MODEL_ID      = 0x00002000,       // uint16 (model id)
    GROUP_UPDATE_FLAG_PET_CUR_HP        = 0x00004000,       // uint32 (HP)
    GROUP_UPDATE_FLAG_PET_MAX_HP        = 0x00008000,       // uint32 (HP)
    GROUP_UPDATE_FLAG_PET_POWER_TYPE    = 0x00010000,       // uint8 (PowerType)
    GROUP_UPDATE_FLAG_PET_CUR_POWER     = 0x00020000,       // uint16 (power value)
    GROUP_UPDATE_FLAG_PET_MAX_POWER     = 0x00040000,       // uint16 (power value)
    GROUP_UPDATE_FLAG_PET_AURAS         = 0x00080000,       // [see GROUP_UPDATE_FLAG_AURAS]
    GROUP_UPDATE_FLAG_VEHICLE_SEAT      = 0x00100000,       // int32 (vehicle seat id)
    GROUP_UPDATE_FLAG_PHASE             = 0x00200000,       // int32 (unk), uint32 (phase count), for (count) uint16(phaseId)
    GROUP_UPDATE_FLAG_UNK400000         = 0x00400000,
    GROUP_UPDATE_FLAG_UNK800000         = 0x00800000,
    GROUP_UPDATE_FLAG_UNK1000000        = 0x01000000,
    GROUP_UPDATE_FLAG_UNK2000000        = 0x02000000,
    GROUP_UPDATE_FLAG_UNK4000000        = 0x04000000,
    GROUP_UPDATE_FLAG_UNK8000000        = 0x08000000,
    GROUP_UPDATE_FLAG_UNK10000000       = 0x10000000,
    GROUP_UPDATE_FLAG_UNK20000000       = 0x20000000,
    GROUP_UPDATE_FLAG_UNK40000000       = 0x40000000,
    GROUP_UPDATE_FLAG_UNK80000000       = 0x80000000,

    GROUP_UPDATE_PET = GROUP_UPDATE_FLAG_PET_GUID | GROUP_UPDATE_FLAG_PET_NAME | GROUP_UPDATE_FLAG_PET_MODEL_ID |
                       GROUP_UPDATE_FLAG_PET_CUR_HP | GROUP_UPDATE_FLAG_PET_MAX_HP | GROUP_UPDATE_FLAG_PET_POWER_TYPE |
                       GROUP_UPDATE_FLAG_PET_CUR_POWER | GROUP_UPDATE_FLAG_PET_MAX_POWER | GROUP_UPDATE_FLAG_PET_AURAS, // all pet flags
    GROUP_UPDATE_FULL = GROUP_UPDATE_FLAG_STATUS | GROUP_UPDATE_FLAG_CUR_HP | GROUP_UPDATE_FLAG_MAX_HP |
                        GROUP_UPDATE_FLAG_POWER_TYPE | GROUP_UPDATE_FLAG_CUR_POWER | GROUP_UPDATE_FLAG_MAX_POWER |
                        GROUP_UPDATE_FLAG_LEVEL | GROUP_UPDATE_FLAG_ZONE | GROUP_UPDATE_FLAG_WMO_GROUP_ID |GROUP_UPDATE_FLAG_POSITION |
                        GROUP_UPDATE_FLAG_AURAS | GROUP_UPDATE_FLAG_PHASE | GROUP_UPDATE_FLAG_UNK400000 | GROUP_UPDATE_FLAG_UNK800000 |
                        GROUP_UPDATE_FLAG_UNK1000000 | GROUP_UPDATE_FLAG_UNK2000000 | GROUP_UPDATE_FLAG_UNK4000000 |
                        GROUP_UPDATE_FLAG_UNK8000000 | GROUP_UPDATE_FLAG_UNK10000000 | GROUP_UPDATE_FLAG_UNK20000000 | GROUP_UPDATE_FLAG_UNK40000000
};

class Roll : public LootValidatorRef
{
    public:
        Roll(ObjectGuid _guid, LootItem const& li);
        ~Roll();
        void setLoot(Loot* pLoot);
        Loot* getLoot();
        void targetObjectBuildLink() override;

        ObjectGuid itemGUID;
        uint32 itemid;
        ItemRandomEnchantmentId itemRandomPropId;
        uint32 itemRandomSuffix;
        uint8 itemCount;
        typedef std::map<ObjectGuid, RollVote> PlayerVote;
        PlayerVote playerVote;                              //vote position correspond with player position (in group)
        uint8 totalPlayersRolling;
        uint8 totalNeed;
        uint8 totalGreed;
        uint8 totalPass;
        uint8 itemSlot;
        uint8 rollVoteMask;
};

struct InstanceGroupBind
{
    InstanceSave* save;
    bool perm;
    /* permanent InstanceGroupBinds exist if the leader has a permanent
       PlayerInstanceBind for the same instance. */
    InstanceGroupBind() : save(nullptr), perm(false) { }
};

struct RaidMarkerInfo
{
    ObjectGuid summonerGuid;
    ObjectGuid markerGuid;
};

typedef std::list<RaidMarkerInfo> RaidMarkerList;

/** request member stats checken **/
/// @todo uninvite people that not accepted invite
class TC_GAME_API Group
{
    public:
        struct MemberSlot
        {
            ObjectGuid  guid;
            std::string name;
            uint8       group;
            uint8       flags;
            uint8       roles;
            uint32      guildId;
        };
        typedef std::list<MemberSlot> MemberSlotList;
        typedef MemberSlotList::const_iterator member_citerator;

        typedef std::unordered_map< uint32 /*mapId*/, InstanceGroupBind> BoundInstancesMap;

        struct GroupDisenchantInfo
        {
            ObjectGuid DisenchanterGUID;
            uint32 MaxDisenchantSkillLevel;

            void Initialize()
            {
                DisenchanterGUID = ObjectGuid::Empty;
                MaxDisenchantSkillLevel = 0;
            }
        };

    protected:
        typedef MemberSlotList::iterator member_witerator;
        typedef std::set<Player*> InvitesList;

        typedef std::vector<std::unique_ptr<Roll>> Rolls;

    public:
        Group();
        ~Group();

        // group manipulation methods
        bool   Create(Player* leader);
        void   LoadGroupFromDB(Field* field);
        void   LoadMemberFromDB(ObjectGuid::LowType guidLow, uint8 memberFlags, uint8 subgroup, uint8 roles);
        bool   AddInvite(Player* player);
        void   RemoveInvite(Player* player);
        void   RemoveAllInvites();
        bool   AddLeaderInvite(Player* player);
        bool   AddMember(Player* player);
        bool   RemoveMember(ObjectGuid guid, const RemoveMethod &method = GROUP_REMOVEMETHOD_DEFAULT, ObjectGuid kicker = ObjectGuid::Empty, char const* reason = nullptr);
        void   ChangeLeader(ObjectGuid guid);
 static void   ConvertLeaderInstancesToGroup(Player* player, Group* group, bool switchLeader);
        void   SetLootMethod(LootMethod method);
        void   SetLooterGuid(ObjectGuid guid);
        void   SetMasterLooterGuid(ObjectGuid guid);
        void   UpdateLooterGuid(WorldObject* pLootedObject, bool ifneed = false);
        void   SetLootThreshold(ItemQualities threshold);
        void   Disband(bool hideDestroy = false);
        void   SetLfgRoles(ObjectGuid guid, uint8 roles);
        uint8  GetLfgRoles(ObjectGuid guid);
        void   SetEveryoneIsAssistant(bool apply);

        void   SetGroupMarkerMask(uint32 mask) { m_markerMask = mask; }
        void   AddGroupMarkerMask(uint32 mask) { m_markerMask |= mask; }
        void   RemoveGroupMarkerMask(uint32 mask) { if (mask == 0x20) m_markerMask = 0x20; m_markerMask &= ~mask; }
        bool   HasMarker(uint32 mask) { return (m_markerMask & mask) != 0; }
        uint32 GetMarkerMask() { return m_markerMask; }

        DynamicObject* GetRaidMarkerBySpellId(uint32 spell);
        void   AddMarkerToList(ObjectGuid summonerGuid, ObjectGuid markerGuid) { m_raidMarkers.push_back({ summonerGuid, markerGuid }); }
        void   RemoveRaidMarkerFromList(ObjectGuid markerGuid);
        void   RemoveAllMarkerFromList() { m_raidMarkers.clear(); }
        void   RemoveMarker();

        // properties accessories
        bool IsFull() const;
        bool isLFGGroup()  const;
        bool isLFRGroup()  const;
        bool isRaidGroup() const;
        bool isBGGroup()   const;
        bool isBFGroup()   const;
        bool IsCreated()   const;
        ObjectGuid GetLeaderGUID() const;
        ObjectGuid GetGUID() const;
        ObjectGuid::LowType GetLowGUID() const;
        const char * GetLeaderName() const;
        LootMethod GetLootMethod() const;
        ObjectGuid GetLooterGuid() const;
        ObjectGuid GetMasterLooterGuid() const;
        ItemQualities GetLootThreshold() const;

        uint32 GetDbStoreId() const { return m_dbStoreId; }

        // member manipulation methods
        bool IsMember(ObjectGuid guid) const;
        bool IsLeader(ObjectGuid guid) const;
        ObjectGuid GetMemberGUID(const std::string& name);
        uint8 GetMemberFlags(ObjectGuid guid) const;
        bool IsAssistant(ObjectGuid guid) const
        {
            return (GetMemberFlags(guid) & MEMBER_FLAG_ASSISTANT) == MEMBER_FLAG_ASSISTANT;
        }

        Player* GetInvited(ObjectGuid guid) const;
        Player* GetInvited(const std::string& name) const;

        bool SameSubGroup(ObjectGuid guid1, ObjectGuid guid2) const;
        bool SameSubGroup(ObjectGuid guid1, MemberSlot const* slot2) const;
        bool SameSubGroup(Player const* member1, Player const* member2) const;
        bool HasFreeSlotSubGroup(uint8 subgroup) const;

        MemberSlotList const& GetMemberSlots() const { return m_memberSlots; }
        GroupReference* GetFirstMember() { return m_memberMgr.getFirst(); }
        GroupReference const* GetFirstMember() const { return m_memberMgr.getFirst(); }
        uint32 GetMembersCount() const { return m_memberSlots.size(); }
        uint32 GetInviteeCount() const { return m_invitees.size(); }
        GroupFlags GetGroupFlags() const { return m_groupFlags; }

        uint8 GetMemberGroup(ObjectGuid guid) const;

        void ConvertToLFG();
        void ConvertToLFR();
        void ConvertToRaid();
        void ConvertToGroup();

        void SetBattlegroundGroup(Battleground* bg);
        void SetBattlefieldGroup(Battlefield* bf);
        GroupJoinBattlegroundResult CanJoinBattlegroundQueue(Battleground const* bgOrTemplate, BattlegroundQueueTypeId bgQueueTypeId, uint32 MinPlayerCount, uint32 MaxPlayerCount, bool isRated, uint32 arenaSlot);

        void ChangeMembersGroup(ObjectGuid guid, uint8 group);
        void SetTargetIcon(uint8 id, ObjectGuid whoGuid, ObjectGuid targetGuid);
        void SetGroupMemberFlag(ObjectGuid guid, bool apply, GroupMemberFlags flag);
        void RemoveUniqueGroupMemberFlag(GroupMemberFlags flag);

        Difficulty GetDifficulty(bool isRaid) const;
        Difficulty GetDungeonDifficulty() const;
        Difficulty GetRaidDifficulty() const;
        void SetDungeonDifficulty(Difficulty difficulty);
        void SetRaidDifficulty(Difficulty difficulty);
        bool InCombatToInstance(uint32 instanceId);
        void ResetInstances(uint8 method, bool isRaid, Player* SendMsgTo);

        // -no description-
        //void SendInit(WorldSession* session);
        void SendTargetIconList(WorldSession* session);
        void SendRaidMarkerUpdate();
        void SendRaidMarkerUpdateToPlayer(ObjectGuid playerGUID, bool remove = false);
        void SendUpdate();
        void SendUpdateToPlayer(ObjectGuid playerGUID, MemberSlot* slot = nullptr);
        void SendUpdateDestroyGroupToPlayer(Player* player);
        void UpdatePlayerOutOfRange(Player* player);

        template<class Worker>
        void BroadcastWorker(Worker& worker)
        {
            for (GroupReference* itr = GetFirstMember(); itr != nullptr; itr = itr->next())
                worker(itr->GetSource());
        }

        template<class Worker>
        void BroadcastWorker(Worker const& worker) const
        {
            for (GroupReference const* itr = GetFirstMember(); itr != nullptr; itr = itr->next())
                worker(itr->GetSource());
        }

        void BroadcastPacket(WorldPacket const* packet, bool ignorePlayersInBGRaid, int group = -1, ObjectGuid ignoredPlayer = ObjectGuid::Empty);
        void BroadcastAddonMessagePacket(WorldPacket const* packet, const std::string& prefix, bool ignorePlayersInBGRaid, int group = -1, uint64 ignore = 0);
        void BroadcastReadyCheck(WorldPacket const* packet);
        void OfflineReadyCheck();

        /*********************************************************/
        /***                   LOOT SYSTEM                     ***/
        /*********************************************************/

        bool isRollLootActive() const;
        void SendLootStartRoll(uint32 CountDown, uint32 mapid, Roll const& r);
        void SendLootStartRollToPlayer(uint32 countDown, uint32 mapId, Player* p, bool canNeed, Roll const& r);
        void SendLootRoll(ObjectGuid SourceGuid, ObjectGuid TargetGuid, int32 RollNumber, uint8 RollType, Roll const& r, bool autoPass = false);
        void SendLootRollWon(ObjectGuid SourceGuid, ObjectGuid TargetGuid, int32 RollNumber, uint8 RollType, Roll const& r);
        void SendLootAllPassed(Roll const& roll);
        void SendLooter(Creature* creature, Player* pLooter);
        void GroupLoot(Loot* loot, WorldObject* pLootedObject);
        void NeedBeforeGreed(Loot* loot, WorldObject* pLootedObject);
        void MasterLoot(Loot* loot, WorldObject* pLootedObject);
        Rolls::iterator GetRoll(ObjectGuid Guid);
        void CountTheRoll(Rolls::iterator roll, Map* allowedMap);
        void CountRollVote(ObjectGuid playerGUID, ObjectGuid Guid, uint8 Choise);
        void EndRoll(Loot* loot, Map* allowedMap);

        // related to disenchant rolls
        void ResetMaxEnchantingLevel();

        void LinkMember(GroupReference* pRef);
        void DelinkMember(ObjectGuid guid);

        InstanceGroupBind* BindToInstance(InstanceSave* save, bool permanent, bool load = false);
        void UnbindInstance(uint32 mapid, uint8 difficulty, bool unload = false);
        InstanceGroupBind* GetBoundInstance(Player* player);
        InstanceGroupBind* GetBoundInstance(Map* aMap);
        InstanceGroupBind* GetBoundInstance(MapEntry const* mapEntry);
        InstanceGroupBind* GetBoundInstance(Difficulty difficulty, uint32 mapId);
        BoundInstancesMap& GetBoundInstances(Difficulty difficulty);

        // FG: evil hacks
        void BroadcastGroupUpdate(void);

        // guild misc
        bool IsGuildGroupFor(Player* player);
        uint32 GetMembersCountOfGuild(uint32 guildId);
        uint32 GetNeededMembersOfSameGuild(uint8 arenaType, Map const* map);
        bool MemberLevelIsInRange(uint32 levelMin, uint32 levelMax);
        float GetGuildXpRateForPlayer(Player* player);
        void UpdateGuildFor(ObjectGuid guid, uint32 guildId);

    protected:
        bool _setMembersGroup(ObjectGuid guid, uint8 group);
        void _homebindIfInstance(Player* player);

        void _initRaidSubGroupsCounter();
        member_citerator _getMemberCSlot(ObjectGuid Guid) const;
        member_witerator _getMemberWSlot(ObjectGuid Guid);
        void SubGroupCounterIncrease(uint8 subgroup);
        void SubGroupCounterDecrease(uint8 subgroup);
        void ToggleGroupMemberFlag(member_witerator slot, uint8 flag, bool apply);

        MemberSlotList      m_memberSlots;
        GroupRefManager     m_memberMgr;
        InvitesList         m_invitees;
        ObjectGuid          m_leaderGuid;
        std::string         m_leaderName;
        GroupFlags          m_groupFlags;
        uint32              m_markerMask;
        Difficulty          m_dungeonDifficulty;
        Difficulty          m_raidDifficulty;
        Battleground*       m_bgGroup;
        Battlefield*        m_bfGroup;
        ObjectGuid          m_targetIcons[TARGET_ICONS_COUNT];
        LootMethod          m_lootMethod;
        ItemQualities       m_lootThreshold;
        ObjectGuid          m_looterGuid;
        ObjectGuid          m_masterLooterGuid;
        Rolls               RollId;
        BoundInstancesMap   m_boundInstances[MAX_DIFFICULTY];
        uint8*              m_subGroupsCounts;
        ObjectGuid          m_guid;
        uint32              m_counter;                      // used only in SMSG_GROUP_LIST
        GroupDisenchantInfo m_disenchantInfo;
        uint32              m_dbStoreId;                    // Represents the ID used in database (Can be reused by other groups if group was disbanded)
        RaidMarkerList      m_raidMarkers;
};

#endif
