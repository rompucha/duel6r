/*
* Copyright (c) 2006, Ondrej Danek (www.ondrej-danek.net)
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Ondrej Danek nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <algorithm>
#include "LevelRenderData.h"

namespace Duel6 {
    LevelRenderData::LevelRenderData(const Level &level, Renderer &renderer, ScreenMode screenMode,
                                     Float32 animationSpeed)
            : level(level), renderer(renderer), screenMode(screenMode), animationSpeed(animationSpeed), animWait(0) {}

    void LevelRenderData::generateFaces() {
        addWallFaces();
        addSpriteFaces();
        addPortalFaces();
        generateWater();
    }

    void LevelRenderData::generateWater() {
        addWaterFaces();
    }

    void LevelRenderData::update(Float32 elapsedTime) {
        animWait += elapsedTime;
        if (animWait > animationSpeed) {
            animWait = 0;
            walls.nextFrame();
            sprites.nextFrame();
            portals.nextFrame();
            water.nextFrame();
        }
    }

    void LevelRenderData::addWallFaces() {
        walls.clear();

        for (Int32 y = 0; y < level.getHeight(); y++) {
            for (Int32 x = 0; x < level.getWidth(); x++) {
                const Block &block = level.getBlockMeta(x, y);

                if (block.is(Block::Type::Wall)) {
                    addWall(block, x, y);
                }
            }
        }

        walls.build(renderer);
    }

    void LevelRenderData::addSpriteFaces() {
        sprites.clear();

        for (Int32 y = 0; y < level.getHeight(); y++) {
            for (Int32 x = 0; x < level.getWidth(); x++) {
                const Block &block = level.getBlockMeta(x, y);

                if (block.isBurning()) {
                    continue;
                }

                if (block.is(Block::Type::FrontAndBackSprite)) {
                    addSprite(sprites, block, x, y, 1.0f);
                    addSprite(sprites, block, x, y, 0.0f);
                } else if (block.is(Block::Type::FrontSprite)) {
                    addSprite(sprites, block, x, y, 1.0f);
                } else if (block.is(Block::Type::BackSprite)) {
                    addSprite(sprites, block, x, y, 0.0f);
                } else if (block.is(Block::Type::Front4Sprite)) {
                    addSprite(sprites, block, x, y, 0.75f);
                } else if (block.is(Block::Type::Back4Sprite)) {
                    addSprite(sprites, block, x, y, 0.25f);
                }
            }
        }

        sprites.build(renderer);
    }

    void LevelRenderData::addWaterFaces() {
        water.clear();

        for (Int32 y = 0; y < level.getHeight(); y++) {
            for (Int32 x = 0; x < level.getWidth(); x++) {
                const Block &block = level.getBlockMeta(x, y);

                if (block.is(Block::Type::Waterfall)) {
                    addSprite(water, block, x, y, 0.75);
                } else if (block.is(Block::Type::Water)) {
                    addWater(block, x, y);
                }
            }
        }

        water.build(renderer);
    }

    void LevelRenderData::addPortalFaces() {
        portals.clear();

        for (Int32 y = 0; y < level.getHeight(); y++) {
            for (Int32 x = 0; x < level.getWidth(); x++) {
                const Block &block = level.getBlockMeta(x, y);

                if (block.is(Block::Type::Portal)) {
                    addSprite(portals, block, x, y, 0.25f);
                }
            }
        }

        portals.build(renderer);
    }

    void LevelRenderData::addWall(const Block &block, Int32 x, Int32 y) {
        Int32 width = level.getWidth();
        Int32 height = level.getHeight();
        bool splitScreen = screenMode == ScreenMode::SplitScreen;
        bool left = splitScreen || x > width / 2;
        bool right = splitScreen || x < width / 2;
        bool top = splitScreen || y < height / 2;
        bool bottom = splitScreen || y > height / 2;

        walls.addFace(Face(block))
                .addVertex(Vertex(0, x, y + 1, 1))
                .addVertex(Vertex(1, x + 1, y + 1, 1))
                .addVertex(Vertex(2, x + 1, y, 1))
                .addVertex(Vertex(3, x, y, 1));


#ifdef D6_RENDER_BACKS
        faceList.addFace(Face(block))
            .addVertex(Vertex(0, x + 1, y + 1, 0))
            .addVertex(Vertex(1, x, y + 1, 0))
            .addVertex(Vertex(2, x, y, 0))
            .addVertex(Vertex(3, x + 1, y, 0));
#endif

        if (left && !level.isWall(x - 1, y, false)) {
            walls.addFace(Face(block))
                    .addVertex(Vertex(0, x, y + 1, 0))
                    .addVertex(Vertex(1, x, y + 1, 1))
                    .addVertex(Vertex(2, x, y, 1))
                    .addVertex(Vertex(3, x, y, 0));
        }
        if (right && !level.isWall(x + 1, y, false)) {
            walls.addFace(Face(block))
                    .addVertex(Vertex(0, x + 1, y + 1, 1))
                    .addVertex(Vertex(1, x + 1, y + 1, 0))
                    .addVertex(Vertex(2, x + 1, y, 0))
                    .addVertex(Vertex(3, x + 1, y, 1));
        }
        if (top && !level.isWall(x, y + 1, false)) {
            walls.addFace(Face(block))
                    .addVertex(Vertex(0, x, y + 1, 1))
                    .addVertex(Vertex(1, x, y + 1, 0))
                    .addVertex(Vertex(2, x + 1, y + 1, 0))
                    .addVertex(Vertex(3, x + 1, y + 1, 1));
        }
        if (bottom && !level.isWall(x, y - 1, false)) {
            walls.addFace(Face(block))
                    .addVertex(Vertex(0, x, y, 1))
                    .addVertex(Vertex(1, x + 1, y, 1))
                    .addVertex(Vertex(2, x + 1, y, 0))
                    .addVertex(Vertex(3, x, y, 0));
        }
    }

    void LevelRenderData::addWater(const Block &block, Int32 x, Int32 y) {
        bool topWater = !level.isWater(x, y + 1);
        Vertex::Flag flowFlag = topWater ? Vertex::Flag::Flow : Vertex::Flag::None;

        water.addFace(Face(block))
                .addVertex(Vertex(0, x, y + 1, 1, flowFlag))
                .addVertex(Vertex(1, x + 1, y + 1, 1, flowFlag))
                .addVertex(Vertex(2, x + 1, y, 1))
                .addVertex(Vertex(3, x, y, 1));

        water.addFace(Face(block))
                .addVertex(Vertex(0, x + 1, y + 1, 0, flowFlag))
                .addVertex(Vertex(1, x, y + 1, 0, flowFlag))
                .addVertex(Vertex(2, x, y, 0))
                .addVertex(Vertex(3, x + 1, y, 0));

        if (topWater) {
            water.addFace(Face(block))
                    .addVertex(Vertex(0, x, y + 1, 1, Vertex::Flag::Flow))
                    .addVertex(Vertex(1, x, y + 1, 0, Vertex::Flag::Flow))
                    .addVertex(Vertex(2, x + 1, y + 1, 0, Vertex::Flag::Flow))
                    .addVertex(Vertex(3, x + 1, y + 1, 1, Vertex::Flag::Flow));
        }
    }

    void LevelRenderData::addSprite(FaceList &faceList, const Block &block, Int32 x, Int32 y, Float32 z) {
        Float32 fx = Float32(x), fy = Float32(y);
        bool bottomWaterfall = (block.is(Block::Type::Waterfall) && level.isWater(x, y - 1));
        Vertex::Flag flowFlag = bottomWaterfall ? Vertex::Flag::Flow : Vertex::Flag::None;

        faceList.addFace(Face(block))
                .addVertex(Vertex(0, fx, fy + 1, z))
                .addVertex(Vertex(1, fx + 1, fy + 1, z))
                .addVertex(Vertex(2, fx + 1, fy, z, flowFlag))
                .addVertex(Vertex(3, fx, fy, z, flowFlag));
    }
}