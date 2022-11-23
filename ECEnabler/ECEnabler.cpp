#include <Headers/kern_api.hpp>

#include "ECEnabler.hpp"

#include <IOKit/80211/apple80211_ioctl.h>
#include <IOKit/80211/IO80211Interface.h>

#define MODULE_SHORT "airportdebug"


static AirPortDebugPatcher *callbackECE = nullptr;

void AirPortDebugPatcher::init() {
    callbackECE = this;
    
    lilu.onKextLoadForce(kextList, arrsize(kextList),
    [](void *user, KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
        callbackECE->processKext(patcher, index, address, size);
    }, this);
}

void AirPortDebugPatcher::deinit()
{
}

static mach_vm_address_t orig_postMessage {};
static mach_vm_address_t orig_apple80211Request {};

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

static const char* IOCTL_NAMES[] = {
     "UNKNOWN",
     "SSID",
     "AUTH_TYPE",
     "CIPHER_KEY",
     "CHANNEL",
     "POWERSAVE",
     "PROTMODE",
     "TXPOWER",
     "RATE",
     "BSSID",
     "SCAN_REQ",
     "SCAN_RESULT",
     "CARD_CAPABILITIES",
     "STATE",
     "PHY_MODE",
     "OP_MODE",
     "RSSI",
     "NOISE",
     "INT_MIT",
     "POWER",
     "ASSOCIATE",
     "ASSOCIATE_RESULT",
     "DISASSOCIATE",
     "STATUS_DEV_NAME",
     "IBSS_MODE",
     "HOST_AP_MODE",
     "AP_MODE",
     "SUPPORTED_CHANNELS",
     "LOCALE",
     "DEAUTH",
     "COUNTERMEASURES",
     "FRAG_THRESHOLD",
     "RATE_SET",
     "SHORT_SLOT",
     "MULTICAST_RATE",
     "SHORT_RETRY_LIMIT",
     "LONG_RETRY_LIMIT",
     "TX_ANTENNA",
     "RX_ANTENNA",
     "ANTENNA_DIVERSITY",
     "ROM",
     "DTIM_INT",
     "STATION_LIST",
     "DRIVER_VERSION",
     "HARDWARE_VERSION",
     "RAND",
     "RSN_IE",
     "BACKGROUND_SCAN",
     "AP_IE_LIST",
     "STATS",
     "ASSOCIATION_STATUS",
     "COUNTRY_CODE",
     "DEBUG_FLAGS",
     "LAST_RX_PKT_DATA",
     "RADIO_INFO",
     "GUARD_INTERVAL",
     "MIMO_POWERSAVE",
     "MCS",
     "RIFS",
     "LDPC",
     "MSDU",
     "MPDU",
     "BLOCK_ACK",
     "PLS",
     "PSMP",
     "PHY_SUB_MODE",
     "MCS_INDEX_SET",
     "CACHE_THRESH_BCAST",
     "CACHE_THRESH_DIRECT",
     "WOW_PARAMETERS",
     "WOW_ENABLED",
     "40MHZ_INTOLERANT",
     "PID_LOCK",
     "STA_IE_LIST",
     "STA_AUTHORIZE",
     "STA_DISASSOCIATE",
     "STA_DEAUTH",
     "RSN_CONF",
     "KEY_RSC",
     "STA_STATS",
     "ROAM_THRESH",
     "VENDOR_DBG_FLAGS",
     "CACHE_AGE_THRESH",
     "PMK_CACHE",
     "LINK_QUAL_EVENT_PARAMS",
     "IE",
     "SCAN_REQ_MULTIPLE",
     "BTCOEX_MODE",
     "WOW_TEST",
     "CLEAR_PMK_CACHE",
     "SCANCACHE_CLEAR",
     "P2P_ENABLE",
     "P2P_LISTEN",
     "P2P_SCAN",
     "VIRTUAL_IF_CREATE",
     "VIRTUAL_IF_DELETE",
     "VIRTUAL_IF_ROLE",
     "VIRTUAL_IF_PARENT",
     "P2P_GO_CONF",
     "P2P_NOA_LIST",
     "P2P_OPP_PS",
     "P2P_CT_WINDOW",
     "BT_COEX_FLAGS",
     "CURRENT_NETWORK",
     "BT_POWER",
     "AVAILABILITY",
     "RSSI_BOUNDS",
     "ROAM",
     "TX_CHAIN_POWER",
     "CDD_MODE",
     "LAST_BCAST_SCAN_TIME",
     "THERMAL_THROTTLING",
     "FACTORY_MODE",
     "REASSOCIATE",
     "???MISSING???",
     "POWER_DEBUG_INFO",
     "AWDL_SYNC_PARAMS",
     "AWDL_SYNC_ENABLED",
     "AWDL_EXTENSION_STATE_MACHINE_PARAMETERS",
     "AWDL_SERVICE_PARAMS",
     "AWDL_PEER_SERVICE_REQUEST",
     "AWDL_ELECTION_ALGORITHM_ENABLED",
     "AWDL_ELECTION_ID",
     "AWDL_MAX_TREE_DEPTH",
     "AWDL_GUARD_TIME",
     "AWDL_BSSID",
     "AWDL_ELECTION_METRIC",
     "AWDL_AVAILABILITY_WINDOW_AP_ALIGNMENT",
     "AWDL_SYNC_FRAME_AP_BEACON_ALIGNMENT",
     "AWDL_SYNCHRONIZATION_CHANNEL_SEQUENCE",
     "PEER_CACHE_MAXIMUM_SIZE",
     "AWDL_OUI",
     "AWDL_MASTER_CHANNEL",
     "AWDL_TOP_MASTER",
     "AWDL_SYNC_STATE",
     "AWDL_ELECTION_RSSI_THRESHOLDS",
     "AWDL_PRESENCE_MODE",
     "AWDL_ELECTION_MASTER_COUNTS",
     "AWDL_PERIODIC_SYNC_FRAME_PACKET_LIFETIME",
     "AWDL_MASTER_MODE_SYNC_FRAME_PERIOD",
     "AWDL_NON_ELECTION_MASTER_MODE_SYNC_FRAME_PERIOD",
     "AWDL_EXPLICIT_AVAILABILITY_WINDOW_EXTENSION_OPT_OUT",
     "AWDL_GET_AWDL_MASTER_DATABASE",
     "PEER_CACHE_CONTROL",
     "AWDL_BATTERY_LEVEL",
     "AWDL_BT_COEX_AW_PROTECTED_PERIOD_LENGTH",
     "AWDL_BT_COEX_AGREEMENT",
     "AWDL_BT_COEX_AGREEMENT_ENABLED",
     "AWDL_STRATEGY",
     "AWDL_OOB_REQUEST",
     "AWDL_MAX_NO_MASTER_PERIODS",
     "AWDL_SYNC_FRAME_TEMPLATE",
     "LOG_FLAGS",
     "PEER_STATS",
     "HT_CAPABILITY",
     "AWDL_ELECTION_PARAMS",
     "LINK_CHANGED_EVENT_DATA",
     "GET_DEBUG_INFO",
     "AWDL_DEVICE_CAPABILITIES",
     "AWDL_RSSI_MEASUREMENT_REQUEST",
     "AWDL_AES_KEY",
     "AWDL_SCAN_RESERVED_TIME",
     "AWDL_CTL",
     "AWDL_SOCIAL_TIME_SLOTS",
     "AWDL_PEER_TRAFFIC_REGISTRATION",
     "EXTENDED_STATS",
     "BEACON_PERIOD",
     "AWDL_FORCED_ROAM_CONFIG",
     "AWDL_QUIET",
     "ACL_POLICY",
     "ACL_ADD",
     "ACL_REMOVE",
     "ACL_FLUSH",
     "ACL_LIST",
     "CHAIN_ACK",
     "DESENSE",
     "OFFLOAD_SCANNING",
     "OFFLOAD_RSN",
     "OFFLOAD_COUNTRY_CODE",
     "OFFLOAD_KEEPALIVE_L2",
     "OFFLOAD_ARP_NDP",
     "VHT_MCS_INDEX_SET",
     "DWDS",
     "INTERRUPT_STATS",
     "INTERRUPT_STATS_RESET",
     "TIMER_STATS",
     "TIMER_STATS_RESET",
     "OFFLOAD_STATS",
     "OFFLOAD_STATS_RESET",
     "OFFLOAD_BEACONS",
     "ROAMING",
     "OFFLOAD_ARP",
     "OFFLOAD_NDP",
     "OFFLOAD_SCAN",
     "DESENSE_LEVEL",
     "MCS_VHT",
     "TX_NSS",
     "GAS_REQ",
     "GAS_START",
     "GAS_SET_PEER",
     "GAS_RESULTS",
     "AWDL_BTLE_PEER_INDICATION",
     "AWDL_BTLE_STATE_PARAMS",
     "AWDL_PEER_DATABASE",
     "AWDL_BTLE_ENABLE_SYNC_WITH_PARAMS",
     "AWDL_SECONDARY_MASTER_CHANNEL",
     "PHY_STATS",
     "CHANNELS_INFO",
     "AWDL_AF_TX_MODE",
     "ERROR_STRING",
     "ERROR_NO",
     "AWDL_PIGGYBACK_SCAN_REQ",
     "AWDL_PRIVATE_ELECTION_ID",
     "AWDL_MIN_RATE",
     "VHT_CAPABILITY",
     "BGSCAN_CACHE_RESULTS",
     "ROAM_PROFILE",
     "AWDL_OPER_MODE",
     "RESTORE_DEFAULTS",
     "AWDL_ENCRYPTION_KEYS",
     "AWDL_ENCRYPTION_TYPE",
     "BTCOEX_PROFILES",
     "BTCOEX_CONFIG",
     "AWDL_STATISTICS",
     "AWDL_ENABLE_ROAMING",
     "AWDL_OOB_AUTO_REQUEST",
     "AWDL_TXCAL_PERIOD",
     "CHIP_COUNTER_STATS",
     "DBG_GUARD_TIME_PARAMS",
     "AWDL_AWDL_ADVERTISERS",
     "LEAKY_AP_STATS_MODE",
     "CAPTURE",
     "LEAKY_AP_STATS",
     "AWDL_BLOCK_SET_COMMANDS",
     "LEAKY_AP_AWD_MODE",
     "BTCOEX_OPTIONS",
     "FORCE_SYNC_TO_PEER",
     "COUNTRY_CHANNELS",
     "PRIVATE_MAC",
     "RESET_CHIP",
     "CRASH",
     "RANGING_ENABLE",
     "RANGING_START",
     "RANGING_AUTHENTICATE",
     "AWDL_PREFERRED_CHANNELS",
     "LEAKY_AP_SSID_STATS",
     "AWDL_RSDB_CAPS",
     "AWDL_DEV_STATS",
     "LAST_ASSOC_HISTORY",
     "AWDL_COMMON_CHANNEL",
     "AWDL_PEERS_INFO",
     "TKO_PARAMS",
     "TKO_DUMP",
     "AWDL_NEARBY_LOG_TRIGGER",
     "HW_SUPPORTED_CHANNELS",
     "BTCOEX_PROFILE",
     "BTCOEX_PROFILE_ACTIVE",
     "TRAP_INFO",
     "THERMAL_INDEX",
     "MAX_NSS_FOR_AP",
     "BTCOEX_2G_CHAIN_DISABLE",
     "POWER_BUDGET",
     "AWDL_DFSP_CONFIG",
     "AWDL_DFSP_UCSA_CONFIG",
     "SCAN_BACKOFF_REPORT",
     "OFFLOAD_TCPKA_ENABLE",
     "RANGING_CAPS",
     "PER_CORE_RSSI_REPORT/SUPPRESS_SCANS",
};

