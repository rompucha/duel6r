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

#include <SDL2/SDL_opengl.h>
#include "GameResources.h"
#include "Defines.h"
#include "Fire.h"

namespace Duel6
{
	GameResources::GameResources(AppService& appService)
	{
		Sound& sound = appService.getSound();
		Console& console = appService.getConsole();
		TextureManager& textureManager = appService.getTextureManager();

		console.printLine("\n===Initializing game resources===");
		console.printLine("...Building water-list");
		waterSet = Water::createWaterSet(sound, appService.getTextureManager());
		console.printLine("...Loading game sounds");
		roundStartSound = sound.loadSample("sound/game/round-start.wav");
		gameOverSound = sound.loadSample("sound/game/game-over.wav");
		console.printLine(Format("...Loading block meta data: {0}") << D6_FILE_BLOCK_META);
		blockMeta = Block::loadMeta(D6_FILE_BLOCK_META);
		console.printLine(Format("...Loading block textures: {0}") << D6_TEXTURE_BLOCK_PATH);
		blockTextures = textureManager.load(D6_TEXTURE_BLOCK_PATH, GL_LINEAR, true);
		console.printLine(Format("...Loading background textures: {0}") << D6_TEXTURE_BCG_PATH);
		bcgTextures = textureManager.load(D6_TEXTURE_BCG_PATH, GL_LINEAR, true);
		console.printLine(Format("...Loading explosion textures: {0}") << D6_TEXTURE_EXPL_PATH);
		explosionTextures = textureManager.load(D6_TEXTURE_EXPL_PATH, GL_NEAREST, true);

		console.printLine(Format("...Loading fire textures: {0}") << D6_TEXTURE_FIRE_PATH);
		for (const FireType& fireType : FireType::values())
		{
			TextureManager::Texture texture = textureManager.load(Format("{0}{1,3|0}/") << D6_TEXTURE_FIRE_PATH << fireType.getId(), GL_LINEAR, true);
			fireTextures[fireType.getId()] = texture;

			glBindTexture(GL_TEXTURE_2D, texture.getGlTextures()[2]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}
}