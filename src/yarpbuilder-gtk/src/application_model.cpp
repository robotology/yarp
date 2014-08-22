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
    #pragma warning (disable : 4099)
#endif

#include <string.h>
#include "application_model.h"
#include "application_window.h"
#include "int_port_model.h"
#include <goocanvas.h>
#include "main_window.h"

#ifndef MAX
    #define MAX(X,Y) (((X) > (Y))?(X):(Y))
#endif

#define MIN_WIDTH           100
#define MIN_HEIGHT          70
#define TEXT_MARGINE        10
#define ITEMS_MARGIE        20
#define FONT_DESC           "Monospace 10"

#define APPLICATION_COLOR        0xeeeeeeaa

using namespace yarp::manager;

ApplicationModel::ApplicationModel(ApplicationWindow* parentWnd, Application* app) : GroupModel()
{
    parentWindow = parentWnd;
    application = app;
    app->setModel(this);

    // adding application name
    text = Goocanvas::TextModel::create(app->getName());
#ifdef GLIBMM_PROPERTIES_ENABLED
    text->property_font() = FONT_DESC;
    text->property_alignment().set_value(Pango::ALIGN_CENTER);
#endif

    PangoLayout *layout = gtk_widget_create_pango_layout((GtkWidget*)parentWindow->gobj(),
                            app->getName());
    PangoFontDescription *fontdesc = pango_font_description_from_string(FONT_DESC);
    pango_layout_set_font_description (layout, fontdesc);
    pango_layout_get_pixel_size (layout, &text_w, &text_h);

    text->property_x().set_value(TEXT_MARGINE);
    text->property_y().set_value(text_h/2.0);

    poly = Goocanvas::PolylineModel::create(0,0,0,0);
    poly->property_close_path().set_value(true);
    poly->property_line_width().set_value(1.2);
    //poly->property_fill_color().set_value(APPLICATION_COLOR);
    poly->property_fill_color_rgba().set_value(APPLICATION_COLOR);
    poly->property_stroke_color().set_value("DodgerBlue3");
    //GooCanvasLineDash *dash = goo_canvas_line_dash_new (2, 3.0, 3.0);
    //g_object_set(poly->gobj(), "line-dash", dash, NULL);

    shadow = Goocanvas::PolylineModel::create(0,0,0,0);
    shadow->property_close_path().set_value(false);
    shadow->property_line_width().set_value(3.0);
    shadow->property_stroke_color().set_value("gray");

    getApplicationBounds(bounds);

    width = MAX(MIN_WIDTH + text_w, bounds.get_x2()-bounds.get_x1() + 2*ITEMS_MARGIE);
    height = MAX(MIN_HEIGHT, bounds.get_y2()-bounds.get_y1() + 2*ITEMS_MARGIE + text_h);
    Goocanvas::Points points(6);
    points.set_coordinate(0, 0, 0);
    points.set_coordinate(1, text_w+TEXT_MARGINE*2, 0);
    points.set_coordinate(2, text_w+TEXT_MARGINE*2, text_h+TEXT_MARGINE);
    points.set_coordinate(3, width, text_h+TEXT_MARGINE);
    points.set_coordinate(4, width, height);
    points.set_coordinate(5, 0, height);
    poly->property_points().set_value(points);

    Goocanvas::Points points3(3);
    points3.set_coordinate(0, width, text_h+TEXT_MARGINE);
    points3.set_coordinate(1, width, height);
    points3.set_coordinate(2, 0, height);
    shadow->property_points().set_value(points3);
    shadow->translate(1,1);

    this->add_child(shadow);
    this->add_child(poly);
    this->add_child(text);


    ModulePContainer modules = parentWindow->manager.getKnowledgeBase()->getModules(application);
    CnnContainer connections = parentWindow->manager.getKnowledgeBase()->getConnections(application);
    ApplicaitonPContainer applications = parentWindow->manager.getKnowledgeBase()->getApplications(application);

    int index = 0;
    ModulePIterator itr;
    for(itr=modules.begin(); itr!=modules.end(); itr++)
    {
        Module* module = (*itr);
        Glib::RefPtr<ModuleModel> mod = ModuleModel::create(parentWindow, module, true);
        this->add_child(mod);
        double x, y;
        x = (module->getModelBase().points.size()>0) ? module->getModelBase().points[0].x : index%900+10;
        y = (module->getModelBase().points.size()>0) ? module->getModelBase().points[0].y : ((index+=300)/900)*100+10;
        x -= bounds.get_x1();
        x += ITEMS_MARGIE;
        y -= bounds.get_y1();
        y += ITEMS_MARGIE + text_h + TEXT_MARGINE;
        mod->translate(x, y);

        double end_x, end_y;
        Goocanvas::Points points = poly->property_points().get_value();
        points.get_coordinate(4, end_x, end_y);
        if((x + mod->getWidth() + ITEMS_MARGIE) >= end_x)
            width = x + mod->getWidth() + ITEMS_MARGIE;
        if((y + mod->getHeight() + ITEMS_MARGIE) >= end_y)
            height = y + mod->getHeight() + ITEMS_MARGIE;

        //points.set_coordinate(0, 0, 0);
        //points.set_coordinate(1, text_w+TEXT_MARGINE*2, 0);
        //points.set_coordinate(2, text_w+TEXT_MARGINE*2, text_h+TEXT_MARGINE);
        points.set_coordinate(3, width, text_h+TEXT_MARGINE);
        points.set_coordinate(4, width, height);
        points.set_coordinate(5, 0, height);
        poly->property_points().set_value(points);
        Goocanvas::Points points3(3);
        points3.set_coordinate(0, width, text_h+TEXT_MARGINE);
        points3.set_coordinate(1, width, height);
        points3.set_coordinate(2, 0, height);
        shadow->property_points().set_value(points3);
    }

}


