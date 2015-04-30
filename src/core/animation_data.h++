/**************************************************************************\
 * camera.hpp - Defining a movable and scalable camera w viewport         *
 *                                               ___                      *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___     *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|    *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \    *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/    *
 *                |___/                              |_|                  *
 *                                                                        *
 * Copyright (c) 2015 Florian Oetke & Sebastian Schalow                   *
 *                                                                        *
 *  This file is part of MagnumOpus and distributed under the MIT License *
 *  See LICENSE file for details.                                         *
\**************************************************************************/

#pragma once

#include <unordered_map>

#include <core/renderer/texture.hpp>

namespace mo {
namespace renderer {

enum class Animation_type{
    idle,
    moving,
    attack,
};

struct Animation_frame_data{
    int row;
    float fps;
    int frames;
};

struct Animation_data{
    int frame_width, frame_height;
    Texture_ptr texture;
    std::unordered_map<Animation_type, Animation_frame_data> animations;
};

}
}

