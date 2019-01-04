//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "Utility.h"

namespace Utility
{
    void UtilityBase::StringReplace(
        std::string*       pStr,
        const char         src,
        const char         dst)

    {
        size_t size = pStr->size();

        for (size_t i = 0; i < size; ++i)
        {
            if ((*pStr)[i] == src)
            {
                (*pStr)[i] = dst;
            }
        }
    }

    std::vector<std::string> UtilityBase::Split(
        const std::string & str,
        const char          delim)

    {
        std::vector<std::string> splitedStrings;

        std::stringstream m_stringstream(str);
        std::string item;

        while (getline(m_stringstream, item, delim))
        {
            if (item.length() > 0)
            {
                splitedStrings.push_back(item);
            }
        }

        return splitedStrings;
    }

    std::string UtilityBase::ToLowercase(
        const std::string& str)
    {
        size_t strLength = str.length();

        std::string result(str);
        for (size_t i = 0; i < strLength; ++i)
        {
            if (str[i] >= 'A' && str[i] <= 'Z')
            {
                result[i] = str[i] + LOWER_UPPER_CASE_ASCII_DIFF;
            }
        }

        return result;
    }

    std::string UtilityBase::ToUppercase(
        const std::string& str)
    {
        size_t strLength = str.length();

        std::string result(str);
        for (size_t i = 0; i < strLength; ++i)
        {
            if (str[i] >= 'a' && str[i] <= 'z')
            {
                result[i] = str[i] - LOWER_UPPER_CASE_ASCII_DIFF;
            }
        }

        return result;
    }

    /*glm::mat4 UtilityBase::ToGLMMat4(
        Math::Mat4 m)
    {
        glm::mat4 result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result[i][j] = m[i][j];
            }
        }

        return result;
    }*/
}