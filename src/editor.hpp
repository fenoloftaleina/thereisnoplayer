#ifndef EDITOR
#define EDITOR
#pragma once

#include <stdio.h>
#include <bgfx/platform.h>
#include <bx/math.h>
#include <vector>

#include "objs.hpp"


struct Editor
{
  Objs* objs;

  void removeOnSpot(const bx::Vec3& spot);

  void addMovingCube(const bx::Vec3& spot);
  void addKidsCube(const bx::Vec3& spot);
  void addStaticCube(const bx::Vec3& spot);
  void addOrUpdateDoor(const bx::Vec3& spot);
  void addWinningDoor(const bx::Vec3& spot);

  int idxForSpot(const std::vector<Cube>& v, const bx::Vec3& spot);
  int idxForSpot(const std::vector<Door>& v, const bx::Vec3& spot);
  bool removeOnIdx(std::vector<Cube>& v, int i);
  bool removeOnIdx(std::vector<Door>& v, int i);
};

#endif
