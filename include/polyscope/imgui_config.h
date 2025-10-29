// Copyright 2017-2019, Nicholas Sharp and the Polyscope contributors. http://polyscope.run.
#pragma once

#include <tuple>
#include <string>
#include <unordered_map>

#include "imgui.h"


namespace polyscope {

// The theme colors available
enum class ThemeColor { Green, Red, Blue, Purple, Brown };

std::string themeColorToString(ThemeColor color);
ThemeColor stringToThemeColor(const std::string& colorStr);

// Default implementations of callbacks to set ImGui style / fonts
void configureImGuiStyle();
std::tuple<ImFontAtlas*, ImFont*, ImFont*> prepareImGuiFonts();

} // namespace polyscope