void ApplicationModel::updateChildItems(void)
{

    ModulePContainer modules = parentWindow->manager.getKnowledgeBase()->getModules(application);
    CnnContainer connections = parentWindow->manager.getKnowledgeBase()->getConnections(application);
    ApplicaitonPContainer applications = parentWindow->manager.getKnowledgeBase()->getApplications(application);

    int index = 0; //index = (index/900)*100+50;
    CnnIterator citr;
    for(citr=connections.begin(); citr<connections.end(); citr++)
    {
        Connection baseCon = *citr;
        GraphicModel model = baseCon.getModelBase();
        InputData* input = NULL;
        OutputData* output = NULL;
        Glib::RefPtr<PortModel> source(NULL);
        Glib::RefPtr<PortModel> dest(NULL);
        findInputOutputData((*citr), modules, input, output);
        if(output)
            source = findModelFromOutput(output);
        else
        {
            Glib::RefPtr<ExternalPortModel> extPort;
            bool bExist = false;
            for(int i=0; i<get_n_children(); i++)
            {
                extPort = Glib::RefPtr<ExternalPortModel>::cast_dynamic(get_child(i));
                if(extPort && compareString(extPort->getPort(), baseCon.from()))
                {
                    source = extPort;
                    bExist = true;
                    break;
                }
            }
            if(!bExist)
            {
                source = ExternalPortModel::create(parentWindow, OUTPUTD, (*citr).from(), true);
                add_child(source);
                 double x ,y;
                if(model.points.size() > 1)
                {
                    x = model.points[1].x - Glib::RefPtr<ExternalPortModel>::cast_dynamic(source)->getWidth()
                                              - bounds.get_x1()+ITEMS_MARGIE;
                    y = model.points[1].y - Glib::RefPtr<ExternalPortModel>::cast_dynamic(source)->getHeight()/2.0
                                              - bounds.get_y1()+ITEMS_MARGIE+text_h+TEXT_MARGINE;
                    source->set_property("x", x);
                    source->set_property("y", y);
                }
                else
                {
                    x = 10;
                    y = index;
                    source->set_property("x", 10);
                    source->set_property("y", index);
                }

                double end_x, end_y;
                double start_x;
                Goocanvas::Points points = poly->property_points().get_value();
                points.get_coordinate(4, end_x, end_y);
                points.get_coordinate(5, start_x, end_y);
                double port_w = Glib::RefPtr<ExternalPortModel>::cast_dynamic(source)->getWidth();
                double port_h = Glib::RefPtr<ExternalPortModel>::cast_dynamic(source)->getHeight();
                if((x + port_w + ITEMS_MARGIE) >= end_x)
                    width = x + port_w + ITEMS_MARGIE;
                if((y + port_h + ITEMS_MARGIE) >= end_y)
                    height = y + port_h + ITEMS_MARGIE;

                double minx = start_x;
                if(x < start_x)
                {
                    minx = x - ITEMS_MARGIE;
                    text->translate(-abs(start_x-x)-ITEMS_MARGIE, 0);
                }
                points.set_coordinate(0, minx, 0);
                points.set_coordinate(1, text_w+TEXT_MARGINE*2+minx, 0);
                points.set_coordinate(2, text_w+TEXT_MARGINE*2+minx, text_h+TEXT_MARGINE);
                points.set_coordinate(3, width, text_h+TEXT_MARGINE);
                points.set_coordinate(4, width, height);
                points.set_coordinate(5, minx, height);
                poly->property_points().set_value(points);
                Goocanvas::Points points3(3);
                points3.set_coordinate(0, width, text_h+TEXT_MARGINE);
                points3.set_coordinate(1, width, height);
                points3.set_coordinate(2, minx, height);
                shadow->property_points().set_value(points3);
            }
            index+=40;
        }

        if(input)
           dest = findModelFromInput(input);
        else
        {
            Glib::RefPtr<ExternalPortModel> extPort;
            bool bExist = false;
            for(int i=0; i<get_n_children(); i++)
            {
                extPort = Glib::RefPtr<ExternalPortModel>::cast_dynamic(get_child(i));
                if(extPort && compareString(extPort->getPort(), baseCon.to()))
                {
                    dest = extPort;
                    bExist = true;
                    break;
                }
            }
            if(!bExist)
            {
                double x,y;
                dest = ExternalPortModel::create(parentWindow, INPUTD, (*citr).to(), true);
                add_child(dest);
                size_t size = model.points.size();
                if(size > 2)
                {
                    x = model.points[size-1].x-bounds.get_x1()+ITEMS_MARGIE;
                    y = model.points[size-1].y - Glib::RefPtr<ExternalPortModel>::cast_dynamic(dest)->getHeight()/2.0
                                            - bounds.get_y1()+ITEMS_MARGIE+text_h+TEXT_MARGINE;
                    dest->set_property("x", x);
                    dest->set_property("y", y);
                }
                else
                {
                    x = 400;
                    y = index;
                    dest->set_property("x", x);
                    dest->set_property("y", y);
                }
                double end_x, end_y;
                Goocanvas::Points points = poly->property_points().get_value();
                points.get_coordinate(4, end_x, end_y);
                double port_w = Glib::RefPtr<ExternalPortModel>::cast_dynamic(dest)->getWidth();
                double port_h = Glib::RefPtr<ExternalPortModel>::cast_dynamic(dest)->getHeight();
                if((x + port_w + ITEMS_MARGIE) >= end_x)
                    width = x + port_w + ITEMS_MARGIE;
                if((y + port_h + ITEMS_MARGIE) >= end_y)
                    height = y + port_h + ITEMS_MARGIE;

                points.set_coordinate(3, width, text_h+TEXT_MARGINE);
                points.set_coordinate(4, width, height);
                points.get_coordinate(5, end_x, end_y);
                points.set_coordinate(5, end_x, height);
                poly->property_points().set_value(points);
                Goocanvas::Points points3(3);
                points3.set_coordinate(0, width, text_h+TEXT_MARGINE);
                points3.set_coordinate(1, width, height);
                points3.set_coordinate(2, end_x, height);
                shadow->property_points().set_value(points3);
            }
        }

        if(source && dest)
        {
            Glib::RefPtr<ArrowModel> arrow = ArrowModel::create(parentWindow, source, dest, &baseCon, this);
            add_child(arrow);
            int size = model.points.size();
            for(int i=2; i<size-1; i++)
                arrow->addMidPoint(model.points[i].x-bounds.get_x1()+ITEMS_MARGIE,
                                   model.points[i].y-bounds.get_y1()+ITEMS_MARGIE+text_h+TEXT_MARGINE, i-2);
            arrow->setSelected(false);
            if(size)
                arrow->setLabelPosition(model.points[0].x-bounds.get_x1()+ITEMS_MARGIE,
                                        model.points[0].y-bounds.get_y1()+ITEMS_MARGIE+text_h+TEXT_MARGINE);
        }
    }
}

