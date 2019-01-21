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

#include "Options.h"
#include "../version.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include <map>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <yaml-cpp/yaml.h>
#include "Exception.h"
#include "Logger.h"
#include "CrossPlatform.h"
#include "FileMap.h"
#include "Screen.h"

namespace OpenXcom
{

namespace Options
{
#define OPT
#include "Options.inc.h"
#undef OPT

std::string _dataFolder;
std::vector<std::string> _dataList;
std::string _userFolder;
std::string _configFolder;
std::vector<std::string> _userList;
std::map<std::string, std::string> _commandLine;
std::vector<OptionInfo> _info;
std::unordered_map<std::string, ModInfo> _modInfos;
std::string _masterMod;
bool _loadLastSave = false;
bool _loadLastSaveExpended = false;

/**
 * Sets up the options by creating their OptionInfo metadata.
 */
void create()
{
	if (!_info.empty()) { _info.clear(); }
#ifdef DINGOO
	_info.push_back(OptionInfo("displayWidth", &displayWidth, Screen::ORIGINAL_WIDTH));
	_info.push_back(OptionInfo("displayHeight", &displayHeight, Screen::ORIGINAL_HEIGHT));
	_info.push_back(OptionInfo("fullscreen", &fullscreen, true));
	_info.push_back(OptionInfo("asyncBlit", &asyncBlit, false));
	_info.push_back(OptionInfo("keyboardMode", (int*)&keyboardMode, KEYBOARD_OFF));
#else
	_info.push_back(OptionInfo("displayWidth", &displayWidth, Screen::ORIGINAL_WIDTH*2));
	_info.push_back(OptionInfo("displayHeight", &displayHeight, Screen::ORIGINAL_HEIGHT*2));
	_info.push_back(OptionInfo("fullscreen", &fullscreen, false));
	_info.push_back(OptionInfo("asyncBlit", &asyncBlit, true));
	_info.push_back(OptionInfo("keyboardMode", (int*)&keyboardMode, KEYBOARD_ON));
#endif

#ifdef __MOBILE__
	_info.push_back(OptionInfo("maxFrameSkip", &maxFrameSkip, 0, "STR_FRAMESKIP", "STR_GENERAL"));
#else
	_info.push_back(OptionInfo("maxFrameSkip", &maxFrameSkip, 0));
#endif
	_info.push_back(OptionInfo("traceAI", &traceAI, false));
	_info.push_back(OptionInfo("verboseLogging", &verboseLogging, false));
	_info.push_back(OptionInfo("listVFSContents", &listVFSContents, false));
	_info.push_back(OptionInfo("StereoSound", &StereoSound, true));
	//_info.push_back(OptionInfo("baseXResolution", &baseXResolution, Screen::ORIGINAL_WIDTH));
	//_info.push_back(OptionInfo("baseYResolution", &baseYResolution, Screen::ORIGINAL_HEIGHT));
	//_info.push_back(OptionInfo("baseXGeoscape", &baseXGeoscape, Screen::ORIGINAL_WIDTH));
	//_info.push_back(OptionInfo("baseYGeoscape", &baseYGeoscape, Screen::ORIGINAL_HEIGHT));
	//_info.push_back(OptionInfo("baseXBattlescape", &baseXBattlescape, Screen::ORIGINAL_WIDTH));
	//_info.push_back(OptionInfo("baseYBattlescape", &baseYBattlescape, Screen::ORIGINAL_HEIGHT));
	_info.push_back(OptionInfo("geoscapeScale", &geoscapeScale, 0));
	_info.push_back(OptionInfo("battlescapeScale", &battlescapeScale, 0));
	_info.push_back(OptionInfo("useScaleFilter", &useScaleFilter, false));
	_info.push_back(OptionInfo("useHQXFilter", &useHQXFilter, false));
	_info.push_back(OptionInfo("useXBRZFilter", &useXBRZFilter, false));
	_info.push_back(OptionInfo("useOpenGL", &useOpenGL, false));
	_info.push_back(OptionInfo("checkOpenGLErrors", &checkOpenGLErrors, false));
	_info.push_back(OptionInfo("useOpenGLShader", &useOpenGLShader, "Shaders/Raw.OpenGL.shader"));
	_info.push_back(OptionInfo("vSyncForOpenGL", &vSyncForOpenGL, true));
	_info.push_back(OptionInfo("useOpenGLSmoothing", &useOpenGLSmoothing, true));
	_info.push_back(OptionInfo("debug", &debug, false));
	_info.push_back(OptionInfo("debugUi", &debugUi, false));
	_info.push_back(OptionInfo("soundVolume", &soundVolume, 2*(MIX_MAX_VOLUME/3)));
	_info.push_back(OptionInfo("musicVolume", &musicVolume, 2*(MIX_MAX_VOLUME/3)));
	_info.push_back(OptionInfo("uiVolume", &uiVolume, MIX_MAX_VOLUME/3));
	_info.push_back(OptionInfo("language", &language, ""));
	_info.push_back(OptionInfo("battleScrollSpeed", &battleScrollSpeed, 8));
#ifdef __MOBILE__
	_info.push_back(OptionInfo("battleEdgeScroll", (int*)&battleEdgeScroll, SCROLL_NONE));
#else
	_info.push_back(OptionInfo("battleEdgeScroll", (int*)&battleEdgeScroll, SCROLL_AUTO));
#endif

#ifdef __MOBILE__
	_info.push_back(OptionInfo("battleDragScrollButton", &battleDragScrollButton, SDL_BUTTON_LEFT));
#else
	_info.push_back(OptionInfo("battleDragScrollButton", &battleDragScrollButton, 0));
#endif
	_info.push_back(OptionInfo("dragScrollTimeTolerance", &dragScrollTimeTolerance, 300)); // miliSecond
	_info.push_back(OptionInfo("dragScrollPixelTolerance", &dragScrollPixelTolerance, 10)); // count of pixel
	_info.push_back(OptionInfo("battleFireSpeed", &battleFireSpeed, 6));
	_info.push_back(OptionInfo("battleXcomSpeed", &battleXcomSpeed, 30));
	_info.push_back(OptionInfo("battleAlienSpeed", &battleAlienSpeed, 30));
#ifdef __MOBILE__
	_info.push_back(OptionInfo("battleNewPreviewPath", (int*)&battleNewPreviewPath, PATH_FULL)); // for android, set full preview by default
#else
	_info.push_back(OptionInfo("battleNewPreviewPath", (int*)&battleNewPreviewPath, PATH_NONE)); // requires double-click to confirm moves
#endif
	_info.push_back(OptionInfo("fpsCounter", &fpsCounter, false));
	_info.push_back(OptionInfo("globeDetail", &globeDetail, true));
	_info.push_back(OptionInfo("globeRadarLines", &globeRadarLines, true));
	_info.push_back(OptionInfo("globeFlightPaths", &globeFlightPaths, true));
	_info.push_back(OptionInfo("globeAllRadarsOnBaseBuild", &globeAllRadarsOnBaseBuild, true));
	_info.push_back(OptionInfo("audioSampleRate", &audioSampleRate, 22050));
	_info.push_back(OptionInfo("audioBitDepth", &audioBitDepth, 16));
	_info.push_back(OptionInfo("audioChunkSize", &audioChunkSize, 1024));
	_info.push_back(OptionInfo("pauseMode", &pauseMode, 0));
	_info.push_back(OptionInfo("battleNotifyDeath", &battleNotifyDeath, false));
	_info.push_back(OptionInfo("showFundsOnGeoscape", &showFundsOnGeoscape, false));
	_info.push_back(OptionInfo("allowResize", &allowResize, false));
	_info.push_back(OptionInfo("windowedModePositionX", &windowedModePositionX, 0));
	_info.push_back(OptionInfo("windowedModePositionY", &windowedModePositionY, 0));
	_info.push_back(OptionInfo("borderless", &borderless, false));
	_info.push_back(OptionInfo("captureMouse", (bool*)&captureMouse, false));
	_info.push_back(OptionInfo("battleTooltips", &battleTooltips, true));
	_info.push_back(OptionInfo("keepAspectRatio", &keepAspectRatio, true));
	_info.push_back(OptionInfo("nonSquarePixelRatio", &nonSquarePixelRatio, false));
	_info.push_back(OptionInfo("cursorInBlackBandsInFullscreen", &cursorInBlackBandsInFullscreen, false));
	_info.push_back(OptionInfo("cursorInBlackBandsInWindow", &cursorInBlackBandsInWindow, true));
	_info.push_back(OptionInfo("cursorInBlackBandsInBorderlessWindow", &cursorInBlackBandsInBorderlessWindow, false));
	_info.push_back(OptionInfo("saveOrder", (int*)&saveOrder, SORT_DATE_DESC));
	_info.push_back(OptionInfo("geoClockSpeed", &geoClockSpeed, 80));
	_info.push_back(OptionInfo("dogfightSpeed", &dogfightSpeed, 30));
	_info.push_back(OptionInfo("geoScrollSpeed", &geoScrollSpeed, 20));
#ifdef __MOBILE__
	_info.push_back(OptionInfo("geoDragScrollButton", &geoDragScrollButton, SDL_BUTTON_LEFT));
#else
	_info.push_back(OptionInfo("geoDragScrollButton", &geoDragScrollButton, SDL_BUTTON_MIDDLE));
#endif
	_info.push_back(OptionInfo("preferredMusic", (int*)&preferredMusic, MUSIC_AUTO));
	_info.push_back(OptionInfo("preferredSound", (int*)&preferredSound, SOUND_AUTO));
	_info.push_back(OptionInfo("preferredVideo", (int*)&preferredVideo, VIDEO_FMV));
	_info.push_back(OptionInfo("musicAlwaysLoop", &musicAlwaysLoop, false));
#ifdef __MOBILE
	_info.push_back(OptionInfo("touchEnabled", &touchEnabled, true));
#else
	_info.push_back(OptionInfo("touchEnabled", &touchEnabled, false));
#endif
	_info.push_back(OptionInfo("rootWindowedMode", &rootWindowedMode, false));
	_info.push_back(OptionInfo("lazyLoadResources", &lazyLoadResources, true));
	// SDL2 scaler options
	_info.push_back(OptionInfo("useNearestScaler", &useNearestScaler, false));
	_info.push_back(OptionInfo("useLinearScaler", &useLinearScaler, true));
	_info.push_back(OptionInfo("useAnisotropicScaler", &useAnisotropicScaler, false));	
	

	// advanced option
	_info.push_back(OptionInfo("playIntro", &playIntro, true, "STR_PLAYINTRO", "STR_GENERAL"));
	_info.push_back(OptionInfo("autosave", &autosave, true, "STR_AUTOSAVE", "STR_GENERAL"));
	_info.push_back(OptionInfo("autosaveFrequency", &autosaveFrequency, 5, "STR_AUTOSAVE_FREQUENCY", "STR_GENERAL"));
	_info.push_back(OptionInfo("newSeedOnLoad", &newSeedOnLoad, false, "STR_NEWSEEDONLOAD", "STR_GENERAL"));
	_info.push_back(OptionInfo("mousewheelSpeed", &mousewheelSpeed, 3, "STR_MOUSEWHEEL_SPEED", "STR_GENERAL"));
	_info.push_back(OptionInfo("changeValueByMouseWheel", &changeValueByMouseWheel, 0, "STR_CHANGEVALUEBYMOUSEWHEEL", "STR_GENERAL"));
	_info.push_back(OptionInfo("soldierDiaries", &soldierDiaries, true));

// this should probably be any small screen touch-device, i don't know the defines for all of them so i'll cover android and IOS as i imagine they're more common
#ifdef __MOBILE__
	_info.push_back(OptionInfo("maximizeInfoScreens", &maximizeInfoScreens, true, "STR_MAXIMIZE_INFO_SCREENS", "STR_GENERAL"));
#elif __APPLE__
	// todo: ask grussel how badly i messed this up.
	#include "TargetConditionals.h"
	#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
		_info.push_back(OptionInfo("maximizeInfoScreens", &maximizeInfoScreens, true, "STR_MAXIMIZE_INFO_SCREENS", "STR_GENERAL"));
	#else
		_info.push_back(OptionInfo("maximizeInfoScreens", &maximizeInfoScreens, false, "STR_MAXIMIZE_INFO_SCREENS", "STR_GENERAL"));
	#endif
#else
	_info.push_back(OptionInfo("maximizeInfoScreens", &maximizeInfoScreens, false, "STR_MAXIMIZE_INFO_SCREENS", "STR_GENERAL"));
#endif
#ifdef __MOBILE__
	_info.push_back(OptionInfo("listDragScroll", &listDragScroll, true, "STR_LISTDRAGSCROLL", "STR_GENERAL"));
#else
	_info.push_back(OptionInfo("listDragScroll", &listDragScroll, false, "STR_LISTDRAGSCROLL", "STR_GENERAL"));
#endif
	_info.push_back(OptionInfo("geoDragScrollInvert", &geoDragScrollInvert, false, "STR_DRAGSCROLLINVERT", "STR_GEOSCAPE")); // true drags away from the cursor, false drags towards (like a grab)
	_info.push_back(OptionInfo("aggressiveRetaliation", &aggressiveRetaliation, false, "STR_AGGRESSIVERETALIATION", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("customInitialBase", &customInitialBase, false, "STR_CUSTOMINITIALBASE", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("allowBuildingQueue", &allowBuildingQueue, false, "STR_ALLOWBUILDINGQUEUE", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("craftLaunchAlways", &craftLaunchAlways, false, "STR_CRAFTLAUNCHALWAYS", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("storageLimitsEnforced", &storageLimitsEnforced, false, "STR_STORAGELIMITSENFORCED", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("canSellLiveAliens", &canSellLiveAliens, false, "STR_CANSELLLIVEALIENS", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("anytimePsiTraining", &anytimePsiTraining, false, "STR_ANYTIMEPSITRAINING", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("anytimeMartialTraining", &anytimeMartialTraining, true, "STR_ANYTIMEMARTIALTRAINING", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("globeSeasons", &globeSeasons, false, "STR_GLOBESEASONS", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("psiStrengthEval", &psiStrengthEval, false, "STR_PSISTRENGTHEVAL", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("canTransferCraftsWhileAirborne", &canTransferCraftsWhileAirborne, false, "STR_CANTRANSFERCRAFTSWHILEAIRBORNE", "STR_GEOSCAPE")); // When the craft can reach the destination base with its fuel
	_info.push_back(OptionInfo("retainCorpses", &retainCorpses, false, "STR_RETAINCORPSES", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("fieldPromotions", &fieldPromotions, false, "STR_FIELDPROMOTIONS", "STR_GEOSCAPE"));
	//_info.push_back(OptionInfo("meetingPoint", &meetingPoint, false, "STR_MEETINGPOINT", "STR_GEOSCAPE"));
#ifdef __MOBILE__
	_info.push_back(OptionInfo("dragSoldierReorder", &dragSoldierReorder, true, "STR_DRAGREORDER", "STR_GEOSCAPE"));
#else
	_info.push_back(OptionInfo("dragSoldierReorder", &dragSoldierReorder, false, "STR_DRAGREORDER", "STR_GEOSCAPE"));
#endif

#if defined(__MOBILE__) || defined (__PSEUDO_ANDROID__)
	_info.push_back(OptionInfo("swipeToTurn", &swipeToTurn, true, "STR_SWIPE", "STR_BATTLESCAPE")); // These first two options are specific to Android;
	_info.push_back(OptionInfo("holdToTurn", &holdToTurn, true, "STR_HOLD", "STR_BATTLESCAPE"));    // they're moved here from a separate menu. 
#else
	_info.push_back(OptionInfo("swipeToTurn", &swipeToTurn, false));
	_info.push_back(OptionInfo("holdToTurn", &holdToTurn, false));
#endif
	_info.push_back(OptionInfo("battleDragScrollInvert", &battleDragScrollInvert, false, "STR_DRAGSCROLLINVERT", "STR_BATTLESCAPE")); // true drags away from the cursor, false drags towards (like a grab)
	_info.push_back(OptionInfo("sneakyAI", &sneakyAI, false, "STR_SNEAKYAI", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("battleUFOExtenderAccuracy", &battleUFOExtenderAccuracy, false, "STR_BATTLEUFOEXTENDERACCURACY", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("showMoreStatsInInventoryView", &showMoreStatsInInventoryView, false, "STR_SHOWMORESTATSININVENTORYVIEW", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("battleHairBleach", &battleHairBleach, true, "STR_BATTLEHAIRBLEACH", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("battleInstantGrenade", &battleInstantGrenade, false, "STR_BATTLEINSTANTGRENADE", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("includePrimeStateInSavedLayout", &includePrimeStateInSavedLayout, false, "STR_INCLUDE_PRIMESTATE_IN_SAVED_LAYOUT", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("battleExplosionHeight", &battleExplosionHeight, 0, "STR_BATTLEEXPLOSIONHEIGHT", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("battleAutoEnd", &battleAutoEnd, false, "STR_BATTLEAUTOEND", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("battleSmoothCamera", &battleSmoothCamera, false, "STR_BATTLESMOOTHCAMERA", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("disableAutoEquip", &disableAutoEquip, false, "STR_DISABLEAUTOEQUIP", "STR_BATTLESCAPE"));
#ifdef __MOBILE__
	_info.push_back(OptionInfo("battleConfirmFireMode", &battleConfirmFireMode, true, "STR_BATTLECONFIRMFIREMODE", "STR_BATTLESCAPE"));
#else
	_info.push_back(OptionInfo("battleConfirmFireMode", &battleConfirmFireMode, false, "STR_BATTLECONFIRMFIREMODE", "STR_BATTLESCAPE"));
#endif
	_info.push_back(OptionInfo("weaponSelfDestruction", &weaponSelfDestruction, false, "STR_WEAPONSELFDESTRUCTION", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("allowPsionicCapture", &allowPsionicCapture, false, "STR_ALLOWPSIONICCAPTURE", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("allowPsiStrengthImprovement", &allowPsiStrengthImprovement, false, "STR_ALLOWPSISTRENGTHIMPROVEMENT", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("strafe", &strafe, false, "STR_STRAFE", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("forceFire", &forceFire, true, "STR_FORCE_FIRE", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("skipNextTurnScreen", &skipNextTurnScreen, false, "STR_SKIPNEXTTURNSCREEN", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("noAlienPanicMessages", &noAlienPanicMessages, false, "STR_NOALIENPANICMESSAGES", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("alienBleeding", &alienBleeding, false, "STR_ALIENBLEEDING", "STR_BATTLESCAPE"));

	// System-specific
	_info.push_back(OptionInfo("systemUI", (int*)&systemUI, SYSTEMUI_ALWAYS_SHOWN));
	_info.push_back(OptionInfo("logToFile", &logToFile, false));
	_info.push_back(OptionInfo("logToSystem", &logToSystem, true));
	_info.push_back(OptionInfo("logTouch", &logTouch, false));
	_info.push_back(OptionInfo("fakeEvents", &fakeEvents, true));
	_info.push_back(OptionInfo("forceGLMode", &forceGLMode, true));
    _info.push_back(OptionInfo("mouseMode", &mouseMode, 0));
	_info.push_back(OptionInfo("longPressDuration", &longPressDuration, 500));
#ifdef __MOBILE__
	_info.push_back(OptionInfo("showCursor", &showCursor, false));
#else
	_info.push_back(OptionInfo("showCursor", &showCursor, true));
#endif

	// OXCE+
	_info.push_back(OptionInfo("twoHandedIndicator", &twoHandedIndicator, true, "STR_TWO_HANDED_INDICATOR", "STR_OXCE"));
	_info.push_back(OptionInfo("twoHandedIndicatorInventory", &twoHandedIndicatorInventory, false, "STR_TWO_HANDED_INDICATOR_INV", "STR_OXCE"));
	_info.push_back(OptionInfo("minimapBorderIndicator", &minimapBorderIndicator, true, "STR_MINIMAP_BORDER_INDICATOR", "STR_OXCE"));
	_info.push_back(OptionInfo("everyoneFightsNobodyQuits", &everyoneFightsNobodyQuits, false, "STR_EVERYONE_FIGHTS_NOBODY_QUITS", "STR_OXCE"));
	_info.push_back(OptionInfo("bleedingIndicator", &bleedingIndicator, false, "STR_BLEEDING_INDICATOR", "STR_OXCE"));
	_info.push_back(OptionInfo("knockOutIndicator", &knockOutIndicator, false, "STR_KNOCK_OUT_INDICATOR", "STR_OXCE"));
	_info.push_back(OptionInfo("statisticalBulletConservation", &statisticalBulletConservation, false, "STR_BULLET_SAVING", "STR_OXCE"));
	_info.push_back(OptionInfo("showItemNameAndWeightInInventory", &showItemNameAndWeightInInventory, false, "STR_SHOW_ITEM_WEIGHT_IN_INVENTORY", "STR_OXCE"));
	_info.push_back(OptionInfo("removeWoundedFromTraining", &removeWoundedFromTraining, false, "STR_REMOVE_WOUNDED_FROM_TRAINING", "STR_OXCE"));
	_info.push_back(OptionInfo("fullNightVision", &fullNightVision, false, "STR_FULL_NIGHT_VISION", "STR_OXCE"));
	_info.push_back(OptionInfo("nightVisionColor", &nightVisionColor, 8, "STR_NIGHT_VISION_COLOR", "STR_OXCE"));
	_info.push_back(OptionInfo("autoNightVision", &autoNightVision, false, "STR_AUTO_NIGHT_VISION", "STR_OXCE"));
	_info.push_back(OptionInfo("autoSell", &autoSell, false, "STR_AUTO_SELL", "STR_OXCE"));
	_info.push_back(OptionInfo("techTreeViewerSpoilerProtection", &techTreeViewerSpoilerProtection, false, "STR_TECH_SPOILER_PROTECTION", "STR_OXCE"));
	_info.push_back(OptionInfo("playBriefingMusicDuringEquipment", &playBriefingMusicDuringEquipment, false, "STR_PLAY_BRIEFING_MUSIC_DURING_EQUIPMENT", "STR_OXCE"));
	_info.push_back(OptionInfo("ufoLandingAlert", &ufoLandingAlert, false, "STR_UFO_LANDING_ALERT", "STR_OXCE"));
	_info.push_back(OptionInfo("friendlyCraftEscort", &friendlyCraftEscort, false, "STR_FRIENDLY_CRAFT_ESCORT", "STR_OXCE"));
	_info.push_back(OptionInfo("drawEnemyRadarCircles", &drawEnemyRadarCircles, true, "STR_DRAW_ENEMY_RADAR_CIRCLES", "STR_OXCE"));
	_info.push_back(OptionInfo("oneHandedUnloading", &oneHandedUnloading, false, "STR_ALLOW_ONE_HANDED_UNLOADING", "STR_OXCE"));

	// controls
	_info.push_back(KeyOptionInfo("keyOk", &keyOk, SDLK_RETURN, "STR_OK", "STR_GENERAL"));
	_info.push_back(KeyOptionInfo("keyCancel", &keyCancel, SDLK_ESCAPE, "STR_CANCEL", "STR_GENERAL"));
	_info.push_back(KeyOptionInfo("keyScreenshot", &keyScreenshot, SDLK_F12, "STR_SCREENSHOT", "STR_GENERAL"));
	_info.push_back(KeyOptionInfo("keyFps", &keyFps, SDLK_F7, "STR_FPS_COUNTER", "STR_GENERAL"));
	_info.push_back(KeyOptionInfo("keyQuickSave", &keyQuickSave, SDLK_F5, "STR_QUICK_SAVE", "STR_GENERAL"));
	_info.push_back(KeyOptionInfo("keyQuickLoad", &keyQuickLoad, SDLK_F9, "STR_QUICK_LOAD", "STR_GENERAL"));
	_info.push_back(KeyOptionInfo("keyGeoLeft", &keyGeoLeft, SDLK_LEFT, "STR_ROTATE_LEFT", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyGeoRight", &keyGeoRight, SDLK_RIGHT, "STR_ROTATE_RIGHT", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyGeoUp", &keyGeoUp, SDLK_UP, "STR_ROTATE_UP", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyGeoDown", &keyGeoDown, SDLK_DOWN, "STR_ROTATE_DOWN", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyGeoZoomIn", &keyGeoZoomIn, SDLK_PLUS, "STR_ZOOM_IN", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyGeoZoomOut", &keyGeoZoomOut, SDLK_MINUS, "STR_ZOOM_OUT", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyGeoSpeed1", &keyGeoSpeed1, SDLK_1, "STR_5_SECONDS", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyGeoSpeed2", &keyGeoSpeed2, SDLK_2, "STR_1_MINUTE", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyGeoSpeed3", &keyGeoSpeed3, SDLK_3, "STR_5_MINUTES", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyGeoSpeed4", &keyGeoSpeed4, SDLK_4, "STR_30_MINUTES", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyGeoSpeed5", &keyGeoSpeed5, SDLK_5, "STR_1_HOUR", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyGeoSpeed6", &keyGeoSpeed6, SDLK_6, "STR_1_DAY", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyGeoIntercept", &keyGeoIntercept, SDLK_i, "STR_INTERCEPT", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyGeoBases", &keyGeoBases, SDLK_b, "STR_BASES", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyGeoGraphs", &keyGeoGraphs, SDLK_g, "STR_GRAPHS", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyGeoUfopedia", &keyGeoUfopedia, SDLK_u, "STR_UFOPAEDIA_UC", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyGeoOptions", &keyGeoOptions, SDLK_ESCAPE, "STR_OPTIONS_UC", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyGeoFunding", &keyGeoFunding, SDLK_f, "STR_FUNDING_UC", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyGeoToggleDetail", &keyGeoToggleDetail, SDLK_TAB, "STR_TOGGLE_COUNTRY_DETAIL", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyGeoToggleRadar", &keyGeoToggleRadar, SDLK_r, "STR_TOGGLE_RADAR_RANGES", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyBaseSelect1", &keyBaseSelect1, SDLK_1, "STR_SELECT_BASE_1", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyBaseSelect2", &keyBaseSelect2, SDLK_2, "STR_SELECT_BASE_2", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyBaseSelect3", &keyBaseSelect3, SDLK_3, "STR_SELECT_BASE_3", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyBaseSelect4", &keyBaseSelect4, SDLK_4, "STR_SELECT_BASE_4", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyBaseSelect5", &keyBaseSelect5, SDLK_5, "STR_SELECT_BASE_5", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyBaseSelect6", &keyBaseSelect6, SDLK_6, "STR_SELECT_BASE_6", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyBaseSelect7", &keyBaseSelect7, SDLK_7, "STR_SELECT_BASE_7", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyBaseSelect8", &keyBaseSelect8, SDLK_8, "STR_SELECT_BASE_8", "STR_GEOSCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleLeft", &keyBattleLeft, SDLK_LEFT, "STR_SCROLL_LEFT", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleRight", &keyBattleRight, SDLK_RIGHT, "STR_SCROLL_RIGHT", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleUp", &keyBattleUp, SDLK_UP, "STR_SCROLL_UP", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleDown", &keyBattleDown, SDLK_DOWN, "STR_SCROLL_DOWN", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleLevelUp", &keyBattleLevelUp, SDLK_PAGEUP, "STR_VIEW_LEVEL_ABOVE", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleLevelDown", &keyBattleLevelDown, SDLK_PAGEDOWN, "STR_VIEW_LEVEL_BELOW", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleCenterUnit", &keyBattleCenterUnit, SDLK_HOME, "STR_CENTER_SELECTED_UNIT", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattlePrevUnit", &keyBattlePrevUnit, SDLK_UNKNOWN, "STR_PREVIOUS_UNIT", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleNextUnit", &keyBattleNextUnit, SDLK_TAB, "STR_NEXT_UNIT", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleDeselectUnit", &keyBattleDeselectUnit, SDLK_BACKSLASH, "STR_DESELECT_UNIT", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleUseLeftHand", &keyBattleUseLeftHand, SDLK_q, "STR_USE_LEFT_HAND", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleUseRightHand", &keyBattleUseRightHand, SDLK_e, "STR_USE_RIGHT_HAND", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleInventory", &keyBattleInventory, SDLK_i, "STR_INVENTORY", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleMap", &keyBattleMap, SDLK_m, "STR_MINIMAP", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleOptions", &keyBattleOptions, SDLK_ESCAPE, "STR_OPTIONS", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleEndTurn", &keyBattleEndTurn, SDLK_BACKSPACE, "STR_END_TURN", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleAbort", &keyBattleAbort, SDLK_a, "STR_ABORT_MISSION", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleStats", &keyBattleStats, SDLK_s, "STR_UNIT_STATS", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleKneel", &keyBattleKneel, SDLK_k, "STR_KNEEL", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleReload", &keyBattleReload, SDLK_r, "STR_RELOAD", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattlePersonalLighting", &keyBattlePersonalLighting, SDLK_l, "STR_TOGGLE_PERSONAL_LIGHTING", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleReserveNone", &keyBattleReserveNone, SDLK_F1, "STR_DONT_RESERVE_TIME_UNITS", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleReserveSnap", &keyBattleReserveSnap, SDLK_F2, "STR_RESERVE_TIME_UNITS_FOR_SNAP_SHOT", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleReserveAimed", &keyBattleReserveAimed, SDLK_F3, "STR_RESERVE_TIME_UNITS_FOR_AIMED_SHOT", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleReserveAuto", &keyBattleReserveAuto, SDLK_F4, "STR_RESERVE_TIME_UNITS_FOR_AUTO_SHOT", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleReserveKneel", &keyBattleReserveKneel, SDLK_j, "STR_RESERVE_TIME_UNITS_FOR_KNEEL", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleZeroTUs", &keyBattleZeroTUs, SDLK_DELETE, "STR_EXPEND_ALL_TIME_UNITS", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleCenterEnemy1", &keyBattleCenterEnemy1, SDLK_1, "STR_CENTER_ON_ENEMY_1", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleCenterEnemy2", &keyBattleCenterEnemy2, SDLK_2, "STR_CENTER_ON_ENEMY_2", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleCenterEnemy3", &keyBattleCenterEnemy3, SDLK_3, "STR_CENTER_ON_ENEMY_3", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleCenterEnemy4", &keyBattleCenterEnemy4, SDLK_4, "STR_CENTER_ON_ENEMY_4", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleCenterEnemy5", &keyBattleCenterEnemy5, SDLK_5, "STR_CENTER_ON_ENEMY_5", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleCenterEnemy6", &keyBattleCenterEnemy6, SDLK_6, "STR_CENTER_ON_ENEMY_6", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleCenterEnemy7", &keyBattleCenterEnemy7, SDLK_7, "STR_CENTER_ON_ENEMY_7", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleCenterEnemy8", &keyBattleCenterEnemy8, SDLK_8, "STR_CENTER_ON_ENEMY_8", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleCenterEnemy9", &keyBattleCenterEnemy9, SDLK_9, "STR_CENTER_ON_ENEMY_9", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleCenterEnemy10", &keyBattleCenterEnemy10, SDLK_0, "STR_CENTER_ON_ENEMY_10", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyBattleVoxelView", &keyBattleVoxelView, SDLK_F10, "STR_SAVE_VOXEL_VIEW", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyInvCreateTemplate", &keyInvCreateTemplate, SDLK_c, "STR_CREATE_INVENTORY_TEMPLATE", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyInvApplyTemplate", &keyInvApplyTemplate, SDLK_v, "STR_APPLY_INVENTORY_TEMPLATE", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyInvClear", &keyInvClear, SDLK_x, "STR_CLEAR_INVENTORY", "STR_BATTLESCAPE"));
	_info.push_back(KeyOptionInfo("keyInvAutoEquip", &keyInvAutoEquip, SDLK_z, "STR_AUTO_EQUIP", "STR_BATTLESCAPE"));

	// OXCE+
	_info.push_back(KeyOptionInfo("keyGeoUfoTracker", &keyGeoUfoTracker, SDLK_t, "STR_UFO_TRACKER", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keyGeoTechTreeViewer", &keyGeoTechTreeViewer, SDLK_q, "STR_TECH_TREE_VIEWER", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keyGeoGlobalResearch", &keyGeoGlobalResearch, SDLK_c, "STR_RESEARCH_OVERVIEW", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keyGraphsZoomIn", &keyGraphsZoomIn, SDLK_KP_PLUS, "STR_GRAPHS_ZOOM_IN", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keyGraphsZoomOut", &keyGraphsZoomOut, SDLK_KP_MINUS, "STR_GRAPHS_ZOOM_OUT", "STR_OXCE"));

	_info.push_back(KeyOptionInfo("keyToggleQuickSearch", &keyToggleQuickSearch, SDLK_q, "STR_TOGGLE_QUICK_SEARCH", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keyCraftLoadoutSave", &keyCraftLoadoutSave, SDLK_F5, "STR_SAVE_CRAFT_LOADOUT_TEMPLATE", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keyCraftLoadoutLoad", &keyCraftLoadoutLoad, SDLK_F9, "STR_LOAD_CRAFT_LOADOUT_TEMPLATE", "STR_OXCE"));

	_info.push_back(KeyOptionInfo("keyMarkAllAsSeen", &keyMarkAllAsSeen, SDLK_x, "STR_MARK_ALL_AS_SEEN", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keySelectAll", &keySelectAll, SDLK_x, "STR_SELECT_ALL", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keyDeselectAll", &keyDeselectAll, SDLK_x, "STR_DESELECT_ALL", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keyResetAll", &keyResetAll, SDLK_x, "STR_RESET_ALL", "STR_OXCE"));

	_info.push_back(KeyOptionInfo("keyInventoryArmor", &keyInventoryArmor, SDLK_a, "STR_INVENTORY_ARMOR", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keyInventoryAvatar", &keyInventoryAvatar, SDLK_m, "STR_INVENTORY_AVATAR", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keyInventorySave", &keyInventorySave, SDLK_F5, "STR_SAVE_EQUIPMENT_TEMPLATE", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keyInventoryLoad", &keyInventoryLoad, SDLK_F9, "STR_LOAD_EQUIPMENT_TEMPLATE", "STR_OXCE"));

	_info.push_back(KeyOptionInfo("keyBattleUseSpecial", &keyBattleUseSpecial, SDLK_w, "STR_USE_SPECIAL_ITEM", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keyBattleActionItem1", &keyBattleActionItem1, SDLK_1, "STR_ACTION_ITEM_1", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keyBattleActionItem2", &keyBattleActionItem2, SDLK_2, "STR_ACTION_ITEM_2", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keyBattleActionItem3", &keyBattleActionItem3, SDLK_3, "STR_ACTION_ITEM_3", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keyBattleActionItem4", &keyBattleActionItem4, SDLK_4, "STR_ACTION_ITEM_4", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keyBattleActionItem5", &keyBattleActionItem5, SDLK_5, "STR_ACTION_ITEM_5", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keyNightVisionToggle", &keyNightVisionToggle, SDLK_SCROLLLOCK, "STR_TOGGLE_NIGHT_VISION", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keyNightVisionHold", &keyNightVisionHold, SDLK_SPACE, "STR_HOLD_NIGHT_VISION", "STR_OXCE"));
	_info.push_back(KeyOptionInfo("keySelectMusicTrack", &keySelectMusicTrack, SDLK_END, "STR_SELECT_MUSIC_TRACK", "STR_OXCE"));

#ifdef __MORPHOS__
	_info.push_back(OptionInfo("FPS", &FPS, 15, "STR_FPS_LIMIT", "STR_GENERAL"));
	_info.push_back(OptionInfo("FPSInactive", &FPSInactive, 15, "STR_FPS_INACTIVE_LIMIT", "STR_GENERAL"));
#else
	_info.push_back(OptionInfo("FPS", &FPS, 60, "STR_FPS_LIMIT", "STR_GENERAL"));
	_info.push_back(OptionInfo("FPSInactive", &FPSInactive, 30, "STR_FPS_INACTIVE_LIMIT", "STR_GENERAL"));
#endif

}

// we can get fancier with these detection routines, but for now just look for
// *something* in the data folders.  case sensitivity can make actually verifying
// that the *correct* files are there complex.
static bool _gameIsInstalled(const std::string &gameName)
{
	// look for game data in either the data or user directorie
	std::string dataGameFolder = CrossPlatform::searchDataFolder(gameName);
	std::string dataGameZipFile = CrossPlatform::searchDataFile(gameName + ".zip");
	std::string userGameFolder = _userFolder + gameName;
	std::string userGameZipFile = _userFolder + gameName + ".zip";
	return (CrossPlatform::folderExists(dataGameFolder)	&& CrossPlatform::getFolderContents(dataGameFolder).size() > 8)
	    || (CrossPlatform::folderExists(userGameFolder)	&& CrossPlatform::getFolderContents(userGameFolder).size() > 8)
		||  CrossPlatform::fileExists( dataGameZipFile )
		||  CrossPlatform::fileExists( userGameZipFile );
}

static bool _ufoIsInstalled()
{
	return _gameIsInstalled("UFO");
}

static bool _tftdIsInstalled()
{
	return _gameIsInstalled("TFTD");
}

static void _setDefaultMods()
{
	bool haveUfo = _ufoIsInstalled();
	if (haveUfo)
	{
		mods.push_back(std::pair<std::string, bool>("xcom1", true));
	}

	if (_tftdIsInstalled())
	{
		mods.push_back(std::pair<std::string, bool>("xcom2", !haveUfo));
	}
}

/**
 * Resets the options back to their defaults.
 */
void resetDefault()
{
	for (std::vector<OptionInfo>::iterator i = _info.begin(); i != _info.end(); ++i)
	{
		i->reset();
	}
	backupDisplay();

	mods.clear();
	if (!_dataList.empty())
	{
		_setDefaultMods();
	}
}

/**
 * Loads options from a set of command line arguments,
 * in the format "-option value".
 * @param argc Number of arguments.
 * @param argv Array of argument strings.
 */
static void loadArgs()
{
	Options::systemLocale = "";
	auto argv = CrossPlatform::getArgs();
	for (size_t i = 1; i < argv.size(); ++i)
	{
		auto arg = argv[i];
		if (arg.size() > 1 && arg[0] == '-')
		{
			std::string argname;
			if (arg[1] == '-' && arg.length() > 2)
				argname = arg.substr(2, arg.length()-1);
			else
				argname = arg.substr(1, arg.length()-1);
			std::transform(argname.begin(), argname.end(), argname.begin(), ::tolower);
			if (argname == "cont" || argname == "continue")
			{
				_loadLastSave = true;
				continue;
			}
			if (argv.size() > i + 1)
			{
				++i; // we'll be consuming the next argument too
				Log(LOG_DEBUG) << "loadArgs(): "<< argname <<" -> " << argv[i];
				if (argname == "data")
				{
					_dataFolder = argv[i];
					if (_dataFolder.size() > 1 && _dataFolder[_dataFolder.size() - 1] != '/') {
						_dataFolder.push_back('/');
					}
				}
				else if (argname == "user")
				{
					_userFolder = argv[i];
					if (_userFolder.size() > 1 && _userFolder[_userFolder.size() - 1] != '/') {
						_userFolder.push_back('/');
					}
				}
				else if (argname == "cfg" || argname == "config")
				{
					_configFolder = argv[i];
					if (_configFolder.size() > 1 && _configFolder[_configFolder.size() - 1] != '/') {
						_configFolder.push_back('/');
					}
				}
				else if (argname == "locale")
				{
					systemLocale += argv[i];
					Log(LOG_INFO) << "Current locale is " << systemLocale;
				}
				else
				{
					//save this command line option for now, we will apply it later
					_commandLine[argname] = argv[i];
				}
			}
			else
			{
				Log(LOG_WARNING) << "Unknown option: " << argname;
			}
		}
	}
}

/*
 * Displays command-line help when appropriate.
 * @param argc Number of arguments.
 * @param argv Array of argument strings.
 */
static bool showHelp()
{
	std::ostringstream help;
	help << "OpenXcom v" << OPENXCOM_VERSION_SHORT << std::endl;
	help << "Usage: openxcom [OPTION]..." << std::endl << std::endl;
	help << "-data PATH" << std::endl;
	help << "        use PATH as the default Data Folder instead of auto-detecting" << std::endl << std::endl;
	help << "-user PATH" << std::endl;
	help << "        use PATH as the default User Folder instead of auto-detecting" << std::endl << std::endl;
	help << "-cfg PATH  or  -config PATH" << std::endl;
	help << "        use PATH as the default Config Folder instead of auto-detecting" << std::endl << std::endl;
	help << "-KEY VALUE" << std::endl;
	help << "        set option KEY to VALUE instead of default/loaded value (eg. -displayWidth 640)" << std::endl << std::endl;
	help << "-help" << std::endl;
	help << "-?" << std::endl;
	help << "        show command-line help" << std::endl;
	for (auto arg: CrossPlatform::getArgs())
	{
		if ((arg[0] == '-' || arg[0] == '/') && arg.length() > 1)
		{
			std::string argname;
			if (arg[1] == '-' && arg.length() > 2)
				argname = arg.substr(2, arg.length()-1);
			else
				argname = arg.substr(1, arg.length()-1);
			std::transform(argname.begin(), argname.end(), argname.begin(), ::tolower);
			if (argname == "help" || argname == "?")
			{
				std::cout << help.str();
				return true;
			}
		}
	}
	return false;
}

const std::unordered_map<std::string, ModInfo> &getModInfos() { return _modInfos; }

/**
 * Splits the game's User folder by master mod,
 * creating a subfolder for each one.
 * Moving the saves from userFolder into subfolders
 * has been removed.
 */
static void userSplitMasters()
{
	for (auto i : _modInfos) {
		if (i.second.isMaster()) {
			std::string masterFolder = _userFolder + i.first;
			if (!CrossPlatform::folderExists(masterFolder)) {
				CrossPlatform::createFolder(masterFolder);
			}
		}
	}
}

/**
 * Handles the initialization of setting up default option
 * and finding and loading any existing ones.
 * @param argc Number of arguments.
 * @param argv Array of argument strings.
 * @return Do we start the game?
 */
bool init()
{
	if (showHelp())
		return false;
	create();
	resetDefault();
	loadArgs();
	setFolders();
	_setDefaultMods();
	updateOptions();

	// set up the logging reportingLevel
#ifdef _DEBUG
	Logger::reportingLevel() = LOG_DEBUG;
#else
	Logger::reportingLevel() = LOG_INFO;
#endif

	if (Options::verboseLogging)
		Logger::reportingLevel() = LOG_VERBOSE;

	// this enables writes to the log file and filters already emitted messages
	CrossPlatform::setLogFileName(getUserFolder() + "openxcom.log");

	Log(LOG_INFO) << "OpenXcom Version: " << OPENXCOM_VERSION_SHORT << OPENXCOM_VERSION_GIT;
#ifdef _WIN64
	Log(LOG_INFO) << "Platform: Windows 64 bit";
#elif _WIN32
	Log(LOG_INFO) << "Platform: Windows 32 bit";
#elif __APPLE__
	Log(LOG_INFO) << "Platform: OSX";
#elif  __ANDROID_API__
	Log(LOG_INFO) << "Platform: Android " << __ANDROID_API__ ;
#elif __linux__
	Log(LOG_INFO) << "Platform: Linux";
#else
	Log(LOG_INFO) << "Platform: Unix-like";
#endif

	Log(LOG_INFO) << "Data folder is: " << _dataFolder;
	Log(LOG_INFO) << "Data search is: ";
	for (std::vector<std::string>::iterator i = _dataList.begin(); i != _dataList.end(); ++i)
	{
		Log(LOG_INFO) << "- " << *i;
	}
	Log(LOG_INFO) << "User folder is: " << _userFolder;
	Log(LOG_INFO) << "Config folder is: " << _configFolder;
	Log(LOG_INFO) << "Options loaded successfully.";

	FileMap::clear();
	return true;
}

// called from the dos screen state (StartState)
void updateMods()
{
	FileMap::clear();
	Log(LOG_INFO) << "Scanning standard mods in '" << getDataFolder() << "'...";
	FileMap::scanModDir(getDataFolder(), "standard");
	Log(LOG_INFO) << "Scanning user mods in '" << getUserFolder() << "'...";
	FileMap::scanModDir(getUserFolder(), "mods");

	// Check mods' dependencies on other mods and extResources (UFO, TFTD, etc),
	// also breaks circular dependency loops.
	FileMap::checkModsDependencies();

	// Now we can get the list of ModInfos from the FileMap -
	// those are the mods that can possibly be loaded.
	_modInfos = FileMap::getModInfos();

	// remove mods from list that no longer exist
	for (auto i = mods.begin(); i != mods.end();)
	{
		auto modIt = _modInfos.find(i->first);
		if (_modInfos.end() == modIt)
		{
			Log(LOG_VERBOSE) << "removing references to missing mod: " << i->first;
			i = mods.erase(i);
			continue;
		}
		++i;
	}

	// add in any new mods picked up from the scan and ensure there is but a single
	// master active
	std::string activeMaster;
	std::string inactiveMaster;
	for (auto i = _modInfos.cbegin(); i != _modInfos.cend(); ++i)
	{
		bool found = false;
		for (std::vector< std::pair<std::string, bool> >::iterator j = mods.begin(); j != mods.end(); ++j)
		{
			if (i->first == j->first)
			{
				found = true;
				if (i->second.isMaster())
				{
					if (j->second)
					{
						if (!activeMaster.empty())
						{
							Log(LOG_WARNING) << "Too many active masters detected; turning off " << j->first;
							j->second = false;
						}
						else
						{
							activeMaster = j->first;
						}
					}
					else
					{
						// prefer activating standard masters over a possibly broken
						// third party master
						if (inactiveMaster.empty() || j->first == "xcom1" || j->first == "xcom2")
						{
							inactiveMaster = j->first;
						}
					}
				}

				break;
			}
		}
		if (found)
		{
			continue;
		}

		// not active by default
		std::pair<std::string, bool> newMod(i->first, false);
		if (i->second.isMaster())
		{
			// it doesn't matter what order the masters are in since
			// only one can be active at a time anyway
			mods.insert(mods.begin(), newMod);

			if (inactiveMaster.empty())
			{
				inactiveMaster = i->first;
			}
		}
		else
		{
			mods.push_back(newMod);
		}
	}

	if (activeMaster.empty())
	{
		if (inactiveMaster.empty())
		{
			Log(LOG_ERROR) << "no mod masters available";
			throw Exception("No X-COM installations found");
		}
		else
		{
			Log(LOG_INFO) << "no master already active; activating " << inactiveMaster;
			std::find(mods.begin(), mods.end(), std::pair<std::string, bool>(inactiveMaster, false))->second = true;
			_masterMod = inactiveMaster;
		}
	}
	else
	{
		_masterMod = activeMaster;
	}

	updateReservedSpace();
	FileMap::setup(getActiveMods());
	userSplitMasters();
}

/**
 * Gets the currently active master mod.
 * @return Mod id.
 */
std::string getActiveMaster()
{
	return _masterMod;
}

bool getLoadLastSave()
{
	return _loadLastSave && !_loadLastSaveExpended;
}

void expendLoadLastSave()
{
	_loadLastSaveExpended = true;
}

void updateReservedSpace()
{
	Log(LOG_VERBOSE) << "Updating reservedSpace for master mods if necessary...";

	Log(LOG_VERBOSE) << "_masterMod = " << _masterMod;

	int maxSize = 1;
	for (auto i = mods.rbegin(); i != mods.rend(); ++i)
	{
		if (!i->second)
		{
			Log(LOG_VERBOSE) << "skipping inactive mod: " << i->first;
			continue;
		}

		const ModInfo &modInfo = _modInfos.find(i->first)->second;
		if (!modInfo.canActivate(_masterMod))
		{
			Log(LOG_VERBOSE) << "skipping mod for non-current master: " << i->first << "(" << modInfo.getMaster() << " != " << _masterMod << ")";
			continue;
		}

		if (modInfo.getReservedSpace() > maxSize)
		{
			maxSize = modInfo.getReservedSpace();
		}
	}

	if (maxSize > 1)
	{
		// Small hack: update ALL masters, not only active master!
		// this is because, there can be a hierarchy of multiple masters (e.g. xcom1 master > fluffyUnicorns master > some fluffyUnicorns mod)
		// and the one that needs to be updated is actually the "root", i.e. xcom1 master
		for (auto i = _modInfos.begin(); i != _modInfos.end(); ++i)
		{
			if (i->second.isMaster())
			{
				if (i->second.getReservedSpace() < maxSize)
				{
					i->second.setReservedSpace(maxSize);
					Log(LOG_INFO) << "reservedSpace for: " << i->first << " updated to: " << i->second.getReservedSpace();
				}
				else
				{
					Log(LOG_INFO) << "reservedSpace for: " << i->first << " is: " << i->second.getReservedSpace();
				}
			}
		}
	}
}

/**
 * Sets up the game's Data folder where the data file
 * are loaded from and the User folder and Config
 * folder where settings and saves are stored in.
 */
void setFolders()
{
	_dataList = CrossPlatform::findDataFolders();
	if (!_dataFolder.empty())
	{
		_dataList.insert(_dataList.begin(), _dataFolder);
		Log(LOG_DEBUG) << "setFolders(): inserting " << _dataFolder;
	}
	if (_userFolder.empty())
	{
		std::vector<std::string> user = CrossPlatform::findUserFolders();

		if (_configFolder.empty())
		{
			_configFolder = CrossPlatform::findConfigFolder();
		}

		// Look for an existing user folder
		for (std::vector<std::string>::reverse_iterator i = user.rbegin(); i != user.rend(); ++i)
		{
			if (CrossPlatform::folderExists(*i))
			{
				_userFolder = *i;
				break;
			}
		}

		// Set up folder
		if (_userFolder.empty())
		{
			for (std::vector<std::string>::iterator i = user.begin(); i != user.end(); ++i)
			{
				if (CrossPlatform::createFolder(*i))
				{
					_userFolder = *i;
					break;
				}
			}
		}
	}
	if (!_userFolder.empty())
	{
		// create mod folder if it doesn't already exist
		CrossPlatform::createFolder(_userFolder + "mods");
	}

	if (_configFolder.empty())
	{
		_configFolder = _userFolder;
	}
}

/**
 * Updates the game's options with those in the configuration
 * file, if it exists yet, and any supplied on the command line.
 */
void updateOptions()
{
	// Load existing option
	if (CrossPlatform::folderExists(_configFolder))
	{
		if (CrossPlatform::fileExists(_configFolder + "options.cfg"))
		{
			load();
		}
		else
		{
			save();
		}
	}
	// Create config folder and save option
	else
	{
		CrossPlatform::createFolder(_configFolder);
		save();
	}

	// now apply options set on the command line, overriding defaults and those loaded from config file
	//if (!_commandLine.empty())
	for (std::vector<OptionInfo>::iterator i = _info.begin(); i != _info.end(); ++i)
	{
		i->load(_commandLine, true);
	}
}

/**
 * Loads options from a YAML file.
 * @param filename YAML filename.
 * @return Was the loading successful?
 */
bool load(const std::string &filename)
{
	std::string s = _configFolder + filename + ".cfg";
	try
	{
		YAML::Node doc = YAML::Load(*CrossPlatform::readFile(s));
		// Ignore old options file
		if (doc["options"]["NewBattleMission"])
		{
			return false;
		}
		for (std::vector<OptionInfo>::iterator i = _info.begin(); i != _info.end(); ++i)
		{
			i->load(doc["options"]);
		}

		mods.clear();
		for (YAML::const_iterator i = doc["mods"].begin(); i != doc["mods"].end(); ++i)
		{
			std::string id = (*i)["id"].as<std::string>();
			bool active = (*i)["active"].as<bool>(false);
			mods.push_back(std::pair<std::string, bool>(id, active));
		}
		if (mods.empty())
		{
			_setDefaultMods();
		}
	}
	catch (YAML::Exception &e)
	{
		Log(LOG_WARNING) << e.what();
		return false;
	}
	return true;
}

void writeNode(const YAML::Node& node, YAML::Emitter& emitter)
{
	switch (node.Type())
	{
		case YAML::NodeType::Sequence:
		{
			emitter << YAML::BeginSeq;
			for (size_t i = 0; i < node.size(); i++)
			{
				writeNode(node[i], emitter);
			}
			emitter << YAML::EndSeq;
			break;
		}
		case YAML::NodeType::Map:
		{
			emitter << YAML::BeginMap;

			// First collect all the keys
			std::vector<std::string> keys(node.size());
			int key_it = 0;
			for (YAML::const_iterator it = node.begin(); it != node.end(); ++it)
			{
				keys[key_it++] = it->first.as<std::string>();
			}

			// Then sort them
			std::sort(keys.begin(), keys.end());

			// Then emit all the entries in sorted order.
			for(size_t i = 0; i < keys.size(); i++)
			{
				emitter << YAML::Key;
				emitter << keys[i];
				emitter << YAML::Value;
				writeNode(node[keys[i]], emitter);
			}
			emitter << YAML::EndMap;
			break;
		}
		default:
			emitter << node;
			break;
	}
}

/**
 * Saves options to a YAML file.
 * @param filename YAML filename.
 * @return Was the saving successful?
 */
bool save(const std::string &filename)
{
	YAML::Emitter out;
	try
	{
		YAML::Node doc, node;
		for (std::vector<OptionInfo>::iterator i = _info.begin(); i != _info.end(); ++i)
		{
			i->save(node);
		}
		doc["options"] = node;

		for (std::vector< std::pair<std::string, bool> >::iterator i = mods.begin(); i != mods.end(); ++i)
		{
			YAML::Node mod;
			mod["id"] = i->first;
			mod["active"] = i->second;
			doc["mods"].push_back(mod);
		}

		writeNode(doc, out);
	}
	catch (YAML::Exception &e)
	{
		Log(LOG_WARNING) << e.what();
		return false;
	}
	std::string filepath = _configFolder + filename + ".cfg";
	std::string data(out.c_str());

	if (!CrossPlatform::writeFile(filepath, data + "\n" ))
	{
		Log(LOG_WARNING) << "Failed to save " << filepath;
		return false;
	}
	return true;
}

/**
 * Returns the game's current Data folder where resource
 * and X-Com files are loaded from.
 * @return Full path to Data folder.
 */
std::string getDataFolder()
{
	return _dataFolder;
}

/**
 * Changes the game's current Data folder where resource
 * and X-Com files are loaded from.
 * @param folder Full path to Data folder.
 */
void setDataFolder(const std::string &folder)
{
	_dataFolder = folder;
	Log(LOG_DEBUG) << "setDataFolder(" << folder <<");";
}

/**
 * Returns the game's list of possible Data folders.
 * @return List of Data paths.
 */
const std::vector<std::string> &getDataList()
{
	return _dataList;
}

/**
 * Returns the game's User folder where
 * saves are stored in.
 * @return Full path to User folder.
 */
std::string getUserFolder()
{
	return _userFolder;
}

/**
 * Returns the game's Config folder where
 * settings are stored in. Normally the same
 * as the User folder.
 * @return Full path to Config folder.
 */
std::string getConfigFolder()
{
	return _configFolder;
}

/**
 * Returns the game's User folder for the
 * currently loaded master mod.
 * @return Full path to User folder.
 */
std::string getMasterUserFolder()
{
	return _userFolder + _masterMod + "/";
}

/**
 * Returns the game's list of all available option information.
 * @return List of OptionInfo's.
 */
const std::vector<OptionInfo> &getOptionInfo()
{
	return _info;
}

/**
 * Returns a list of currently active mods.
 * They must be enabled and activable.
 * @sa ModInfo::canActivate
 * @return List of info for the active mods.
 */
std::vector<const ModInfo *> getActiveMods()
{
	std::vector<const ModInfo*> activeMods;
	for (std::vector< std::pair<std::string, bool> >::iterator i = mods.begin(); i != mods.end(); ++i)
	{
		if (i->second)
		{
			const ModInfo *info = &_modInfos.at(i->first);
			if (info->canActivate(_masterMod))
			{
				activeMods.push_back(info);
			}
		}
	}
	return activeMods;
}

/**
 * Saves display settings temporarily to be able
 * to revert to old ones.
 */
void backupDisplay()
{
	Options::newDisplayWidth = Options::displayWidth;
	Options::newDisplayHeight = Options::displayHeight;
	Options::newBattlescapeScale = Options::battlescapeScale;
	Options::newGeoscapeScale = Options::geoscapeScale;
	Options::newOpenGL = Options::useOpenGL;
	Options::newScaleFilter = Options::useScaleFilter;
	Options::newHQXFilter = Options::useHQXFilter;
	Options::newOpenGLShader = Options::useOpenGLShader;
	Options::newXBRZFilter = Options::useXBRZFilter;
	Options::newRootWindowedMode = Options::rootWindowedMode;
	Options::newWindowedModePositionX = Options::windowedModePositionX;
	Options::newWindowedModePositionY = Options::windowedModePositionY;
	Options::newFullscreen = Options::fullscreen;
	Options::newAllowResize = Options::allowResize;
	Options::newBorderless = Options::borderless;
	Options::newNearestScaler = Options::useNearestScaler;
	Options::newLinearScaler = Options::useLinearScaler;
	Options::newAnisotropicScaler = Options::useAnisotropicScaler;
}

/**
 * Switches old/new display options for temporarily
 * testing a new display setup.
 */
void switchDisplay()
{
	std::swap(displayWidth, newDisplayWidth);
	std::swap(displayHeight, newDisplayHeight);
	std::swap(useOpenGL, newOpenGL);
	std::swap(useScaleFilter, newScaleFilter);
	std::swap(battlescapeScale, newBattlescapeScale);
	std::swap(geoscapeScale, newGeoscapeScale);
	std::swap(useHQXFilter, newHQXFilter);
	std::swap(useOpenGLShader, newOpenGLShader);
	std::swap(useXBRZFilter, newXBRZFilter);
	std::swap(rootWindowedMode, newRootWindowedMode);
	std::swap(windowedModePositionX, newWindowedModePositionX);
	std::swap(windowedModePositionY, newWindowedModePositionY);
	std::swap(fullscreen, newFullscreen);
	std::swap(allowResize, newAllowResize);
	std::swap(borderless, newBorderless);
	std::swap(useNearestScaler, newNearestScaler);
	std::swap(useLinearScaler, newLinearScaler);
	std::swap(useAnisotropicScaler, newAnisotropicScaler);
}

void setUserFolder(const std::string &userFolder)
{
	_userFolder = userFolder;
	Log(LOG_INFO) << "Options::setUserFolder: user folder set to " << _userFolder;
}

void setConfFolder(const std::string &confFolder)
{
	_configFolder = confFolder;
	Log(LOG_INFO) << "Options::setConfFolder: config folder set to " << _configFolder;
}

}

}
