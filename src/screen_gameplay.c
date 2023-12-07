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

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------

static const Vector3 UP_VEC = (Vector3){0, 1, 0};
static const Vector3 UNIT3_VEC = (Vector3){1, 0, 0};

typedef struct playerPos_t {
  Vector2 pos;
  float dir;
  float cooldown;
} playerPos_t;

typedef struct bulletEntity_t {
  Vector2 pos;
  float dir;
} bulletEntity_t;

static int framesCounter = 0;
static int finishScreen = 0;
static Camera3D camera = {0};
static playerPos_t playerPos;
static bulletEntity_t *bullets;
static int numBullets;
static float fireRate = 0.4;
static Vector2 mousePos;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void) {
  // TODO: Initialize GAMEPLAY screen variables here!
  framesCounter = 0;
  finishScreen = 0;
  camera.fovy = 60;
  camera.target = (Vector3){0, 0, 0};
  camera.position = (Vector3){0, 20, 1};
  camera.up = (Vector3){0, 1, 0};
  camera.projection = CAMERA_PERSPECTIVE;
  playerPos.pos = Vector2Zero();
  playerPos.dir = 0.0f;
  playerPos.cooldown = fireRate;
  bullets = MemAlloc(sizeof(bulletEntity_t) + 1);
  numBullets = 0;
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

// Gameplay Screen Update logic
void UpdateGameplayScreen(void) {
  // TODO: Update GAMEPLAY screen variables here!
  SetMouseScale(40.0 / GetScreenWidth(), 22.0 / GetScreenHeight());
  SetMouseOffset(-GetScreenWidth() / 2, -GetScreenHeight() / 2);
  mousePos = GetMousePosition();

  UpdateBullets();
  // Press enter or tap to change to ENDING screen
  if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
    finishScreen = 1;
    PlaySound(fxCoin);
  }
  if (IsKeyDown(KEY_UP)) {
    playerPos.pos.y -= 10.0f * GetFrameTime();
  }
  if (IsKeyDown(KEY_DOWN)) {
    playerPos.pos.y += 10.0f * GetFrameTime();
  }
  if (IsKeyDown(KEY_LEFT)) {
    playerPos.pos.x -= 10.0f * GetFrameTime();
  }
  if (IsKeyDown(KEY_RIGHT)) {
    playerPos.pos.x += 10.0f * GetFrameTime();
  }
  /* if (IsKeyDown(KEY_Z)) { */
  /*   playerPos.dir = Wrap(playerPos.dir - 10.0f * GetFrameTime(), 0, 2 * PI);
   */
  /* } */
  /* if (IsKeyDown(KEY_X)) { */
  /*   playerPos.dir = Wrap(playerPos.dir + 10.0f * GetFrameTime(), 0, 2 * PI);
   */
  /* } */
  /* playerPos.dir = Vector2Angle(mousePos, playerPos.pos) + PI; */
  playerPos.dir =
      Vector2Angle(Vector2Subtract(mousePos, playerPos.pos), ((Vector2){1, 0}));
  if ((playerPos.cooldown <= 0) && IsKeyDown(KEY_SPACE)) {
    bulletEntity_t bullet;
    bullet.pos = playerPos.pos;
    bullet.dir = playerPos.dir;
    bullets[numBullets] = bullet;
    numBullets++;
    bullets = MemRealloc(bullets, sizeof(bulletEntity_t) * (numBullets + 1));
    playerPos.cooldown = fireRate;
  }

  if (playerPos.cooldown > 0) {
    playerPos.cooldown -= GetFrameTime();
  }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void) {
  // TODO: Draw GAMEPLAY screen here!
  /* DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), PURPLE); */
  ClearBackground(BLACK);
  // DrawTextEx(font, "GAMEPLAY SCREEN", pos, font.baseSize * 3.0f, 4,
  // MAROON); DrawText("PRESS ENTER or TAP to JUMP to ENDING SCREEN", 130,
  // 220, 20, MAROON);
  BeginMode3D(camera);
  Vector3 playerPosition = (Vector3){playerPos.pos.x, 0, playerPos.pos.y};
  DrawCube(playerPosition, 1, 1, 1, BLUE);
  DrawCubeWires(playerPosition, 1, 1, 1, WHITE);

  Vector3 mouse = (Vector3){mousePos.x, 0, mousePos.y};
  DrawCube(mouse, 1, 1, 1, PURPLE);
  DrawCubeWires(mouse, 1, 1, 1, WHITE);

  Vector3 lookingVec =
      Vector3Add(playerPosition,
                 Vector3RotateByAxisAngle(UNIT3_VEC, UP_VEC, playerPos.dir));
  DrawLine3D(playerPosition, lookingVec, RED);

  for (int i = 0; i < numBullets; i++) {
    Vector3 bulletPos = (Vector3){bullets[i].pos.x, 0, bullets[i].pos.y};
    DrawCube(bulletPos, 1, 1, 1, RED);
  }
  EndMode3D();
  DrawText(TextFormat("Yaw: %f", playerPos.dir), 5, 5, 30, WHITE);
  DrawText(TextFormat("Cooldown: %f", playerPos.cooldown), 5, 35, 30, WHITE);
  DrawText(TextFormat("Mouse: %f %f", mousePos.x, mousePos.y), 5, 65, 30,
           WHITE);
  DrawText(TextFormat("Player: %f %f", playerPos.pos.x, playerPos.pos.y), 5, 95,
           30, WHITE);
  DrawText(TextFormat("Bullets: %d", numBullets), 5, 125, 30, WHITE);
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void) {
  // TODO: Unload GAMEPLAY screen variables here!
  MemFree(bullets);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void) { return finishScreen; }
