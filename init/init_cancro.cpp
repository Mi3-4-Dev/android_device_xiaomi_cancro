/*
   Copyright (c) 2015, The Linux Foundation. All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <android-base/logging.h>
#include <android-base/properties.h>

#include "vendor_init.h"
#include "property_service.h"

#define ISMATCH(a,b)    (!strncmp(a,b,PROP_VALUE_MAX))

#define RAW_ID_PATH     "/sys/devices/system/soc/soc0/raw_id"
#define BUF_SIZE         64
static char tmp[BUF_SIZE];

static int read_file2(const char *fname, char *data, int max_size)
{
    int fd, rc;

    if (max_size < 1)
        return 0;

    fd = open(fname, O_RDONLY);
    if (fd < 0) {
        LOG(ERROR) << "failed to open '" << fname << "'\n";
        return 0;
    }

    rc = read(fd, data, max_size - 1);
    if ((rc > 0) && (rc < max_size))
        data[rc] = '\0';
    else
        data[0] = '\0';
    close(fd);

    return 1;
}

void property_override(char const prop[], char const value[], bool add = true)
{
    auto pi = (prop_info *) __system_property_find(prop);

    if (pi != nullptr) {
        __system_property_update(pi, value, strlen(value));
    } else if (add) {
        __system_property_add(prop, strlen(prop), value, strlen(value));
    }
}

void property_override_triple(char const product_prop[], char const system_prop[], char const vendor_prop[], char const value[])
{
    property_override(product_prop, value);
    property_override(system_prop, value);
    property_override(vendor_prop, value);
}

void init_alarm_boot_properties()
{
    char const *alarm_file = "/proc/sys/kernel/boot_reason";
    char buf[BUF_SIZE];
    if(read_file2(alarm_file, buf, sizeof(buf))) {

    /*
     * Setup ro.alarm_boot value to true when it is RTC triggered boot up
     * For existing PMIC chips, the following mapping applies
     * for the value of boot_reason:
     *
     * 0 -> unknown
     * 1 -> hard reset
     * 2 -> sudden momentary power loss (SMPL)
     * 3 -> real time clock (RTC)
     * 4 -> DC charger inserted
     * 5 -> USB charger insertd
     * 6 -> PON1 pin toggled (for secondary PMICs)
     * 7 -> CBLPWR_N pin toggled (for external power supply)
     * 8 -> KPDPWR_N pin toggled (power key pressed)
     */
        if(buf[0] == '3')
            property_override("ro.alarm_boot", "true");
        else
            property_override("ro.alarm_boot", "false");
    }
}

void vendor_load_properties()
{
    int rc;
    unsigned long raw_id = -1;

    /* get raw ID */
    rc = read_file2(RAW_ID_PATH, tmp, sizeof(tmp));
    if (rc) {
        raw_id = strtoul(tmp, NULL, 0);
    }

    property_override_triple("ro.product.device", "ro.product.system.device", "ro.vendor.product.device", "cancro");
    property_override_triple("ro.product.name", "ro.product.system.name", "ro.vendor.product.name", "cancro");
    property_override_triple("ro.build.fingerprint", "ro.system.build.fingerprint", "ro.vendor.build.fingerprint", "Xiaomi/cancro/cancro:6.0.1/MMB29M/V9.5.7.0.MXDMIFA:user/release-keys");
    property_override("ro.build.description", "cancro-user 6.0.1 MMB29M V9.5.7.0.MXDMIFA release-keys");

    switch (raw_id) {
        case 1978:
            property_override_triple("ro.product.model", "ro.product.system.model", "ro.vendor.product.model", "MI 3W");
            property_override("ro.nfc.port", "I2C");
            break;
        case 1974:
            property_override_triple("ro.product.model", "ro.product.system.model", "ro.vendor.product.model", "MI 4");
            break;
        case 1972:
            property_override_triple("ro.product.model", "ro.product.system.model", "ro.vendor.product.model", "MI 4LTE");
            property_override("ro.telephony.default_network", "8");
            property_override("telephony.lteOnGSMDevice", "1");
            break;
        default:
            // Other unsupported variants
            property_override_triple("ro.product.model", "ro.product.system.model", "ro.vendor.product.model", "Unsupported MI Cancro");
            break;
    }

    property_override("rild.libargs", "-d /dev/smd0");
    init_alarm_boot_properties();
}
