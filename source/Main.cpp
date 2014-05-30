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

#include "Globals.h"
#include "InfoMessageQueue.h"
#include "Game.h"
#include "Menu.h"
#include "World.h"
#include "Setup.h"
#include "Sound.h"
#include "Video.h"
#include "Font.h"
#include "Main.h"

namespace Duel6
{
	app_s g_app;
	appInp_s g_inp;
	con_c d6Console(CON_F_EXPAND);
	Video d6Video;
	Game d6Game;
	Menu d6Menu;
	Font d6Font;
	TextureManager d6TextureManager;
	Float32 d6Cos[450];
	bool d6Wireframe = false;
	bool d6ShowFps = false;
	std::vector<PlayerSkinColors> d6PlayerColors(D6_MAX_PLAYERS);
	int d6AmmoRangeMin = 15, d6AmmoRangeMax = 15;
	bool d6ShowRanking = true;
	InfoMessageQueue d6MessageQueue(D6_INFO_DURATION);
	SpriteList d6SpriteList;
	World d6World(D6_FILE_ANM, D6_ANM_SPEED, D6_WAVE_HEIGHT);

	void Main::textInputEvent(Context& context, const char* text)
	{
		if (d6Console.isActive())
		{
			d6Console.textInputEvent(text);
		}
		else
		{
			context.textInputEvent(text);
		}
	}

	void Main::keyEvent(Context& context, SDL_Keycode keyCode, Uint16 keyModifiers)
	{
		if (keyCode == SDLK_BACKQUOTE)
		{
			d6Console.toggle();
			if (d6Console.isActive())
			{
				SDL_StartTextInput();
			}
			else if (context.is(d6Game))
			{
				SDL_StopTextInput();
			}
		}

		if (d6Console.isActive())
		{
			d6Console.keyEvent(keyCode, keyModifiers);
		}
		else
		{
			context.keyEvent(keyCode, keyModifiers);
		}
	}

	void Main::processEvents(Context& context)
	{
		SDL_Event event;
		SDL_Keysym key;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				key = event.key.keysym;
				g_inp.pressedKeys.insert(key.sym);
				keyEvent(context, key.sym, key.mod);
				break;
			case SDL_KEYUP:
				key = event.key.keysym;
				g_inp.pressedKeys.erase(key.sym);
				break;
			case SDL_TEXTINPUT:
				textInputEvent(context, event.text.text);
				break;
			case SDL_QUIT:
				g_app.flags |= APP_FLAG_QUIT;
				break;
			}
		}
	}

	void Main::syncUpdateAndRender(Context& context)
	{
		static Uint32 curTime = 0;
		Uint32 lastTime = 0;

		lastTime = curTime;
		curTime = SDL_GetTicks();

		// Draw
		context.render();
		d6Video.swapBuffers(d6Console);

		// Update
		if (curTime - lastTime < 70)
		{
			float elapsedTime = (curTime - lastTime) * 0.001f;;
			context.update(elapsedTime);
		}
	}

	void Main::run()
	{
		while (!(g_app.flags & APP_FLAG_QUIT))
		{
			Context& context = Context::getCurrent();
			processEvents(context);
			syncUpdateAndRender(context);
		}
	}
}

/*
==================================================
Ulozeni obsahu konzoly na disk a uklid po chybe
==================================================
*/
void MAIN_ErrorHandler(const char *str)
{
    Duel6::d6Console.printf(str);
    Duel6::d6Console.exec("dump chyba.con");

    MY_DeInit ();
    SDL_Quit ();
}

/*
==================================================
Vstupni bod aplikace
==================================================
*/
int main(int argc, char *argv[])
{
	//Duel6::Main app;
	//return app.run();

    Duel6::g_app.flags = APP_FLAG_NONE;

    MY_Init ();
    MY_ErrCallback(MAIN_ErrorHandler);

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		MY_Err(MY_ErrDump("%S: %s\n", MY_L("COSTR0006|Chyba pri inicializaci grafickeho modu"), SDL_GetError()));
	}

    Duel6::d6Font.load(APP_FILE_FONT);
    Duel6::d6Console.setfont(Duel6::d6Font.get());
    CON_RegisterBasicCmd(Duel6::d6Console);
    Duel6::SET_Init ();

	for (int i = 1; i < argc; i++)
	{
		Duel6::d6Console.exec(argv[i]);
	}

	Duel6::Main app;
	app.run();

    Duel6::Sound::deInit();
	Duel6::d6TextureManager.freeAll();

    MY_DeInit ();
    SDL_Quit ();

    return 0;
}