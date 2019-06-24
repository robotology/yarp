#!/usr/bin/perl -w

# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

my @files = `git ls-files src example tests scripts cmake bindings CMakeLists.txt`;
s{^\s+|\s+$}{}g foreach @files;

open my $handle, '<', "tests/misc/check_license_skip.txt";
chomp(my @skip_files = <$handle>);
close $handle;


my $copyright_iit = "Copyright \\(C\\) 2006-2019 Istituto Italiano di Tecnologia \\(IIT\\)";
my $copyright_robocub = "Copyright \\(C\\) 2006-2010 RobotCub Consortium";
my $copyright_others = "Copyright \\(C\\) [^\n]+";


my $str_bsd3_cpp = <<END;
^/\\*
 \\* $copyright_iit(
 \\* $copyright_robocub)?(
 \\* $copyright_others)*
 \\* All rights reserved\\.
 \\*
 \\* This software may be modified and distributed under the terms of the
 \\* BSD-3-Clause license\\. See the accompanying LICENSE file for details\\.
 \\*/

END

my $str_bsd3_script_no_sb = <<END;
^(#\\.rst:(
#[^\n]*)+

)?# $copyright_iit(
# $copyright_robocub)?(
# $copyright_others)*
# All rights reserved\\.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license\\. See the accompanying LICENSE file for details\\.

END

my $str_bsd3_script = <<END;
^#!.+

# $copyright_iit(
# $copyright_robocub)?(
# $copyright_others)*
# All rights reserved\\.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license\\. See the accompanying LICENSE file for details\\.

END

my $str_bsd3_xml = <<END;
^<!--
  $copyright_iit(
  $copyright_robocub)?(
  $copyright_others)*
  All rights reserved\\.

  This software may be modified and distributed under the terms of the
  BSD-3-Clause license\\. See the accompanying LICENSE file for details\\.
-->

END

my $str_bsd3_lua = <<END;
^(#!.+

)?-- $copyright_iit(
-- $copyright_robocub)?(
-- $copyright_others)*
-- All rights reserved\\.
--
-- This software may be modified and distributed under the terms of the
-- BSD-3-Clause license\\. See the accompanying LICENSE file for details\\.

END

my $str_bsd3_swig = <<END;
^// $copyright_iit(
// $copyright_robocub)?(
// $copyright_others)*
// All rights reserved\\.
//
// This software may be modified and distributed under the terms of the
// BSD-3-Clause license\\. See the accompanying LICENSE file for details\\.

END

my $str_bsd3_mat = <<END;
^% $copyright_iit(
% $copyright_robocub)?(
% $copyright_others)*
% All rights reserved\\.
%
% This software may be modified and distributed under the terms of the
% BSD-3-Clause license\\. See the accompanying LICENSE file for details\\.

END

my $str_lgpl2_1_or_later = <<END;
^/\\*
 \\* $copyright_iit(
 \\* $copyright_robocub)?
 \\*
 \\* This library is free software; you can redistribute it and/or
 \\* modify it under the terms of the GNU Lesser General Public
 \\* License as published by the Free Software Foundation; either
 \\* version 2\\.1 of the License, or \\(at your option\\) any later version\\.
 \\*
 \\* This library is distributed in the hope that it will be useful,
 \\* but WITHOUT ANY WARRANTY; without even the implied warranty of
 \\* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE\\.  See the GNU
 \\* Lesser General Public License for more details\\.
 \\*
 \\* You should have received a copy of the GNU Lesser General Public
 \\* License along with this library; if not, write to the Free Software
 \\* Foundation, Inc\\., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 \\*/

END

