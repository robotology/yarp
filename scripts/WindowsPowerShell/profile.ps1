# Copyright: (C) 2013 Istituto Italiano di Tecnologia
# Author: Elena Ceseracciu <elena.ceseracciu@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


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