//TODO: check nested applications too
void ApplicationModel::getApplicationBounds(Goocanvas::Bounds& bounds)
{
    ModulePContainer modules = parentWindow->manager.getKnowledgeBase()->getModules(application);
    CnnContainer connections = parentWindow->manager.getKnowledgeBase()->getConnections(application);
    ApplicaitonPContainer applications = parentWindow->manager.getKnowledgeBase()->getApplications(application);

    ModulePIterator itr;
    bounds.set_x1(0);
    bounds.set_x2(0);
    bounds.set_y1(0);
    bounds.set_y2(0);
    for(itr=modules.begin(); itr!=modules.end(); itr++)
    {
        Module* module = (*itr);
        //if(module->owner() == application)
        //{
            if(module->getModelBase().points.size()>0)
            {
                if((bounds.get_x1() == 0) || (module->getModelBase().points[0].x < bounds.get_x1()) )
                    bounds.set_x1(module->getModelBase().points[0].x);
                if((bounds.get_x2() == 0) || (module->getModelBase().points[0].x > bounds.get_x1()) )
                    bounds.set_x2(module->getModelBase().points[0].x);
                if((bounds.get_y1() == 0) || (module->getModelBase().points[0].y < bounds.get_y1()) )
                    bounds.set_y1(module->getModelBase().points[0].y);
                if((bounds.get_y2() == 0) || (module->getModelBase().points[0].y > bounds.get_y2()) )
                    bounds.set_y2(module->getModelBase().points[0].y);
            }
        //}
     }

    CnnIterator citr;
    for(citr=connections.begin(); citr<connections.end(); citr++)
    {
        Connection baseCon = *citr;
        //if(baseCon.owner() == application)
        //{
            GraphicModel model = baseCon.getModelBase();
            int size = model.points.size();
            for(int i=0; i<size; i++)
            {
                if((bounds.get_x1() == -1) || (model.points[i].x < bounds.get_x1()) )
                    bounds.set_x1(model.points[i].x);
                if((bounds.get_x2() == -1) || (model.points[i].x > bounds.get_x2()) )
                    bounds.set_x2(model.points[i].x);
                if((bounds.get_y1() == -1) || (model.points[i].y < bounds.get_y1()) )
                    bounds.set_y1(model.points[i].y);
                if((bounds.get_y2() == -1) || (model.points[i].y > bounds.get_y2()) )
                    bounds.set_y2(model.points[i].y);
            }
        //}
    }
}

