/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 *  Most part of the code in this file is taken from the article "Different CellRenderers on the same Column" (http://kapo-cpp.blogspot.it/).
 *
 */

#ifndef __CUSTOM_CELLRENDER__
#define __CUSTOM_CELLRENDER__


#include "ymm-types.h"
#include <gtkmm.h>


/**
 * @brief Interface for CellRenderer stored on CellRendererRouter
 */
class CellRendererChildInterface
{
public:
    CellRendererChildInterface()
    {
    }
    virtual ~CellRendererChildInterface()
    {    
    }
    
    virtual void set_text(const Glib::ustring& value) = 0;
    
    virtual void get_size_vfunc (Gtk::Widget& widget, const Gdk::Rectangle* cell_area, int* x_offset, int* y_offset, int* width, int* height) const = 0;
    virtual void render_vfunc (const Glib::RefPtr<Gdk::Drawable>& window, Gtk::Widget& widget, const Gdk::Rectangle& background_area, const Gdk::Rectangle& cell_area, const Gdk::Rectangle& expose_area, Gtk::CellRendererState flags) = 0;
    virtual bool activate_vfunc (GdkEvent* event, Gtk::Widget& widget, const Glib::ustring& path, const Gdk::Rectangle& background_area, const Gdk::Rectangle& cell_area, Gtk::CellRendererState flags) = 0;
    virtual Gtk::CellEditable* start_editing_vfunc(GdkEvent* event, Gtk::Widget& widget, const Glib::ustring& path, const Gdk::Rectangle& background_area, const Gdk::Rectangle& cell_area, Gtk::CellRendererState flags) = 0;
};



/**
 * @brief Proxy class for CellRenderer stored on CellRendererRouter
 * 
 * Implement the CellRendererChildInterface for expose public functions
 * and forward call to the baseT (a CellRenderer) class.
 * 
 * The baseT class must implement properties:
 * - property_editable of type bool
 * - property_text of type Glib::ustring
 * 
 * @param baseT a CellRenderer inherited class
 */
template< class baseT >
class CellRendererProxy : public virtual CellRendererChildInterface, public baseT
{
public:
    CellRendererProxy(CellRendererRouter* parent_ptr) :
        Glib::ObjectBase( typeid(CellRendererProxy) ),
        baseT(),
        parent_ptr_( parent_ptr )
    {
        //Make the CellRenderer editable, and handle its editing signals:
#ifdef GLIBMM_PROPERTIES_ENABLED
        baseT::property_editable() = true;
#else
        baseT::set_property("editable", true);
#endif
        baseT::signal_edited().connect (sigc::mem_fun (*this, &CellRendererProxy::onEdited));
        
    }
    virtual ~CellRendererProxy()
    {
        
    }

    /* implements */virtual void set_text(const Glib::ustring& value)
    {
#ifdef GLIBMM_PROPERTIES_ENABLED
        baseT::property_text() = value;
#else
        baseT::set_property("text", value);
#endif        
    }
    
    /* implements */virtual void get_size_vfunc (Gtk::Widget& widget, const Gdk::Rectangle* cell_area, int* x_offset, int* y_offset, int* width, int* height) const
    {
        baseT::get_size_vfunc (widget, cell_area, x_offset, y_offset, width, height);
    }
    
    /* implements */virtual void render_vfunc (const Glib::RefPtr<Gdk::Drawable>& window, Gtk::Widget& widget, const Gdk::Rectangle& background_area, const Gdk::Rectangle& cell_area, const Gdk::Rectangle& expose_area, Gtk::CellRendererState flags)
    {
        baseT::render_vfunc (window, widget, background_area, cell_area, expose_area, flags);
    }
    
    /* implements */virtual bool activate_vfunc (GdkEvent* event, Gtk::Widget& widget, const Glib::ustring& path, const Gdk::Rectangle& background_area, const Gdk::Rectangle& cell_area, Gtk::CellRendererState flags)
    {
        return baseT::activate_vfunc (event, widget, path, background_area, cell_area, flags);
    }
    
    /* implements */virtual Gtk::CellEditable* start_editing_vfunc(GdkEvent* event, Gtk::Widget& widget, const Glib::ustring& path, const Gdk::Rectangle& background_area, const Gdk::Rectangle& cell_area, Gtk::CellRendererState flags)
    {
        return baseT::start_editing_vfunc(event, widget, path, background_area, cell_area, flags);
    }
    
protected:
    CellRendererRouter* parent_ptr_;
    
    void onEdited(const Glib::ustring& path_string, const Glib::ustring& new_text);
};

/**
 * @brief Tell the CellRendererRouter to emit the edited signal
 */
template< class baseT >
void CellRendererProxy< baseT >::onEdited(const Glib::ustring& path_string, const Glib::ustring& new_text)
{
    parent_ptr_->notifyEdited (path_string, new_text);
};




/**
 * @brief Multiple CellRenderer for single column
 * 
 * Add a CellRendererRouter to a TreeView for implement
 * multiple CellRenderer for a single colum.
 * The CellRendererRouter is called by the system for
 * render or start edit, and it simply forward the call
 * to the registered CellRenderer for the current type.
 * Use registerTypeRenderer for define all CellRenderer
 * managed by the CellRendererRouter class.
 */
class CellRendererRouter : public Gtk::CellRenderer
{
public:
    typedef CellRendererChildInterface* cell_renderer_ptr_t;
    typedef std::map< Glib::ustring, cell_renderer_ptr_t > cell_renderer_map_t;
    typedef cell_renderer_map_t::iterator iterator_t;
    
    CellRendererRouter();
    virtual ~CellRendererRouter();
    
    /**
     * @brief Register a CellRenderer for a type-id
     * 
     * At least one CellRenderer must be registered and at least one
     * CellRenderer must be registered as default. The default CellRenderer
     * is used when the system ask for some operation on a row that
     * was not defined. If more than one CellRenderer are
     * registered as default only the last one is the default.
     * 
     * @param type_id the type-id
     * @param renderer_ptr the CellRenderer
     * @param is_default use the CellRenderer for unknown path
     */
    void registerTypeRenderer(const Glib::ustring& type_id, cell_renderer_ptr_t renderer_ptr, bool is_default=false);

    /**
     * @brief Select a registered CellRenderer as default
     * 
     * @param type_id type-id of a registered CellRenderer
     */
    void selectDefaultTypeRenderer(const Glib::ustring& type_id);

    /**
     * @brief Return the CellRenderer registered for gived type-id
     * 
     * If no CellRenderer is registered for gived type-id the default
     * CellRenderer is returned
     * 
     * @param type_id the type-id to search for
     * 
     * @return the CellRenderer registered for gived type-id or NULL if
     * no CellRenderer is registerer and the default CellRenderer is not set
     */
    cell_renderer_ptr_t getTypeRenderer(const Glib::ustring& type_id) const; 
    
    /**
     * @brief Remove an already registered CellRenderer
     * 
     * When a default CellRenderer is removed call selectDefaultTypeRenderer
     * for select a new default CellRenderer.
     * 
     * @param type_id the type-id of the registered CellRenderer to remove
     */
    void unregisterTypeRenderer(const Glib::ustring& type_id);

    /**
     * @brief Remove all registered CellRenderer
     */
    void unregisterAll();

    /**
     * @brief Emit the edited signal
     * 
     * @param path_string path of edited item
     * @param new_text new value of the item
     */
    void notifyEdited(const Glib::ustring& path_string, const Glib::ustring& new_text);
    
    /** @name Properties
     */    
    //@{
    
    /**
     * @brief Item text value
     */
    Glib::PropertyProxy< Glib::ustring > property_text();
    
    /**
     * @brief Item type
     */
    Glib::PropertyProxy< Glib::ustring > property_type();
    
    /**
     * @brief Item editable state
     */
    Glib::PropertyProxy< bool > property_editable();
    
    //@}
    
    /** @name Signals
     */    
    //@{

    typedef sigc::signal< void, const Glib::ustring &, const Glib::ustring& > signal_edited_t;
    signal_edited_t& signal_edited();
    
    //@}

protected:
    cell_renderer_map_t type_renderer_map_;
    Glib::ustring default_renderer_;
    Glib::Property< Glib::ustring > property_text_;
    Glib::Property< Glib::ustring > property_type_;
    Glib::Property< bool > property_editable_;
    signal_edited_t signal_edited_;
    
    /* override */virtual void get_size_vfunc (Gtk::Widget& widget, const Gdk::Rectangle* cell_area, int* x_offset, int* y_offset, int* width, int* height) const;
    /* override */virtual void render_vfunc (const Glib::RefPtr<Gdk::Drawable>& window, Gtk::Widget& widget, const Gdk::Rectangle& background_area, const Gdk::Rectangle& cell_area, const Gdk::Rectangle& expose_area, Gtk::CellRendererState flags);
    /* override */virtual bool activate_vfunc (GdkEvent* event, Gtk::Widget& widget, const Glib::ustring& path, const Gdk::Rectangle& background_area, const Gdk::Rectangle& cell_area, Gtk::CellRendererState flags);
    /* override */virtual Gtk::CellEditable* start_editing_vfunc(GdkEvent* event, Gtk::Widget& widget, const Glib::ustring& path, const Gdk::Rectangle& background_area, const Gdk::Rectangle& cell_area, Gtk::CellRendererState flags);
    
    // property_text events
    virtual void onTextChanged();
};


void CellRendererRouter::registerTypeRenderer(const Glib::ustring& type_id, cell_renderer_ptr_t renderer_ptr, bool is_default/*=false*/)
{
    if (type_renderer_map_.find(type_id) != type_renderer_map_.end())
        throw std::runtime_error ( "A CellRenderer was already registered for type: " + type_id );
    
    // TODO: who manage the pointer?
    type_renderer_map_.insert (std::make_pair (type_id, renderer_ptr));
    
    if (is_default)
        default_renderer_ = type_id;
}

void CellRendererRouter::selectDefaultTypeRenderer(const Glib::ustring& type_id)
{
    default_renderer_ = type_id;
}

CellRendererRouter::cell_renderer_ptr_t CellRendererRouter::getTypeRenderer(const Glib::ustring& type_id) const
{
    // Search for the CellRenderer to use
    if (type_renderer_map_.find (type_id) != type_renderer_map_.end())
    {
        // Use registered CellRenderer
        return type_renderer_map_.at(property_type_);
    }
    else
    {
        // Use default CellRenderer
        if (type_renderer_map_.find (default_renderer_) != type_renderer_map_.end())
        {
            return type_renderer_map_.at(default_renderer_);
        }
    }
    
    // TODO: Critical, throw an exception? 
    return 0;
}

void CellRendererRouter::unregisterTypeRenderer(const Glib::ustring& type_id)
{
    if (type_renderer_map_.find(type_id) == type_renderer_map_.end())
        throw std::runtime_error ( "Non existent CellRenderer registered for type: " + type_id );
    
    type_renderer_map_.erase (type_id);
    
    if (default_renderer_ == type_id)
        default_renderer_.clear();
}

void CellRendererRouter::unregisterAll()
{
    type_renderer_map_.clear();
    default_renderer_.clear();
}

void CellRendererRouter::get_size_vfunc (Gtk::Widget& widget, const Gdk::Rectangle* cell_area, int* x_offset, int* y_offset, int* width, int* height) const
{
    getTypeRenderer(property_type_)->get_size_vfunc (widget, cell_area, x_offset, y_offset, width, height);
}

void CellRendererRouter::render_vfunc (const Glib::RefPtr<Gdk::Drawable>& window, Gtk::Widget& widget, const Gdk::Rectangle& background_area, const Gdk::Rectangle& cell_area, const Gdk::Rectangle& expose_area, Gtk::CellRendererState flags)
{
    getTypeRenderer(property_type_)->render_vfunc (window, widget, background_area, cell_area, expose_area, flags);
}

bool CellRendererRouter::activate_vfunc (GdkEvent* event, Gtk::Widget& widget, const Glib::ustring& path, const Gdk::Rectangle& background_area, const Gdk::Rectangle& cell_area, Gtk::CellRendererState flags)
{
    return getTypeRenderer(property_type_)->activate_vfunc (event, widget, path, background_area, cell_area, flags);
}

Gtk::CellEditable* CellRendererRouter::start_editing_vfunc(GdkEvent* event, Gtk::Widget& widget, const Glib::ustring& path, const Gdk::Rectangle& background_area, const Gdk::Rectangle& cell_area, Gtk::CellRendererState flags)
{
    return getTypeRenderer(property_type_)->start_editing_vfunc(event, widget, path, background_area, cell_area, flags);
}


#endif //__CUSTOM_CELLRENDER_

