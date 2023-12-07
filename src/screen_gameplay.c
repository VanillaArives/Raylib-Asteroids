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
static const Vector3 UNIT_VEC = (Vector3){1, 0, 0};

typedef struct playerPos_t {
  Vector2 pos;
  float dir;
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
  camera.position = (Vector3){5, 20, 0};
  camera.up = (Vector3){0, 1, 0};
  camera.projection = CAMERA_PERSPECTIVE;
  playerPos.pos = Vector2Zero();
  playerPos.dir = 0.0f;
  bullets = MemAlloc(sizeof(bulletEntity_t) + 1);
  numBullets = 0;
}

void UpdateBullets(void) {
  for (int i = 0; i < numBullets; i++) {
    Vector2 newPosVec = Vector2Rotate(
        Vector2Scale((Vector2){10, 0}, GetFrameTime()), bullets[i].dir);
    bullets[i].pos = Vector2Add(bullets[i].pos, newPosVec);
  }
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void) {
  // TODO: Update GAMEPLAY screen variables here!

  UpdateBullets();
  // Press enter or tap to change to ENDING screen
  if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
    finishScreen = 1;
    PlaySound(fxCoin);
  }
  if (IsKeyDown(KEY_UP)) {
    playerPos.pos.x -= 10.0f * GetFrameTime();
  }
  if (IsKeyDown(KEY_DOWN)) {
    playerPos.pos.x += 10.0f * GetFrameTime();
  }
  if (IsKeyDown(KEY_LEFT)) {
    playerPos.pos.y += 10.0f * GetFrameTime();
  }
  if (IsKeyDown(KEY_RIGHT)) {
    playerPos.pos.y -= 10.0f * GetFrameTime();
  }
  if (IsKeyDown(KEY_Z)) {
    playerPos.dir += 10.0f * GetFrameTime();
  }
  if (IsKeyDown(KEY_X)) {
    playerPos.dir -= 10.0f * GetFrameTime();
  }
  if (IsKeyDown(KEY_SPACE)) {
    bulletEntity_t bullet;
    bullet.pos = playerPos.pos;
    bullet.dir = playerPos.dir;
    bullets[numBullets] = bullet;
    numBullets++;
    bullets = MemRealloc(bullets, sizeof(bulletEntity_t) * (numBullets + 1));
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

  Vector3 lookingVec =
      Vector3Add(playerPosition,
                 Vector3RotateByAxisAngle(UNIT_VEC, UP_VEC, playerPos.dir));
  DrawLine3D(playerPosition, lookingVec, RED);

  for (int i = 0; i < numBullets; i++) {
    Vector3 bulletPos = (Vector3){bullets[i].pos.x, 0, bullets[i].pos.y};
    DrawCube(bulletPos, 1, 1, 5, RED);
  }
  EndMode3D();
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void) {
  // TODO: Unload GAMEPLAY screen variables here!
  MemFree(bullets);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void) { return finishScreen; }