void ApplicationModel::findInputOutputData(Connection& cnn,  ModulePContainer &modules,
                                            InputData* &input_, OutputData* &output_)
{
    input_ = NULL;
    output_ = NULL;
    string strTo = cnn.to();
    string strFrom = cnn.from();

    ModulePIterator itr;
    for(itr=modules.begin(); itr!=modules.end(); itr++)
    {
        Module* module = (*itr);
        if(module->owner() == application)
        {
            for(int i=0; i<module->inputCount(); i++)
            {
                InputData &input = module->getInputAt(i);
                string strInput = string(module->getPrefix()) + string(input.getPort());
                if(strTo == strInput)
                {
                    input_ = &input;
                    break;
                }
            }

            for(int i=0; i<module->outputCount(); i++)
            {
                OutputData &output = module->getOutputAt(i);
                string strOutput = string(module->getPrefix()) + string(output.getPort());
                if(strFrom == strOutput)
                {
                    output_ = &output;
                    break;
                }
            }
        }
    }
}

Glib::RefPtr<PortModel> ApplicationModel::findModelFromOutput(OutputData* output)
{

    for(int i=0; i<get_n_children(); i++)
    {
        // if is an application model
        Glib::RefPtr<ApplicationModel> application = Glib::RefPtr<ApplicationModel>::cast_dynamic(get_child(i));
        if(application)
        {
            for(int k=0; k<application->get_n_children(); k++)
            {
                Glib::RefPtr<ModuleModel> module = Glib::RefPtr<ModuleModel>::cast_dynamic(application->get_child(k));
                if(module)
                {
                    for(int j=0; j<module->get_n_children(); j++)
                    {
                        Glib::RefPtr<InternalPortModel> intPort = Glib::RefPtr<InternalPortModel>::cast_dynamic(module->get_child(j));
                        if(intPort && (intPort->getOutput() == output))
                            return intPort;
                    }
                }
            }
        }

        // if is an module model
        Glib::RefPtr<ModuleModel> module = Glib::RefPtr<ModuleModel>::cast_dynamic(get_child(i));
        if(module)
        {
            for(int j=0; j<module->get_n_children(); j++)
            {
                Glib::RefPtr<InternalPortModel> intPort = Glib::RefPtr<InternalPortModel>::cast_dynamic(module->get_child(j));
                if(intPort && (intPort->getOutput() == output))
                    return intPort;
            }
        }
    }
    return Glib::RefPtr<PortModel>(NULL);
}


