/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 2026 Nathan Gill

This file is part of the Doom 3 BFG Edition GPL Source Code ("Doom 3 BFG Edition Source Code").

Doom 3 BFG Edition Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 BFG Edition Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 BFG Edition Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 BFG Edition Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 BFG Edition Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include "precompiled.h"
#pragma hdrstop

#include "DiscordRPC.h"

#define DISCORD_DOOM_CLASSIC "Doom (1993)"
#define DISCORD_DOOM2_CLASSIC "Doom II"
#define DISCORD_DOOM3 "Doom 3"

#define DISCORD_DOOM_CLASSIC_KEY "doom-logo"
#define DISCORD_DOOM2_CLASSIC_KEY "doom-2-logo"
#define DISCORD_DOOM3_KEY "doom-3-logo"

static const char* APPLICATION_ID = "1559205752990859314";

exDiscordRpcManager _rpcManager;
exDiscordRpcManager* discordRpcManager = &_rpcManager;

// always start with doom 3
CurrentGame exDiscordRpcManager::game = CurrentGame::Doom3;
idStr exDiscordRpcManager::mapName = "";
bool exDiscordRpcManager::dirty = true;
idSysMutex exDiscordRpcManager::lock;

static const char* getGameName(CurrentGame game)
{
    switch (game)
    {
        case CurrentGame::DoomClassic:
            return DISCORD_DOOM_CLASSIC;
        case CurrentGame::Doom2Classic:
            return DISCORD_DOOM2_CLASSIC;
        case CurrentGame::Doom3:
            return DISCORD_DOOM3;
    }

    return NULL;
}

static const char* getGameImageKey(CurrentGame game)
{
    switch (game)
    {
        case CurrentGame::DoomClassic:
            return DISCORD_DOOM_CLASSIC_KEY;
        case CurrentGame::Doom2Classic:
            return DISCORD_DOOM2_CLASSIC_KEY;
        case CurrentGame::Doom3:
            return DISCORD_DOOM3_KEY;
    }

    return NULL;
}

void exDiscordRpcManager::Init()
{
    start_time = time(NULL);

    DiscordEventHandlers eventHandlers =
    {
        .ready = exDiscordRpcManager::HandleReady,
        .disconnected = exDiscordRpcManager::HandleDisconnect,
        .errored = exDiscordRpcManager::HandleError,
        .joinGame = NULL,
        .spectateGame = NULL,
        .joinRequest = NULL,
    };

    Discord_Initialize( APPLICATION_ID, &eventHandlers, 1, NULL );
    common->Printf( "discord: initialized new rpc client: app_id=%s\n", APPLICATION_ID );
}

void exDiscordRpcManager::Shutdown()
{
    Discord_Shutdown();
    common->Printf( "discord: shutdown\n" );
}

void exDiscordRpcManager::SetGame(CurrentGame game)
{
    lock.Lock();
    exDiscordRpcManager::game = game;
    common->Printf( "discord: set game: %s\n", getGameName(game) );
    dirty = true;
    lock.Unlock();
}

void exDiscordRpcManager::SetMapName(idStr& mapName)
{
    lock.Lock();
    exDiscordRpcManager::mapName = mapName;
    common->Printf( "discord: set map name: %s\n", mapName.c_str() );
    dirty = true;
    lock.Unlock();
}

void exDiscordRpcManager::ClearMapName()
{
    idStr x = "";
    SetMapName(x);
}

void exDiscordRpcManager::OnFrame()
{
    lock.Lock();
    if (dirty)
    {
        FlushPending();
    }
    lock.Unlock();

    Discord_RunCallbacks();
}

void exDiscordRpcManager::FlushPending()
{
    const char *details = NULL;
    idStr _details;
    if (mapName.Length() > 0)
    {
        _details.Format( "Playing %s: %s", getGameName(game), mapName.c_str() );
        details = _details.c_str();
    }
    else
    {
        _details.Format( "Playing %s", getGameName(game) );
        details = _details.c_str();
    }

    const char *imageKey = getGameImageKey( game );

    DiscordRichPresence rp = {0};
    rp.details = details;
    rp.startTimestamp = start_time;
    rp.largeImageKey = imageKey;
    Discord_UpdatePresence( &rp );

    common->Printf("discord: sent presence update: %s\n", details);

    dirty = false;
}

void exDiscordRpcManager::HandleReady(const DiscordUser* user)
{
    common->Printf( "discord: connected: %s#%s, %s\n", user->username, user->discriminator, user->userId );
}

void exDiscordRpcManager::HandleDisconnect(int errcode, const char* msg)
{
    common->Printf( "discord: disconnect: %d: %s\n", errcode, msg );
}

void exDiscordRpcManager::HandleError(int errcode, const char* msg)
{
    common->Printf( "discord: error: %d: %s\n", errcode, msg );
}
