#ifndef kern_ece_hpp
#define kern_ece_hpp

#include <Headers/kern_patcher.hpp>

static const char *pathAirportItlwm[] { "/System/Library/Extensions/AirportItlwm.kext/Contents/MacOS/AirPortItlwm" };
static const char *pathAirPortBrcmNIC[]  { "/System/Library/Extensions/IO80211FamilyLegacy.kext/Contents/PlugIns/AirPortBrcmNIC.kext/Contents/MacOS/AirPortBrcmNIC" };

static KernelPatcher::KextInfo kextList[] {
    { "com.zxystd.AirportItlwm", pathAirportItlwm, arrsize(pathAirportItlwm), {true}, {}, KernelPatcher::KextInfo::Unloaded },
    { "com.apple.driver.AirPort.BrcmNIC", pathAirPortBrcmNIC, arrsize(pathAirPortBrcmNIC), {true}, {}, KernelPatcher::KextInfo::Unloaded }
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
};

#endif //kern_ece_hpp
