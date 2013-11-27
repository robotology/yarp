Push-Location (Split-Path -Path $MyInvocation.MyCommand.Definition -Parent)

# Load posh-git module from current directory
#Import-Module .\posh-git

# If module is installed in a default location ($env:PSModulePath),
# use this instead (see about_Modules for more information):
# Import-Module posh-git
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
        # Execute git tab completion for all git-related commands
        'yarp(server|-config|run)?(.exe)? (.*)' {
		YarpTabExpansion $lastBlock $lastWord }
        # Fall back on existing tab expansion
        default {
		DefaultTabExpansion $line $lastWord }
    }
}

Pop-Location
