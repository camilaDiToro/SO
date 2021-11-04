#!/bin/bash
if [[ "$1" = "gdb" ]]; then
    qemu-system-x86_64 --rtc base=localtime -S  -s -hda Image/x64BareBonesImage.qcow2 -m 512 #-d int
else
    qemu-system-x86_64 --rtc base=localtime  -hda Image/x64BareBonesImage.qcow2 -m 512
fi
