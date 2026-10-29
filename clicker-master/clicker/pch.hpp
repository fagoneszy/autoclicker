#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <atomic>
#include <cstdint>
#include <cstring>

#include <Windows.h>

using namespace std::chrono_literals;

#include "console.hpp"
#include "utils.hpp"
#include "keybind.hpp"
#include "config.hpp"
#include "vars.hpp"
