//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>

#include "TypeDef.h"
#include "../Math/Math.h"

#define LOWER_UPPER_CASE_ASCII_DIFF 32

namespace Utility
{
    struct CallbackInfo
    {
        int keyboardCallBack[1024];

        struct MousePosCallback
        {
            MousePosCallback()
                : delta_x(0),
                delta_y(0)
            {
            }

            inline void reset()
            {
                delta_x = 0.0;
                delta_y = 0.0;
            }

            double delta_x;
            double delta_y;

        }cursorPosCallBack;

        CallbackInfo()
            :cursorPosCallBack()
        {
            memset(keyboardCallBack, 0, 1024 * sizeof(int));
        }
    };

    class UtilityBase
    {
    public:
        static void StringReplace(
            std::string*       pStr,
            const char         src,
            const char         dst = ' ');

        static std::vector<std::string> Split(
            const std::string& str,
            const char         delim);

        static std::string ToLowercase(
            const std::string& str);

        static std::string ToUppercase(
            const std::string& str);

        static std::unique_ptr<image_data> ReadImageData(
            const std::string& imageFile,
            int* const         width,
            int* const         height,
            int* const         type);

        static void ReleaseImageData(
            std::unique_ptr<image_data> imageData);

        // Test
        glm::mat4 ToGLMMat4(
            Math::Mat4 m);
    };
}