#!/sbin/sh
#
# Copyright (C) 2016 CyanogenMod Project
#
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

RAW_ID=`cat /sys/devices/system/soc/soc0/raw_id`

echo "Mounting system"
mkdir /system_root
mount -t ext4 /dev/block/platform/msm_sdcc.1/by-name/system /system_root -o rw,discard

if [ $RAW_ID == 1974 ] || [ $RAW_ID == 1972 ]; then
    # Remove NFC
    rm -rf /system_root/system/app/NfcNci
    rm -rf /system_root/system/priv-app/Tag
    rm -rf /system_root/system/lib/*nfc*
    rm -rf /system_root/system/etc/*nfc*
    rm -rf /system_root/system/vendor/etc/permissions/*nfc*
    rm -rf /system_root/system/vendor/firmware/bcm2079x-b5_firmware.ncd
    rm -rf /system_root/system/vendor/firmware/bcm2079x-b5_pre_firmware.ncd
    rm -rf /system_root/system/vendor/lib/hw/android.hardware.nfc@1.0-impl-bcm.so
    # Use Mi4 audio configs
    rm -f /system_root/system/etc/acdbdata/MTP/MTP_Speaker_cal.acdb
    mv /system_root/system/etc/acdbdata/MTP/MTP_Speaker_cal_4.acdb /system_root/system/etc/acdbdata/MTP/MTP_Speaker_cal.acdb
    rm -f /system_root/system/vendor/etc/mixer_paths.xml
    mv /system_root/system/vendor/etc/mixer_paths_4.xml /system_root/system/vendor/etc/mixer_paths.xml
    # Mi4 libdirac config
    rm -f /system_root/system/vendor/etc/diracmobile.config
    mv /system_root/system/vendor/etc/diracmobile_4.config /system_root/system/vendor/etc/diracmobile.config
else
    # Remove Mi4 consumerir support
    rm -rf /system_root/system/vendor/etc/permissions/android.hardware.consumerir.xml
    rm -rf /system_root/system/vendor/bin/hw/android.hardware.ir@1.0-service.cancro
    rm -rf /system_root/system/vendor/etc/init/android.hardware.ir@1.0-service.cancro.rc
    rm -rf /system_root/system/vendor/etc/vintf/manifest/android.hardware.ir@1.0-service.cancro.xml
    # Remove Mi4 audio configs
    rm -rf /system_root/system/etc/acdbdata/MTP/MTP_Speaker_cal_4.acdb
    rm -f /system_root/system/vendor/etc/mixer_paths_4.xml
    # Remove Mi4 libdirac config
    rm -f /system_root/system/vendor/etc/diracmobile_4.config
fi

umount /system_root

if [ $RAW_ID == 1978 ] || [ $RAW_ID == 1972 ] || [ $RAW_ID == 1974 ]; then
    # Supported device (Mi3w - 1978 or Mi4 - 1972, 1974)
    exit 0
else
    # Unsupported device
    exit 1
fi
