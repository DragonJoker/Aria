/*
 * Copyright (c) 2020-2022, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Visualizing and Communicating Errors in Rendered Images
// Ray Tracing Gems II, 2021,
// by Pontus Andersson, Jim Nilsson, and Tomas Akenine-Moller.
// Pointer to the chapter: https://research.nvidia.com/publication/2021-08_Visualizing-and-Communicating.

// Visualizing Errors in Rendered High Dynamic Range Images
// Eurographics 2021,
// by Pontus Andersson, Jim Nilsson, Peter Shirley, and Tomas Akenine-Moller.
// Pointer to the paper: https://research.nvidia.com/publication/2021-05_HDR-FLIP.

// FLIP: A Difference Evaluator for Alternating Images
// High Performance Graphics 2020,
// by Pontus Andersson, Jim Nilsson, Tomas Akenine-Moller,
// Magnus Oskarsson, Kalle Astrom, and Mark D. Fairchild.
// Pointer to the paper: https://research.nvidia.com/publication/2020-07_FLIP.

// Code by Pontus Andersson, Jim Nilsson, and Tomas Akenine-Moller.

#pragma once

#include <algorithm>
#include <cstdlib>

#include "sharedflip.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace FLIP
{

    static const float ToneMappingCoefficients[3][6] =
    {
        { 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },                                                 // Reinhard.
        { 0.6f * 0.6f * 2.51f, 0.6f * 0.03f, 0.0f, 0.6f * 0.6f * 2.43f, 0.6f * 0.59f, 0.14f },  // ACES, 0.6 is pre-exposure cancellation.
        { 0.231683f, 0.013791f, 0.0f, 0.18f, 0.3f, 0.018f },                                    // Hable.
    };

    union int3
    {
        struct { int x, y, z; };
    };

    template<typename T = color3>
    class tensor
    {
    protected:
        int3 mDim;
        int mArea, mVolume;
        T* mvpHostData;

    protected:

        bool allocateHost(void)
        {
            this->mvpHostData = (T*)malloc(this->mVolume * sizeof(T));

            if (this->mvpHostData == nullptr)
            {
                return false;
            }

            return true;
        }

        void init(const int3 dim, bool bClear = false, T clearColor = T(0.0f))
        {
            this->mDim = dim;
            this->mArea = dim.x * dim.y;
            this->mVolume = dim.x * dim.y * dim.z;

            allocateHost();

            if (bClear)
            {
                this->clear(clearColor);
            }
        }

    public:

        tensor()
        {
        }

        tensor(const int width, const int height, const int depth)
        {
            this->init({ width, height, depth });
        }

        tensor(const int width, const int height, const int depth, const T clearColor)
        {
            this->init({ width, height, depth }, true, clearColor);
        }

        tensor(const int3 dim, const T clearColor)
        {
            this->init(dim, true, clearColor);
        }

        tensor(tensor& image)
        {
            this->init(image.mDim);
            this->copy(image);
        }

        tensor(const color3* pColorMap, int size)
        {
            this->init({ size, 1, 1 });
            memcpy(this->mvpHostData, pColorMap, size * sizeof(color3));
        }

        ~tensor(void)
        {
            free(this->mvpHostData);
        }

        T* getHostData(void)
        {
            return this->mvpHostData;
        }

        inline int index(int x, int y = 0, int z = 0) const
        {
            return (z * this->mDim.y + y) * mDim.x + x;
        }

        T get(int x, int y, int z) const
        {
            return this->mvpHostData[this->index(x, y, z)];
        }

        void set(int x, int y, int z, T value)
        {
            this->mvpHostData[this->index(x, y, z)] = value;
        }

        int3 getDimensions(void) const
        {
            return this->mDim;
        }

        int getWidth(void) const
        {
            return this->mDim.x;
        }

        int getHeight(void) const
        {
            return this->mDim.y;
        }

        int getDepth(void) const
        {
            return this->mDim.z;
        }

        void colorMap(tensor<float>& srcImage, tensor<color3>& colorMap)
        {
            for (int z = 0; z < this->getDepth(); z++)
            {
#pragma omp parallel for
                for (int y = 0; y < this->getHeight(); y++)
                {
                    for (int x = 0; x < this->getWidth(); x++)
                    {
                        this->set(x, y, z, colorMap.get(int(srcImage.get(x, y, z) * 255.0f + 0.5f) % colorMap.getWidth(), 0, 0));
                    }
                }
            }
        }

        void sRGB2YCxCz(void)
        {
            for (int z = 0; z < this->getDepth(); z++)
            {
#pragma omp parallel for
                for (int y = 0; y < this->getHeight(); y++)
                {
                    for (int x = 0; x < this->getWidth(); x++)
                    {
                        this->set(x, y, z, color3::XYZ2YCxCz(color3::LinearRGB2XYZ(color3::sRGB2LinearRGB(this->get(x, y, z)))));
                    }
                }
            }
        }

        void LinearRGB2sRGB(void)
        {
            for (int z = 0; z < this->getDepth(); z++)
            {
#pragma omp parallel for
                for (int y = 0; y < this->getHeight(); y++)
                {
                    for (int x = 0; x < this->getWidth(); x++)
                    {
                        this->set(x, y, z, color3::LinearRGB2sRGB(this->get(x, y, z)));
                    }
                }
            }
        }

        void clear(const T color = T(0.0f))
        {
            for (int z = 0; z < this->getDepth(); z++)
            {
#pragma omp parallel for
                for (int y = 0; y < this->getHeight(); y++)
                {
                    for (int x = 0; x < this->getWidth(); x++)
                    {
                        this->set(x, y, z, color);
                    }
                }
            }
        }

        void clamp(float low = 0.0f, float high = 1.0f)
        {
            for (int z = 0; z < this->getDepth(); z++)
            {
#pragma omp parallel for
                for (int y = 0; y < this->getHeight(); y++)
                {
                    for (int x = 0; x < this->getWidth(); x++)
                    {
                        this->set(x, y, z, color3::clamp(this->get(x, y, z), low, high));
                    }
                }
            }
        }

        void copy(tensor<T>& srcImage)
        {
            if (this->mDim.x == srcImage.getWidth() && this->mDim.y == srcImage.getHeight() && this->mDim.z == srcImage.getDepth())
            {
                memcpy(this->mvpHostData, srcImage.getHostData(), this->mVolume * sizeof(T));
            }
        }

        void copyFloat2Color3(tensor<float>& srcImage)
        {
            for (int z = 0; z < this->getDepth(); z++)
            {
#pragma omp parallel for
                for (int y = 0; y < this->getHeight(); y++)
                {
                    for (int x = 0; x < this->getWidth(); x++)
                    {
                        this->set(x, y, z, color3(srcImage.get(x, y, z)));
                    }
                }
            }
        }

        bool load(const std::string& filename, const int z = 0)
        {
            int width, height, bpp;
            unsigned char* pixels = stbi_load(filename.c_str(), &width, &height, &bpp, 3);
            if (!pixels)
            {
                return false;
            }

            this->init({ width, height, z + 1 });

#pragma omp parallel for
            for (int y = 0; y < this->mDim.y; y++)
            {
                for (int x = 0; x < this->mDim.x; x++)
                {
                    this->set(x, y, z, color3(&pixels[3 * this->index(x, y)]));
                }
            }
            delete[] pixels;

            return true;
        }

    };
}