Glib::RefPtr<PortModel> ApplicationModel::findModelFromInput(InputData* input)
{
    for(int i=0; i<get_n_children(); i++)
    {
        // if is an application model
        Glib::RefPtr<ApplicationModel> application = Glib::RefPtr<ApplicationModel>::cast_dynamic(get_child(i));
        if(application)
        {
            for(int k=0; k<application->get_n_children(); k++)
            {
                Glib::RefPtr<ModuleModel> module = Glib::RefPtr<ModuleModel>::cast_dynamic(application->get_child(k));
                if(module)
                {
                    for(int j=0; j<module->get_n_children(); j++)
                    {
                        Glib::RefPtr<InternalPortModel> intPort = Glib::RefPtr<InternalPortModel>::cast_dynamic(module->get_child(j));
                        if(intPort && (intPort->getInput() == input))
                            return intPort;
                    }
                }
            }
        }

        Glib::RefPtr<ModuleModel> module = Glib::RefPtr<ModuleModel>::cast_dynamic(get_child(i));
        if(module)
        {
            for(int j=0; j<get_child(i)->get_n_children(); j++)
            {
                Glib::RefPtr<InternalPortModel> intPort = Glib::RefPtr<InternalPortModel>::cast_dynamic(get_child(i)->get_child(j));
                if(intPort && (intPort->getInput() == input))
                    return intPort;
            }
        }
    }
    return Glib::RefPtr<PortModel>(NULL);
}



ApplicationModel::~ApplicationModel(void)
{
    if(!application)
        return;

    if(application)
       parentWindow->manager.getKnowledgeBase()->removeIApplicationFromApplication(
                                    parentWindow->manager.getKnowledgeBase()->getApplication(),
                                    application->getLabel());
    application = NULL;
}


void ApplicationModel::releaseApplication(void)
{
    //First, delteing all arrows
    for(int k=0; k<this->get_n_children(); k++)
        for(int j=0; j<this->get_n_children(); j++)
        {
            Glib::RefPtr<ArrowModel> arw = Glib::RefPtr<ArrowModel>::cast_dynamic(this->get_child(j));
            if(arw)
            {
                int id = this->find_child(arw);
                if(id != -1)
                    this->remove_child(id);
                 arw.clear();
            }
        }
}


Glib::RefPtr<ApplicationModel> ApplicationModel::create(ApplicationWindow* parentWnd, Application* app)
{
    return Glib::RefPtr<ApplicationModel>(new ApplicationModel(parentWnd, app));
}


bool ApplicationModel::onItemButtonPressEvent(const Glib::RefPtr<Goocanvas::Item>& item,
                    GdkEventButton* event)
{
    if(item && Glib::RefPtr<InternalPortModel>::cast_dynamic(item->get_model()))
        return Glib::RefPtr<InternalPortModel>::cast_dynamic(item->get_model())->onItemButtonPressEvent(item, event);

    if(event->type == GDK_2BUTTON_PRESS)
    {
       //parentWindow->onUpdateApplicationProperty(application);
       return true;
    }

    if(event->button == 1)
    {
        _dragging = item ;
        _drag_x = (int) event->x ;
        _drag_y = (int) event->y ;
        return false;
    }
    else
    if(event->button == 3)
    {
        MainWindow* wnd = parentWindow->getMainWindow();
        wnd->m_refActionGroup->get_action("EditDelete")->set_sensitive(selected);
        wnd->m_refActionGroup->get_action("EditCopy")->set_sensitive(selected);
        Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
        wnd->m_refUIManager->get_widget("/PopupModuleModel"));
        if(pMenu)
            pMenu->popup(event->button, event->time);
    }

    return true;
}

