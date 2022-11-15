#ifndef kern_ece_hpp
#define kern_ece_hpp

#include <Headers/kern_patcher.hpp>

static const char *pathAirportItlwm[] { "/System/Library/Extensions/AirportItlwm.kext/Contents/MacOS/AirPortItlwm" };
static const char *pathAirPortBrcmNIC[]  { "/System/Library/Extensions/IO80211FamilyLegacy.kext/Contents/PlugIns/AirPortBrcmNIC.kext/Contents/MacOS/AirPortBrcmNIC" };
static const char *pathIO80211FamilyLegacy[]  { "/System/Library/Extensions/IO80211FamilyLegacy.kext/Contents/MacOS/IO80211FamilyLegacy" };

static KernelPatcher::KextInfo kextList[] {
    { "com.zxystd.AirportItlwm", pathAirportItlwm, arrsize(pathAirportItlwm), {true}, {}, KernelPatcher::KextInfo::Unloaded },
    { "com.apple.driver.AirPort.BrcmNIC", pathAirPortBrcmNIC, arrsize(pathAirPortBrcmNIC), {true}, {}, KernelPatcher::KextInfo::Unloaded },
    { "com.apple.iokit.IO80211FamilyLegacy", pathIO80211FamilyLegacy, arrsize(pathIO80211FamilyLegacy), {true}, {}, KernelPatcher::KextInfo::Unloaded }
};

class IO80211Interface;
class IO80211Controller;

class AirPortDebugPatcher {
public:
    void init();
    void deinit();
    
private:
    void processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t addres, size_t size);
    
    static SInt32 patched_apple80211Request(IO80211Controller* self,
                                            unsigned int request_type,
                                         int request_number,
                                         IO80211Interface *interface,
                                     void *data);
    static void postMessage(IO80211Interface* self, unsigned int, void* data = NULL, unsigned long dataLen = 0);
};

#endif //kern_ece_hpp
