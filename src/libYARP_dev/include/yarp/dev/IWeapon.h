/*
 * Copyright (C) 2017 ICub Facility - Istituto italiano di Tecnologia
 * Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *          Marco Randazzo <marco.randazzo@iit.it>
 *          Andrea Ruzzenenti <andrea.ruzzenenti@iit.it>
 *          Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */



#ifndef YARP_DEV_IWEAPON_H
#define YARP_DEV_IWEAPON_H

#include <yarp/dev/DeviceDriver.h>

#define VOCAB_WEAPON_FIRE         VOCAB4('f','i','r','e')
#define VOCAB_WEAPON_RELOAD       VOCAB4('r','l','o','d')
#define VOCAB_WEAPON_DRAW         VOCAB4('d','r','a','w')

namespace yarp {
    namespace dev {
        class IWeapon;
    }
}

/**
* @ingroup dev_iface_other
*
* A generic interface for weapons installed onboard or carried by the robot
* or vehicle.
*/
class YARP_dev_API yarp::dev::IWeapon
{
public:
    enum WeaponType {
        WeaponRangeUnknown            = 0x00000000,
        WeaponRangeMelee              = 0x00000001, //!< Either a part of the robot or an object that can be used in melee combat
        WeaponRangeShort              = 0x00000002, //!< A thrown object or weapon
        WeaponRangeMedium             = 0x00000004, //!< A bow or a gun
        WeaponRangeLong               = 0x00000008, //!< From precision rifle to interplanetary wars weapons

        WeaponDamageTypeUnknown       = 0x00000000,
        WeaponDamageTypeBludgeoning   = 0x00000010, //!< Used mostly by caverobots
        WeaponDamageTypePiercing      = 0x00000020, //!< Creates an opening in which jewelry may be worn
        WeaponDamageTypeSlashing      = 0x00000040, //!< ///////////////////////////////////////////////
        WeaponDamageTypeExplosive     = 0x00000080, //!< Something that you want the robot to be far away when it blows
        WeaponDamageTypeBurning       = 0x00000100, //!< Burn baby burn! - Disco inferno!
        WeaponDamageTypeElectrocuting = 0x00000200, //!< Pikachu, I choose you!
        WeaponDamageTypeBio           = 0x00000400, //!< Lipstick taser
        WeaponDamageTypeOther         = 0x00000800, //!< Many words hurt more than swords.

        WeaponDetonationModeUnknown   = 0x0000000,
        WeaponDetonationModeNone      = 0x0000100, //!< No matter how much you shake it, it won't blow off
        WeaponDetonationModeOnImpact  = 0x0000200, //!< Explodes when it hits the target
        WeaponDetonationModeDelayed   = 0x0000400, //!< Explodes after a specific amount of time
        WeaponDetonationModeTimer     = 0x0000800, //!< Explodes at a specific time

        WeaponNoiseLevelUnknown       = 0x0000000, //!< If a chainsaw cuts a tree in a forest and there's nobody listening, does it make noise?
        WeaponNoiseLevelLow           = 0x0001000, //!< A quiet or silenced weapon
        WeaponNoiseLevelMedium        = 0x0002000, //!< You can cover it with some loud music
        WeaponNoiseLevelHigh          = 0x0004000, //!< Your neighbour will probably call the police

        WeaponNoiseLevelShout         = 0x0008000, //!< No matter how quiet the weapon is, either the robot or someone else has to shout the name of the weapon

        WeaponTargetSizeUnknown       = 0x00000000,
        WeaponTargetSizeTiny          = 0x00100000, //!< A surgery laser or a toothpick
        WeaponTargetSizeSmall         = 0x00200000, //!< A fly swatter
        WeaponTargetSizeMedium        = 0x00400000, //!< A weapon to hit from a can to a large gorilla
        WeaponTargetSizeLarge         = 0x00800000, //!< A weapon to destroy a vehicle
        WeaponTargetSizeHuge          = 0x01000000, //!< A weapon to demolish a building
        WeaponTargetSizeCity          = 0x02000000, //!< It will leave just a big crater
        WeaponTargetSizeContinent     = 0x04000000, //!< Extintion risk level weapon
        WeaponTargetSizePlanet        = 0x08000000, //!< Death Star like weapon

        WeaponTypeTopSecret           = 0x10000000, //!< Undisclosed meaning

        WeaponTypeReserved            = 0xffffffff,

