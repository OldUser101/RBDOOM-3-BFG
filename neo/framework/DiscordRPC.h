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

#ifndef __DISCORDRPC_H__
#define __DISCORDRPC_H__

#include <ctime>

#include "idlib/Thread.h"
#include "discord_rpc.h"

enum CurrentGame
{
    DoomClassic,
    Doom2Classic,
    Doom3,
};

class
exDiscordRpcManager
{
public:
    exDiscordRpcManager() {}
    ~exDiscordRpcManager() {}

    void Init();
    void Shutdown();
    void OnFrame();

    void SetGame(CurrentGame game);
    void SetMapName(idStr& mapName);
    void ClearMapName();

private:
    static enum CurrentGame game;
    static idStr mapName;
    static bool dirty;
    static idSysMutex lock;
    
    static void HandleReady(const DiscordUser* user);
    static void HandleDisconnect(int errcode, const char* message);
    static void HandleError(int errcode, const char* message);

    time_t start_time;
    void FlushPending();
};

extern exDiscordRpcManager* discordRpcManager;

#endif // __DISCORDRPC_H__
