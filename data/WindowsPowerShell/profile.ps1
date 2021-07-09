# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

Push-Location (Split-Path -Path $MyInvocation.MyCommand.Definition -Parent)

# Load YarpTabExpansion module from current directory
Import-Module .\YarpTabExpansion.ps1

if(-not (Test-Path Function:\DefaultTabExpansion)) {
    if(Test-Path  Function:\TabExpansion )
    {
        Rename-Item Function:\TabExpansion DefaultTabExpansion
    }
}

# Set up tab expansion and include yarp expansion
function TabExpansion ($line, $lastWord) {
    $lastBlock = [regex]::Split($line, '[|;]')[-1]
    switch -regex($lastBlock) {
        # Execute yarp tab completion for all yarp-related commands
        'yarp(server|-config|run)?(.exe)? (.*)' {
        YarpTabExpansion $lastBlock $lastWord }
        # Fall back on existing tab expansion
        default {
        DefaultTabExpansion $line $lastWord }
    }
}

Pop-Location
