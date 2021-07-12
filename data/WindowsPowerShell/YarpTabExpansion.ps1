# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

## helper for yarp exe
function script:getYarpPorts($filter)
{
    $portList = @()
    $portList+= yarp name list 2> $null|
            where {$_ -like "registration*"} |
            % {$tmp=$_.split(); write $tmp[2] }|
            where  { $_ -like "$filter*" }

    $portList | sort
}

function script:getYarpCarriers($filter)
{
    $carrierList = @()
    $carrierList+= yarp connect --list-carriers 2> $null | %{$_.split()} |
            where  { $_ -like "$filter*" }
    $carrierList | sort
}

function script:getYarpPlugins($filter)
{
    $carrierList = @()
    $carrierList+= yarp plugin --list 2> $null | %{$_.split()} |
            where  { $_ -like "$filter*" }
    $carrierList | sort
}

$global:ops = @{
    yarprun = '--on', '--server'
    yarprun1 = '--cmd', '--as', '--geometry', '--hold', '--stdio', '--workdir'
    yarprun2 = '--kill', '--sigterm', '--sigtermall', '--ps', '--isrunning', '--exit'
    yarpconfig = 'context', 'robot', '--help', '--namespace', '--nameserver', '--version'
    yarpcontext = '--where', '--import', '--remove', '--diff', '--diff-list', '--merge', '--list', '--help', '--import-all'
    yarpplugin = '--help', '--list', '--all', '--search-path'
}

function script:getYarpRunTags($server, $filter)
{
    $tagList=@()
    [string[]] $outputMsg=& yarp run --on $server --ps 2>&1
    $tagList += $outputMsg | where {$_  -match "\(pid"} | % { $_ -match "\(tag (\S*)\)"} | %{$matches[1]}
    $tagList | sort
}

function script:yarpConnect($wholeBlock, $filter) {
    $tmp=$wholeBlock.split()
    $ind=$tmp.length-1
    if ($ind -le 3 )
    {
        getYarpPorts($filter)
    }
    else
    {
        getYarpCarriers($filter)
    }
}

function script:yarpDisconnect($wholeBlock, $filter) {
    $cmdList = @()
    $tmp=$wholeBlock.split()
    $ind=$tmp.length-1
    if ($ind -le 3 )
    {
        getYarpPorts($filter)
    }
    $cmdList | sort
}

function script:yarpCommands($filter) {
    $cmdList = @()
    $cmdList += yarp help 2> $null |
            Select -skip 3 |
            ForEach-object { $_.SubString(0, $_.IndexOf(' '))} |
            where { $_ -like "$filter*" }
    $cmdList | sort
}

function script:yarpRunExpansion($wholeBlock, $lastWord)
{
    $cmdList = @()
    $tmp=$wholeBlock.split()
    $ind=$tmp.length-1
    switch -regex ($wholeBlock)
    {
        'run(.exe)? (\S*)$'
        {
            $cmdList+= $ops.yarprun |
            where { $_ -like "$lastWord*" }
        }
        'run(.exe)? --(on|stdio) (\S*)$'
        {
            getYarpPorts($lastWord)
            break;
        }
        '--on (\S+) (\S*)$'
        {
            $cmdList+=$ops.yarprun1 + $ops.yarprun2|
            where { $_ -like "$lastWord*" }

        }
        'run(.exe)? --(on|stdio) (\S+) --(kill|sigterm|isrunning) (\S*)$'
        {
            getYarpRunTags $matches[3] $lastword
            break
        }

        '--(cmd|geometry|stdio|as|workdir) (\S+) (\S*)$'
        {
            $cmdList+=$ops.yarprun1 |
            where { $_ -like "$lastWord*" }
        }
        '--hold (\S*)$'
        {
            $cmdList+=$ops.yarprun1 |
            where { $_ -like "$lastWord*" }
        }

    }
      $cmdList | sort
}