bool ApplicationModel::onItemButtonReleaseEvent(const Glib::RefPtr<Goocanvas::Item>& item,
                    GdkEventButton* event)
{
  if(event->button == 1)
  {
        snapToGrid();
       _dragging.clear() ;
  }
  return true;
}

bool ApplicationModel::onItemMotionNotifyEvent(const Glib::RefPtr<Goocanvas::Item>& item,
                    GdkEventMotion* event)
{
    if(item && _dragging && (item == _dragging))
    {
        parentWindow->setModified();

        double new_x = event->x ;
        double new_y = event->y ;
        item->get_parent()->translate(new_x - _drag_x, new_y - _drag_y);

        Goocanvas::Bounds bi = item->get_parent()->get_bounds();
        bi = item->get_parent()->get_bounds();

        if(bi.get_x1() < 0)
            item->get_parent()->translate(-bi.get_x1(), 0);
        if(bi.get_y1() < 0)
            item->get_parent()->translate(0, -bi.get_y1());

        snapToGrid();

        bi = item->get_parent()->get_bounds();
        this->points.clear();
        GyPoint pt;
        pt.x = bi.get_x1();
        pt.y = bi.get_y1();
        this->points.push_back(pt);

        for(int i=0; i<get_n_children(); i++)
        {
            if(Glib::RefPtr<ModuleModel>::cast_dynamic(get_child(i)))
                Glib::RefPtr<ModuleModel>::cast_dynamic(get_child(i))->updateArrowCoordination();
            if(Glib::RefPtr<ExternalPortModel>::cast_dynamic(get_child(i)))
                Glib::RefPtr<ExternalPortModel>::cast_dynamic(get_child(i))->updateArrowCoordination();
        }

        // updating canvas boundries
        bool needUpdate = false;
        if(parentWindow->m_Canvas)
        {
            Goocanvas::Bounds bc;
            parentWindow->m_Canvas->get_bounds(bc);
            needUpdate = (bi.get_x2() > bc.get_x2()) || (bi.get_y2() > bc.get_y2());
            if(needUpdate)
            {
                new_x = (bi.get_x2() > bc.get_x2()) ? bi.get_x2() : bc.get_x2();
                new_y = (bi.get_y2() > bc.get_y2()) ? bi.get_y2() : bc.get_y2();
                parentWindow->m_Canvas->set_bounds(0,0, new_x, new_y);
                if(parentWindow->m_Grid)
                {
                    g_object_set(parentWindow->m_Grid, "width", new_x, NULL);
                    g_object_set(parentWindow->m_Grid, "height", new_y, NULL);
                }
            }
        }

    }
    return true;
}

bool ApplicationModel::onItemEnterNotify(const Glib::RefPtr<Goocanvas::Item>& item,
                    GdkEventCrossing* event)
{
    /*
    if(item && Glib::RefPtr<InternalPortModel>::cast_dynamic(item->get_model()))
        Glib::RefPtr<InternalPortModel>::cast_dynamic(item->get_model())->onItemEnterNotify(item, event);
    */
    return true;
}

bool ApplicationModel::onItemLeaveNotify(const Glib::RefPtr<Goocanvas::Item>& item,
                    GdkEventCrossing* event)
{
    /*
    if(item && Glib::RefPtr<InternalPortModel>::cast_dynamic(item->get_model()))
        Glib::RefPtr<InternalPortModel>::cast_dynamic(item->get_model())->onItemLeaveNotify(item, event);
    */
    return true;
}

void ApplicationModel::setArrowsSelected(bool sel)
{
    for(int i=0; i<this->get_n_children(); i++)
    {
        Glib::RefPtr<ModuleModel> module =
            Glib::RefPtr<ModuleModel>::cast_dynamic(this->get_child(i));
        if(module)
            module->setArrowsSelected(sel);
    }
}


