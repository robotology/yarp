/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/profiler/NetworkProfiler.h>
#include <yarp/os/Network.h>
#include<fstream>

using namespace std;

int main(int argc, char *argv[])
{
    yarp::os::Network yarp;

    yarp::os::Property p;
    if (argc > 1) {
        p.fromCommand(argc, argv);
    }

    std::vector <string> color_map;
    color_map.push_back ("cadetblue1"); //0
    color_map.push_back ("antiquewhite1");
    color_map.push_back ("darkolivegreen2");
    color_map.push_back ("gold2");
    color_map.push_back ("darkorange2");
    color_map.push_back ("cadetblue4");
    color_map.push_back ("coral2");
    color_map.push_back ("firebrick3");
    color_map.push_back ("forestgreen");
    color_map.push_back ("cornflowerblue");

    std::string from_ip = p.check("from_ip",yarp::os::Value(std::string("*")), "").asString();
    std::string to_ip = p.check("to_ip",yarp::os::Value(std::string("*")), "").asString();
    std::string from_portnumber = p.check("from_portnumber",yarp::os::Value(std::string("*")), "").asString();
    std::string to_portnumber = p.check("to_portnumber",yarp::os::Value(std::string("*")), "").asString();
    std::string from_portname = p.check("from_portname",yarp::os::Value(std::string("*")), "").asString();
    std::string to_portname = p.check("to_portname",yarp::os::Value(std::string("*")), "").asString();
    std::string to_dotfile = p.check("to_dotfile", yarp::os::Value(std::string("")), "").asString();
    bool display_yarprun_processes = false;
    bool display_log_ports = false;
    bool display_clock_ports = false;
    bool display_unconnected_ports = false;
    if (p.check("display_yarprun_processes")) { display_yarprun_processes = true;}
    if (p.check("display_log_ports")) { display_log_ports = true; }
    if (p.check("display_clock_ports")) { display_clock_ports = true; }
    if (p.check("display_unconnnected_ports")) { display_unconnected_ports = true; }

    yarp::profiler::NetworkProfiler prof;
    yarp::profiler::NetworkProfiler::connections_set conns_orig;
    yarp::profiler::NetworkProfiler::connections_set conns;
    yarp::profiler::NetworkProfiler::connections_set conns_filtered;
    yarp::profiler::NetworkProfiler::ports_name_set  ports_list;

    prof.getPortsList(ports_list);
    prof.getConnectionsList(conns_orig);
    conns=conns_orig;

    if (from_ip != "*" || to_ip != "*")
    {
        prof.filterConnectionListByIp(conns, conns_filtered, from_ip,to_ip);
        conns= conns_filtered;
    }
    if (from_portnumber != "*" || to_portnumber != "*")
    {
        prof.filterConnectionListByPortNumber(conns, conns_filtered, from_portnumber, to_portnumber);
        conns = conns_filtered;
    }
    if (from_portname != "*" || to_portname != "*")
    {
        prof.filterConnectionListByName(conns, conns_filtered, from_portname, to_portname);
        conns = conns_filtered;
    }

    std::string sss = "Connections:\n";
    if (conns.size() !=0)
    for (auto it = conns.begin(); it!=conns.end(); it++)
    {
        sss += it->src.name + " (" + it->src.ip + ":" + it->src.port_number + ") -> " +
               it->dst.name + " (" + it->dst.ip + ":" + it->dst.port_number + ") with carrier: (" +
               it->carrier + ")\n";
    }
    else
    {
        sss +="Empty list";
    }

    yInfo() << sss;

    if (to_dotfile != "")
    {
        //saveToFile
        std::ofstream file;
        file.open(to_dotfile.c_str(), std::ios::out);
        file << "digraph G{" <<endl;
        file << "splines=\"compound\"" << endl;

        yarp::profiler::NetworkProfiler::ports_detail_set detailed_ports_filtered_by_ip;
        yarp::profiler::NetworkProfiler::ports_detail_set detailed_ports_filtered_by_ip_and_process;
        yarp::profiler::NetworkProfiler::ports_detail_set detailed_ports;
        prof.getPortsDetailedList(detailed_ports);
        yarp::profiler::NetworkProfiler::machines_list ml;
        prof.getMachinesList(detailed_ports, ml);
        yarp::profiler::NetworkProfiler::processes_list pl;
        prof.getProcessesList(detailed_ports, pl);

        file << "subgraph cluster_net{" << endl;
        size_t machine_counter = 0;
        size_t color_map_index = 0;
        for (auto machines_ip_it = ml.begin(); machines_ip_it != ml.end(); machines_ip_it++, machine_counter++)
        {
            prof.filterPortsListByIp(detailed_ports, detailed_ports_filtered_by_ip, *machines_ip_it);
            file << "subgraph cluster_machine"<<machine_counter <<" {" << endl;

            size_t process_counter = 0;
            for (auto process_name_it = pl.begin(); process_name_it != pl.end(); process_name_it++, process_counter++)
            {
                //this option skips all yarprun processes
                if (display_yarprun_processes == false)
                {
                    std::size_t found = process_name_it->find("yarprun");
                    if (found != std::string::npos) { process_counter--; continue;}
                }

                file << "subgraph cluster_process" << process_counter<<" {" << endl;
                prof.filterPortsListByProcess(detailed_ports_filtered_by_ip, detailed_ports_filtered_by_ip_and_process, *process_name_it);

                for (auto ports_it = detailed_ports_filtered_by_ip_and_process.begin(); ports_it != detailed_ports_filtered_by_ip_and_process.end(); ports_it++)
                {
                    //this option skips all logger ports
                    if (display_log_ports == false)
                    {
                        std::size_t found = ports_it->info.name.find("/log");
                        if (found != std::string::npos) { continue; }
                    }

                    //this option skips all clock ports
                    if (display_clock_ports == false)
                    {
                        std::size_t found = ports_it->info.name.find("/clock:i");
                        if (found != std::string::npos) { continue; }
                    }

                    //this option skips all unconnected ports
                    if (display_unconnected_ports == false)
                    {
                        if (ports_it->inputs.size() == 0 &&
                            ports_it->outputs.size() == 0)
                        {
                            continue;
                        }
                    }

                    file <<  "\"" << ports_it->info.name <<  "\";" << endl;
                }
                file << "label = \""<< *process_name_it << "\";" << endl;
                file << "}" << endl;
            }
            file << "label = \"" << *machines_ip_it << "\";" << endl;
            file << "style = filled" << endl;
            file << "color = " << color_map[color_map_index++] << endl;
            if (color_map_index >=10) color_map_index=0;
            file << "}" << endl;
        }
        file << "}" << endl;

        for (auto connection_it = conns.begin(); connection_it != conns.end(); connection_it++)
        {
            //this option skips all logger ports
            if (display_log_ports == false)
            {
                std::size_t founds = connection_it->src.name.find("/log");
                std::size_t foundd = connection_it->dst.name.find("/log");
                if (founds != std::string::npos ||
                    foundd != std::string::npos) { continue; }
            }

            //this option skips all clock ports
            if (display_clock_ports == false)
            {
                std::size_t founds = connection_it->src.name.find("/clock:i");
                std::size_t foundd = connection_it->dst.name.find("/clock:i");
                if (founds != std::string::npos ||
                    foundd != std::string::npos) {
                    continue;
                }
            }

            std::string protocol_name = connection_it->carrier; //"unknown"
            std::string color_connection = "black";
            if      (protocol_name == "tcp") { color_connection = "blue";}
            else if (protocol_name == "udp") { color_connection = "red"; }
            else if (protocol_name == "fast_tcp") { color_connection = "green"; }
            else if (protocol_name == "mjpeg") { color_connection = "darkorange2"; }

            file << "\"" << connection_it->src.name <<  "\"" << " -> "
                 << "\"" << connection_it->dst.name <<  "\"" << " [label = \""<< protocol_name << "\" color = "<< color_connection <<"]" << endl;
        }

        file << "}" << endl;
        file.close();
    }

    return 1;
}

/* dot file example

digraph G {


	splines="compound"

	rankdir="TB"
	subgraph cluster_net {


	subgraph cluster_1 {

		subgraph cluster_1a {

			node [style=filled];
			a0;
			a1;
			a2;
			a3;
			label = "process a";

			color=blue
		}
		subgraph cluster_1b {
			node [style=filled];
			b0;
			b1;
			b2;
			b3;
			label = "process b";
			color=blue
		}
		label = "machine1";
	}

	subgraph cluster_2 {
			subgraph cluster_2a {
				node [style=filled];
				c0 [label="/c0/kkk11"];
				c1;
				c2;
				c3;
				label = "process c";
				color=blue
			}
			subgraph cluster_2b {
				node [style=filled];
				d0;
				d1;
				d2;
				d3;
				label = "process d";
				color=blue
			}
		label = "machine";
	}

	a1 -> c1 [label="tcp" color=blue];
	c1 -> a1 [label="tcp" color=blue];
	b2 -> c3 [label="tcp" color=blue];
	a3 -> c0 [label="tcp" color=green];
	a3 -> d0 [label="tcp" color=green];
	b3 -> d3 [label="tcp" color=blue];
}
*/

