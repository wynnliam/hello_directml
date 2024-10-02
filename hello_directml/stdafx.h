// Liam Wynn, 10/1/2024, Hello DirectML

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <wrl.h>

#include <dxgi1_6.h>
#define DML_TARGET_VERSION_USE_LATEST
#include <DirectML.h>

using namespace Microsoft::WRL;
