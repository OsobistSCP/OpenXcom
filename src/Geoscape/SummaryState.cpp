/*
 * Copyright 2010-2016 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "../Engine/Game.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Options.h"
#include "../Engine/Unicode.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/TextList.h"
#include "../Interface/Window.h"
#include "../Mod/Mod.h"
#include "../Mod/RuleCountry.h"
#include "../Savegame/Country.h"
#include "../Savegame/SavedGame.h"
#include "SummaryState.h"
#include <sstream>
#include <string>

namespace OpenXcom
{

/**
 * Initializes all the elements in the Funding screen.
 * @param game Pointer to the core game.
 */
SummaryState::SummaryState()
{
	_screen = false;

	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_btnOk = new TextButton(50, 12, 135, 180);
	_txtTitle = new Text(320, 17, 0, 8);

	// Set palette
	setInterface("SWindow");

	add(_window, "window", "SWindow");
	add(_btnOk, "button", "fundingWindow");
	add(_txtTitle, "text", "SWindow");

	centerAllSurfaces();

	// Set up objects
	setWindowBackground(_window, "fundingWindow");

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&SummaryState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&SummaryState::btnOkClick, Options::keyOk);
	_btnOk->onKeyboardPress((ActionHandler)&SummaryState::btnOkClick, Options::keyCancel);
	_btnOk->onKeyboardPress((ActionHandler)&SummaryState::btnOkClick, Options::keyGeoFunding);

	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(tr("STR_SUMMARY_GEOSCAPE"));

	
}

/**
 *
 */
SummaryState::~SummaryState()
{
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SummaryState::btnOkClick(Action *)
{
	_game->popState();
}

}
