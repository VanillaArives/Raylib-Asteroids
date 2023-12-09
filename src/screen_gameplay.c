/**********************************************************************************************
 *
 *   raylib - Advance Game template
 *
 *   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
 *
 *   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
 *
 *   This software is provided "as-is", without any express or implied warranty.
 *In no event will the authors be held liable for any damages arising from the
 *use of this software.
 *
 *   Permission is granted to anyone to use this software for any purpose,
 *including commercial applications, and to alter it and redistribute it freely,
 *subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not
 *claim that you wrote the original software. If you use this software in a
 *product, an acknowledgment in the product documentation would be appreciated
 *but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not
 *be misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source
 *distribution.
 *
 **********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "screens.h"
#define BULLET_SPEED 5
#define radToDegree(rad) (rad * 360 / (2 * PI))

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------

static const Vector3 UP_VEC = (Vector3){0, 1, 0};
static const Vector3 UNIT3_VEC = (Vector3){1, 0, 0};

typedef struct playerEntity_t {
  Model model;
  Vector2 pos;
  float dir;
  float cooldown;
} playerPos_t;

typedef struct bulletEntity_t {
  Model model;
  Vector2 pos;
  float dir;
} bulletEntity_t;

typedef struct rockEntity_t {
  Model model;
  Vector2 pos;
  float dir;
  float speed;
  float lifeTime;
} rockEntity_t;

static int framesCounter = 0;
static int finishScreen = 0;
static Camera3D camera = {0};
static playerPos_t playerEntity;
static bulletEntity_t *bullets;
static int numBullets;
static rockEntity_t *rocks;
static int numRocks;
static float fireRate = 0.4;
static Vector2 mousePos;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void) {
  framesCounter = 0;
  finishScreen = 0;
  camera.fovy = 60;
  camera.target = (Vector3){0, 0, 0};
  camera.position = (Vector3){0, 20, 1};
  camera.up = (Vector3){0, 1, 0};
  camera.projection = CAMERA_PERSPECTIVE;

  Mesh playerMesh = GenMeshCube(1, 1, 1);
  playerEntity.model = LoadModelFromMesh(playerMesh);
  /* UnloadMesh(playerMesh); */
  playerEntity.pos = Vector2Zero();
  playerEntity.dir = 0.0f;
  playerEntity.cooldown = fireRate;
  bullets = MemAlloc(sizeof(bulletEntity_t));
  rocks = MemAlloc(sizeof(rockEntity_t));
  numBullets = 0;

  Mesh rockMesh = GenMeshCube(4, 4, 4);
  rocks[0] = (rockEntity_t){
      .model = LoadModelFromMesh(rockMesh),
      .pos = Vector2Zero(),
      .dir = 0,
      .speed = 5.0,
      .lifeTime = 5.0,
  };
  numRocks = 1;
}

void UpdateBullets(void) {
  int *invalidBullets = MemAlloc(sizeof(int));
  int numInvalid = 0;

  for (int i = 0; i < numBullets; i++) {
    Vector2 newPosVec = Vector2Rotate(
        Vector2Scale((Vector2){20, 0}, GetFrameTime()), -bullets[i].dir);
    bullets[i].pos = Vector2Add(bullets[i].pos, newPosVec);
    int bulletX = bullets[i].pos.x + 20;
    int bulletY = bullets[i].pos.y + 11;
    if ((bulletX > 40) || (bulletX < 0) || (bulletY > 22) || (bulletY < 0)) {
      UnloadModel(bullets[i].model);
      invalidBullets[numInvalid] = i;
      numInvalid++;
      invalidBullets =
          MemRealloc(invalidBullets, sizeof(int) * (numInvalid + 1));
    }
  }

  // Remove bullets from heap
  int offset = 0;
  for (int i = 0; i < numInvalid; i++) {
    int removeIndex = invalidBullets[i] + offset;
    for (int j = removeIndex; j < numBullets - 1; j++) {
      bullets[j] = bullets[j + 1];
    }
    numBullets--;
    MemRealloc(invalidBullets, sizeof(int) * (numInvalid + 1));
    offset--;
  }
  MemFree(invalidBullets);
}