my $str_lgpl2_1_or_later_other = <<END;
^/\\*
 \\* $copyright_iit(
 \\* $copyright_robocub)?(
 \\* $copyright_others)*
 \\*
 \\* This library is free software; you can redistribute it and/or
 \\* modify it under the terms of the GNU Lesser General Public
 \\* License as published by the Free Software Foundation; either
 \\* version 2\\.1 of the License, or \\(at your option\\) any later version\\.
 \\*
 \\* This library is distributed in the hope that it will be useful,
 \\* but WITHOUT ANY WARRANTY; without even the implied warranty of
 \\* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE\\.  See the GNU
 \\* Lesser General Public License for more details\\.
 \\*
 \\* You should have received a copy of the GNU Lesser General Public
 \\* License along with this library; if not, write to the Free Software
 \\* Foundation, Inc\\., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 \\*/

END

my $str_gpl2_or_later = <<END;
^/\\*
 \\* $copyright_iit(
 \\* $copyright_robocub)?
 \\*
 \\* This program is free software; you can redistribute it and/or modify it under
 \\* the terms of the GNU General Public License as published by the Free Software
 \\* Foundation; either version 2 of the License, or \\(at your option\\) any later
 \\* version\\.
 \\*
 \\* This program is distributed in the hope that it will be useful, but WITHOUT
 \\* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 \\* FOR A PARTICULAR PURPOSE\\.  See the GNU General Public License for more
 \\* details\\.
 \\*
 \\* You should have received a copy of the GNU General Public License along with
 \\* this program; if not, write to the Free Software Foundation, Inc\\.,
 \\* 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA\\.
 \\*/

END

my $str_gpl2_or_later_other = <<END;
^/\\*
 \\* $copyright_iit(
 \\* $copyright_robocub)?(
 \\* $copyright_others)*
 \\*
 \\* This program is free software; you can redistribute it and/or modify it under
 \\* the terms of the GNU General Public License as published by the Free Software
 \\* Foundation; either version 2 of the License, or \\(at your option\\) any later
 \\* version\\.
 \\*
 \\* This program is distributed in the hope that it will be useful, but WITHOUT
 \\* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 \\* FOR A PARTICULAR PURPOSE\\.  See the GNU General Public License for more
 \\* details\\.
 \\*
 \\* You should have received a copy of the GNU General Public License along with
 \\* this program; if not, write to the Free Software Foundation, Inc\\.,
 \\* 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA\\.
 \\*/

END

my $str_apache_2 = <<END;
^/\\*
 \\* $copyright_iit
 \\*
 \\* Licensed under the Apache License, Version 2\\.0 \\(the "License"\\);
 \\* you may not use this file except in compliance with the License\\.
 \\* You may obtain a copy of the License at
 \\*
 \\*     http://www\\.apache\\.org/licenses/LICENSE-2\\.0
 \\*
 \\* Unless required by applicable law or agreed to in writing, software
 \\* distributed under the License is distributed on an "AS IS" BASIS,
 \\* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied\\.
 \\* See the License for the specific language governing permissions and
 \\* limitations under the License\\.
 \\*/

END

my $str_lgpl2_1_or_later_lua = <<END;
-- $copyright_iit(
-- $copyright_robocub)?
--
-- This library is free software; you can redistribute it and/or
-- modify it under the terms of the GNU Lesser General Public
-- License as published by the Free Software Foundation; either
-- version 2\\.1 of the License, or \\(at your option\\) any later version\\.
--
-- This library is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE\\.  See the GNU
-- Lesser General Public License for more details\\.
--
-- You should have received a copy of the GNU Lesser General Public
-- License along with this library; if not, write to the Free Software
-- Foundation, Inc\\., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

END

my $str_lgpl2_1_or_later_lua_other = <<END;
-- $copyright_iit(
-- $copyright_robocub)?(
-- $copyright_others)*
--
-- This library is free software; you can redistribute it and/or
-- modify it under the terms of the GNU Lesser General Public
-- License as published by the Free Software Foundation; either
-- version 2\\.1 of the License, or \\(at your option\\) any later version\\.
--
-- This library is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE\\.  See the GNU
-- Lesser General Public License for more details\\.
--
-- You should have received a copy of the GNU Lesser General Public
-- License along with this library; if not, write to the Free Software
-- Foundation, Inc\\., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

END

