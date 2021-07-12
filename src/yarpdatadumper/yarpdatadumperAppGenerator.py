#!/usr/bin/python3

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

import argparse

def printPortTags(out_file, ports, port_type, host, additional_flags):
    for port in ports:
        # print datadumper launch
        out_file.write("\t<module>\n");
        out_file.write("\t\t<name>yarpdatadumper</name>\n");
        out_file.write("\t\t<parameters>--name /dumper"+port+" --type "+port_type+" "+additional_flags+"</parameters>\n");
        out_file.write("\t\t<node>"+host+"</node>\n");
        out_file.write("\t</module>\n");

        # print connection between datadumper and port
        out_file.write("\t<connection>\n");
        out_file.write("\t\t<from>"+port+"</from>\n");
        out_file.write("\t\t<to>/dumper"+port+"</to>\n");
        out_file.write("\t\t<protocol>udp</protocol>\n");
        out_file.write("\t</connection>\n");

def main():
    parser = argparse.ArgumentParser(description='Tool for generating a YarpManager XML application for dumping a list of YARP ports using the yarpdatadumper.')
    parser.add_argument('--ports', nargs='+', dest="ports", action='store', required=True, help='list of ports (serializable to bottles) to dump')
    parser.add_argument('--imagePorts', nargs='+', dest="imagePorts", action='store', help='list of ports (of to dump')
    parser.add_argument('--host', nargs=1,  dest="host", action='store', required=True, help='host where to launch the dataDumpers')
    parser.add_argument('--name', nargs=1,  dest="name", action='store', required=True, help='name of the application, the file will be saved as name.xml')
    parser.add_argument('--rxTime', action='store_true',help='pass --rxTime flag to the yarpdatadumpers')
    parser.add_argument('--txTime', action='store_true',help='pass --txTime flag to the yarpdatadumpers')
    parser.add_argument('--addVideo', action='store_true',help='pass --addVideo flag to the yarpdatadumpers')
    args = parser.parse_args()

    # open output file
    filename = args.name[0] + ".xml";
    out_file = open(filename,"w")

    out_file.write("<application>\n");
    out_file.write("\t<name>"+args.name[0]+"</name>\n");
    out_file.write("\t<dependencies>\n");
    for port in args.ports:
        out_file.write("\t\t<port>"+port+"</port>\n");
    out_file.write("\t</dependencies>\n");

    additional_flags = "";
    if(args.rxTime):
        additional_flags += " --rxTime";
    if(args.txTime):
        additional_flags += " --txTime";
    if(args.addVideo):
        additional_flags += " --addVideo";

    if( args.ports is not None):
        printPortTags(out_file,args.ports,"bottle",args.host[0],additional_flags);
    if( args.imagePorts is not None):
        printPortTags(out_file,args.imagePorts,"image",args.host[0],additional_flags);

    out_file.write("</application>\n");

if __name__ == "__main__":
    main()