        WeaponTypeBayonet = WeaponRangeMelee | WeaponRangeShort | WeaponDamageTypePiercing | WeaponTargetSizeMedium, //!< A very tactical weapon if your robot can travel back in time
        WeaponTypeBombchu = RangedWeapon | WeaponDamageTypeExplosive | ShortRange | WeaponTargetSizeMedium, //!< A type of mobile explosive that travel along the ground (and, if a certain kind, walls and ceilings)
        WeaponTypeDeathStar = WeaponRangeLong | WeaponDamageTypeUnknown | WeaponTargetSizePlanet, //!< The target area is only two meters wide. It's a small thermal exhaust port, right below the main port. The shaft leads directly to the reactor system.
        WeaponTypeFlameThrower = WeaponRangeShort | WeaponDamageTypeBurning, //!< The very existence of flame-throwers proves that some time, somewhere, someone said to themselves, You know, I want to set those people over there on fire, but I'm just not close enough to get the job done.
        WeaponTypeGreenOnions = WeaponRangeMelee | WeaponDamageTypeBludgeoning | WeaponNoiseLevelShout, //!< It is used much like a metal sword.
        WeaponTypeGunBlade = WeaponRangeMelee | WeaponRangeShort | WeaponDamageTypePiercing | WeaponDamageTypeSlashing | WeaponDamageTypeExplosive | WeaponTargetSizeMedium, //!< WeaponDamageTypeExplosive only if you hit R1 in the right moment
        WeaponTypeInsultSwordFighting = WeaponRangeMelee | WeaponDamageTypeOther, //!< How appropriate. You fight like a cow.
        WeaponTypeLightSaber = WeaponRangeMelee | WeaponDamageTypeOther, //!< Use the Force.
        WeaponTypeNapalm WeaponRangeMedium | WeaponDamageTypeBurning, //!< You smell that? Do you smell that? Napalm, son. Nothing else in the world smells like that. I love the smell of napalm in the morning.
        WeaponTypeNuke = WeaponRangeMedium | WeaponDamageTypeExplosive | WeaponDetonationModeDelayed | WeaponTargetSizeCity, //!< When Johnny Comes Marching Home.
        WeaponTypePaper = WeaponRangeMelee | WeaponDamageTypeSlashing | WeaponTargetSizeMedium, //!< Ouch! Papercuts hurt!
        WeaponTypeProtonTorpedo = RangedWeapon | WeaponDamageTypeExplosive | LongRange | WeaponTargetSizePlanet, //!< A type of mobile explosive that travel along the ground (and, if a certain kind, walls and ceilings)
        WeaponTypeSunAttack = WeaponRangeMedium | WeaponTargetSizeHuge | WeaponNoiseLevelShout, //!< サンアタック
        WeaponTypeTaser = WeaponRangeShort | WeaponDamageTypeElectrocuting, // Lipstick tasers fall in this category
        WeaponTypeTorpedo = RangedWeapon | WeaponDamageTypeExplosive | LongRange | WeaponTargetSizeLarge, //!< A self-propelled weapon with an explosive warhead
        WeaponTypeWreckingBall = WeaponRangeMelee | WeaponDamageTypeBludgeoning | WeaponTargetSizeHuge
    };

    /**
     * @brief Get the type of weapon
     *
     * @return the type of weapon
     * @sa WeaponType
     */
    virtual WeaponType getWeaponType() const = 0;

    /**
     * @brief The common name for this type of weapon
     *
     * Examples are "sword", "gun", "stick"
     */
    virtual ConstString getGenericWeaponName() const = 0;

    /**
     * @brief The proper weapon name
     *
     * Examples are "Excalibur", "Masamune", "Andúril", "Durendal"
     *
     * @return The name of the weapon
     * @sa getGenericWeaponName
     */
    virtual ConstString getWeaponName() const = 0;

    /**
     * @brief Get weapon producer
     *
     * @return The producer of the weapon
     * @sa getWeaponModel, getWeaponSerialNumber
     */
    virtual ConstString getWeaponProducer() const = 0;

    /**
     * @brief Get weapon model
     *
     * @return The model of the weapon
     * @sa getWeaponName, getWeaponSerialNumber
     */
    virtual ConstString getWeaponModel() const = 0;

    /**
     * @brief Get weapon serial number
     *
     * @return The serial number of the weapon
     * @sa getWeaponName, getWeaponModel
     */
    virtual ConstString getWeaponSerialNumber() const = 0;
    /**
     * @brief Shoot the weapon
     * 
     * @return false on misfire
     */
    virtual bool fire() = 0;

    /**
     * @brief Reload the weapon
     *
     * @return The number of ammonitions after reloading
     */
    virtual size_t reload() = 0;

    /**
     * @brief Draw the weapom
     *
     * @return true if the weapon was drawn
     */
    virtual bool draw() = 0;

    /**
     * @brief Get current number of ammonitions
     *
     * @return Current number of ammonitions
     */
    virtual size_t getAmmo() const = 0;

    /**
     * @brief Get aximum number of ammonitions that can fit in the magazine
     *
     * @return Maximum number of ammonitions
     */
    virtual size_t getMaxAmmo() const = 0;

    /**
     * @brief Get weapon rate of fire
     *
     * @return The number of shoots per second
     */
    virtual double getRateOfFire() const = 0;

    /**
     * @brief Time required to reload or recharge the weapon
     *
     * Not used for melee weapons.
     *
     * @return The time required to reload the weapon in seconds
     */
    virtual double getEstimatedReloadTime() const = 0;

    /**
     * @brief Whether the weapon is ready to shoot.
     *
     * @return true if the weapon is drawn, loaded/charged and
     * ready to be used.
     */
    virtual bool isReady() const = 0;

    /**
     * @brief Whether the weapon is broken
     *
     * @return true if the weapon is working
     */
    virtual bool isOk() const = 0;

    /**
     * @brief Whether the weapon has at least one magic property.
     *
     * @return true for weapons that defy the laws of nature
     */
    virtual bool hasMagicProperty() const = 0;
};

#endif // YARP_DEV_IWEAPON_H
