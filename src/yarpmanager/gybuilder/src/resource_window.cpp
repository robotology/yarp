/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#if defined(WIN32)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#include <yarp/manager/yarpbroker.h>
#include <yarp/manager/primresource.h>
#include <yarp/manager/ymm-dir.h>

#include "resource_window.h"
#include "icon_res.h"
#include "main_window.h"

using namespace yarp::manager;


ResourceWindow::ResourceWindow(Computer* computer, MainWindow* parent, 
                               Manager* manager)
{
    m_pComputer = computer;
    m_pParent = parent;
    m_pManager = manager;

    /* Create a new scrolled window, with scrollbars only if needed */
    set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    add(m_TreeView);

    /* create tree store */
    m_refTreeModel = Gtk::TreeStore::create(m_Columns);
    m_TreeView.set_model(m_refTreeModel);

    //Add the Model’s column to the View’s columns: 
    Gtk::CellRendererText* cellText = Gtk::manage(new Gtk::CellRendererText());
    Gtk::CellRendererPixbuf* cellPix = Gtk::manage(new Gtk::CellRendererPixbuf()); 
    Gtk::TreeViewColumn* col = Gtk::manage(new Gtk::TreeViewColumn("Item"));
    col->pack_start(*cellPix, false);
    col->pack_start(*cellText, true);
    col->add_attribute(*cellText, "text", 1);
    col->add_attribute(*cellPix, "pixbuf", 0);
    m_TreeView.append_column(*col);
    m_TreeView.get_column(0)->set_resizable(true);

    m_TreeView.append_column("Value", m_Columns.m_col_value);
    m_TreeView.get_column(1)->set_resizable(true);
    
    updateWidget();
    show_all_children();
}


ResourceWindow::~ResourceWindow()
{
}

void ResourceWindow::onRefresh()
{
    ErrorLogger* logger  = ErrorLogger::Instance();

    if(m_pManager->updateResource(m_pComputer->getName()))
    {
        YarpBroker broker;
        broker.init();
    
        string strServer = m_pComputer->getName();
        if(strServer[0] != '/')
            strServer = string("/") + strServer;
        // Processes list
        ProcessContainer processes;
        if(broker.getAllProcesses(strServer.c_str(), processes))
        {
            m_pComputer->getProcesses().clear();
            for(unsigned int i=0; i<processes.size(); i++)
                m_pComputer->getProcesses().push_back(processes[i]);
        }
        else
            logger->addError(broker.error());        

        updateWidget();
    }
    m_pParent->reportErrors();
}

void ResourceWindow::onTabCloseRequest() 
{ 
    m_pParent->onTabCloseRequest(this); 
}