function script:yarpPlugin($wholeBlock, $filter) {
    $cmdList = @()
    $tmp=$wholeBlock.split()
    $ind=$tmp.length-1
    if ($ind -le 3 )
    {
        getYarpPlugins($filter)
    }
    $cmdList | sort
    $cmdList+=$ops.yarpplugin |
    where { $_ -like "$filter*" }
}

## helpers for yarp-config helpers:
#yarp-config context
function script:GetYarpAllContexts($filter) {
    $results=@()
    $results+= yarp-config context --list 2> $null | where {$_ -match "^[^\*]" }  | get-unique | where { $_ -like "$filter*" }
    $results | sort
}

function script:GetYarpUserContexts($filter) {
    $results=@()
    $results+= yarp-config context --list --user 2> $null | where {$_ -match "^[^\*]" }  | get-unique | where { $_ -like "$filter*" }
    $results | sort
}

function script:GetYarpInstalledContexts($filter) {
    $results=@()
    $results+= yarp-config context --list --installed 2> $null | where {$_ -match "^[^\*]" } | get-unique | where { $_ -like "$filter*" }
    $results | sort
}

function script:getFilesForImportContexts ($context, $filter)
{
    $results=@()
    $results+=yarp-config context --where $context --installed 2> $null | foreach-object {$path=$_ ; ls -r $path} |
                % { $_.FullName.substring($path.length+1) + $(if($_.PsIsContainer){'\'}) } |
                where { $_ -like "$filter*" }
    $results | get-unique | sort
}

function script:getFilesForRemoveContexts ($context, $filter)
{
    $results=@()
    $results+=yarp-config context --where $context --user 2> $null | foreach-object {$path=$_ ; ls -r $path} |
                % { $_.FullName.substring($path.length+1) + $(if($_.PsIsContainer){'\'}) } |
                where { $_ -like "$filter*" }
    $results | get-unique | sort
}

#yarp-config robot
function script:GetYarpAllRobots($filter) {
    $results=@()
    $results+= yarp-config robot --list 2> $null | where {$_ -match "^[^\*]" }  | get-unique | where { $_ -like "$filter*" }
    $results | sort
}

function script:GetYarpUserRobots($filter) {
    $results=@()
    $results+= yarp-config robot --list --user 2> $null | where {$_ -match "^[^\*]" }  | get-unique | where { $_ -like "$filter*" }
    $results | sort
}

function script:GetYarpInstalledRobots($filter) {
    $results=@()
    $results+= yarp-config robot --list --installed 2> $null | where {$_ -match "^[^\*]" } | get-unique | where { $_ -like "$filter*" }
    $results | sort
}

function script:getFilesForImportRobots ($context, $filter)
{
    $results=@()
    $results+=yarp-config robot --where $context --installed 2> $null | foreach-object {$path=$_ ; ls -r $path} |
                % { $_.FullName.substring($path.length+1) + $(if($_.PsIsContainer){'\'}) } |
                where { $_ -like "$filter*" }
    $results | get-unique | sort
}

function script:getFilesForRemoveRobots ($context, $filter)
{
    $results=@()
    $results+=yarp-config robot --where $context --user 2> $null | foreach-object {$path=$_ ; ls -r $path} |
                % { $_.FullName.substring($path.length+1) + $(if($_.PsIsContainer){'\'}) } |
                where { $_ -like "$filter*" }
    $results | get-unique | sort
}
#
function script:yarpconfig($lastBlock, $lastWord)
{
    switch -regex ($lastBlock) {

###### CONTEXT
    '^yarp-config(.exe?) context --import (\S*)'
    {
        $tmp=$lastBlock.split()
        $ind=$tmp.length -1
        if ($ind -le 3 )
        {
            getYarpInstalledContexts $lastWord
        }
        else
        {
            getFilesForImportContexts $tmp[3] $lastWord
        }
        break
    }
    '^yarp-config(.exe?) context --(remove|merge) (\S*)'
    {
        $tmp=$lastBlock.split()
        $ind=$tmp.length -1
        if ($ind -le 3 )
        {
            getYarpUserContexts $lastWord
        }
        else
        {
            getFilesForRemoveContexts $tmp[3] $lastWord
        }
        break
    }
    'yarp-config(.exe?) context --diff (\S*)$'
    {
        getYarpUserContexts $lastWord
        break
    }

    'yarp-config(.exe?) context --where (\S*)$'
    {
        getYarpAllContexts $lastWord
        break
    }

    'yarp-config(.exe?) context --list (\S*)$'
    {
        $results = '--user', '--sysadm', '--installed' |
                where { $_ -like "$lastWord*" }
        $results | sort
        break
    }

    'yarp-config(.exe?) context (\S*)$'
    {
        $ops.yarpcontext |
        where { $_ -like "$lastWord*" }
        break
    }

    ###### ROBOT
    '^yarp-config(.exe?) robot --import (\S*)'
    {
        $tmp=$lastBlock.split()
        $ind=$tmp.length -1
        if ($ind -le 3 )
        {getYarpInstalledRobots $lastWord }
        else
        {
        getFilesForImportRobots $tmp[3] $lastWord
        }
        break
    }
    '^yarp-config(.exe?) robot --(remove|merge) (\S*)'
    {
        $tmp=$lastBlock.split()
        $ind=$tmp.length -1
        if ($ind -le 3 )
        {getYarpUserContexts $lastWord }
        else
        {
        getFilesForRemoveRobots $tmp[3] $lastWord
        }
        break
    }
    'yarp-config(.exe?) robot --diff (\S*)$'
    {
        getYarpUserRobots $lastWord
        break
    }

    'yarp-config(.exe?) robot --where (\S*)$'
    {
        getYarpAllRobots $lastWord
        break
    }

    'yarp-config(.exe?) robot --list (\S*)$'
    {
        $results = '--user', '--sysadm', '--installed' |
                where { $_ -like "$lastWord*" }
        $results | sort
        break
    }

    'yarp-config(.exe?) robot (\S*)$'
    {
        $ops.yarpcontext + '--current' |
        where { $_ -like "$lastWord*" }
        break
    }
    ######### MAIN
    'yarp-config(.exe?) (\S*)$'
    {
        $ops.yarpconfig |
        where { $_ -like "$lastWord*" }
    }
}
}
function YarpTabExpansion($lastBlock, $lastWord) {

    switch -regex ($lastBlock) {
        'yarp(.exe)? connect (.*)$'
        {
            yarpConnect $lastBlock $lastWord
            break;
        }
        'yarp(.exe)? disconnect (.*)$'
        {
            yarpDisconnect $lastBlock $lastWord
            break;
        }
        'yarp(.exe)? (exists|ping|rpc|terminate|wait) (\S*)$'
        {
            getYarpPorts($lastWord)
            break
        }
        'yarp(.exe)? (read|write) (\S+) (\S*)$'
        {
            getYarpPorts($lastWord)
            break
        }
        'yarp(.exe)? detect (\S*)$'
        {
            write '--write' |
             where { $_ -like "$lastWord*" }
            break
        }
        'yarp(.exe)? name (\S*)$'
        {
            write '--list' |
            where { $_ -like "$lastWord*" }
            break;
        }
        'yarp(.exe)? conf (\S*)$'
        {
            write '--clean' |
            where { $_ -like "$lastWord*" }
            break
        }
        'yarp(.exe)? plugin (\S*)$'
        {
            yarpPlugin $lastBlock $lastWord
            break
        }
        # Handles yarp <cmd>
        'yarp(.exe)? (\S*)$'
        {
            yarpCommands $lastWord
            break
        }
        '(yarprun(.exe)? (\S*))|(yarp(.exe)? run (\S*))'
        {
            yarprunExpansion $lastBlock $lastWord
            break
        }
        'yarp-config(.exe?) (.*)$'
        {
           yarpconfig $lastBlock $lastWord
           break
        }
    }
}
