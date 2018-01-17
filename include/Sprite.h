#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <Python.h>
#include "Config.h"

#include "Ability.h"
#include "Item.h"

#include "AtkDmgType.h"

#include <memory>
#include <unordered_map>
#include <vector>

//forward decl
class cppSimulatorImp;

//SpriteType map to Valve's UnitType enum in the Protobuf
enum UnitType 
{
    UNITTYPE_INVALID,
    UNITTYPE_HERO,
    UNITTYPE_CREEP_HERO,
    UNITTYPE_LANE_CREEP,
    UNITTYPE_JUNGLE_CREEP,
    UNITTYPE_ROSHAN,
    UNITTYPE_TOWER,          // 6
    UNITTYPE_BARRACKS,
    UNITTYPE_SHRINE,
    UNITTYPE_ANCIENT,
    UNITTYPE_EFFIGY,         // 10
    UNITTYPE_COURIER,
    UNITTYPE_WARD
};

enum TeamID
{
    TEAM_UNKNOWN = 1,
    TEAM_RADIANT = 2,
    TEAM_DIRE = 3,
    TEAM_NEUTRAL = 4
};

#define SETATTR(data,type,attr) attr = *(type*)data.at(#attr)

#define GET_CFG \
Engine->get_config()

#define DEF_INIT_DATA_FN(TYPE)\
static SpriteData init##TYPE##Data(cppSimulatorImp* Engine,\
    std::string type_name, void(*p_fn)(SpriteData*,std::string)  = nullptr)\
{\
    std::string json_path = GET_CFG->ConfigDir + "/" + #TYPE + "/" + type_name + ".json";\
    auto data = *ConfigCacheMgr<SpriteData>::getInstance().get(json_path, p_fn);\
    return data;\
}

#define INIT_DATA(ENG, TYPE, TYPE_NAME, FN)\
init##TYPE##Data(ENG, TYPE_NAME, FN)


typedef std::unordered_map<std::string, std::unordered_map<std::string, void*> > SpriteDataType;


class Sprite {
public:
    Sprite(SpriteData data) : unit_type(UNITTYPE_INVALID), addiHPReg(0.0), LastAttackTime(-1),
        exp(0), gold(0), is_alive(true), b_move(false), canvas(NULL), v_handle(NULL), data(data) {}

    virtual ~Sprite(){
        remove_visual_ent();
        Py_XDECREF(v_handle);
    }

    //update data every tick
    virtual void update_data();

    virtual void step() = 0;
    virtual void draw() = 0;

    pos_tup pos_in_wnd();

    void attack(Sprite* target);
    bool isAttacking();
    inline void set_move(pos_tup target) {
        b_move = true;
        move_target = target;
    }
    void move();
    bool damaged(Sprite* attacker, double dmg);
    double attakedDmg(Sprite* attacker, double dmg);
    void dead(Sprite*  attacker);
    void remove_visual_ent();

    static double S2Sdistance(const Sprite& s1,const Sprite& s2);

    inline cppSimulatorImp* get_engine() { return Engine; }

    inline UnitType get_UnitType() { return unit_type; }

    inline double get_HP() { return data.HP; }
    inline double get_AttackTime() { return data.AttackTime; }
    inline double get_Attack() { return data.Attack; }
    inline Side get_side() { return side; }
    inline double get_SightRange() { return data.SightRange; }
    inline pos_tup get_location() { return location; }
    inline bool isAlive() { return is_alive; }
    inline double get_ProjectileSpeed() { return data.ProjectileSpeed; }
    double TimeToDamage(const Sprite* s);

    inline bool isBuilding() { return unit_type >= UNITTYPE_TOWER && unit_type <= UNITTYPE_EFFIGY; }

protected:
    cppSimulatorImp* Engine;
    PyObject* canvas;
    SpriteData data;
    Side side;

    double LastAttackTime;
    double AttackTime;
    double exp;
    double gold;

    // PROTOBUF members
    size_t handle;      // a unique identifier
    UnitType unit_type;
    std::string name;
    TeamID team_id;
    size_t level;
    pos_tup location;
    bool is_alive;
    int player_id;

    size_t bounding_radius;
    size_t facing;          // 0 - 359 degrees
    size_t ground_height;

    size_t vision_range_daytime;
    size_t vision_range_nighttime;

    size_t health;
    size_t max_health;
    float health_regen;

    size_t mana;
    size_t max_mana;
    float mana_regen;

    size_t base_movement_speed;
    size_t current_movement_speed;

    size_t base_damage;
    size_t base_damage_variance;
    size_t bonus_damage;
    size_t attack_damage;
    size_t attack_range;
    float attack_speed;
    float attack_anim_point;    // this is the time when hit is registered but animation not done yet
    size_t attack_acquisition_range;
    size_t attack_projectile_speed;
    size_t attack_target_handle;

    std::vector<Ability*> abilities;
    std::vector<Item*> items;

    // DERIVED & HELPER members
    double addiHPReg;//addtional HP reg: item, talent, etc 

    bool b_move;
    PyObject* v_handle;
    pos_tup move_target;
};

#endif//__SPRITE_H__