void ResourceWindow::updateWidget()
{
    m_refTreeModel->clear();

    Gtk::TreeModel::Row row;
    Gtk::TreeModel::Row childrow;
//    Gtk::TreeModel::Row cchildrow;

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Name";
    row[m_Columns.m_col_value] = m_pComputer->getName();

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Description";
    row[m_Columns.m_col_value] = m_pComputer->getDescription();

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Disable";
    if(!m_pComputer->getDisable())
    {
        row[m_Columns.m_col_value] = "No";
        row.set_value(0, Gdk::Pixbuf::create_from_data(computer_ico.pixel_data, 
                                                Gdk::COLORSPACE_RGB,
                                                true,
                                                8,
                                                computer_ico.width,
                                                computer_ico.height,
                                                computer_ico.bytes_per_pixel*computer_ico.width));
    }
    else
    {
        row[m_Columns.m_col_value] = "Yes";
        row.set_value(0, Gdk::Pixbuf::create_from_data(computer_disable_ico.pixel_data, 
                                                Gdk::COLORSPACE_RGB,
                                                true,
                                                8,
                                                computer_disable_ico.width,
                                                computer_disable_ico.height,
                                                computer_disable_ico.bytes_per_pixel*computer_disable_ico.width));
    }


    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Availablity";
    if(m_pComputer->getAvailability())
    {
        row[m_Columns.m_col_value] = "Yes";
        row.set_value(0, Gdk::Pixbuf::create_from_data(yesres_ico.pixel_data, 
                                                Gdk::COLORSPACE_RGB,
                                                true,
                                                8,
                                                yesres_ico.width,
                                                yesres_ico.height,
                                                yesres_ico.bytes_per_pixel*yesres_ico.width));
    }
    else
    {
        row[m_Columns.m_col_value] = "No";
        row.set_value(0, Gdk::Pixbuf::create_from_data(nores_ico.pixel_data, 
                                                Gdk::COLORSPACE_RGB,
                                                true,
                                                8,
                                                nores_ico.width,
                                                nores_ico.height,
                                                nores_ico.bytes_per_pixel*nores_ico.width));
        return;
    }

    // platform 
    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Platform";
    row[m_Columns.m_col_value] = "";
    row.set_value(0, Gdk::Pixbuf::create_from_data(module_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            module_ico.width,
                                            module_ico.height,
                                            module_ico.bytes_per_pixel*module_ico.width));
     
    childrow = *(m_refTreeModel->append(row.children()));
    childrow[m_Columns.m_col_name] = "Name";
    childrow[m_Columns.m_col_value] = m_pComputer->getPlatform().getName();
   
    childrow = *(m_refTreeModel->append(row.children()));
    childrow[m_Columns.m_col_name] = "Distribution";
    childrow[m_Columns.m_col_value] = m_pComputer->getPlatform().getDistribution();

    childrow = *(m_refTreeModel->append(row.children()));
    childrow[m_Columns.m_col_name] = "Release";
    childrow[m_Columns.m_col_value] = m_pComputer->getPlatform().getRelease();

    // processor
    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Processor";
    row[m_Columns.m_col_value] = "";
    row.set_value(0, Gdk::Pixbuf::create_from_data(processor_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            processor_ico.width,
                                            processor_ico.height,
                                            processor_ico.bytes_per_pixel*processor_ico.width));

    childrow = *(m_refTreeModel->append(row.children()));
    childrow[m_Columns.m_col_name] = "Model";
    childrow[m_Columns.m_col_value] = m_pComputer->getProcessor().getModel();
  
    childrow = *(m_refTreeModel->append(row.children()));
    childrow[m_Columns.m_col_name] = "Architecture";
    childrow[m_Columns.m_col_value] = m_pComputer->getProcessor().getArchitecture();
    
    char buff[64];
    childrow = *(m_refTreeModel->append(row.children()));
    childrow[m_Columns.m_col_name] = "Cores";
    sprintf(buff, "%d", (int)m_pComputer->getProcessor().getCores());
    childrow[m_Columns.m_col_value] = Glib::ustring(buff);

    childrow = *(m_refTreeModel->append(row.children()));
    childrow[m_Columns.m_col_name] = "Siblings";
    sprintf(buff, "%d", (int)m_pComputer->getProcessor().getSiblings());
    childrow[m_Columns.m_col_value] = Glib::ustring(buff);

    childrow = *(m_refTreeModel->append(row.children()));
    childrow[m_Columns.m_col_name] = "Frequency";
    sprintf(buff, "%.2f", (float)m_pComputer->getProcessor().getFrequency());
    childrow[m_Columns.m_col_value] = Glib::ustring(buff);

    // CPU Load
    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "CPU Load";
    row[m_Columns.m_col_value] = "";
    row.set_value(0, Gdk::Pixbuf::create_from_data(cpuload_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            cpuload_ico.width,
                                            cpuload_ico.height,
                                            cpuload_ico.bytes_per_pixel*cpuload_ico.width));

    childrow = *(m_refTreeModel->append(row.children()));
    childrow[m_Columns.m_col_name] = "Load instant";
    if(compareString(m_pComputer->getPlatform().getName(), "WINDOWS"))
        sprintf(buff, "%.1f%%", (float)m_pComputer->getProcessor().getCPULoad().loadAverageInstant);
    else
    {
        float sibling = (float)m_pComputer->getProcessor().getSiblings();
        if(!sibling) sibling = 1.0F;
        float loadP = (float)(m_pComputer->getProcessor().getCPULoad().loadAverageInstant / sibling)*100.0F;
        if(loadP > 100.0F) loadP = 100.0F;
        sprintf(buff, "%.1f%%", loadP);
    }
    childrow[m_Columns.m_col_value] = Glib::ustring(buff);

    float sibling = (float)m_pComputer->getProcessor().getSiblings();
    if(!sibling) sibling = 1.0F;
    float load1P = ((float)m_pComputer->getProcessor().getCPULoad().loadAverage1 / sibling)*100.0F;
    if(load1P > 100.0F) load1P = 100.0F; 
    float load5P = ((float)m_pComputer->getProcessor().getCPULoad().loadAverage5 / sibling)*100.0F;
    if(load5P > 100.0F) load5P = 100.0F; 
    float load15P = ((float)m_pComputer->getProcessor().getCPULoad().loadAverage15 / sibling)*100.0F;
    if(load15P > 100.0F) load15P = 100.0F; 
  
    childrow = *(m_refTreeModel->append(row.children()));
    childrow[m_Columns.m_col_name] = "Load average 1";
    sprintf(buff, "%.1f%%", load1P);
    childrow[m_Columns.m_col_value] = Glib::ustring(buff);

    childrow = *(m_refTreeModel->append(row.children()));
    childrow[m_Columns.m_col_name] = "Load average 5";
    sprintf(buff, "%.1f%%", load5P);
    childrow[m_Columns.m_col_value] = Glib::ustring(buff);

    childrow = *(m_refTreeModel->append(row.children()));
    childrow[m_Columns.m_col_name] = "Load average 15";
    sprintf(buff, "%.1f%%", load15P);
    childrow[m_Columns.m_col_value] = Glib::ustring(buff);
    m_TreeView.expand_row(m_refTreeModel->get_path(row), true);

    // Memory
    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Memory";
    row[m_Columns.m_col_value] = "";
    row.set_value(0, Gdk::Pixbuf::create_from_data(memory_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            memory_ico.width,
                                            memory_ico.height,
                                            memory_ico.bytes_per_pixel*memory_ico.width));

    childrow = *(m_refTreeModel->append(row.children()));
    childrow[m_Columns.m_col_name] = "Total space";
    sprintf(buff, "%ldKB", (long)m_pComputer->getMemory().getTotalSpace());
    childrow[m_Columns.m_col_value] = Glib::ustring(buff);

    childrow = *(m_refTreeModel->append(row.children()));
    childrow[m_Columns.m_col_name] = "Free space";
    sprintf(buff, "%ldKB", (long)m_pComputer->getMemory().getFreeSpace());
    childrow[m_Columns.m_col_value] = Glib::ustring(buff);

    // Storage
    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Storage";
    row[m_Columns.m_col_value] = "";
    row.set_value(0, Gdk::Pixbuf::create_from_data(storage_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            storage_ico.width,
                                            storage_ico.height,
                                            storage_ico.bytes_per_pixel*storage_ico.width));

    childrow = *(m_refTreeModel->append(row.children()));
    childrow[m_Columns.m_col_name] = "Total space";
    sprintf(buff, "%ldKB", (long)m_pComputer->getStorage().getTotalSpace());
    childrow[m_Columns.m_col_value] = Glib::ustring(buff);

    childrow = *(m_refTreeModel->append(row.children()));
    childrow[m_Columns.m_col_name] = "Free space";
    sprintf(buff, "%ldKB", (long)m_pComputer->getStorage().getFreeSpace());
    childrow[m_Columns.m_col_value] = Glib::ustring(buff);

    // Peripherals
    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Peripheral devices";
    row[m_Columns.m_col_value] = "";
    row.set_value(0, Gdk::Pixbuf::create_from_data(device_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            device_ico.width,
                                            device_ico.height,
                                            device_ico.bytes_per_pixel*device_ico.width));
    for(int i=0; i<m_pComputer->peripheralCount(); i++)
    {
        childrow = *(m_refTreeModel->append(row.children()));
        childrow[m_Columns.m_col_name] = m_pComputer->getPeripheralAt(i).getTypeName();
        childrow[m_Columns.m_col_value] = m_pComputer->getPeripheralAt(i).getName();
    }

    // Processes
    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Processes";
    row[m_Columns.m_col_value] = "";
    row.set_value(0, Gdk::Pixbuf::create_from_data(process_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            process_ico.width,
                                            process_ico.height,
                                            process_ico.bytes_per_pixel*process_ico.width));

    ProcessContainer processes = m_pComputer->getProcesses();
    for(unsigned int i=0; i<processes.size(); i++)
    {
        childrow = *(m_refTreeModel->append(row.children()));
        char buff[64];
        sprintf(buff, "%d", processes[i].pid);
        childrow[m_Columns.m_col_name] = Glib::ustring(buff);
        childrow[m_Columns.m_col_value] = processes[i].command;
    }

}


