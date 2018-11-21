#!/usr/bin/env python

import os, sys, fnmatch
from subprocess import call

def find_files(directory, pattern):
    for root, dirs, files in os.walk(directory):
        for basename in files:
            if fnmatch.fnmatch(basename, pattern):
                filename = os.path.join(root, basename)
                yield filename

metadata = {
  "Arduino_Yun.ino"             : { "fqbn": "arduino:avr:yun" },
  "Arduino_Zero_M0_Serial.ino"  : { "fqbn": "arduino:samd:arduino_zero_native" },
  "Arduino_Due.ino"             : { "fqbn": "arduino:sam:arduino_due_x" },
  "ENC28J60.ino"                : { "fqbn": "arduino:avr:nano:cpu=atmega328" },
  "ESP8266_Shield_HardSer.ino"  : { "fqbn": "arduino:avr:mega:cpu=atmega2560" },0
  "ESP8266_Shield_SoftSer.ino"  : { },
  "RN_XV_WiFly.ino"             : { "fqbn": "arduino:avr:leonardo" },
  "Serial_HM10_HC08.ino"        : { "fqbn": "arduino:avr:leonardo" },
  "Seeed_EthernetV2_0.ino"      : { "fqbn": "arduino:avr:uno" },

  # ESP8266
  "ESP8266_DirectConnect.ino"   : { "fqbn": "esp8266:esp8266:nodemcuv2" },
  "ESP8266_Standalone.ino"      : { "fqbn": "esp8266:esp8266:nodemcuv2" },
  "ESP8266_Standalone_SmartConfig.ino"  : { "fqbn": "esp8266:esp8266:nodemcuv2" },
  "ESP8266_Standalone_SSL.ino"  : { "fqbn": "esp8266:esp8266:nodemcuv2" },

  # Digistump
  "Digistump_Digispark.ino"     : { "fqbn": "digistump:avr:digispark-tiny" },
  "Digistump_Digispark_Pro.ino" : { "fqbn": "digistump:avr:digispark-pro" },
  "Digistump_Oak.ino"           : { "fqbn": "digistump:oak:oak1" },

  # Intel
  "Intel_Edison_WiFi.ino"       : { "fqbn": "Intel:i686:izmir_ec" },
  "Intel_Galileo.ino"           : { "fqbn": "Intel:i586:izmir_fg" },
  "Arduino_101_BLE.ino"         : { "fqbn": "Intel:arc32:arduino_101" },

  # RBL
  "RedBear_Duo_WiFi.ino"        : { "fqbn": "RedBear:STM32F2:RedBear_Duo" },
  "RedBear_Duo_BLE.ino"         : { "fqbn": "RedBear:STM32F2:RedBear_Duo" },
  "RedBearLab_BLE_Nano.ino"     : { "fqbn": "RedBearLab:nRF51822:nRF51822_NANO_32KB" },
  "RedBearLab_BlendMicro"       : { "fqbn": "RedBearLab:avr:blendmicro8" },

  #Other
  "Simblee_BLE.ino"             : { "fqbn": "Simblee:Simblee:Simblee" },
  "TinyDuino_WiFi.ino"          : { "fqbn": "arduino:avr:pro:cpu=8MHzatmega328" },
  "WildFire.ino"                : { "fqbn": "WickedDevice:avr:wildfireo3" },
  "chipKIT_Uno32.ino"           : { "fqbn": "chipKIT:pic32:uno_pic32" },
  "LightBlue_Bean.ino"          : { "fqbn": "bean:avr:lightblue-bean" },
  "Teensy3.ino"                 : { "fqbn": "teensy:avr:teensy31" },

  # Special examples
  "ESP8266_ReadPin.ino"         : { "fqbn": "esp8266:esp8266:nodemcuv2" },
  "ESP8266_WritePin.ino"        : { "fqbn": "esp8266:esp8266:nodemcuv2" },
  "ThingSpeak.ino"              : { "fqbn": "esp8266:esp8266:nodemcuv2" },

  # No linux support
  "LinkItONE.ino"               : { "skip": True },
  "Arduino_Ethernet2.ino"       : { "skip": True }, # Arduino.org?

  # Energia
  "Energia_WiFi.ino"            : { "skip": True },
  "TI_MSP430F5529_CC3100.ino"   : { "skip": True },
  "RedBearLab_CC3200.ino"       : { "skip": True },
  "RedBearLab_WiFi_Mini.ino"    : { "skip": True },
  "TI_CC3200_LaunchXL.ino"      : { "skip": True },
  "TI_Stellaris_LaunchPad.ino"  : { "skip": True },
  "TI_TivaC_Connected.ino"      : { "skip": True },
}

examples = find_files('examples', '*.ino')
abs_examples = map(lambda x: os.path.abspath(x), examples)

logfile = open("./build.log","wb")

os.chdir("/data2/arduino-1.6.8/")
'''
builder = "./arduino-builder"

builder_args = [
  "-verbose",

  "-hardware", "./hardware",
  "-hardware", os.path.expanduser("~/.arduino15/packages/"),
  "-hardware", "/data2/sketchbook/hardware",

  "-tools",    "./hardware/tools",
  "-tools",    os.path.expanduser("~/.arduino15/packages/"),
  "-tools",    "./tools-builder",

  "-libraries",os.path.expanduser("~/.arduino15/packages/"),
  "-libraries","./libraries",
  "-libraries","/data2/sketchbook/libraries/",
]
'''

failed = []
built = []
skipped = []

for fn in abs_examples:
    path, ino = os.path.split(fn)

    if len(sys.argv) > 1:
        if ino != sys.argv[1]:
            continue

    if ino in metadata:
        m = metadata[ino]
        if "skip" in m:
            skipped.append(ino)
            continue
        if not "fqbn" in m:
            m["fqbn"] = "arduino:avr:uno"
    else:
        #continue
        m = { "fqbn": "arduino:avr:uno" }

    print >>logfile, "\n\n", "================="
    print >>logfile, "Building:", ino
    print >>logfile, "=================", "\n"
    logfile.flush()
    
    print "Building:", ino, "...", 
    sys.stdout.flush()
    
    #cmd = [builder] + builder_args + ["-fqbn", m["fqbn"]] + [fn]
    cmd = [
        "./arduino",
        "--verbose",
        "--verify",
        "--board", m["fqbn"],
        fn
    ]
    #print cmd
    rc = call(cmd, stdout=logfile, stderr=logfile)
    if rc:
        print "Failed"
        failed.append(ino)
    else:
        print "OK"
        built.append(ino)

print "=================="
if len(failed):
    print " Failed:", failed
    sys.exit(1)
else:    
    print " All", len(built), "examples OK"
