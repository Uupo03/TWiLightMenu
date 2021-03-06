#include "pergamesettings.h"
#include "dsimenusettings.h"
#include "common/inifile.h"
#include "tool/stringtool.h"
#include "tool/dbgtool.h"
#include "bootstrappaths.h"
#include <string.h>
#include <cstdio>
#include "systemdetails.h"

PerGameSettings::PerGameSettings(const std::string &romFileName)
{
    _iniPath = formatString(PERGAMESETTINGS_PATH, (ms().secondaryDevice ? "fat:" : "sd:"), romFileName.c_str());
    language = ELangDefault;
    boostCpu = EDefault;
    boostVram = EDefault;
    dsiMode = EDefault;
    directBoot = EFalse;
    bootstrapFile = EDefault;
    loadSettings();
}

void PerGameSettings::loadSettings()
{
    CIniFile pergameini(_iniPath);
    dbg_printf("CINI LOAD %s", _iniPath.c_str());
    directBoot = (TDefaultBool)pergameini.GetInt("GAMESETTINGS", "DIRECT_BOOT", ms().secondaryDevice);	// Homebrew only
    dsiMode = (TDefaultBool)pergameini.GetInt("GAMESETTINGS", "DSI_MODE", dsiMode);
	language = (TLanguage)pergameini.GetInt("GAMESETTINGS", "LANGUAGE", language);
	boostCpu = (TDefaultBool)pergameini.GetInt("GAMESETTINGS", "BOOST_CPU", boostCpu);
	boostVram = (TDefaultBool)pergameini.GetInt("GAMESETTINGS", "BOOST_VRAM", boostVram);
    bootstrapFile = (TDefaultBool)pergameini.GetInt("GAMESETTINGS", "BOOTSTRAP_FILE", bootstrapFile);
}

void PerGameSettings::saveSettings()
{
    CIniFile pergameini(_iniPath);
    pergameini.SetInt("GAMESETTINGS", "DIRECT_BOOT", directBoot);	// Homebrew only
    pergameini.SetInt("GAMESETTINGS", "LANGUAGE", language);
	if (isDSiMode()) {
		pergameini.SetInt("GAMESETTINGS", "DSI_MODE", dsiMode);
		pergameini.SetInt("GAMESETTINGS", "BOOST_CPU", boostCpu);
		pergameini.SetInt("GAMESETTINGS", "BOOST_VRAM", boostVram);
	}
    pergameini.SetInt("GAMESETTINGS", "BOOTSTRAP_FILE", bootstrapFile);
    pergameini.SaveIniFile(_iniPath);
}

bool PerGameSettings::checkIfShowAPMsg() {
    CIniFile pergameini(_iniPath);
	if (pergameini.GetInt("GAMESETTINGS", "NO_SHOW_AP_MSG", 0) == 0) {
		return true;	// Show AP message
	}
	return false;	// Don't show AP message
}

void PerGameSettings::dontShowAPMsgAgain() {
    CIniFile pergameini(_iniPath);
	pergameini.SetInt("GAMESETTINGS", "NO_SHOW_AP_MSG", 1);
	pergameini.SaveIniFile(_iniPath);
}
