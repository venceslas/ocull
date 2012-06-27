/*
 *  Copyright 2010-2011 NVIDIA Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#pragma once

//------------------------------------------------------------------------

#define CR_MAXVIEWPORT_LOG2     11      // ViewportSize / PixelSize.
#define CR_SUBPIXEL_LOG2        4       // PixelSize / SubpixelSize.

#define CR_MAXBINS_LOG2         4       // ViewportSize / BinSize.
#define CR_BIN_LOG2             4       // BinSize / TileSize.
#define CR_TILE_LOG2            3       // TileSize / PixelSize.

#define CR_COVER8X8_LUT_SIZE    768     // 64-bit entries.
#define CR_FLIPBIT_FLIP_Y       2
#define CR_FLIPBIT_FLIP_X       3
#define CR_FLIPBIT_SWAP_XY      4
#define CR_FLIPBIT_COMPL        5

#define CR_BIN_STREAMS_LOG2     4
#define CR_BIN_SEG_LOG2         9       // 32-bit entries.
#define CR_TILE_SEG_LOG2        5       // 32-bit entries.

#define CR_MAXSUBTRIS_LOG2      24      // Triangle structs. Dictated by CoarseRaster.
#define CR_COARSE_QUEUE_LOG2    10      // Triangles.

#define CR_SETUP_WARPS          2
#define CR_SETUP_OPT_BLOCKS     8
#define CR_BIN_WARPS            16
#define CR_COARSE_WARPS         16      // Must be a power of two.
#define CR_FINE_MAX_WARPS       20      // Absolute maximum for 48KB of shared mem.
#define CR_FINE_OPT_WARPS       20      // Preferred value.

//------------------------------------------------------------------------

#define CR_MAXVIEWPORT_SIZE     (1 << CR_MAXVIEWPORT_LOG2)
#define CR_SUBPIXEL_SIZE        (1 << CR_SUBPIXEL_LOG2)
#define CR_SUBPIXEL_SQR         (1 << (CR_SUBPIXEL_LOG2 * 2))

#define CR_MAXBINS_SIZE         (1 << CR_MAXBINS_LOG2)
#define CR_MAXBINS_SQR          (1 << (CR_MAXBINS_LOG2 * 2))
#define CR_BIN_SIZE             (1 << CR_BIN_LOG2)
#define CR_BIN_SQR              (1 << (CR_BIN_LOG2 * 2))

#define CR_MAXTILES_LOG2        (CR_MAXBINS_LOG2 + CR_BIN_LOG2)
#define CR_MAXTILES_SIZE        (1 << CR_MAXTILES_LOG2)
#define CR_MAXTILES_SQR         (1 << (CR_MAXTILES_LOG2 * 2))
#define CR_TILE_SIZE            (1 << CR_TILE_LOG2)
#define CR_TILE_SQR             (1 << (CR_TILE_LOG2 * 2))

#define CR_BIN_STREAMS_SIZE     (1 << CR_BIN_STREAMS_LOG2)
#define CR_BIN_SEG_SIZE         (1 << CR_BIN_SEG_LOG2)
#define CR_TILE_SEG_SIZE        (1 << CR_TILE_SEG_LOG2)

#define CR_MAXSUBTRIS_SIZE      (1 << CR_MAXSUBTRIS_LOG2)
#define CR_COARSE_QUEUE_SIZE    (1 << CR_COARSE_QUEUE_LOG2)

//------------------------------------------------------------------------
// When evaluating interpolated Z/W/U/V at pixel centers, we introduce an
// error of (+-CR_LERP_ERROR) ULPs. If it wasn't for integer overflows,
// we could utilize the full U32 range for depth to get maximal
// precision. However, to avoid overflows, we must shrink the range
// slightly from both ends. With W/U/V, another difficulty is that quad
// rendering can cause them to be evaluated outside the triangle, so we
// need to shrink the range even more.

#define CR_LERP_ERROR(SAMPLES_LOG2) (2200u << (SAMPLES_LOG2))
#define CR_DEPTH_MIN                CR_LERP_ERROR(3)
#define CR_DEPTH_MAX                (FW_U32_MAX - CR_LERP_ERROR(3))
#define CR_BARY_MAX                 ((1 << (30 - CR_SUBPIXEL_LOG2)) - 1)

//------------------------------------------------------------------------
