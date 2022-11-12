//
//  ECEStart.cpp
//  ECEnabler
//
//  Created by Avery Black on 4/26/21.
//

#include <Headers/plugin_start.hpp>
#include <Headers/kern_api.hpp>

#include "ECEnabler.hpp"

static AirPortDebugPatcher ece;

const char *bootargOff[] {
    "-airportdebugoff"
};

const char *bootargDebug[] {
    "-airportdebugdebug"
};

const char *bootargBeta[] {
    "-airportdebugbeta"
};

PluginConfiguration ADDPR(config) {
    xStringify(PRODUCT_NAME),
    parseModuleVersion(xStringify(MODULE_VERSION)),
    LiluAPI::AllowNormal |
    LiluAPI::AllowSafeMode |
    LiluAPI::AllowInstallerRecovery,
    bootargOff,
    arrsize(bootargOff),
    bootargDebug,
    arrsize(bootargDebug),
    bootargBeta,
    arrsize(bootargBeta),
    KernelVersion::Ventura,
    KernelVersion::Ventura,
    []() {
        ece.init();
    }
};