static char* bytesToStr(uint8_t *bytes, size_t len) {
    char* s = (char*)IOMalloc(len * 3 + 1);
    char* p = s;
    for (size_t i = 0; i < len; i++) {
        sprintf(p, "%02x ", bytes[i]);
        p += 3;
    }
    *p = 0;
    return s;
}

static const char* ether_sprintf(const uint8_t *ap) {
     static char etherbuf[18];
     snprintf(etherbuf, sizeof (etherbuf), "%6D", ap, ":");
     return (etherbuf);
}

void AirPortDebugPatcher::postMessage(IO80211Interface *interface, unsigned int message, void* data, unsigned long dataLen) {
    char* dataStr = bytesToStr((uint8_t*)data, (size_t)dataLen);
    SYSLOG(MODULE_SHORT, "Posting message %d with data (length %d) %s", message, dataLen, dataStr);
    IOFree(dataStr, dataLen * 3 + 1);
    FunctionCast(postMessage, orig_postMessage)(interface, message, data, dataLen);
}

static void printScanReq(struct apple80211_scan_data *sd) {
    SYSLOG(MODULE_SHORT, "%s Type: %u BSS Type: %u PHY Mode: %u Dwell time: %u Rest time: %u Num channels: %u SSID: %s BSSID: %s",
           __FUNCTION__,
           sd->scan_type,
           sd->bss_type,
           sd->phy_mode,
           sd->dwell_time,
           sd->rest_time,
           sd->num_channels,
           sd->ssid,
           ether_sprintf(sd->bssid.octet));
}