my $str_gpl3_or_later = <<END;
^/\\*
 \\* $copyright_iit(
 \\* $copyright_robocub)?
 \\*
 \\* This program is free software: you can redistribute it and/or modify
 \\* it under the terms of the GNU General Public License as published by
 \\* the Free Software Foundation, either version 3 of the License, or
 \\* \\(at your option\\) any later version\\.
 \\*
 \\* This program is distributed in the hope that it will be useful,
 \\* but WITHOUT ANY WARRANTY; without even the implied warranty of
 \\* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE\\.  See the
 \\* GNU General Public License for more details\\.
 \\*
 \\* You should have received a copy of the GNU General Public License
 \\* along with this program\\.  If not, see <https://www\\.gnu\\.org/licenses/>\\.
 \\*/

END

my $str_gpl3_or_later_other = <<END;
^/\\*
 \\* $copyright_iit(
 \\* $copyright_robocub)?(
 \\* $copyright_others)*
 \\*
 \\* This program is free software: you can redistribute it and/or modify
 \\* it under the terms of the GNU General Public License as published by
 \\* the Free Software Foundation, either version 3 of the License, or
 \\* \\(at your option\\) any later version\\.
 \\*
 \\* This program is distributed in the hope that it will be useful,
 \\* but WITHOUT ANY WARRANTY; without even the implied warranty of
 \\* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE\\.  See the
 \\* GNU General Public License for more details\\.
 \\*
 \\* You should have received a copy of the GNU General Public License
 \\* along with this program\\.  If not, see <https://www\\.gnu\\.org/licenses/>\\.
 \\*/

END

my $files = 0;
my $ok = 0;
my $skip = 0;
my $errors = 0;
my $known = 0;

