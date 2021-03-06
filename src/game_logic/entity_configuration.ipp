/* Copyright (C) 2016, Nikolai Wuttke. All rights reserved.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// This file is meant to be included into entity_factory.cpp. It's only
// a separate file to make the amount of code in one file more manageable.

namespace {

using namespace engine::components::parameter_aliases;
using namespace game_logic::components::parameter_aliases;


// The game draws player projectiles after drawing all regular actors, which
// makes them appear on top of everything. But in our case, they are rendered
// using the same mechanism as the other sprites, so we have to explicitly
// assign an order (which is higher than all regular actors' draw order).
const auto PLAYER_PROJECTILE_DRAW_ORDER = data::GameTraits::maxDrawOrder + 1;

const auto MUZZLE_FLASH_DRAW_ORDER = PLAYER_PROJECTILE_DRAW_ORDER + 1;

const auto EFFECT_DRAW_ORDER = MUZZLE_FLASH_DRAW_ORDER + 1;


const auto SCORE_NUMBER_LIFE_TIME = 60;

const base::Point<float> SCORE_NUMBER_MOVE_SEQUENCE[] = {
  {0.0f, -1.0f},
  {0.0f, -1.0f},
  {0.0f, -1.0f},
  {0.0f, -1.0f},
  {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f},
  {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f},
  {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f},
  {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f},
  {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f},
  {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f},
  {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f},
  {0.0f, 0.0f},
  {0.0f, -1.0f}
};


const int SCORE_NUMBER_ANIMATION_SEQUENCE[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1,
  0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1,
  0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2
};


const base::Point<float> CONTAINER_BOUNCE_SEQUENCE[] = {
  {0.0f, -3.0f},
  {0.0f, -2.0f},
  {0.0f, -1.0f},
  {0.0f, 0.0f},
  {0.0f, 1.0f},
  {0.0f, 2.0f},
  {0.0f, 3.0f},
  {0.0f, -1.0f},
  {0.0f, 1.0f}
};


#include "destruction_effect_specs.ipp"


// NOTE: This is only an animation sequence (as opposed to a simple loop)
// because we cannot have more than one instance of the same component type
// per entity, i.e. we can't have two AnimationLoop components.
const int SODA_CAN_ROCKET_FIRE_ANIMATION[] = {6, 7};

const int BOMB_DROPPING_ANIMATION[] = {0, 1, 1, 2};

const int HINT_GLOBE_ANIMATION[] = {
  0, 1, 2, 3, 4, 5, 4, 5, 4, 5, 4, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


base::Point<float> directionToVector(const ProjectileDirection direction) {
  const auto isNegative =
    direction == ProjectileDirection::Left ||
    direction == ProjectileDirection::Up;
  const auto value = isNegative ? -1.0f : 1.0f;

  using Vec = base::Point<float>;
  return isHorizontal(direction) ? Vec{value, 0.0f} : Vec{0.0f, value};
}


ActorID actorIdForProjectile(
  const ProjectileType type,
  const ProjectileDirection direction
) {
  const auto isGoingRight = direction == ProjectileDirection::Right;
  const auto isGoingUp = direction == ProjectileDirection::Up;

  switch (type) {
    case ProjectileType::PlayerRegularShot:
      return isHorizontal(direction) ? 26 : 27;

    case ProjectileType::PlayerLaserShot:
      return isHorizontal(direction) ? 24 : 25;

    case ProjectileType::PlayerRocketShot:
      return isHorizontal(direction)
        ? (isGoingRight ? 10 : 9)
        : (isGoingUp ? 7 : 8);

    case ProjectileType::PlayerFlameShot:
      return isHorizontal(direction)
        ? (isGoingRight ? 206 : 205)
        : (isGoingUp ? 21 : 204);

    case ProjectileType::ReactorDebris:
      return isGoingRight ? 86 : 85;

    case ProjectileType::EnemyLaserShot:
      assert(isHorizontal(direction));
      return 136;

    case ProjectileType::EnemyRocket:
      return isHorizontal(direction)
        ? (isGoingRight ? 57 : 55)
        : 56;

  }

  assert(false);
  return 0;
}


float speedForProjectileType(const ProjectileType type) {
  switch (type) {
    case ProjectileType::PlayerLaserShot:
    case ProjectileType::PlayerFlameShot:
      return 5.0f;

    case ProjectileType::ReactorDebris:
      return 3.0f;
      break;

    case ProjectileType::EnemyRocket:
      return 1.0f;

    default:
      return 2.0f;
  }
}


int damageForProjectileType(const ProjectileType type) {
  switch (type) {
    case ProjectileType::PlayerFlameShot:
    case ProjectileType::PlayerLaserShot:
      return 2;

    case ProjectileType::ReactorDebris:
      return 5;
      break;

    case ProjectileType::PlayerRocketShot:
      return 8;

    default:
      return 1;
  }
}


constexpr bool isPlayerProjectile(const ProjectileType type) {
  return
    type == ProjectileType::PlayerRegularShot ||
    type == ProjectileType::PlayerLaserShot ||
    type == ProjectileType::PlayerFlameShot ||
    type == ProjectileType::PlayerRocketShot;
}


using Message = ai::components::MessengerDrone::Message;

Message MESSAGE_TYPE_BY_INDEX[] = {
  Message::YourBrainIsOurs,
  Message::BringBackTheBrain,
  Message::LiveFromRigel,
  Message::Die,
  Message::CantEscape
};


int messengerDroneTypeIndex(const ActorID id) {
  switch (id) {
    case 213:
    case 214:
    case 215:
    case 216:
      return id - 213; // 0 to 3

    case 220:
      return 4;

    default:
      assert(false);
      return 0;
  }
}


template<typename EntityLike>
void configureMovingEffectSprite(
  EntityLike& entity,
  const SpriteMovement movement
) {
  using namespace engine::components::parameter_aliases;

  entity.template assign<ActivationSettings>(ActivationSettings::Policy::Always);
  // TODO: To match the original, the condition should actually be
  // OnLeavingActiveRegion, but only after the movement sequence is
  // finished.
  entity.template assign<AutoDestroy>(AutoDestroy::afterTimeout(120));

  const auto movementIndex = static_cast<int>(movement);
  entity.template assign<MovementSequence>(MOVEMENT_SEQUENCES[movementIndex]);
  entity.template assign<MovingBody>(
    Velocity{},
    GravityAffected{false},
    IgnoreCollisions{true});
}


void assignSpecialEffectSpriteProperties(ex::Entity entity, const ActorID id) {
  switch (id) {
    case 3:
      entity.assign<BehaviorController>(behaviors::TileBurner{});
      break;

    case 43:
    case 100:
      entity.assign<PlayerDamaging>(1);
      break;

    default:
      break;
  }
}


auto createBlueGuardAiComponent(const ActorID id) {
  using ai::components::BlueGuard;

  if (id == 217) {
    return BlueGuard::typingOnTerminal();
  } else {
    const auto orientation = id == 159 ? Orientation::Right : Orientation::Left;
    return BlueGuard::patrolling(orientation);
  }
}


auto skeletonAiConfig() {
  static auto config = []() {
    ai::components::SimpleWalker::Configuration c;
    c.mAnimEnd = 3;
    c.mWalkAtFullSpeed = false;
    return c;
  }();

  return &config;
}


auto turkeyAiConfig() {
  static auto config = []() {
    ai::components::SimpleWalker::Configuration c;
    c.mAnimEnd = 1;
    c.mWalkAtFullSpeed = true;
    return c;
  }();

  return &config;
}


void configureBonusGlobe(
  ex::Entity entity,
  const BoundingBox& boundingBox,
  const int scoreValue
) {
  entity.assign<AnimationLoop>(1, 0, 3, 0);
  entity.assign<Shootable>(Health{1}, GivenScore{100});
  entity.assign<DestructionEffects>(BONUS_GLOBE_KILL_EFFECT_SPEC);
  entity.assign<ActorTag>(ActorTag::Type::ShootableBonusGlobe);
  addDefaultMovingBody(entity, boundingBox);

  CollectableItem item;
  item.mGivenScore = scoreValue;
  entity.assign<CollectableItem>(item);

  // The entity's sprite contains both the "glass ball" background as
  // well as the colored contents, by using two render slots. The background
  // is using the 2nd render slot (see actorIDListForActor()), so by removing
  // that one, we get just the content.
  auto crystalSprite = *entity.component<Sprite>();
  crystalSprite.mFramesToRender.pop_back();

  ItemContainer coloredDestructionEffect;
  coloredDestructionEffect.assign<Sprite>(crystalSprite);
  coloredDestructionEffect.assign<BoundingBox>(boundingBox);
  coloredDestructionEffect.assign<OverrideDrawOrder>(EFFECT_DRAW_ORDER);
  coloredDestructionEffect.assign<AnimationLoop>(1, 0, 3);
  configureMovingEffectSprite(coloredDestructionEffect, SpriteMovement::FlyUp);

  entity.assign<ItemContainer>(std::move(coloredDestructionEffect));
}


ActorID scoreNumberActor(const ScoreNumberType type) {
  static_assert(
    int(ScoreNumberType::S100) == 0 &&
    int(ScoreNumberType::S500) == 1 &&
    int(ScoreNumberType::S2000) == 2 &&
    int(ScoreNumberType::S5000) == 3 &&
    int(ScoreNumberType::S10000) == 4);

  const auto intType = static_cast<ActorID>(type);
  return 123 + intType;
}


auto actorIDListForActor(const ActorID ID) {
  std::vector<ActorID> actorParts;

  switch (ID) {
    case 0:
      actorParts.push_back(0);
      actorParts.push_back(69);
      break;

    case 5: // player facing left
    case 6: // player facing right
      actorParts.push_back(5);
      actorParts.push_back(6);
      break;

    case 45:
    case 46:
    case 47:
    case 48:
      actorParts.push_back(ID);
      actorParts.push_back(44);
      break;

    case 50:
      actorParts.push_back(51);
      break;

    case 67:
      actorParts.push_back(67);
      actorParts.push_back(70);
      break;

    case 98:
      actorParts.push_back(98);
      actorParts.push_back(99);
      break;

    case 130:
      actorParts.push_back(130);
      actorParts.push_back(260);
      break;

    case 144:
      actorParts.push_back(144);
      actorParts.push_back(149);
      break;

    case 171:
    case 217:
      actorParts.push_back(159);
      break;

    case 201:
      actorParts.push_back(202);
      break;

    // Flying message ships
    case 213:
    case 214:
    case 215:
    case 216:
    case 220:
      actorParts.push_back(107);
      actorParts.push_back(108);
      actorParts.push_back(109);
      actorParts.push_back(110);
      actorParts.push_back(111);
      actorParts.push_back(112);
      actorParts.push_back(113);
      actorParts.push_back(ID);
      break;

    default:
      actorParts.push_back(ID);
      break;
  }
  return actorParts;
}


void configureSprite(Sprite& sprite, const ActorID actorID) {
  switch (actorID) {
    case 0:
      sprite.mFramesToRender = {0};
      break;

    case 62:
      sprite.mFramesToRender = {0, 1};
      break;

    case 67:
      sprite.mFramesToRender = {0};
      break;

    case 98:
      sprite.mFramesToRender = {0};
      break;

    case 115:
      sprite.mFramesToRender = {0, 4};
      break;

    case 144:
      sprite.mFramesToRender = {0};
      break;

    case 150:
      sprite.mFramesToRender = {1};
      break;

    case 154:
      sprite.mFramesToRender = {6};
      break;

    case 171:
      sprite.mFramesToRender = {6};
      break;

    case 200:
      sprite.mFramesToRender = {0, 2};
      break;

    case 209:
      sprite.mFramesToRender = {5, 0};
      break;

    case 217:
      sprite.mFramesToRender = {12};
      break;

    case 231:
      sprite.mFramesToRender = {3};
      break;

    case 237:
      sprite.mFramesToRender = {0, 1, 2, 3};
      break;

    case 279:
      sprite.mFramesToRender = {0, 2};
      break;
  }
}


bool hasAssociatedSprite(const ActorID actorID) {
  switch (actorID) {
    default:
      return true;

    case 102:
    case 106:
    case 116:
    case 137:
    case 138:
    case 141:
    case 142:
    case 143:
    case 139:
    case 221:
    case 233:
    case 234:
    case 241:
    case 250:
    case 251:
    case 254:
      return false;
  }
}


ActorID actorIdForBoxColor(const ContainerColor color) {
  switch (color) {
    case ContainerColor::White: return 161;
    case ContainerColor::Green: return 162;
    case ContainerColor::Red: return 163;
    case ContainerColor::Blue: return 164;
  }

  assert(false);
  return 161;
}


int adjustedDrawOrder(const ActorID id, const int baseDrawOrder) {
  auto scale = [](const int drawOrderValue) {
    constexpr auto SCALE_FACTOR = 10;
    return drawOrderValue * SCALE_FACTOR;
  };

  switch (id) {
    case 7: case 8: case 9: case 10:
    case 24: case 25: case 26: case 27:
    case 21: case 204: case 205: case 206:
    case 85: case 86:
      return scale(PLAYER_PROJECTILE_DRAW_ORDER);

    case 33: case 34: case 35: case 36: // player muzzle flash
      return scale(MUZZLE_FLASH_DRAW_ORDER);

    case 1: // small explosion
    case 2: // rocket explosion
    case 3: // impact flame
    case 11: // rocket smoke
    case 12: // hover-bot debris
    case 13: // hover-bot debris
    case 15: // nuclear waste barrel debris
    case 16: // nuclear waste barrel debris
    case 17: // nuclear waste barrel debris
    case 18: // nuclear waste barrel debris
    case 43: // nuclear explosion
    case 60: // watch-bot container debris
    case 61: // watch-bot container debris
    case 65: // napalm bomb fire
    case 71: // Duke's death particles
    case 72: // bonus globe debris
    case 73: // bonus globe debris
    case 74: // white circular flash
    case 75: // nuclear sludge (from barrel)
    case 84: // smoke cloud
    case 94: // biological debris
    case 96: // missile debris
    case 100: // eye ball projectile
    case 147: // enemy laser muzzle flash
    case 148: // enemy laser muzzle flash
    case 152: // metal grabber claw debris
    case 153: // metal grabber claw debris
    case 165: // yellow fire ball
    case 166: // green fire ball
    case 167: // blue fire ball
    case 169: // soda can debris
    case 170: // soda can debris
    case 192: // petrified monster stone shell debris
    case 193: // petrified monster stone shell debris
    case 194: // petrified monster stone shell debris
    case 195: // petrified monster stone shell debris
    case 196: // petrified monster stone shell debris
    case 197: // petrified monster stone shell debris
    case 198: // petrified monster stone shell debris
    case 199: // petrified monster stone shell debris
    case 232: // spider (shaken off)
    case 241: // wind-blown spider part
    case 242: // wind-blown spider part
    case 243: // wind-blown spider
    case 255: // prisoner debris
    case 300: // Rigelatin soldier projectile
      return scale(EFFECT_DRAW_ORDER);

    // floating score numbers
    case 123:
    case 124:
    case 125:
    case 126:
    case 127:
      return scale(EFFECT_DRAW_ORDER);

    case 63:
      // Make the bomb appear behind the bomber plane
      return scale(baseDrawOrder) - 1;

    default:
      return scale(baseDrawOrder);
  }
}


template<typename... Args>
void addToContainer(components::ItemContainer& container, Args&&... components) {
  (container.mContainedComponents.emplace_back(std::move(components)), ...);
}


template<typename... Args>
components::ItemContainer makeContainer(Args&&... components) {
  typename components::ItemContainer container;
  addToContainer(container, components...);
  return container;
}


void turnIntoContainer(
  ex::Entity entity,
  Sprite containerSprite,
  const int givenScore,
  components::ItemContainer&& container
) {
  // We don't assign a position here, as the container might move before being
  // opened. The item container's onHit callback will set the spawned entity's
  // position when the container is opened.
  auto originalSprite = *entity.component<Sprite>();
  addToContainer(container, originalSprite);

  entity.assign<components::ItemContainer>(std::move(container));
  entity.assign<Shootable>(Health{1}, givenScore);
  addDefaultMovingBody(
    entity,
    engine::inferBoundingBox(containerSprite, entity));
  entity.remove<Sprite>();
  entity.assign<Sprite>(std::move(containerSprite));
}


void addBarrelDestroyEffect(ex::Entity entity) {
  auto container = makeContainer();
  container.mStyle = ItemContainer::ReleaseStyle::NuclearWasteBarrel;
  entity.assign<ItemContainer>(std::move(container));
}


void addItemBoxDestroyEffect(ex::Entity entity) {
  auto container = makeContainer();
  container.mStyle = ItemContainer::ReleaseStyle::ItemBox;
  entity.assign<ItemContainer>(std::move(container));
}

} // namespace


template<typename... Args>
void EntityFactory::configureItemBox(
  ex::Entity entity,
  const ContainerColor color,
  const int givenScore,
  Args&&... components
) {
  auto container = makeContainer(components...);
  container.mStyle = components::ItemContainer::ReleaseStyle::ItemBox;
  addToContainer(
    container,
    Active{},
    MovementSequence{
      CONTAINER_BOUNCE_SEQUENCE, ResetAfterSequence{true}, EnableX{false}});
  addDefaultMovingBody(
    container,
    engine::inferBoundingBox(*entity.component<Sprite>(), entity));

  auto containerSprite = createSpriteForId(actorIdForBoxColor(color));
  turnIntoContainer(entity, containerSprite, givenScore, std::move(container));
  entity.assign<DestructionEffects>(CONTAINER_BOX_KILL_EFFECT_SPEC);
}


void EntityFactory::configureEntity(
  ex::Entity entity,
  const ActorID actorID,
  const BoundingBox& boundingBox
) {
  using namespace effects;

  using DGType = behaviors::DynamicGeometryController::Type;

  const auto difficultyOffset = mDifficulty != Difficulty::Easy
    ? (mDifficulty == Difficulty::Hard ? 2 : 1)
    : 0;

  switch (actorID) {
    case 45: // Blue bonus globe
      configureBonusGlobe(entity, boundingBox, GivenScore{500});
      break;

    case 46: // Red bonus globe
      configureBonusGlobe(entity, boundingBox, GivenScore{2000});
      break;

    case 47: // Green bonus globe
      configureBonusGlobe(entity, boundingBox, GivenScore{5000});
      break;

    case 48: // White bonus globe
      configureBonusGlobe(entity, boundingBox, GivenScore{10000});
      break;

    // Circuit card force field
    case 119:
      entity.assign<PlayerDamaging>(9, true);
      interaction::configureForceField(entity, mSpawnIndex);
      {
        const auto& position = *entity.component<WorldPosition>();

        // There is some additional decoration representing the "emitters"
        // on top/bottom.
        auto fieldEmitters = createSprite(119, position);
        fieldEmitters.component<Sprite>()->mFramesToRender = {0, 1};
      }
      break;

    case 120: // Keyhole (circuit board)
      interaction::configureKeyCardSlot(entity, boundingBox);
      break;

    // Keyhole (blue key)
    case 122:
      interaction::configureKeyHole(entity, boundingBox);
      break;

    // ----------------------------------------------------------------------
    // Empty boxes
    // ----------------------------------------------------------------------
    case 162: // Empty green box
    case 163: // Empty red box
    case 164: // Empty blue box
    case 161: // Empty white box
      entity.assign<Shootable>(Health{1}, GivenScore{100});
      entity.assign<DestructionEffects>(CONTAINER_BOX_KILL_EFFECT_SPEC);
      addDefaultMovingBody(entity, boundingBox);
      addItemBoxDestroyEffect(entity);
      break;

    // ----------------------------------------------------------------------
    // White boxes
    // ----------------------------------------------------------------------
    case 37: // Circuit board
      {
        CollectableItem item;
        item.mGivenScore = 500;
        item.mGivenItem = InventoryItemType::CircuitBoard;
        item.mShownTutorialMessage = TutorialMessageId::FoundAccessCard;
        configureItemBox(
          entity,
          ContainerColor::White,
          100,
          item);
        entity.remove<ActivationSettings>();
      }
      break;

    case 121: // Blue key
      {
        CollectableItem item;
        item.mGivenScore = 500;
        item.mGivenItem = InventoryItemType::BlueKey;
        item.mShownTutorialMessage = TutorialMessageId::FoundBlueKey;
        configureItemBox(
          entity,
          ContainerColor::White,
          100,
          item);
        entity.remove<ActivationSettings>();
      }
      break;

    case 53: // Rapid fire item
      {
        CollectableItem item;
        item.mGivenScore = 500;
        item.mGivenItem = InventoryItemType::RapidFire;
        item.mShownTutorialMessage = TutorialMessageId::FoundRapidFire;
        auto animation = AnimationLoop{1};
        configureItemBox(
          entity,
          ContainerColor::White,
          100,
          item,
          animation);
        entity.remove<ActivationSettings>();
      }
      break;

    case 114: // Cloaking device
      {
        CollectableItem item;
        item.mGivenScore = 500;
        item.mGivenItem = InventoryItemType::CloakingDevice;
        item.mSpawnScoreNumbers = false;
        auto animation = AnimationLoop{1};
        configureItemBox(
          entity,
          ContainerColor::White,
          100,
          item,
          animation);
        entity.remove<ActivationSettings>();
      }
      break;

    // ----------------------------------------------------------------------
    // Red boxes
    // ----------------------------------------------------------------------
    case 42: // Napalm Bomb
      {
        const auto originalDrawOrder =
          entity.component<Sprite>()->mpDrawData->mDrawOrder;

        auto shootable = Shootable{Health{1}};
        shootable.mDestroyWhenKilled = false;
        configureItemBox(
          entity,
          ContainerColor::Red,
          100,
          AnimationLoop{1},
          shootable,
          ActorTag{ActorTag::Type::FireBomb},
          DestructionEffects{
            NAPALM_BOMB_KILL_EFFECT_SPEC,
            DestructionEffects::TriggerCondition::Manual},
          BehaviorController{behaviors::NapalmBomb{}});

        entity.assign<OverrideDrawOrder>(originalDrawOrder);
        entity.assign<ActorTag>(ActorTag::Type::FireBomb);
        entity.remove<ActivationSettings>();
      }
      break;

    case 168: // Soda can
      {
        CollectableItem intactSodaCanCollectable;
        intactSodaCanCollectable.mGivenScore = 100;
        intactSodaCanCollectable.mGivenHealth = 1;
        intactSodaCanCollectable.mShownTutorialMessage =
          TutorialMessageId::FoundSoda;

        CollectableItem flyingSodaCanCollectable;
        flyingSodaCanCollectable.mGivenScore = 2000;

        auto flyingSodaCanSprite = *entity.component<Sprite>();
        // HACK: This is a little trick in order to get the soda can fly up
        // animation to look (almost) exactly as in the original game. The
        // problem is that (in the original) the rocket flame only appears once
        // the can has started moving, which happens one frame after being hit.
        // While our version also correctly starts movement on the frame after
        // being hit, the animation would start one frame too early if we were
        // to initialize the 2nd render slot correctly by pushing back the
        // first element of SODA_CAN_ROCKET_FIRE_ANIMATION. This would be quite
        // noticeable, since the flame can be visible through the floor tiles.
        // So to avoid that, we instead initialize the 2nd render slot with
        // frame 0, which is redundant, since the 1st render slot is already
        // showing it, but that doesn't hurt, and it will be overriden by
        // the animation sequence on the next frame.
        //
        // Note that there is still a small difference between the original and
        // our version: The "shot" soda can will always restart the soda can
        // "turn" animation from frame 0, whereas in the original game, it
        // starts from the frame that was previously shown during the
        // "intact/not shot" version. This is barely noticeable though, and
        // would require a custom Component and System in order to fix -
        // doesn't seem worth it for such a small detail.
        flyingSodaCanSprite.mFramesToRender.push_back(0);

        auto flyingSodaCanContainer = makeContainer(
          flyingSodaCanCollectable,
          flyingSodaCanSprite,
          boundingBox,
          DestructionEffects{
            SODA_CAN_ROCKET_KILL_EFFECT_SPEC,
            DestructionEffects::TriggerCondition::OnCollision},
          AnimationLoop{1, 0, 5},
          AnimationSequence{SODA_CAN_ROCKET_FIRE_ANIMATION, 1, true},
          MovingBody{Velocity{0.0f, -1.0f}, GravityAffected{false}},
          ActivationSettings{ActivationSettings::Policy::Always},
          AutoDestroy{AutoDestroy::Condition::OnWorldCollision});

        configureItemBox(
          entity,
          ContainerColor::Red,
          100,
          intactSodaCanCollectable,
          flyingSodaCanContainer,
          Shootable{Health{1}, GivenScore{0}},
          AnimationLoop{1, 0, 5});
      }
      break;

    case 174: // 6-pack soda
      {
        CollectableItem item;
        item.mGivenScore = 100;
        item.mGivenHealth = 6;
        item.mShownTutorialMessage = TutorialMessageId::FoundSoda;
        configureItemBox(
          entity,
          ContainerColor::Red,
          100,
          item,
          Shootable{Health{1}, GivenScore{10000}},
          DestructionEffects{SODA_SIX_PACK_KILL_EFFECT_SPEC});
      }
      break;

    case 201: // Turkey
      {
        // BUG in the original game: The turkey triggers a floating '100', but
        // doesn't actually give the player any score. Therefore, we don't
        // assign givenScore here.
        CollectableItem cookedTurkeyCollectable;
        cookedTurkeyCollectable.mGivenHealth = 2;

        CollectableItem walkingTurkeyCollectable;
        walkingTurkeyCollectable.mGivenHealth = 1;

        auto cookedTurkeySprite = *entity.component<Sprite>();
        // TODO: It would be nice if we could apply startAnimationLoop() on
        // containers. Since we can't, we currently have to manually setup
        // the render slot with the right frame, in addition to adding a
        // matching AnimationLoop component
        cookedTurkeySprite.mFramesToRender[0] = 4;

        // The turkey is implemented as a nested container: First, the box
        // spawns the living turkey, which in turn is a container spawning
        // the cooked turkey.
        auto cookedTurkeyContainer = makeContainer(
          cookedTurkeyCollectable,
          cookedTurkeySprite,
          AnimationLoop{1, 4, 7},
          Active{});
        addDefaultMovingBody(cookedTurkeyContainer, boundingBox);

        auto livingTurkeyContainer = makeContainer(
          walkingTurkeyCollectable,
          Shootable{1, 0},
          DestructionEffects{LIVING_TURKEY_KILL_EFFECT_SPEC},
          cookedTurkeyContainer,
          AnimationLoop{1, 0, 1},
          ai::components::SimpleWalker{turkeyAiConfig()},
          Active{});
        addDefaultMovingBody(livingTurkeyContainer, boundingBox);

        // We don't use configureItemBox here, since we don't want the bounce
        // we normally get after opening a box.
        turnIntoContainer(
          entity,
          createSpriteForId(actorIdForBoxColor(ContainerColor::Red)),
          100,
          std::move(livingTurkeyContainer));
        entity.assign<DestructionEffects>(CONTAINER_BOX_KILL_EFFECT_SPEC);
        entity.component<ItemContainer>()->mStyle =
          ItemContainer::ReleaseStyle::ItemBox;
      }
      break;

    // ----------------------------------------------------------------------
    // Green boxes
    // ----------------------------------------------------------------------
    case 19: // Rocket launcher
      {
        CollectableItem item;
        item.mGivenScore = 2000;
        item.mGivenWeapon = WeaponType::Rocket;
        item.mShownTutorialMessage = TutorialMessageId::FoundRocketLauncher;
        configureItemBox(
          entity,
          ContainerColor::Green,
          100,
          item,
          ActorTag{ActorTag::Type::CollectableWeapon});
        entity.assign<ActorTag>(ActorTag::Type::CollectableWeapon);
        entity.remove<ActivationSettings>();
      }
      break;

    case 20: // Flame thrower
      {
        CollectableItem item;
        item.mGivenScore = 2000;
        item.mGivenWeapon = WeaponType::FlameThrower;
        item.mShownTutorialMessage = TutorialMessageId::FoundFlameThrower;
        configureItemBox(
          entity,
          ContainerColor::Green,
          100,
          item,
          ActorTag{ActorTag::Type::CollectableWeapon});
        entity.assign<ActorTag>(ActorTag::Type::CollectableWeapon);
        entity.remove<ActivationSettings>();
      }
      break;

    case 22: // Default weapon
      {
        CollectableItem item;
        item.mGivenScore = 2000;
        item.mGivenWeapon = WeaponType::Normal;
        item.mShownTutorialMessage = TutorialMessageId::FoundRegularWeapon;
        configureItemBox(
          entity,
          ContainerColor::Green,
          100,
          item,
          ActorTag{ActorTag::Type::CollectableWeapon});
        entity.assign<ActorTag>(ActorTag::Type::CollectableWeapon);
        entity.remove<ActivationSettings>();
      }
      break;

    case 23: // Laser
      {
        CollectableItem item;
        item.mGivenScore = 2000;
        item.mGivenWeapon = WeaponType::Laser;
        item.mShownTutorialMessage = TutorialMessageId::FoundLaser;
        configureItemBox(
          entity,
          ContainerColor::Green,
          100,
          item,
          ActorTag{ActorTag::Type::CollectableWeapon});
        entity.assign<ActorTag>(ActorTag::Type::CollectableWeapon);
        entity.remove<ActivationSettings>();
      }
      break;

    // ----------------------------------------------------------------------
    // Blue boxes
    // ----------------------------------------------------------------------
    case 28: // Health molecule
      {
        CollectableItem item;
        item.mGivenScore = 500;
        item.mGivenScoreAtFullHealth = 10000;
        item.mGivenHealth = 1;
        item.mShownTutorialMessage = TutorialMessageId::FoundHealthMolecule;
        configureItemBox(
          entity,
          ContainerColor::Blue,
          0,
          item,
          AnimationLoop{1},
          ActorTag{ActorTag::Type::Merchandise});
        entity.assign<ActorTag>(ActorTag::Type::Merchandise);
      }
      break;

    case 155: // Collectable letter N in blue box
      {
        CollectableItem item;
        item.mGivenCollectableLetter = CollectableLetterType::N;
        item.mShownTutorialMessage = TutorialMessageId::FoundLetterN;
        configureItemBox(
          entity,
          ContainerColor::Blue,
          0,
          item,
          ActorTag{ActorTag::Type::Merchandise});
        entity.assign<ActorTag>(ActorTag::Type::Merchandise);
      }
      break;

    case 156: // Collectable letter U in blue box
      {
        CollectableItem item;
        item.mGivenCollectableLetter = CollectableLetterType::U;
        item.mShownTutorialMessage = TutorialMessageId::FoundLetterU;
        configureItemBox(
          entity,
          ContainerColor::Blue,
          0,
          item,
          ActorTag{ActorTag::Type::Merchandise});
        entity.assign<ActorTag>(ActorTag::Type::Merchandise);
      }
      break;

    case 157: // Collectable letter K in blue box
      {
        CollectableItem item;
        item.mGivenCollectableLetter = CollectableLetterType::K;
        item.mShownTutorialMessage = TutorialMessageId::FoundLetterK;
        configureItemBox(
          entity,
          ContainerColor::Blue,
          0,
          item,
          ActorTag{ActorTag::Type::Merchandise});
        entity.assign<ActorTag>(ActorTag::Type::Merchandise);
      }
      break;

    case 158: // Collectable letter E in blue box
      {
        CollectableItem item;
        item.mGivenCollectableLetter = CollectableLetterType::E;
        item.mShownTutorialMessage = TutorialMessageId::FoundLetterE;
        configureItemBox(
          entity,
          ContainerColor::Blue,
          0,
          item,
          ActorTag{ActorTag::Type::Merchandise});
        entity.assign<ActorTag>(ActorTag::Type::Merchandise);
      }
      break;

    case 187: // Collectable letter M in blue box
      {
        CollectableItem item;
        item.mGivenCollectableLetter = CollectableLetterType::M;
        configureItemBox(
          entity,
          ContainerColor::Blue,
          0,
          item,
          ActorTag{ActorTag::Type::Merchandise});
        entity.assign<ActorTag>(ActorTag::Type::Merchandise);
      }
      break;

    case 160: // Video game cartridge in blue box
      {
        CollectableItem item;
        item.mGivenScore = 500;
        configureItemBox(
          entity,
          ContainerColor::Blue,
          0,
          item,
          ActorTag{ActorTag::Type::Merchandise});
        entity.assign<ActorTag>(ActorTag::Type::Merchandise);
      }
      break;

    case 172: // Sunglasses in blue box
      {
        CollectableItem item;
        item.mGivenScore = 100;
        configureItemBox(
          entity,
          ContainerColor::Blue,
          0,
          item,
          ActorTag{ActorTag::Type::Merchandise});
        entity.assign<ActorTag>(ActorTag::Type::Merchandise);
      }
      break;

    case 173: // Phone in blue box
      {
        CollectableItem item;
        item.mGivenScore = 2000;
        configureItemBox(
          entity,
          ContainerColor::Blue,
          0,
          item,
          ActorTag{ActorTag::Type::Merchandise});
        entity.assign<ActorTag>(ActorTag::Type::Merchandise);
      }
      break;

    case 181: // Boom box in blue box
      {
        CollectableItem item;
        item.mGivenScore = 1000;
        configureItemBox(
          entity,
          ContainerColor::Blue,
          0,
          item,
          ActorTag{ActorTag::Type::Merchandise});
        entity.assign<ActorTag>(ActorTag::Type::Merchandise);
      }
      break;

    case 182: // Game disk in blue box
      {
        CollectableItem item;
        item.mGivenScore = 500;
        configureItemBox(
          entity,
          ContainerColor::Blue,
          0,
          item,
          ActorTag{ActorTag::Type::Merchandise});
        entity.assign<ActorTag>(ActorTag::Type::Merchandise);
      }
      break;

    case 183: // TV in blue box
      {
        CollectableItem item;
        item.mGivenScore = 1500;
        configureItemBox(
          entity,
          ContainerColor::Blue,
          0,
          item,
          ActorTag{ActorTag::Type::Merchandise});
        entity.assign<ActorTag>(ActorTag::Type::Merchandise);
      }
      break;

    case 184: // Camera in blue box
      {
        CollectableItem item;
        item.mGivenScore = 2500;
        configureItemBox(
          entity,
          ContainerColor::Blue,
          0,
          item,
          ActorTag{ActorTag::Type::Merchandise});
        entity.assign<ActorTag>(ActorTag::Type::Merchandise);
      }
      break;

    case 185: // Computer in blue box
      {
        CollectableItem item;
        item.mGivenScore = 500;
        configureItemBox(
          entity,
          ContainerColor::Blue,
          0,
          item,
          ActorTag{ActorTag::Type::Merchandise});
        entity.assign<ActorTag>(ActorTag::Type::Merchandise);
      }
      break;

    case 186: // CD in blue box
      {
        CollectableItem item;
        item.mGivenScore = 500;
        configureItemBox(
          entity,
          ContainerColor::Blue,
          0,
          item,
          ActorTag{ActorTag::Type::Merchandise});
        entity.assign<ActorTag>(ActorTag::Type::Merchandise);
      }
      break;

    case 274: // T-Shirt in blue box
      {
        CollectableItem item;
        item.mGivenScore = 5000;
        configureItemBox(
          entity,
          ContainerColor::Blue,
          0,
          item,
          ActorTag{ActorTag::Type::Merchandise});
        entity.assign<ActorTag>(ActorTag::Type::Merchandise);
      }
      break;

    case 275: // Video tape in blue box
      {
        CollectableItem item;
        item.mGivenScore = 500;
        configureItemBox(
          entity,
          ContainerColor::Blue,
          0,
          item,
          ActorTag{ActorTag::Type::Merchandise});
        entity.assign<ActorTag>(ActorTag::Type::Merchandise);
      }
      break;

    case 50: // teleporter
    case 51: // teleporter
      entity.assign<AnimationLoop>(1);
      entity.assign<Interactable>(InteractableType::Teleporter);
      entity.assign<BoundingBox>(BoundingBox{{2, 0}, {2, 5}});
      break;

    case 133: // respawn checkpoint
      entity.assign<BehaviorController>(interaction::RespawnCheckpoint{});
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<ActivationSettings>(ActivationSettings::Policy::Always);
      break;

    case 239: // Special hint globe
      entity.assign<Shootable>(Health{3}, GivenScore{100});
      entity.assign<DestructionEffects>(TECH_KILL_EFFECT_SPEC);
      entity.assign<AnimationSequence>(HINT_GLOBE_ANIMATION, 0, true);
      addDefaultMovingBody(entity, boundingBox);
      {
        CollectableItem item;
        item.mGivenScore = 10000;
        item.mGivenItem = InventoryItemType::SpecialHintGlobe;
        entity.assign<CollectableItem>(item);
      }
      break;


    // ----------------------------------------------------------------------
    // Enemies
    // ----------------------------------------------------------------------

    case 0: // Cylindrical robot with blinking 'head', aka hover-bot
      entity.assign<Shootable>(Health{1 + difficultyOffset}, GivenScore{150});
      addDefaultMovingBody(entity, boundingBox);
      entity.component<Sprite>()->mShow = false;
      entity.assign<ai::components::HoverBot>();
      entity.assign<DestructionEffects>(
        HOVER_BOT_KILL_EFFECT_SPEC,
        DestructionEffects::TriggerCondition::OnKilled,
        mSpriteFactory.actorFrameRect(actorID, 0));
      break;

    // Green panther
    case 31:
    case 32:
      entity.assign<DestructionEffects>(
        BIOLOGICAL_ENEMY_KILL_EFFECT_SPEC,
        DestructionEffects::TriggerCondition::OnKilled,
        mSpriteFactory.actorFrameRect(actorID, 0));
      break;

    // Wall-mounted flame thrower
    case 38: // ->
    case 39: // <-
      entity.assign<DestructionEffects>(TECH_KILL_EFFECT_SPEC);
      break;

    case 49: // Bouncing robot with big eye
      entity.assign<Shootable>(Health{6 + difficultyOffset}, GivenScore{1000});
      entity.assign<PlayerDamaging>(Damage{1});
      entity.assign<DestructionEffects>(
        SIMPLE_TECH_KILL_EFFECT_SPEC,
        DestructionEffects::TriggerCondition::OnKilled,
        mSpriteFactory.actorFrameRect(actorID, 0));
      addDefaultMovingBody(entity, boundingBox);
      entity.component<MovingBody>()->mGravityAffected = false;
      entity.assign<BehaviorController>(behaviors::WatchBot{});
      break;

    case 54: // Rocket launcher turret
      entity.assign<Shootable>(Health{3}, GivenScore{500});
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<PlayerDamaging>(Damage{1});
      entity.assign<ai::components::RocketTurret>();
      entity.assign<DestructionEffects>(
        SIMPLE_TECH_KILL_EFFECT_SPEC,
        DestructionEffects::TriggerCondition::OnKilled,
        mSpriteFactory.actorFrameRect(actorID, 0));
      break;

    // Rocket turret rockets
    case 55:
    case 56:
    case 57:
      entity.assign<Shootable>(Health{1}, GivenScore{10});
      entity.assign<DestructionEffects>(TECH_KILL_EFFECT_SPEC);
      entity.assign<BoundingBox>(boundingBox);
      entity.component<Sprite>()->mFramesToRender.push_back(1);
      entity.assign<AnimationLoop>(1, 1, 2, 1);
      break;

    case 58: // Watch-bot container carrier
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<Shootable>(Health{5}, GivenScore{500});
      entity.assign<PlayerDamaging>(1);
      entity.assign<DestructionEffects>(TECH_KILL_EFFECT_SPEC);
      entity.assign<ActivationSettings>(
        ActivationSettings::Policy::AlwaysAfterFirstActivation);
      entity.assign<BehaviorController>(behaviors::WatchBotCarrier{});
      break;

    case 62: // Bomb dropping space ship
      // Not player damaging, only the bombs are
      entity.assign<Shootable>(Health{6 + difficultyOffset}, GivenScore{5000});
      entity.assign<DestructionEffects>(TECH_KILL_EFFECT_SPEC);
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<AnimationLoop>(1, 1, 2, 1);
      entity.assign<ActivationSettings>(
        ActivationSettings::Policy::AlwaysAfterFirstActivation);
      entity.assign<BehaviorController>(behaviors::BomberPlane{});
      break;

    case 63: // Big bomb
      entity.assign<Shootable>(Health{1}, GivenScore{200});
      entity.assign<PlayerDamaging>(1);
      entity.assign<AnimationSequence>(BOMB_DROPPING_ANIMATION);
      entity.assign<DestructionEffects>(
        BIG_BOMB_DETONATE_EFFECT_SPEC,
        DestructionEffects::TriggerCondition::OnCollision);
      entity.assign<BehaviorController>(behaviors::BigBomb{});
      addDefaultMovingBody(entity, boundingBox);
      engine::reassign<ActivationSettings>(
        entity, ActivationSettings{ActivationSettings::Policy::Always});
      break;

    case 64: // Bouncing spike ball
      entity.assign<Shootable>(Health{6 + difficultyOffset}, GivenScore{1000});
      entity.assign<DestructionEffects>(SPIKE_BALL_KILL_EFFECT_SPEC);
      entity.assign<PlayerDamaging>(1);
      entity.assign<BoundingBox>(boundingBox);
      ai::configureSpikeBall(entity);
      break;

    case 67: // Green slime blob
      entity.assign<Shootable>(Health{6 + difficultyOffset}, GivenScore{1500});
      entity.assign<DestructionEffects>(
        BIOLOGICAL_ENEMY_KILL_EFFECT_SPEC,
        DestructionEffects::TriggerCondition::OnKilled,
        mSpriteFactory.actorFrameRect(actorID, 0));
      entity.assign<PlayerDamaging>(Damage{1});
      entity.assign<ai::components::SlimeBlob>();
      addDefaultMovingBody(entity, boundingBox);
      entity.component<MovingBody>()->mGravityAffected = false;
      break;

    case 68: // Green slime container
      entity.assign<Shootable>(Health{1}, GivenScore{100});
      ai::configureSlimeContainer(entity);
      entity.assign<DestructionEffects>(SLIME_CONTAINER_KILL_EFFECT_SPEC);
      break;

    case 76: // Small bomb
      entity.assign<PlayerDamaging>(1);
      entity.assign<AnimationSequence>(BOMB_DROPPING_ANIMATION);
      entity.assign<DestructionEffects>(
        SMALL_BOMB_DETONATE_EFFECT_SPEC,
        DestructionEffects::TriggerCondition::OnCollision);
      entity.assign<BehaviorController>(behaviors::BigBomb{});
      addDefaultMovingBody(entity, boundingBox);
      engine::reassign<ActivationSettings>(
        entity, ActivationSettings{ActivationSettings::Policy::Always});
      break;

    case 78: // Snake
      // Not player damaging, but can eat duke
      // Only 1 health when Duke has been eaten
      entity.assign<Shootable>(Health{8 + difficultyOffset}, GivenScore{5000});
      entity.assign<DestructionEffects>(TECH_KILL_EFFECT_SPEC);
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<ActivationSettings>(
        ActivationSettings::Policy::AlwaysAfterFirstActivation);
      entity.assign<BehaviorController>(behaviors::Snake{});
      entity.assign<Orientation>(Orientation::Left);
      break;

    case 79: // Security camera, ceiling-mounted
    case 80: // Security camera, floor-mounted
      entity.assign<Shootable>(Health{1}, GivenScore{100});
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<BehaviorController>(behaviors::SecurityCamera{});
      entity.assign<DestructionEffects>(CAMERA_KILL_EFFECT_SPEC);
      entity.assign<ActorTag>(ActorTag::Type::ShootableCamera);
      break;

    case 81: // Green creature attached to ceiling, sucking in player
      entity.assign<Shootable>(
        Health{15 + 3 * difficultyOffset}, GivenScore{300});
      entity.assign<DestructionEffects>(
        BIOLOGICAL_ENEMY_KILL_EFFECT_SPEC,
        DestructionEffects::TriggerCondition::OnKilled,
        mSpriteFactory.actorFrameRect(actorID, 0));
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<BehaviorController>(behaviors::CeilingSucker{});
      break;

    case 97: // Small eye-shaped robot, walking on wall
      entity.assign<DestructionEffects>(TECH_KILL_EFFECT_SPEC);
      break;

    case 98: // Eye-ball throwing monster
      entity.assign<Shootable>(Health{8}, GivenScore{2000});
      entity.assign<DestructionEffects>(EYE_BALL_THROWER_KILL_EFFECT_SPEC);
      entity.assign<PlayerDamaging>(1);
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<Orientation>(Orientation::Left);
      entity.assign<ActivationSettings>(
        ActivationSettings::Policy::AlwaysAfterFirstActivation);
      entity.assign<BehaviorController>(behaviors::EyeballThrower{});
      break;

    case 115: // hover bot generator
      entity.assign<AnimationLoop>(1, 0, 3);
      entity.assign<Shootable>(Health{20}, GivenScore{2500});
      entity.assign<DestructionEffects>(TECH_KILL_EFFECT_SPEC);
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<ai::components::HoverBotSpawnMachine>();
      break;

    case 134: // Walking skeleton
      entity.assign<Shootable>(Health{2 + difficultyOffset}, GivenScore{100});
      entity.assign<DestructionEffects>(
        SKELETON_KILL_EFFECT_SPEC,
        DestructionEffects::TriggerCondition::OnKilled,
        mSpriteFactory.actorFrameRect(actorID, 0));
      entity.assign<PlayerDamaging>(Damage{1});
      entity.assign<ai::components::SimpleWalker>(skeletonAiConfig());
      addDefaultMovingBody(entity, boundingBox);
      break;

    case 151: // Floating ball, opens up and shoots lasers
      entity.assign<Shootable>(Health{3 + difficultyOffset}, GivenScore{1000});
      entity.assign<DestructionEffects>(TECH_KILL_EFFECT_SPEC);
      entity.assign<PlayerDamaging>(Damage{1});
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<ActivationSettings>(
        ActivationSettings::Policy::AlwaysAfterFirstActivation);
      entity.assign<BehaviorController>(behaviors::FloatingLaserBot{});
      break;

    case 154: // Spider
      entity.assign<Shootable>(Health{1 + difficultyOffset}, GivenScore{101});
      entity.assign<DestructionEffects>(SPIDER_KILL_EFFECT_SPEC);
      entity.assign<PlayerDamaging>(Damage{1});
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<Orientation>(Orientation::Left);
      entity.assign<MovingBody>(Velocity{0.f, 0.f}, GravityAffected{false});
      entity.assign<ActivationSettings>(
        ActivationSettings::Policy::AlwaysAfterFirstActivation);
      entity.assign<ai::components::Spider>();
      break;

    case 176: // green bird
      entity.assign<DestructionEffects>(
        BIOLOGICAL_ENEMY_KILL_EFFECT_SPEC,
        DestructionEffects::TriggerCondition::OnKilled,
        mSpriteFactory.actorFrameRect(actorID, 0));
      break;

    // petrified green monster
    case 189: // <-
    case 190: // ->
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<DestructionEffects>(
        EXTENDED_BIOLOGICAL_ENEMY_KILL_EFFECT_SPEC,
        DestructionEffects::TriggerCondition::OnKilled,
        mSpriteFactory.actorFrameRect(actorID, 0));
      break;

    case 271: // Small flying ship 1
    case 272: // Small flying ship 2
    case 273: // Small flying ship 3
      entity.assign<PlayerDamaging>(Damage{1});
      entity.assign<Shootable>(Health{2 + difficultyOffset}, GivenScore{100});
      entity.assign<ActivationSettings>(
        ActivationSettings::Policy::AlwaysAfterFirstActivation);
      entity.assign<BoundingBox>(boundingBox);
      break;

    // Guard wearing blue space suit
    case 159: // ->
    case 171: // <-
    case 217: // using terminal
      entity.assign<PlayerDamaging>(Damage{1});
      entity.assign<Shootable>(Health{2 + difficultyOffset}, GivenScore{3000});
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<ActivationSettings>(
        ActivationSettings::Policy::AlwaysAfterFirstActivation);
      entity.assign<ai::components::BlueGuard>(
        createBlueGuardAiComponent(actorID));
      entity.assign<DestructionEffects>(
        BLUE_GUARD_KILL_EFFECT_SPEC,
        DestructionEffects::TriggerCondition::OnKilled,
        mSpriteFactory.actorFrameRect(159, 0));
      break;


    // Laser turret
    case 131:
      // gives one point when shot with normal shot, 500 when destroyed.
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<ActorTag>(ActorTag::Type::MountedLaserTurret);
      ai::configureLaserTurret(entity, GivenScore{500});
      break;

    case 200: // Boss (episode 1)
      entity.assign<AnimationLoop>(1, 0, 1);
      entity.assign<PlayerDamaging>(Damage{1});
      entity.assign<Shootable>(
        Health{110 + 20 * difficultyOffset}, GivenScore{0});
      entity.component<Shootable>()->mDestroyWhenKilled = false;
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<MovingBody>(Velocity{}, GravityAffected{false});
      entity.assign<BehaviorController>(behaviors::BossEpisode1{});
      entity.assign<ActivationSettings>(
        ActivationSettings::Policy::AlwaysAfterFirstActivation);
      break;

    case 203: // Red bird
      entity.assign<Shootable>(Health{1 + difficultyOffset}, GivenScore{100});
      entity.assign<DestructionEffects>(RED_BIRD_KILL_EFFECT_SPEC);
      entity.assign<PlayerDamaging>(Damage{1});
      entity.assign<BoundingBox>(boundingBox);
      configureRedBird(entity);
      break;

    case 219: // Smash hammer
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<ActivationSettings>(
        ActivationSettings::Policy::AlwaysAfterFirstActivation);
      entity.assign<BehaviorController>(behaviors::SmashHammer{});
      entity.assign<CustomRenderFunc>(&behaviors::SmashHammer::render);
      break;

    case 244: // Small uni-cycle robot
      entity.assign<DestructionEffects>(TECH_KILL_EFFECT_SPEC);
      break;

    case 253: // Monster in prison cell, aggressive
      entity.assign<ai::components::Prisoner>(true);
      entity.assign<BoundingBox>(BoundingBox{{2,0}, {3, 3}});
      entity.assign<Shootable>(Health{1}, GivenScore{500});
      entity.component<Shootable>()->mInvincible = true;
      entity.component<Shootable>()->mDestroyWhenKilled = false;
      break;

    case 261: // Monster in prison cell, passive
      entity.assign<ai::components::Prisoner>(false);
      entity.assign<BoundingBox>(boundingBox);
      break;

    case 280: // final boss projectile
      entity.assign<DestructionEffects>(
        BOSS4_PROJECTILE_KILL_EFFECT_SPEC,
        DestructionEffects::TriggerCondition::OnKilled,
        mSpriteFactory.actorFrameRect(actorID, 0));
      break;

    case 299: // Rigelatin soldier
      entity.assign<Shootable>(
        Health{27 + 2 * difficultyOffset}, GivenScore{2100});
      entity.assign<BehaviorController>(behaviors::RigelatinSoldier{});
      entity.assign<Orientation>(Orientation::Left);
      addDefaultMovingBody(entity, boundingBox);
      entity.component<MovingBody>()->mGravityAffected = false;
      entity.assign<DestructionEffects>(
        RIGELATIN_KILL_EFFECT_SPEC,
        DestructionEffects::TriggerCondition::OnKilled,
        mSpriteFactory.actorFrameRect(299, 0));
      break;

    // ----------------------------------------------------------------------
    // Various
    // ----------------------------------------------------------------------

    case 14: // Nuclear waste barrel, empty
      entity.assign<Shootable>(Health{1}, GivenScore{100});
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<DestructionEffects>(NUCLEAR_WASTE_BARREL_KILL_EFFECT_SPEC);
      addBarrelDestroyEffect(entity);
      break;

    case 75: // Nuclear waste barrel, slime inside
      {
        const auto& sprite = *entity.component<Sprite>();
        const auto numAnimationFrames = static_cast<int>(
          sprite.mpDrawData->mFrames.size());
        auto container = makeContainer(
          PlayerDamaging{Damage{1}},
          AnimationLoop{1},
          AutoDestroy::afterTimeout(numAnimationFrames),
          Active{});
        container.mStyle = ItemContainer::ReleaseStyle::NuclearWasteBarrel;
        addDefaultMovingBody(container, boundingBox);

        auto barrelSprite = createSpriteForId(14);
        turnIntoContainer(entity, barrelSprite, 200, std::move(container));
        entity.assign<DestructionEffects>(
          NUCLEAR_WASTE_BARREL_KILL_EFFECT_SPEC);
      }
      break;

    case 66: // Destroyable reactor
      entity.assign<Shootable>(Health{10}, GivenScore{20000});
      entity.assign<PlayerDamaging>(Damage{9}, IsFatal{true});
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<AnimationLoop>(1);
      entity.assign<DestructionEffects>(
        REACTOR_KILL_EFFECT_SPEC,
        DestructionEffects::TriggerCondition::OnKilled,
        mSpriteFactory.actorFrameRect(66, 0));
      entity.assign<ActorTag>(ActorTag::Type::Reactor);
      break;

    case 93: // Blue force field (disabled by cloak)
      entity.assign<PlayerDamaging>(Damage{1});
      entity.assign<Shootable>(Health{100});
      entity.component<Shootable>()->mDestroyWhenKilled = false;

      entity.assign<BoundingBox>(boundingBox);
      entity.assign<BehaviorController>(behaviors::SuperForceField{});
      entity.assign<ActivationSettings>(ActivationSettings::Policy::Always);
      break;

    case 95: // Missile, broken (falls over)
      {
        auto shootable = Shootable{Health{1}};
        shootable.mDestroyWhenKilled = false;

        addDefaultMovingBody(entity, boundingBox);
        entity.assign<Shootable>(shootable);
        entity.assign<DestructionEffects>(
          BROKEN_MISSILE_DETONATE_EFFECT_SPEC,
          DestructionEffects::TriggerCondition::Manual);
        entity.assign<BehaviorController>(behaviors::BrokenMissile{});
      }
      break;

    case 128: // Sliding door, vertical
      entity.assign<ai::components::VerticalSlidingDoor>();
      entity.assign<BoundingBox>(BoundingBox{{0, 0}, {1, 8}});
      entity.assign<engine::components::SolidBody>();
      entity.assign<CustomRenderFunc>(&renderVerticalSlidingDoor);
      break;

    case 130: // Blowing fan
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<ActivationSettings>(
        ActivationSettings::Policy::AlwaysAfterFirstActivation);
      entity.assign<BehaviorController>(behaviors::BlowingFan{});
      break;

    case 132: // Sliding door, horizontal
      entity.assign<ai::components::HorizontalSlidingDoor>();
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<engine::components::SolidBody>();
      break;

    case 144: // Wall-destroying missile
      {
        auto shootable = Shootable{Health{1}};
        shootable.mDestroyWhenKilled = false;

        entity.assign<Shootable>(shootable);
        entity.assign<BoundingBox>(boundingBox);
        entity.assign<DestructionEffects>(
          MISSILE_DETONATE_EFFECT_SPEC,
          DestructionEffects::TriggerCondition::Manual);
        entity.assign<ActivationSettings>(
          ActivationSettings::Policy::AlwaysAfterFirstActivation);
        entity.assign<BehaviorController>(behaviors::Missile{});
      }
      break;

    case 209: // Rocket elevator
      interaction::configureElevator(entity);
      break;

    case 212: // Lava pool
    case 235: // Slime pool
    case 262: // Fire (variant 1)
    case 263: // Fire (variant 2)
      entity.assign<PlayerDamaging>(Damage{1});
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<AnimationLoop>(1);
      break;

    case 117: // Pipe dripping green stuff
      entity.assign<AnimationLoop>(1);
      entity.assign<DrawTopMost>();
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<BehaviorController>(behaviors::SlimePipe{});
      break;

    case 208: // floating exit sign to right
    case 252: // floating exit sign to left
      entity.assign<Shootable>(Health{5}, GivenScore{10000});
      entity.assign<DestructionEffects>(EXIT_SIGN_KILL_EFFECT_SPEC);
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<AnimationLoop>(1);
      break;

    case 296: // floating arrow
      entity.assign<Shootable>(Health{5}, GivenScore{500});
      entity.assign<DestructionEffects>(FLOATING_ARROW_KILL_EFFECT_SPEC);
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<AnimationLoop>(1);
      break;

    case 236: // Radar dish
      entity.assign<Shootable>(Health{4}, GivenScore{2000});
      entity.assign<DestructionEffects>(RADAR_DISH_KILL_EFFECT_SPEC);
      entity.assign<BoundingBox>(boundingBox);
      entity.assign<AnimationLoop>(1);
      entity.assign<components::RadarDish>();
      break;

    case 237: // Radar dish computer
      entity.assign<components::RadarComputer>();
      break;

    case 240: // Special hint machine
      entity.assign<Interactable>(InteractableType::HintMachine);
      entity.assign<BoundingBox>(boundingBox);
      break;

    case 188: // rotating floor spikes
      entity.assign<PlayerDamaging>(1);
      entity.assign<AnimationLoop>(1);
      break;

    case 210: // Computer showing "Duke escaped"
    case 222: // Lava fall left
    case 223: // Lava fall right
    case 224: // Water fall left
    case 225: // Water fall right
    case 228: // Water surface splash left
    case 229: // Water surface splash center
    case 230: // Water surface splash right
    case 257: // Shallow water (variant 1)
    case 258: // Shallow water (variant 2)
      entity.assign<AnimationLoop>(1);
      break;

    // Flying message ships
    case 213: // "Your brain is ours!"
    case 214: // "Bring back the brain! ... Please stand by"
    case 215: // "Live from Rigel it's Saturday night!"
    case 216: // "Die!"
    case 220: // "You cannot escape us! You will get your brain sucked!"
      {
        const auto typeIndex = messengerDroneTypeIndex(actorID);

        // The original game uses the actor's "score" field to store which
        // type of message is shown. The result is that the message ships will
        // give between 0 and 4 points of score, depending on their type.
        // It's unclear whether this is intentional, it seems like it might not
        // be because this score value is assigned in the update() function,
        // not when constructing the actor.
        entity.assign<Shootable>(Health{1}, GivenScore{typeIndex});
        entity.assign<DestructionEffects>(TECH_KILL_EFFECT_SPEC);
        entity.assign<BoundingBox>(boundingBox);
        entity.component<Sprite>()->mFramesToRender.clear();

        entity.assign<ai::components::MessengerDrone>(
          MESSAGE_TYPE_BY_INDEX[typeIndex]);
        entity.assign<ActivationSettings>(
          ActivationSettings::Policy::AlwaysAfterFirstActivation);
      }
      break;

    case 231: // Lava riser
      entity.assign<AnimationLoop>(1, 3, 5);
      break;

    case 246: // Rocket exhaust flame left
    case 247: // Rocket exhaust flame right
    case 248: // Small rocket exhaust flame left
    case 249: // Small rocket exhaust flame right
      entity.assign<AnimationLoop>(2);
      break;

    case 139: // level exit
      entity.assign<Trigger>(TriggerType::LevelExit);
      entity.assign<BoundingBox>(BoundingBox{{0, 0}, {1, 1}});
      break;

    case 106: // shootable wall, explodes into small pieces
      entity.assign<Shootable>(Health{1});
      entity.component<Shootable>()->mAlwaysConsumeInflictor = true;
      entity.component<Shootable>()->mCanBeHitWhenOffscreen = true;
      {
        // Shootable walls have a bounding box that's one unit wider than the
        // actual area.
        auto adjustedBbox = boundingBox;
        adjustedBbox.size.width += 2;
        adjustedBbox.size.height += 2;
        adjustedBbox.topLeft.x -= 1;
        adjustedBbox.topLeft.y += 1;
        entity.assign<BoundingBox>(adjustedBbox);
      }
      break;

    case 116: // door, opened by blue key (slides into ground)
      interaction::configureLockedDoor(entity, mSpawnIndex, boundingBox);
      break;

    case 102: // dynamic geometry
      {
        const auto height = boundingBox.size.height;
        entity.assign<ActivationSettings>(
          ActivationSettings::Policy::AlwaysAfterFirstActivation);
        entity.assign<BoundingBox>(BoundingBox{{-1, -(height - 1)}, {1, 1}});
        entity.assign<BehaviorController>(behaviors::DynamicGeometryController{
          DGType::FallDownAfterDelayThenSinkIntoGround});
      }
      break;

    case 137: // dynamic geometry
      {
        const auto height = boundingBox.size.height;
        entity.assign<BoundingBox>(BoundingBox{{-1, -(height - 1)}, {1, 1}});
        entity.assign<BehaviorController>(behaviors::DynamicGeometryController{
          DGType::FallDownWhileEarthQuakeActiveThenExplode});
      }
      break;

    case 138: // dynamic geometry
      {
        const auto height = boundingBox.size.height;
        entity.assign<ActivationSettings>(ActivationSettings::Policy::Always);
        entity.assign<BoundingBox>(BoundingBox{{-1, -(height - 1)}, {1, 1}});
        entity.assign<BehaviorController>(behaviors::DynamicGeometryController{
          DGType::FallDownImmediatelyThenStayOnGround});
      }
      break;

    case 141: // dynamic geometry
      {
        const auto height = boundingBox.size.height;
        entity.assign<ActivationSettings>(
          ActivationSettings::Policy::AlwaysAfterFirstActivation);
        entity.assign<BoundingBox>(BoundingBox{{-1, -(height - 1)}, {1, 1}});
        entity.assign<BehaviorController>(behaviors::DynamicGeometryController{
          DGType::FallDownWhileEarthQuakeActiveThenStayOnGround});
      }
      break;

    case 142: // dynamic geometry
      {
        const auto height = boundingBox.size.height;
        entity.assign<ActivationSettings>(ActivationSettings::Policy::Always);
        entity.assign<BoundingBox>(BoundingBox{{-1, -(height - 1)}, {1, 1}});
        entity.assign<BehaviorController>(behaviors::DynamicGeometryController{
          DGType::FallDownImmediatelyThenExplode});
      }
      break;

    case 143: // dynamic geometry
      {
        const auto height = boundingBox.size.height;
        entity.assign<ActivationSettings>(
          ActivationSettings::Policy::AlwaysAfterFirstActivation);
        entity.assign<BoundingBox>(BoundingBox{{-1, -(height - 1)}, {1, 1}});
        entity.assign<BehaviorController>(behaviors::DynamicGeometryController{
          DGType::FallDownAfterDelayThenStayOnGround});
      }
      break;

    case 221: // water
      entity.assign<BoundingBox>(BoundingBox{{0, 1}, {2, 2}});
      entity.assign<ActorTag>(ActorTag::Type::WaterArea);
      break;

    case 226: // water drop
      addDefaultMovingBody(entity, boundingBox);
      entity.assign<AutoDestroy>(AutoDestroy{
        AutoDestroy::Condition::OnWorldCollision});
      engine::reassign<ActivationSettings>(
        entity, ActivationSettings::Policy::Always);
      break;

    case 227: // water drop spawner
      entity.assign<BehaviorController>(WaterDropGenerator{});
      entity.assign<ActivationSettings>(ActivationSettings::Policy::Always);
      entity.assign<BoundingBox>(boundingBox);
      break;

    case 233: // water with animated surface
      entity.assign<BoundingBox>(BoundingBox{{0, 1}, {2, 2}});
      entity.assign<ActorTag>(ActorTag::Type::AnimatedWaterArea);
      break;

    case 234: // water with animated surface (double sized block)
      entity.assign<BoundingBox>(BoundingBox{{0, 3}, {4, 4}});
      entity.assign<ActorTag>(ActorTag::Type::AnimatedWaterArea);
      break;

    case 241: // windblown-spider generator
      entity.assign<BehaviorController>(WindBlownSpiderGenerator{});
      entity.assign<ActivationSettings>(ActivationSettings::Policy::Always);
      entity.assign<BoundingBox>(boundingBox);
      break;

    case 250: // airlock effect, left
    case 251: // airlock effect, right
      break;

    case 254: // explosion effect trigger
      entity.assign<BehaviorController>(ExplosionEffect{});
      entity.assign<BoundingBox>(BoundingBox{{}, {1, 1}});
      entity.assign<DestructionEffects>(EXPLOSION_EFFECT_EFFECT_SPEC);
      break;

    // Various projectiles. Damage, velocity etc. are assigned by the
    // projectile configurarion functions
    case 7: case 8: case 9: case 10:
    case 24: case 25: case 26: case 27:
    case 21: case 204: case 205: case 206:
    case 136:
    case 85: case 86:
      entity.assign<BoundingBox>(boundingBox);
      break;

    default:
      break;
  }

  ++mSpawnIndex;
}