static void printScanReqMultiple(struct apple80211_scan_multiple_data *sd) {
    int i;
    SYSLOG(MODULE_SHORT, "%s Type: %u SSID Count: %u BSSID Count: %u PHY Mode: %u Dwell time: %u Rest time: %u Num channels: %u Unk: %u",
          __FUNCTION__,
          sd->scan_type,
          sd->ssid_count,
          sd->bssid_count,
          sd->phy_mode,
          sd->dwell_time,
          sd->rest_time,
          sd->num_channels,
          sd->unk_2);
    for (i = 0; i < sd->ssid_count; i++) {
        SYSLOG(MODULE_SHORT, "%s index=%d ssid=%s ssid_len=%d", __FUNCTION__, i, sd->ssids[i].ssid_bytes, sd->ssids[i].ssid_len);
    }
}

static void printScanResult(struct apple80211_scan_result *result) {
    SYSLOG(MODULE_SHORT, "%s version=%d channel=(%d,%d,%d) unk=%d noise=%d snr=%d rssi=%d beacon_int=%d cap=%d bssid=%s nrates=%d nr_unk=%d ssid_len=%d ssid=%s unk=%d unk2=%d age=%d unk3=%d ie_len=%d unk3=%d", __FUNCTION__, result->version, result->asr_channel.version, result->asr_channel.channel, result->asr_channel.flags, result->asr_unk, result->asr_noise, result->asr_snr, result->asr_rssi, result->asr_beacon_int, result->asr_cap, ether_sprintf(result->asr_bssid), result->asr_nrates, result->asr_nr_unk, result->asr_ssid_len, result->asr_ssid, result->unk, result->unk2, result->asr_age, result->unk3, result->asr_ie_len, result->asr_unk3);
}

