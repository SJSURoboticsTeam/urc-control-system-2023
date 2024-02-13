#!/bin/bash

subsystems=("drive" "arm")
for subsystem in ${subsystems[@]}; do
    clang-format -i $subsystem/applications/*
    clang-format -i $subsystem/dto/*
    clang-format -i $subsystem/implementations/*
    clang-format -i $subsystem/include/*
    clang-format -i $subsystem/platform-implementations/*
    clang-format -i $subsystem/platforms/*
done