foreach my $filename (@files) {
    $files++;

    # For now skip files known to have a broken license
    my $match = 0;
    for (@skip_files) {
        if ("$filename" eq "$_") {
            $match = 1;
            last;
        }
    }

    if( $match ) {
        print "[SKIP (known)] $filename\n";
        $known++;
        next;
    }

    # Skip images and binary files
    if ("$filename" =~ /\.(png|svg|jpg|ppm|bmp|ico|icns)$/) {
        print "[SKIP (image - $1)] $filename\n";
        $skip++;
        next;
    }

    if ("$filename" =~ /\.(avi)$/) {
        print "[SKIP (video - $1)] $filename\n";
        $skip++;
        next;
    }

    if ("$filename" =~ /\.(wav)$/) {
        print "[SKIP (audio - $1)] $filename\n";
        $skip++;
        next;
    }

    if ("$filename" =~ /\.(rar)$/) {
        print "[SKIP (binary - $1)] $filename\n";
        $skip++;
        next;
    }


    # Read the file
    my $txt = "";
    open(FIN,"<$filename");
    while (<FIN>) {
        $txt .= $_;
    }
    close(FIN);


    # Check if the file contains the unicode BOM
    if ("$txt" =~ /^\xEF\xBB\xBF/s) {
        print "[NOT OK (Contains BOM)] $filename\n";
        $errors++;
        next;
    }


    # Skip a specific files
    if ("$filename" =~ /src\/(carriers|devices)\/.+\.ini$/) {
        print "[SKIP (plugin .ini)] $filename\n";
        $skip++;
        next;
    }

    if ("$filename" =~ /\/(README|readme)(\.(txt|TXT|md))?$/) {
        print "[SKIP (README)] $filename\n";
        $skip++;
        next;
    }

    if ("$filename" =~ /\/qmldir$/) {
        print "[SKIP (qmldir)] $filename\n";
        $skip++;
        next;
    }

    if ("$filename" =~ /\/Doxyfile(.in)?$/) {
        print "[SKIP (Doxyfile)] $filename\n";
        $skip++;
        next;
    }

    if ("$filename" eq "src/carriers/portmonitor_carrier/lua/lua_swig.h" ||
        "$filename" =~ /src\/idls\/thrift\/src_gen\/thrift\// ||
        "$filename" =~ /\/qtquick2applicationviewer/) {
        print "[SKIP (autogen)] $filename\n";
        $skip++;
        next;
    }

    if ("$filename" =~ /\.(ui|rc|qrc|xml|xslt|xsl|msg|srv|ini|cfg|txt|TXT|dox|md|plist.in|tex|mdl)$/) {
        if ("$filename" !~ /CMakeLists.txt$/) {
            print "[SKIP ($1)] $filename\n";
            $skip++;
            next;
        }
    }

    # C++ style BSD-3-Clause
    if ("$txt" =~ /$str_bsd3_cpp/s) {
        if ("$filename" =~ /\.(cpp|cpp.in|c|h|h.in|thrift|mm|qml|java|cs)$/) {
            print "[OK - BSD ($1)] $filename\n";
            $ok++;
        } elsif ("$filename" =~ /\/compiler\..+\.in$/) {
            print "[OK - BSD (compiler.*.in)] $filename\n";
            $ok++;
        } else {
            print "[NOT OK - BSD (c++ style)] $filename\n";
            $errors++;
        }
        next;
    }

    # Script style BSD-3-Clause (no shebang)
    if ("$txt" =~ /$str_bsd3_script_no_sb/s) {
        if ("$filename" =~ /\.(cmake|cmake\.in|cmake\.template|ps1)$/) {
            print "[OK - BSD ($1)] $filename\n";
            $ok++;
        } elsif ("$filename" =~ /CMakeLists.txt$/) {
            print "[OK - BSD (CMakeLists.txt)] $filename\n";
            $ok++;
        } else {
            print "[NOT OK - BSD (script)] $filename\n";
            $errors++;
        }
        next;
    }

    # Script style BSD-3-Clause (with shebang)
    if ("$txt" =~ /$str_bsd3_script/s) {

        # Ensure that scripts are executable on linux
        if ($^O eq "linux") {
            stat($filename);
            if (! -x _) {
                print "[NOT OK - BSD (script not executable)] $filename\n";
                $errors++;
                next;
            }
        }

        if ("$filename" =~ /\.(sh|py|pl|tcl)$/) {
            print "[OK - BSD (.$1)] $filename\n";
            $ok++;
        } else {
            print "[OK - BSD (script)] $filename\n";
            $ok++;
        }
        next;
    }

    # Xml style BSD-3-Clause
    if ("$txt" =~ /$str_bsd3_xml/s) {
        if ("$filename" =~ /\.xml$/) {
            print "[OK - BSD (.xml)] $filename\n";
            $ok++;
        } else {
            print "[NOT OK - BSD (xml style)] $filename\n";
            $errors++;
        }
        next;
    }

    # Lua style BSD-3-Clause
    if ("$txt" =~ /$str_bsd3_lua/s) {
        if ("$filename" =~ /\.lua$/) {
            print "[OK - BSD (.lua)] $filename\n";
            $ok++;
        } else {
            print "[NOT OK - BSD (lua style)] $filename\n";
            $errors++;
        }
        next;
    }

    # Swig style BSD-3-Clause
    if ("$txt" =~ /$str_bsd3_swig/s) {
        if ("$filename" =~ /\.(i)$/) {
            print "[OK - BSD ($1)] $filename\n";
            $ok++;
        } else {
            print "[NOT OK - BSD (swig style)] $filename\n";
            $errors++;
        }
        next;
    }

    # Matlab style BSD-3-Clause
    if ("$txt" =~ /$str_bsd3_mat/s) {
        if ("$filename" =~ /\.(m)$/) {
            print "[OK - BSD ($1)] $filename\n";
            $ok++;
        } else {
            print "[NOT OK - BSD (mat style)] $filename\n";
            $errors++;
        }
        next;
    }

    # C++ style LGPL2.1+
    if ("$txt" =~ /$str_lgpl2_1_or_later/s) {
        if ("$filename" =~ /src\/libYARP_([a-zA-Z]+)/) {
            print "[NOT OK - LGPL2.1+ (library $1)] $filename\n";
            $errors++;
        } elsif ("$filename" =~ /\.(cpp|h|thrift|qml)$/) {
            print "[OK - LGPL2.1+ ($1)] $filename\n";
            $ok++;
        } else {
            print "[NOT OK - LGPL2.1+] $filename\n";
            $errors++;
        }
        next;
    }

    # Lua style LGPL2.1+ (other)
    if ("$txt" =~ /$str_lgpl2_1_or_later_lua/s) {
        if ("$filename" =~ /src\/libYARP_([a-zA-Z]+)/) {
            print "[NOT OK - LGPL2.1+ (library $1)] $filename\n";
            $errors++;
        } elsif ("$filename" =~ /\.(lua)$/) {
            print "[OK - LGPL2.1+ ($1)] $filename\n";
            $ok++;
        } else {
            print "[NOT OK - LGPL2.1+] $filename\n";
            $errors++;
        }
        next;
    }

    # LGPL2.1+ (other)
    if ("$txt" =~ /$str_lgpl2_1_or_later_other/s ||
        "$txt" =~ /$str_lgpl2_1_or_later_lua_other/s) {
        print "[NOT OK - LGPL2.1+ (other)] $filename\n";
        $errors++;
        next;
    }

    # GPL2+
    if ("$txt" =~ /$str_gpl2_or_later/s) {
        if ("$filename" =~ /src\/libYARP_gsl\/.*\.(cpp|h)/ ||
            "$filename" =~ /example\/matrix\/.*\.(cpp|h)/) {
            # YARP_gsl library is necessarily GPL2+
            print "[OK - GPL2+ ($1)] $filename\n";
            $ok++;
        } else {
            print "[NOT OK - GPL2+] $filename\n";
            $errors++;
        }
        next;
    } elsif ("$txt" =~ /$str_gpl2_or_later_other/s) {
        print "[NOT OK - GPL2+ (other)] $filename\n";
        $errors++;
        next;
    }

    # GPL3+
    if ("$txt" =~ /$str_gpl3_or_later/s) {
        if ("$filename" =~ /src\/yarpviz\/.*\.(cpp|h|qml)/ ||
            "$filename" =~ /src\/yarpscope\/.*\.(cpp|h|qml)/ ||
            "$filename" =~ /example\/ContainerExample\/.*\.(cpp|h|qml)/) {
            # yarpviz and yarpscope are necessarily GPL3+
            print "[OK - GPL3+ ($1)] $filename\n";
            $ok++;
        } else {
            print "[NOT OK - GPL3+] $filename\n";
            $errors++;
        }
        next;
    } elsif ("$txt" =~ /$str_gpl3_or_later_other/s) {
        print "[NOT OK - GPL3+ (other)] $filename\n";
        $errors++;
        next;
    }

    # Apache 2.0
    if ("$txt" =~ /$str_apache_2/s) {
        if ("$filename" eq "src/idls/thrift/src/t_yarp_generator.cc") {
            print "[OK - Apache 2.0] $filename\n";
            $ok++;
        } else {
            print "[NOT OK - Apache 2.0] $filename\n";
            $errors++;
        }
        next;
    }

    print "[NOT OK (unknown license)] $filename\n";
    $errors++;
    next;
}

print "FILES:  $files\n";
print "OK:     $ok\n";
print "SKIP:   $skip\n";
print "KNOWN:  $known\n";
print "ERRORS: $errors\n";

if ($ok + $skip + $known + $errors != $files) {
    print "\n[ERROR: Some file was not counted]\n";
    exit 1;
}

if ($known < scalar(@skip_files)) {
    print "\n[ERROR: Some known file was not found and the skip file was not updated]\n";
    exit 1;
}

if ($known > scalar(@skip_files)) {
    print "\n[ERROR: Some new known file was added and the skip file was not updated]\n";
    exit 1;
}

if ($errors != 0) {
    print "\n[ERROR: Some file has an invalid license]\n";
    exit 1;
}

exit 0;