void UpdateRocks() {
  for (int i = 0; i < numRocks; i++) {
    Vector2 dRockPos = Vector2Rotate(
        (Vector2){rocks[i].speed * GetFrameTime(), 0}, rocks[i].dir);
    rocks[i].pos = Vector2Add(rocks[i].pos, dRockPos);

    rocks[i].lifeTime -= GetFrameTime();

    if (rocks[i].lifeTime < 0) {
      // bruh;
    }
  }
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void) {
  // TODO: Update GAMEPLAY screen variables here!
  SetMouseScale(40.0 / GetScreenWidth(), 22.0 / GetScreenHeight());
  SetMouseOffset(-GetScreenWidth() / 2, -GetScreenHeight() / 2);
  mousePos = GetMousePosition();

  UpdateBullets();
  UpdateRocks();
  // Press enter or tap to change to ENDING screen
  if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
    finishScreen = 1;
    PlaySound(fxCoin);
  }
  if (IsKeyDown(KEY_W)) {
    playerEntity.pos.y -= 10.0f * GetFrameTime();
  }
  if (IsKeyDown(KEY_S)) {
    playerEntity.pos.y += 10.0f * GetFrameTime();
  }
  if (IsKeyDown(KEY_A)) {
    playerEntity.pos.x -= 10.0f * GetFrameTime();
  }
  if (IsKeyDown(KEY_D)) {
    playerEntity.pos.x += 10.0f * GetFrameTime();
  }
  playerEntity.dir = Vector2Angle(Vector2Subtract(mousePos, playerEntity.pos),
                                  ((Vector2){1, 0}));
  if ((playerEntity.cooldown <= 0) && IsKeyDown(KEY_SPACE)) {
    bulletEntity_t bullet;
    Mesh bulletMesh = GenMeshCube(0.25, 0.25, 2.0);
    bullet.model = LoadModelFromMesh(bulletMesh);

    Vector2 spawnOffset = Vector2Rotate((Vector2){1, 0}, -playerEntity.dir);
    Vector2 spawnVec = Vector2Add(playerEntity.pos, spawnOffset);
    bullet.pos = spawnVec;
    bullet.dir = playerEntity.dir;
    bullets[numBullets] = bullet;
    numBullets++;
    bullets = MemRealloc(bullets, sizeof(bulletEntity_t) * (numBullets + 1));
    playerEntity.cooldown = fireRate;
  }

  if (playerEntity.cooldown > 0) {
    playerEntity.cooldown -= GetFrameTime();
  }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void) {
  /* DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), PURPLE); */
  ClearBackground(BLACK);
  // DrawTextEx(font, "GAMEPLAY SCREEN", pos, font.baseSize * 3.0f, 4,
  // MAROON); DrawText("PRESS ENTER or TAP to JUMP to ENDING SCREEN", 130,
  // 220, 20, MAROON);
  BeginMode3D(camera);
  Vector3 playerPosition = (Vector3){playerEntity.pos.x, 0, playerEntity.pos.y};
  /* DrawCube(playerPosition, 1, 1, 1, BLUE); */
  /* DrawCubeWires(playerPosition, 1, 1, 1, WHITE); */
  DrawModelEx(playerEntity.model, playerPosition, UP_VEC,
              radToDegree(playerEntity.dir), Vector3One(), BLUE);
  DrawModelWiresEx(playerEntity.model, playerPosition, UP_VEC,
                   radToDegree(playerEntity.dir), Vector3One(), WHITE);

  Vector3 mouse = (Vector3){mousePos.x, 0, mousePos.y};
  DrawCube(mouse, 1, 1, 1, PURPLE);
  DrawCubeWires(mouse, 1, 1, 1, WHITE);

  Vector3 lookingVec =
      Vector3Add(playerPosition,
                 Vector3RotateByAxisAngle(UNIT3_VEC, UP_VEC, playerEntity.dir));
  DrawLine3D(playerPosition, lookingVec, RED);

  for (int i = 0; i < numBullets; i++) {
    Vector3 bulletPos = (Vector3){bullets[i].pos.x, 0, bullets[i].pos.y};
    DrawModelEx(bullets[i].model, bulletPos, UP_VEC,
                radToDegree(bullets[i].dir) + 90, Vector3One(), RED);
  }

  Vector3 rockPos = (Vector3){rocks[0].pos.x, 0, rocks[0].pos.y};
  /* DrawCube(rockPos, 1, 1, 1, GRAY); */
  DrawModelEx(rocks[0].model, rockPos, UP_VEC, 0.0, Vector3One(), GRAY);
  EndMode3D();
  DrawText(TextFormat("Yaw: %f", playerEntity.dir), 5, 5, 30, WHITE);
  DrawText(TextFormat("Cooldown: %f", playerEntity.cooldown), 5, 35, 30, WHITE);
  DrawText(TextFormat("Mouse: %f %f", mousePos.x, mousePos.y), 5, 65, 30,
           WHITE);
  DrawText(TextFormat("Player: %f %f", playerEntity.pos.x, playerEntity.pos.y),
           5, 95, 30, WHITE);
  DrawText(TextFormat("Bullets: %d", numBullets), 5, 125, 30, WHITE);
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void) {
  MemFree(bullets);
  UnloadModel(playerEntity.model);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void) { return finishScreen; }