void ApplicationModel::setSelected(bool sel)
{
    selected = sel;
    if(selected)
    {
        poly->property_fill_color().set_value("LightSteelBlue");
        this->raise();
    }
    else
    {
        poly->property_fill_color_rgba().set_value(APPLICATION_COLOR);
        for(int i=0; i<parentWindow->getRootModel()->get_n_children(); i++)
        {
            if(Glib::RefPtr<ArrowModel>::cast_dynamic(parentWindow->getRootModel()->get_child(i)))
                this->lower(parentWindow->getRootModel()->get_child(i));
        }
    }

}

void ApplicationModel::updateBounds(void)
{
    for(int i=0; i<this->get_n_children(); i++)
    {
        Glib::RefPtr<Goocanvas::Item> item = parentWindow->m_Canvas->get_item(this->get_child(i));
        if(item)
        {
            Goocanvas::Bounds bd = item->get_bounds();
            bounds.set_x1( (bounds.get_x1() < bd.get_x1()) ? bounds.get_x1(): bd.get_x1() );
            bounds.set_x2( (bounds.get_x2() > bd.get_x2()) ? bounds.get_x2(): bd.get_x2() );
            bounds.set_y1( (bounds.get_y1() < bd.get_y1()) ? bounds.get_y1(): bd.get_y1() );
            bounds.set_y2( (bounds.get_y2() > bd.get_y2()) ? bounds.get_y2(): bd.get_y2() );
        }
    }

    width = MAX(MIN_WIDTH + text_w, bounds.get_x2()-bounds.get_x1() + 2*ITEMS_MARGIE);
    height = MAX(MIN_HEIGHT, bounds.get_y2()-bounds.get_y1() + 2*ITEMS_MARGIE + text_h);

    Goocanvas::Points points(6);
    points.set_coordinate(0, 0, 0);
    points.set_coordinate(1, text_w+TEXT_MARGINE*2, 0);
    points.set_coordinate(2, text_w+TEXT_MARGINE*2, text_h+TEXT_MARGINE);
    points.set_coordinate(3, width, text_h+TEXT_MARGINE);
    points.set_coordinate(4, width, height);
    points.set_coordinate(5, 0, height);
    poly->property_points().set_value(points);

    Goocanvas::Points points3(3);
    points3.set_coordinate(0, width, text_h+TEXT_MARGINE);
    points3.set_coordinate(1, width, height);
    points3.set_coordinate(2, 0, height);
    shadow->property_points().set_value(points3);

}

void ApplicationModel::updateCoordination(void)
{
    for(int i=0; i<this->get_n_children(); i++)
    {
        Glib::RefPtr<ModuleModel> mod =
            Glib::RefPtr<ModuleModel>::cast_dynamic(this->get_child(i));
        if(mod)
            mod->updateArrowCoordination();
    }
}



void ApplicationModel::snapToGrid(void)
{
    GooCanvasItemModel* model = (GooCanvasItemModel*) this->gobj();
    GooCanvas* canvas = (GooCanvas*) parentWindow->m_Canvas->gobj();
    if(model && canvas)
    {
        GooCanvasItem* item = goo_canvas_get_item(canvas, model);
        GooCanvasBounds bi;
        goo_canvas_item_get_bounds(item, &bi);
        double xs, ys;
        g_object_get (parentWindow->m_Grid,
               "x_step", &xs,
               "y_step", &ys,
               NULL);
        double c_x = (int) (((int)bi.x1) % (int)xs);
        double c_y = (int) (((int)bi.y1) % (int)ys);
        //printf("cx:%f, cy:%f\n", c_x, c_y);
        //printf("x:%f, y:%f\n", bi.x1, bi.y1);
        if(parentWindow->m_snapToGrid)
            goo_canvas_item_translate(item, -c_x, -c_y);
        // force update
        goo_canvas_item_get_bounds(item, &bi);
        this->points.clear();
        GyPoint pt;
        pt.x = bi.x1;
        pt.y = bi.y1;
        this->points.push_back(pt);
        updateCoordination();
    }
}