SInt32 AirPortDebugPatcher::patched_apple80211Request(IO80211Controller* controller,
                                                      unsigned int request_type,
                                     int request_number,
                                     IO80211Interface *interface,
                                     void *data) {
    if (request_type == SIOCSA80211) { // print SET requests before execution
        switch (request_number) {
            case APPLE80211_IOC_SCAN_REQ:
                printScanReq((struct apple80211_scan_data*)data);
                break;
                
            case APPLE80211_IOC_SCAN_REQ_MULTIPLE:
                printScanReqMultiple((struct apple80211_scan_multiple_data*)data);
                break;
        }
    }
    SInt32 ret = FunctionCast(patched_apple80211Request, orig_apple80211Request)(controller, request_type, request_number, interface, data);
    SYSLOG(MODULE_SHORT, "%s %d (%s) returned %d", request_type == SIOCSA80211 ? "SET" : (request_type == SIOCGA80211 ? "GET" : "UNKNOWN"), request_number, IOCTL_NAMES[request_number >= ARRAY_SIZE(IOCTL_NAMES) ? 0: request_number], ret);
    if (request_type == SIOCGA80211 && ret == 0) { // print results of GET requests after successful execution
        switch (request_number) {
            case APPLE80211_IOC_SCAN_RESULT:
                printScanResult(*((struct apple80211_scan_result**)data));
                break;
        }
    }
    return ret;
}


void AirPortDebugPatcher::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size)
{
    if (kextList[0].loadIndex == index) {
        KernelPatcher::RouteRequest itlwmRoute = KernelPatcher::RouteRequest("__ZN12AirportItlwm17apple80211RequestEjiP16IO80211InterfacePv", patched_apple80211Request, orig_apple80211Request);
        if (patcher.routeMultiple(index, &itlwmRoute, 1, address, size))
            SYSLOG(MODULE_SHORT, "routed __ZN12AirportItlwm17apple80211RequestEjiP16IO80211InterfacePv");
        else
            SYSLOG(MODULE_SHORT, "failed to route __ZN12AirportItlwm17apple80211RequestEjiP16IO80211InterfacePv");
    }
    else if (kextList[1].loadIndex == index) {
        KernelPatcher::RouteRequest brcmApple80211RequestRoute = KernelPatcher::RouteRequest("__ZN15AirPort_BrcmNIC17apple80211RequestEjiP16IO80211InterfacePv", patched_apple80211Request, orig_apple80211Request);
        if (patcher.routeMultiple(index, &brcmApple80211RequestRoute, 1, address, size))
            SYSLOG(MODULE_SHORT, "routed __ZN15AirPort_BrcmNIC17apple80211RequestEjiP16IO80211InterfacePv");
        else
            SYSLOG(MODULE_SHORT, "Failed to route __ZN15AirPort_BrcmNIC17apple80211RequestEjiP16IO80211InterfacePv");
    }
    else if (kextList[2].loadIndex == index) {
        KernelPatcher::RouteRequest postMessageRequestRoute = KernelPatcher::RouteRequest("__ZN16IO80211Interface11postMessageEjPvm", postMessage, orig_postMessage);
        if (patcher.routeMultiple(index, &postMessageRequestRoute, 1, address, size))
            SYSLOG(MODULE_SHORT, "routed __ZN16IO80211Interface11postMessageEjPvm");
        else
            SYSLOG(MODULE_SHORT, "Failed to route __ZN16IO80211Interface11postMessageEjPvm");
    }
}
