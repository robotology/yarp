/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: Apache-2.0
 */

#include "thrift/generate/t_generator.h"
#include "thrift/generate/t_oop_generator.h"
#include "thrift/platform.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <sys/stat.h>

# define THRIFT_DEBUG_COMMENT(out) if (debug_generator_) { out << "/* " << __FUNCTION__ << ":" << __LINE__ << " */\n"; }

/**
 * YARP code generator
 *
 * Serialization:
 *   Bottle compatible, why not.
 *   BOTTLE_CODE
 *   LENGTH
 *
 * mostly copy/pasting/tweaking from mcslee's work.
 */
class t_yarp_generator : public t_oop_generator
{
    // Options
    bool use_include_prefix_{false};
    bool no_namespace_prefix_{true};
    bool no_copyright_{false};
    bool no_editor_{false};
    bool no_doc_{false};
    bool debug_generator_{false};

    // Other
    bool need_common_{false}; //are there consts and typedef that we need to keep in a common file?
    int indent_h_{0};
    int indent_cpp_{0};

    // Files
    ofstream_with_content_based_conditional_update f_out_common_;
    ofstream_with_content_based_conditional_update f_out_index_;

    std::map<std::string, std::string> structure_names_;

public:
    t_yarp_generator(t_program* program,
                     const std::map<std::string, std::string>& parsed_options,
                     const std::string& option_string) :
            t_oop_generator{program},
            use_include_prefix_{parsed_options.find("include_prefix") != parsed_options.end()},
            no_namespace_prefix_{parsed_options.find("no_namespace_prefix") != parsed_options.end()},
            no_copyright_{parsed_options.find("no_copyright") != parsed_options.end()},
            no_editor_{parsed_options.find("no_editor") != parsed_options.end()},
            no_doc_{parsed_options.find("no_doc") != parsed_options.end()},
            debug_generator_{parsed_options.find("debug_generator") != parsed_options.end()}
    {
        out_dir_base_ = "gen-yarp";
    }

/******************************************************************************/
// BEGIN Indentation methods

    // Generic
    std::string indent_str() const override
    {
        return "    ";
    }

    std::string indent_initializer_str() const
    {
        return "        ";
    }

    std::string indent_whitespaces_str(size_t whitespaces) const
    {
        std::string ind = "";
        for (size_t i = 0; i < whitespaces; ++i) {
            ind += ' ';
        }
        return ind;
    }

    // Implementations
    using indent_count_t = std::function<int()>;
    using indent_t = std::function<std::string()>;

    std::string indent_impl(indent_count_t indent_count_fn)
    {
        std::string ind = "";
        for (int i = 0; i < indent_count_fn(); ++i) {
            ind += indent_str();
        }
        return ind;
    }

    std::string indent_access_specifier_str_impl(indent_count_t indent_count_fn) {
        std::string ind = "";
        for (int i = 0; i < indent_count_fn() - 1; ++i) {
            ind += indent_str();
        }
        return ind;
    }

    std::string inline_return_impl(indent_t indent_fn, const std::string& val = "")
    {
        std::string str = " {\n";
        str += indent_fn();
        str += indent_str();
        str += "return";
        if (!val.empty()) {
            str += " ";
            str += val;
        }
        str += ";\n";
        str += indent_fn();
        str += "}\n";
        return str;
    }

    std::string indent_initializer_impl(indent_t indent_fn)
    {
        std::string str = indent_fn();
        str += indent_initializer_str();
        return str;
    }

    std::string indent_whitespaces_impl(indent_t indent_fn, size_t whitespaces)
    {
        std::string str = indent_fn();
        str += indent_whitespaces_str(whitespaces);
        return str;
    }

    // Base
    std::string indent_access_specifier_str() { return indent_access_specifier_str_impl([this](){ return indent_count(); }); }
    std::string inline_return(const std::string& val = "") { return inline_return_impl([this](){ return indent(); }, val); }
    std::string indent_initializer() { return indent_initializer_impl([this](){ return indent(); }); }
    std::string indent_whitespaces(size_t whitespaces) { return indent_whitespaces_impl([this](){ return indent(); }, whitespaces); }
    std::string indent_access_specifier() { return indent_access_specifier_str_impl([this](){ return indent_count(); }); }

    // .h
    void indent_up_h() { ++indent_h_; }
    void indent_down_h() { --indent_h_; }
    int indent_count_h() const { return indent_h_; }
    std::string indent_h() { return indent_impl([this](){ return indent_count_h(); }); }
    std::string indent_access_specifier_str_h() { return indent_access_specifier_str_impl([this](){ return indent_count_h(); }); }
    std::string inline_return_h(const std::string& val = "") { return inline_return_impl([this](){ return indent_h(); }, val); }
    std::string indent_initializer_h() { return indent_initializer_impl([this](){ return indent_h(); }); }
    std::string indent_whitespaces_h(size_t whitespaces) { return indent_whitespaces_impl([this](){ return indent_h(); }, whitespaces); }
    std::string indent_access_specifier_h() { return indent_access_specifier_str_impl([this](){ return indent_count_h(); }); }

    // .cpp
    void indent_up_cpp() { ++indent_cpp_; }
    void indent_down_cpp() { --indent_cpp_; }
    int indent_count_cpp() const { return indent_cpp_; }
    std::string indent_cpp() { return indent_impl([this](){ return indent_count_cpp(); }); }
    std::string indent_access_specifier_str_cpp() { return indent_access_specifier_str_impl([this](){ return indent_count_cpp(); }); }
    std::string inline_return_cpp(const std::string& val = "") { return inline_return_impl([this](){ return indent_cpp(); }, val); }
    std::string indent_initializer_cpp() { return indent_initializer_impl([this](){ return indent_cpp(); }); }
    std::string indent_whitespaces_cpp(size_t whitespaces) { return indent_whitespaces_impl([this](){ return indent_cpp(); }, whitespaces); }
    std::string indent_access_specifier_cpp() { return indent_access_specifier_str_impl([this](){ return indent_count_cpp(); }); }

// END Indentation methods
/******************************************************************************/

    std::string get_include_prefix(const t_program* const program) const;
    void get_needed_type(t_type* curType, std::set<std::string>& neededTypes);

    void print_doc(std::ostringstream& out, t_doc* tdoc);
    void quote_doc(std::vector<std::string>& doxyPar, t_doc* tdoc);
    std::vector<std::string> print_help(t_function* tdoc);
    std::string print_const_value(t_const_value* tvalue, t_type* ttype = nullptr);
    std::string function_prototype(t_function* tfn,
                                   bool include_defaults,
                                   bool include_returntype = true,
                                   bool include_qualifiers = false,
                                   const std::string& prefix = {},
                                   const std::string& override_name = {});
    std::string declare_field(t_field* tfield,
                              bool init = false,
                              bool pointer = false,
                              bool is_constant = false,
                              bool reference = false,
                              const std::string& force_type = {},
                              const std::string& prefix = {});

    std::string type_name(t_type* ttype, bool in_typedef = false, bool arg = false, bool ret = false);
    std::string base_type_name(t_base_type::t_base tbase);
    std::string namespace_prefix(std::string ns);
    void namespace_open(std::ostream& out, std::string ns);
    void namespace_close(std::ostream& out, std::string ns);

    bool is_complex_type(t_type* ttype);

    void generate_serialize_field(std::ostringstream& out,
                                  t_field* tfield,
                                  const std::string& prefix = "",
                                  const std::string& suffix = "",
                                  bool force_nesting = false);

    void generate_serialize_struct(std::ostringstream& out,
                                   t_struct* tstruct,
                                   const std::string& prefix = "",
                                   bool force_nesting = false);

    void generate_serialize_container(std::ostringstream& out,
                                      t_type* ttype,
                                      const std::string& prefix = "");

    void generate_serialize_map_element(std::ostringstream& out,
                                        t_map* tmap,
                                        const std::string& item_name);

    void generate_serialize_set_element(std::ostringstream& out,
                                        t_set* tmap,
                                        const std::string& item_name);

    void generate_serialize_list_element(std::ostringstream& out,
                                         t_list* tlist,
                                         const std::string& item_name);

    void generate_function_call(std::ostream& out,
                                t_function* tfunction,
                                const std::string& target,
                                const std::string& iface,
                                const std::string& arg_prefix);

    void generate_deserialize_field_fallback(std::ostringstream& out,
                                             t_field* tfield);

    void generate_deserialize_field(std::ostringstream& out,
                                    t_field* tfield,
                                    const std::string& prefix = "",
                                    const std::string& suffix = "",
                                    bool force_nested = false);

    void generate_deserialize_struct(std::ostringstream& out,
                                     t_struct* tstruct,
                                     const std::string& prefix = "",
                                     bool force_nested = true);

    void generate_deserialize_container(std::ostringstream& out,
                                        t_type* ttype,
                                        const std::string& prefix = "");

    void generate_deserialize_set_element(std::ostringstream& out,
                                          t_set* tset,
                                          const std::string& prefix = "");

    void generate_deserialize_map_element(std::ostringstream& out,
                                          t_map* tmap,
                                          const std::string& prefix = "");

    void generate_deserialize_list_element(std::ostringstream& out,
                                           t_list* tlist,
                                           const std::string& prefix,
                                           bool push_back,
                                           const std::string& index);

    std::string type_to_enum(t_type* ttype);

    std::string get_struct_name(t_struct* tstruct);

    void print_const_value(std::ostringstream& out, const std::string& name, t_type* type, t_const_value* value);
    std::string render_const_value(std::ostringstream& out, const std::string& name, t_type* type, t_const_value* value);

    int flat_element_count(t_type* type);
    int flat_element_count(t_struct* type);
    int flat_element_count(t_function* fn);

    std::string copyright_comment();
    std::string autogen_comment() override;

    void init_generator() override;
    void close_generator() override;

    void generate_namespace_open(std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_namespace_close(std::ostringstream& f_h_, std::ostringstream& f_cpp_);

    void generate_typedef(t_typedef* ttypedef) override;

    void generate_enum(t_enum* tenum) override;
    void generate_enum_constant_list(t_enum* tenum, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_enum_fromstring(t_enum* tenum, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_enum_tostring(t_enum* tenum, std::ostringstream& f_h_, std::ostringstream& f_cpp_);

    void generate_const(t_const* tconst) override;

    void generate_struct(t_struct* tstruct) override;
    void generate_struct_fields(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_default_constructor(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_constructor(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_read_wirereader(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_read_connectionreader(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_write_wirereader(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_write_connectionreader(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_tostring(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_unwrapped_helper(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_default_constructor(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_baseclass_constructor(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_destructor(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_edit(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_isvalid(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_state(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_start_editing(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_stop_editing(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_field_setter(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_field_setter_list(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_field_getter(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_field_will_set(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_field_did_set(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_clean(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_read(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_write(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_communicate(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_mark_dirty(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_field_mark_dirty(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_editor_dirty_flags(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_field_read(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_field_write(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_field_nested_read(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_struct_field_nested_write(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_);

    void generate_service(t_service* tservice) override;
    void generate_service_helper_classes(t_service* tservice, std::ostringstream& f_cpp_);
    void generate_service_helper_classes_decl(t_function* function, std::ostringstream& f_cpp);
    void generate_service_helper_classes_impl_ctor(t_function* function, std::ostringstream& f_cpp_);
    void generate_service_helper_classes_impl_write(t_function* function, std::ostringstream& f_cpp_);
    void generate_service_helper_classes_impl_read(t_function* function, std::ostringstream& f_cpp_);

    void generate_service_constructor(t_service* tservice, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_service_method(t_service* tservice, t_function* function, std::ostringstream&  f_h_, std::ostringstream& f_cpp_);
    void generate_service_help(t_service* tservice, std::ostringstream& f_h_, std::ostringstream& f_cpp_);
    void generate_service_read(t_service* tservice, std::ostringstream& f_h_, std::ostringstream& f_cpp_);

    void generate_xception(t_struct* txception) override;
};


std::string t_yarp_generator::type_to_enum(t_type* type)
{
    type = get_true_type(type);

    if (type->is_base_type()) {
        t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
        switch (tbase) {
        case t_base_type::TYPE_VOID:
            throw "NO T_VOID CONSTRUCT";
        case t_base_type::TYPE_STRING:
            return "BOTTLE_TAG_STRING";
        case t_base_type::TYPE_BOOL:
            return "BOTTLE_TAG_VOCAB32";
        case t_base_type::TYPE_I8:
            return "BOTTLE_TAG_INT8";
        case t_base_type::TYPE_I16:
            return "BOTTLE_TAG_INT16";
        case t_base_type::TYPE_I32:
        {
            auto it = type->annotations_.find("yarp.type");
            if (it != type->annotations_.end() && it->second == "yarp::conf::vocab32_t") {
                return "BOTTLE_TAG_VOCAB32";
            }
            return "BOTTLE_TAG_INT32";
        }
        case t_base_type::TYPE_I64:
            return "BOTTLE_TAG_INT64";
        case t_base_type::TYPE_DOUBLE:
        {
            auto it = type->annotations_.find("yarp.type");
            if (it != type->annotations_.end() && it->second == "yarp::conf::float32_t") {
                return "BOTTLE_TAG_FLOAT32";
            } else {
                return "BOTTLE_TAG_FLOAT64";
            }
        }
        }
    } else if (type->is_enum()) {
        return "BOTTLE_TAG_INT32";
    } else if (type->is_struct()) {
        return "BOTTLE_TAG_LIST";
    } else if (type->is_xception()) {
        return "::apache::thrift::protocol::T_STRUCT";
    } else if (type->is_map()) {
        return "BOTTLE_TAG_LIST";
    } else if (type->is_set()) {
        return "BOTTLE_TAG_LIST";
    } else if (type->is_list()) {
        return "BOTTLE_TAG_LIST";
    }

    throw "INVALID TYPE IN type_to_enum: " + type->get_name();
}

std::string t_yarp_generator::get_struct_name(t_struct* tstruct)
{
    std::string name = tstruct->get_name();
    if (structure_names_.find(name) == structure_names_.end()) {
        return name;
    }
    return structure_names_[name];
}

std::string t_yarp_generator::copyright_comment()
{
    std::string ret = "";
    if (!no_copyright_) {
        ret += "/*\n";
        ret += " * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)\n";
        ret += " * SPDX-License-Identifier: BSD-3-Clause\n";
        ret += " */\n";
        ret += "\n";
    }
    return ret;
}

std::string t_yarp_generator::autogen_comment()
{
    std::string ret = "";
    ret += "// " + autogen_summary() + "\n";
    ret += "//\n";
    ret += "// This is an automatically generated file.\n";
    ret += "// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.\n";
    ret += "\n";
    return ret;
}

/////////////////////////////////////////////////////////////////////
/// C++ generator code begins
/////////////////////////////////////////////////////////////////////

std::string t_yarp_generator::type_name(t_type* ttype, bool in_typedef, bool arg, bool ret)
{
    if (ttype->is_base_type()) {
        std::string bname = base_type_name(((t_base_type*)ttype)->get_base());
        auto it = ttype->annotations_.find("yarp.type");
        if (it != ttype->annotations_.end()) {
            bname = it->second;
        }

        if (!arg) {
            return bname;
        }

        if (((t_base_type*)ttype)->get_base() == t_base_type::TYPE_STRING) {
            return "const " + bname + "&";
        }

        if (ret) {
            return bname;
        }

        return "const " + bname;
    }

    // Check for a custom overloaded C++ name
    if (ttype->is_container()) {
        std::string cname;

        t_container* tcontainer = (t_container*)ttype;
        if (tcontainer->has_cpp_name()) {
            cname = tcontainer->get_cpp_name();
        } else if (ttype->is_map()) {
            t_map* tmap = (t_map*)ttype;
            cname = "std::map<" + type_name(tmap->get_key_type(), in_typedef) + ", " + type_name(tmap->get_val_type(), in_typedef) + ">";
        } else if (ttype->is_set()) {
            t_set* tset = (t_set*)ttype;
            cname = "std::set<" + type_name(tset->get_elem_type(), in_typedef) + ">";
        } else if (ttype->is_list()) {
            t_list* tlist = (t_list*)ttype;
            cname = "std::vector<" + type_name(tlist->get_elem_type(), in_typedef) + ">";
        }

        if (arg) {
            return "const " + cname + "&";
        } else {
            return cname;
        }
    }

    std::string class_prefix;
    if (in_typedef && (ttype->is_struct() || ttype->is_xception())) {
        class_prefix = "class ";
    }

    // Check if it needs to be namespaced
    std::string pname;

    if (ttype->is_struct()) {
        if (structure_names_.find(ttype->get_name()) != structure_names_.end()) {
            pname = structure_names_[ttype->get_name()];
        }
    }

    if (pname == "") {
        t_program* program = ttype->get_program();
        if (program != nullptr && program != program_) {
            pname = class_prefix + namespace_prefix(program->get_namespace("yarp")) + ttype->get_name();
        } else {
            pname = class_prefix + ttype->get_name();
        }
    }

    if (arg) {
        if (is_complex_type(ttype)) {
            return "const " + pname + "&";
        } else if (ret) {
            return pname;
        } else {
            return "const " + pname;
        }
    } else {
        return pname;
    }
}

std::string t_yarp_generator::base_type_name(t_base_type::t_base tbase)
{
    switch (tbase) {
    case t_base_type::TYPE_VOID:
        return "void";
    case t_base_type::TYPE_STRING:
        return "std::string";
    case t_base_type::TYPE_BOOL:
        return "bool";
    case t_base_type::TYPE_I8:
        return "std::int8_t";
    case t_base_type::TYPE_I16:
        return "std::int16_t";
    case t_base_type::TYPE_I32:
        return "std::int32_t";
    case t_base_type::TYPE_I64:
        return "std::int64_t";
    case t_base_type::TYPE_DOUBLE:
        return "double";
    default:
        throw "compiler error: no C++ base type name for base type " + t_base_type::t_base_name(tbase);
    }
}

std::string t_yarp_generator::namespace_prefix(std::string ns)
{
    if (ns.empty()) {
        return {};
    }
    std::string result;
    std::string::size_type loc;
    while ((loc = ns.find(".")) != std::string::npos) {
        result += ns.substr(0, loc);
        result += "::";
        ns = ns.substr(loc + 1);
    }
    if (ns.size() > 0) {
        result += ns + "::";
    }
    return result;
}

void t_yarp_generator::namespace_open(std::ostream& out, std::string ns)
{
    if (ns.empty()) {
        return;
    }

    ns += ".";
    std::string separator = "";
    std::string::size_type loc;
    while ((loc = ns.find(".")) != std::string::npos) {
        out << separator << "namespace " << ns.substr(0, loc) << " {";
        separator = "\n";
        ns = ns.substr(loc + 1);
    }
    out << '\n';
    out << '\n';
}

void t_yarp_generator::namespace_close(std::ostream& out, std::string ns)
{
    if (ns.empty()) {
        return;
    }

    ns += ".";
    std::string::size_type loc;

    out << '\n';
    while ((loc = ns.find(".")) != std::string::npos) {
        out << "} // namespace " << ns.substr(0, loc) << '\n';
        ns = ns.substr(loc + 1);
    }
}

/////////////////////////////////////////////////////////////////////
/// C++ generator code ends
/////////////////////////////////////////////////////////////////////

bool t_yarp_generator::is_complex_type(t_type* ttype)
{
    ttype = get_true_type(ttype);
    return ttype->is_container() || ttype->is_struct() || ttype->is_xception() || (ttype->is_base_type() && (((t_base_type*)ttype)->get_base() == t_base_type::TYPE_STRING));
}

/**
 * Prepares for file generation by opening up the necessary file output
 * stream.
 */
void t_yarp_generator::init_generator()
{
    // Make output directory
    MKDIR(get_out_dir().c_str());
    std::string dir = get_include_prefix(program_);
    std::string subdir = get_out_dir();
    std::string::size_type loc;
    while ((loc = dir.find("/")) != std::string::npos) {
        subdir = subdir + "/" + dir.substr(0, loc);
        MKDIR(subdir.c_str());
        dir = dir.substr(loc + 1);
    }
    if (dir.size() > 0) {
        subdir = subdir + "/" + dir;
        MKDIR(subdir.c_str());
    }

    std::string findex_name = get_out_dir() + program_->get_name() + "_index.txt";
    f_out_index_.open(findex_name);

    // Typedefs and constants are defined in the _common.h file
    if (!program_->get_typedefs().empty() || !program_->get_consts().empty()) {
        need_common_ = true;
        std::string fcommon_name = get_out_dir() + get_include_prefix(program_) + program_->get_name() + "_common.h";
        f_out_common_.open(fcommon_name);

        f_out_common_ << copyright_comment();
        f_out_common_ << autogen_comment();

        f_out_common_ << "#ifndef YARP_THRIFT_GENERATOR_COMMON_" << upcase_string(program_->get_name()) << "_H"<< '\n';
        f_out_common_ << "#define YARP_THRIFT_GENERATOR_COMMON_" << upcase_string(program_->get_name()) << "_H"<< '\n';
        f_out_common_ << '\n';

        f_out_index_ << get_include_prefix(program_) << program_->get_name() + "_common.h\n";

        const auto& consts = program_->get_consts();
        const auto& typedefs = program_->get_typedefs();

        //check needed inclusions
        std::set<std::string> neededTypes;
        for (const auto& const_ : consts) {
            get_needed_type(const_->get_type(), neededTypes);
        }
        for (const auto& tdef : typedefs) {
            get_needed_type(tdef->get_type(), neededTypes);
        }

        for (const auto& neededType : neededTypes) {
            f_out_common_ << "#include <" << neededType << ">\n";
        }
        f_out_common_ << '\n';

        namespace_open(f_out_common_, program_->get_namespace("yarp"));
    }

    // Each enum produces a .cpp and a .h files
    for (const auto& enum_ : program_->get_enums()) {
        f_out_index_ << get_include_prefix(program_) << enum_->get_name() << ".h\n";
        f_out_index_ << get_include_prefix(program_) << enum_->get_name() << ".cpp\n";
    }

    // Each struct and exception produces a .h and a .cpp files unless
    // annotated with "yarp.includefile"
    for (const auto& obj : program_->get_objects()) {
        if (obj->annotations_.find("yarp.includefile") == obj->annotations_.end()) {
            f_out_index_ << get_include_prefix(program_) << obj->get_name() << ".h\n";
            f_out_index_ << get_include_prefix(program_) << obj->get_name() << ".cpp\n";
        }
    }

    // Each service produces a .h and a .cpp files
    for (const auto& service : program_->get_services()) {
        f_out_index_ << get_include_prefix(program_) << service->get_name() << ".h\n";
        f_out_index_ << get_include_prefix(program_) << service->get_name() << ".cpp\n";
    }

    f_out_index_.close();
}

void t_yarp_generator::close_generator()
{
    if (need_common_) {
        namespace_close(f_out_common_, program_->get_namespace("yarp"));
        f_out_common_ << '\n';
        f_out_common_ << "#endif // YARP_THRIFT_GENERATOR_COMMON_" << upcase_string(program_->get_name()) << "_H"<<  '\n';
    }
    f_out_common_.close();
}

/**
 * If the provided documentable object has documentation attached, this
 * will emit it to the output stream in YARP format.
 */
void t_yarp_generator::print_doc(std::ostringstream& f_h_, t_doc* tdoc)
{
    if (!no_doc_ && tdoc->has_doc()) {
        f_h_ << indent_h() << "/**" << '\n';
        std::string doc = tdoc->get_doc();
        size_t index;
        while ((index = doc.find_first_of("\r\n")) != std::string::npos) {
            if (index == 0) {
            } else {
                f_h_ << indent_h() << " * " << doc.substr(0, index) << '\n';
            }
            if (index + 1 < doc.size() && doc.at(index) != doc.at(index + 1) && (doc.at(index + 1) == '\r' || doc.at(index + 1) == '\n')) {
                index++;
            }
            doc = doc.substr(index + 1);
        }
        f_h_ << indent_h() << " */\n";
    }
}

static std::string replaceInString(const std::string& originalString, std::string toFind, std::string replacement)
{
    std::string docString(originalString);
    size_t foundToken = docString.find(toFind);
    while (foundToken != std::string::npos) {
        docString.replace(foundToken, toFind.size(), replacement);
        foundToken = docString.find(toFind, foundToken + replacement.size());
    }
    return docString;
};

void t_yarp_generator::quote_doc(std::vector<std::string>& doxyPar, t_doc* tdoc)
{
    std::string quotes = "\"";
    std::string replacement = "\\\"";
    std::string result;
    if (tdoc->has_doc()) {
        std::string doc = tdoc->get_doc();
        size_t index;
        while ((index = doc.find_first_of("\r\n")) != std::string::npos) {
            if (index != 0) {
                //escape all quotes (TODO: may need to escape other characters?)
                doxyPar.push_back(replaceInString(doc.substr(0, index), quotes, replacement));
            }
            if (index + 1 < doc.size() && doc.at(index) != doc.at(index + 1) && (doc.at(index + 1) == '\r' || doc.at(index + 1) == '\n')) {
                index++;
            }
            doc = doc.substr(index + 1);
        }
    }
}

std::vector<std::string> t_yarp_generator::print_help(t_function* tdoc)
{
    std::vector<std::string> doxyPar;
    std::string quotes = "\"";
    std::string replacement = "\\\"";
    doxyPar.push_back(replaceInString(function_prototype(tdoc, true, true, true), quotes, replacement));
    quote_doc(doxyPar, tdoc);
    return doxyPar;
}

/**
 * Prints out an YARP representation of the provided constant value
 */
std::string t_yarp_generator::print_const_value(t_const_value* tvalue, t_type* ttype)
{
    std::string result;
    bool first = true;
    char buf[10000];
    switch (tvalue->get_type()) {
    case t_const_value::CV_INTEGER:
        sprintf(buf, "%d", (int)tvalue->get_integer());
        result += buf;
        break;
    case t_const_value::CV_DOUBLE:
        sprintf(buf, "%g", tvalue->get_double());
        result += buf;
        break;
    case t_const_value::CV_STRING:
        result += std::string("\"") + get_escaped_string(tvalue) + "\"";
        break;
    case t_const_value::CV_MAP: {
        result += "{ ";
        const auto& value_map = tvalue->get_map();
        for (const auto& value_item : value_map) {
            if (!first) {
                result += ", ";
            }
            first = false;
            result += print_const_value(value_item.first);
            result += " = ";
            result += print_const_value(value_item.second);
        }
        result += " }";
    } break;
    case t_const_value::CV_LIST: {
        result += "{ ";
        const auto& elems = tvalue->get_list();
        for (const auto& elem : elems) {
            if (!first) {
                result += ", ";
            }
            first = false;
            result += print_const_value(elem);
        }
        result += " }";
    } break;
    default:
    {
        bool done = false;
        if (ttype != nullptr) {
            if (ttype->is_enum()) {
                result += tvalue->get_identifier_name();
                done = true;
            }
        }
        if (!done) {
            result += "UNKNOWN";
        }
    } break;
    }
    return result;
}

/******************************************************************************/
// BEGIN generate helpers

void t_yarp_generator::generate_namespace_open(std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    const auto& ns = program_->get_namespace("yarp");
    namespace_open(f_h_, ns);
    namespace_open(f_cpp_, ns);
}

void t_yarp_generator::generate_namespace_close(std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    const auto& ns = program_->get_namespace("yarp");
    namespace_close(f_h_, ns);
    namespace_close(f_cpp_, ns);
}

/**
 * Returns the include prefix to use for a file generated by program, or the
 * empty string if no include prefix should be used.
 */
std::string t_yarp_generator::get_include_prefix(const t_program* const program) const
{
    std::string include_prefix = program->get_include_prefix();
    if (!use_include_prefix_ || (include_prefix.size() > 0 && include_prefix[0] == '/')) {
        // if flag is turned off or this is absolute path, return the file path
        // is not used.
        include_prefix = "";
    }
    if (!no_namespace_prefix_) {
        // Add namespace to the include path
        std::string ns = program->get_namespace("yarp");
        if (!ns.empty()) {
            std::replace(ns.begin(), ns.end(), '.', '/');
            include_prefix += ns;
            include_prefix += "/";
        }
    }

    return include_prefix;
}

void t_yarp_generator::get_needed_type(t_type* curType, std::set<std::string>& neededTypes)
{
    std::string mtype;
    const auto& program = curType->get_program();

    if (curType->is_struct()) {
        if (((t_struct*)curType)->annotations_.find("yarp.includefile") != ((t_struct*)curType)->annotations_.end()) {
            mtype = ((t_struct*)curType)->annotations_["yarp.includefile"];
        } else {
            mtype = get_include_prefix(program) + curType->get_name() + ".h";
        }

        neededTypes.insert(mtype);
        return;
    }

    if (curType->is_enum()) {
        mtype = get_include_prefix(program) + curType->get_name() + ".h";
        neededTypes.insert(mtype);
        return;
    }

    if (curType->is_list()) {
        get_needed_type(((t_list*)curType)->get_elem_type(), neededTypes);
        return;
    }

    if (curType->is_set()) {
        get_needed_type(((t_set*)curType)->get_elem_type(), neededTypes);
        return;
    }

    if (curType->is_map()) {
        get_needed_type(((t_map*)curType)->get_key_type(), neededTypes);
        get_needed_type(((t_map*)curType)->get_val_type(), neededTypes);
        return;
    }
}


std::string t_yarp_generator::function_prototype(t_function* tfn,
                                                 bool include_defaults,
                                                 bool include_returntype,
                                                 bool include_qualifiers,
                                                 const std::string& prefix,
                                                 const std::string& override_name)
{
    std::string result = "";

    if (include_returntype) {
        std::string return_type = type_name(tfn->get_returntype());
        if (!override_name.empty()) {
            return_type = "void";
        }
        result += return_type;
        result += std::string(" ");
    }

    if (!prefix.empty()) {
        result += prefix;
        result += "::";
    }

    std::string fn_name = tfn->get_name();
    if (!override_name.empty()) {
        fn_name = override_name;
    }

    result += fn_name + "(";
    bool first = true;
    for (const auto& arg : tfn->get_arglist()->get_members()) {
        if (!first) {
            result += ", ";
        }
        first = false;
        result += type_name(arg->get_type(), false, true);
        result += std::string(" ") + arg->get_name();
        if (include_defaults && arg->get_value() != nullptr) {
            result += " = ";
            result += print_const_value(arg->get_value(), arg->get_type());
        }
    }
    result += ")";

    if (include_qualifiers) {
        auto it = tfn->annotations_.find("yarp.qualifier");
        if (it != tfn->annotations_.end()) {
            // For now support only the "const" qualifier.
            assert(it->second == "const");
            result += " ";
            result += it->second;
        }
    }

    return result;
}

void t_yarp_generator::generate_serialize_field(std::ostringstream& f_cpp_,
                                                t_field* tfield,
                                                const std::string& prefix,
                                                const std::string& suffix,
                                                bool force_nesting)
{
    THRIFT_DEBUG_COMMENT(f_cpp_);

    t_type* type = get_true_type(tfield->get_type());

    std::string name = prefix + tfield->get_name() + suffix;

    // Do nothing for void types
    if (type->is_void()) {
        throw "CANNOT GENERATE SERIALIZE CODE FOR void TYPE: " + name;
    }

    // Force nesting for fields annotated as "yarp.nested"
    auto it = tfield->annotations_.find("yarp.nested");
    if (it != tfield->annotations_.end() && it->second == "true") {
        force_nesting = true;
    }

    if (type->is_struct() || type->is_xception()) {
        f_cpp_ << indent_cpp() << "if (!writer.";
        generate_serialize_struct(f_cpp_,
                                  (t_struct*)type,
                                  name,
                                  force_nesting);
        f_cpp_ << ")" << inline_return_cpp("false");
    } else if (type->is_container()) {
        generate_serialize_container(f_cpp_, type, name);
    } else if (type->is_base_type() || type->is_enum()) {
        f_cpp_ << indent_cpp() << "if (!writer.";
        if (type->is_base_type()) {
            t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
            switch (tbase) {
            case t_base_type::TYPE_VOID:
                throw "compiler error: cannot serialize void field in a struct: " + name;
                break;
            case t_base_type::TYPE_STRING:
                if (((t_base_type*)type)->is_binary()) {
                    f_cpp_ << "writeBinary(" << name << ")";
                } else {
                    f_cpp_ << "writeString(" << name << ")";
                }
                break;
            case t_base_type::TYPE_BOOL:
                f_cpp_ << "writeBool(" << name << ")";
                break;
            case t_base_type::TYPE_I8:
            {
                auto it = type->annotations_.find("yarp.type");
                if (it != type->annotations_.end() && (it->second == "std::uint8_t" ||
                                                       it->second == "uint8_t" ||
                                                       it->second.find("unsigned") != std::string::npos)) {
                    f_cpp_ << "writeUI8(" << name << ")";
                } else {
                    f_cpp_ << "writeI8(" << name << ")";
                }
                break;
            }
            case t_base_type::TYPE_I16:
            {
                auto it = type->annotations_.find("yarp.type");
                if (it != type->annotations_.end() && (it->second == "std::uint16_t" ||
                                                       it->second == "uint16_t" ||
                                                       it->second.find("unsigned") != std::string::npos)) {
                    f_cpp_ << "writeUI16(" << name << ")";
                } else {
                    f_cpp_ << "writeI16(" << name << ")";
                }
                break;
            }
            case t_base_type::TYPE_I32:
            {
                auto it = type->annotations_.find("yarp.type");
                if (it != type->annotations_.end() && it->second == "yarp::conf::vocab32_t") {
                    f_cpp_ << "writeVocab32(" << name << ")";
                } else if (it != type->annotations_.end() && (it->second == "std::size_t" ||
                                                              it->second == "size_t")) {
                    f_cpp_ << "writeSizeT(" << name << ")";
                } else if (it != type->annotations_.end() && (it->second == "std::uint32_t" ||
                                                              it->second == "uint32_t" ||
                                                              it->second.find("unsigned") != std::string::npos)) {
                    f_cpp_ << "writeUI32(" << name << ")";
                } else {
                    f_cpp_ << "writeI32(" << name << ")";
                }
                break;
            }
            case t_base_type::TYPE_I64:
            {
                auto it = type->annotations_.find("yarp.type");
                if (it != type->annotations_.end() && (it->second == "std::uint64_t" ||
                                                       it->second == "uint64_t" ||
                                                       it->second.find("unsigned") != std::string::npos)) {
                    f_cpp_ << "writeUI64(" << name << ")";
                } else {
                    f_cpp_ << "writeI64(" << name << ")";
                }
                break;
            }
            case t_base_type::TYPE_DOUBLE:
            {
                auto it = type->annotations_.find("yarp.type");
                if (it != type->annotations_.end() && it->second == "yarp::conf::float32_t") {
                    f_cpp_ << "writeFloat32(" << name << ")";
                } else {
                    f_cpp_ << "writeFloat64(" << name << ")";
                }
                break;
                }
            default:
                throw "compiler error: no C++ writer for base type " + t_base_type::t_base_name(tbase) + name;
            }
        } else if (type->is_enum()) {
            f_cpp_ << "writeI32(static_cast<int32_t>(" << name << "))";
        }
        f_cpp_ << ")" << inline_return_cpp("false");
    } else {
        printf("DO NOT KNOW HOW TO SERIALIZE FIELD '%s' TYPE '%s'\n",
               name.c_str(),
               type_name(type).c_str());
    }
}

void t_yarp_generator::generate_serialize_struct(std::ostringstream& f_cpp_,
                                                 t_struct* tstruct,
                                                 const std::string& prefix,
                                                 bool force_nesting)
{
    THRIFT_DEBUG_COMMENT(f_cpp_);

    if (force_nesting) {
        f_cpp_ << "writeNested(" << prefix << ")";
    } else {
        f_cpp_ << "write(" << prefix << ")";
    }
}

void t_yarp_generator::generate_serialize_container(std::ostringstream& f_cpp_,
                                                    t_type* ttype,
                                                    const std::string& prefix)
{
    THRIFT_DEBUG_COMMENT(f_cpp_);

    if (ttype->is_map()) {
        f_cpp_ << indent_cpp() << "if (!writer.writeMapBegin("
                        << type_to_enum(((t_map*)ttype)->get_key_type())
                        << ", "
                        << type_to_enum(((t_map*)ttype)->get_val_type());
    } else if (ttype->is_set()) {
        f_cpp_ << indent_cpp() << "if (!writer.writeSetBegin("
                        << type_to_enum(((t_set*)ttype)->get_elem_type());
    } else if (ttype->is_list()) {
        f_cpp_ << indent_cpp() << "if (!writer.writeListBegin("
                        << type_to_enum(((t_list*)ttype)->get_elem_type());
    }
    f_cpp_ << ", " << "static_cast<uint32_t>(" << prefix << ".size())))" << inline_return_cpp("false");

    std::string item = tmp("_item");
    f_cpp_ << indent_cpp() << "for (const auto& " << item << " : " << prefix << ") {\n";
    indent_up_cpp();
    {
        if (ttype->is_map()) {
            generate_serialize_map_element(f_cpp_, (t_map*)ttype, item);
        } else if (ttype->is_set()) {
            generate_serialize_set_element(f_cpp_, (t_set*)ttype, item);
        } else if (ttype->is_list()) {
            generate_serialize_list_element(f_cpp_, (t_list*)ttype, item);
        }
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";

    if (ttype->is_map()) {
        f_cpp_ << indent_cpp() << "if (!writer.writeMapEnd())" << inline_return_cpp("false");
    } else if (ttype->is_set()) {
        f_cpp_ << indent_cpp() << "if (!writer.writeSetEnd())" << inline_return_cpp("false");
    } else if (ttype->is_list()) {
        f_cpp_ << indent_cpp() << "if (!writer.writeListEnd())" << inline_return_cpp("false");
    }
}

void t_yarp_generator::generate_serialize_map_element(std::ostringstream& f_cpp_,
                                                      t_map* tmap,
                                                      const std::string& item_name)
{
    THRIFT_DEBUG_COMMENT(f_cpp_);

    f_cpp_ << indent_cpp() << "if (!writer.writeListBegin(0,2))" << inline_return_cpp("false");

    t_field kfield(tmap->get_key_type(), item_name + ".first");
    generate_serialize_field(f_cpp_, &kfield);

    t_field vfield(tmap->get_val_type(), item_name + ".second");
    generate_serialize_field(f_cpp_, &vfield, "", "", true);

    f_cpp_ << indent_cpp() << "if (!writer.writeListEnd())" << inline_return_cpp("false");
}

void t_yarp_generator::generate_serialize_set_element(std::ostringstream& f_cpp_,
                                                      t_set* tset,
                                                      const std::string& item_name)
{
    THRIFT_DEBUG_COMMENT(f_cpp_);

    t_field efield(tset->get_elem_type(), item_name);
    generate_serialize_field(f_cpp_, &efield, "", "", true);
}

void t_yarp_generator::generate_serialize_list_element(std::ostringstream& f_cpp_,
                                                       t_list* tlist,
                                                       const std::string& item_name)
{
    THRIFT_DEBUG_COMMENT(f_cpp_);

    t_field efield(tlist->get_elem_type(), item_name);
    generate_serialize_field(f_cpp_, &efield, "", "", true);
}


void t_yarp_generator::generate_deserialize_field_fallback(std::ostringstream& f_cpp_,
                                                           t_field* tfield)
{
    THRIFT_DEBUG_COMMENT(f_cpp_);

    if (tfield->get_value() != nullptr) {
        f_cpp_ << indent_cpp() << tfield->get_name() << " = " << print_const_value(tfield->get_value(), tfield->get_type()) << ";\n";
    } else {
        f_cpp_ << indent_cpp() << "reader.fail();\n";
        f_cpp_ << indent_cpp() << "return false;\n";
    }
}

void t_yarp_generator::generate_deserialize_field(std::ostringstream& f_cpp_,
                                                  t_field* tfield,
                                                  const std::string &prefix,
                                                  const std::string &suffix,
                                                  bool force_nested)
{
    THRIFT_DEBUG_COMMENT(f_cpp_);

    t_type* type = get_true_type(tfield->get_type());

    if (type->is_void()) {
        throw "CANNOT GENERATE DESERIALIZE CODE FOR void TYPE: " + prefix + tfield->get_name();
    }

    // Force nesting for fields annotated as "yarp.nested"
    auto it = tfield->annotations_.find("yarp.nested");
    if (it != tfield->annotations_.end() && it->second == "true") {
        force_nested = true;
    }

    std::string name = prefix + tfield->get_name() + suffix;

    if (type->is_struct() || type->is_xception()) {
        f_cpp_ << indent_cpp() << "if (!reader.";
        generate_deserialize_struct(f_cpp_, (t_struct*)type, name, force_nested);
        f_cpp_ << ") {\n";
        indent_up_cpp();
        {
            generate_deserialize_field_fallback(f_cpp_, tfield);
        }
        indent_down_cpp();
        f_cpp_ << indent_cpp() << "}\n";
    } else if (type->is_container()) {
        generate_deserialize_container(f_cpp_, type, name);
    } else if (type->is_base_type()) {
        f_cpp_ << indent_cpp() << "if (!reader.";
        t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
        switch (tbase) {
        case t_base_type::TYPE_VOID:
            throw "compiler error: cannot serialize void field in a struct: " + name;
            break;
        case t_base_type::TYPE_STRING:
            if (((t_base_type*)type)->is_binary()) {
                f_cpp_ << "readBinary(" << name << ")";
            } else {
                f_cpp_ << "readString(" << name << ")";
            }
            break;
        case t_base_type::TYPE_BOOL:
            f_cpp_ << "readBool(" << name << ")";
            break;
        case t_base_type::TYPE_I8:
        {
            auto it = type->annotations_.find("yarp.type");
            if (it != type->annotations_.end() && (it->second == "std::uint8_t" ||
                                                   it->second == "uint8_t" ||
                                                   it->second.find("unsigned") != std::string::npos)) {
                f_cpp_ << "readUI8(" << name << ")";
            } else {
                f_cpp_ << "readI8(" << name << ")";
            }
            break;
        }
        case t_base_type::TYPE_I16:
        {
            auto it = type->annotations_.find("yarp.type");
            if (it != type->annotations_.end() && (it->second == "std::uint16_t" ||
                                                   it->second == "uint16_t" ||
                                                   it->second.find("unsigned") != std::string::npos)) {
                f_cpp_ << "readUI16(" << name << ")";
            } else {
                f_cpp_ << "readI16(" << name << ")";
            }
            break;
        }
        case t_base_type::TYPE_I32:
        {
            auto it = type->annotations_.find("yarp.type");
            if (it != type->annotations_.end() && it->second == "yarp::conf::vocab32_t") {
                f_cpp_ << "readVocab32(" << name << ")";
            } else if (it != type->annotations_.end() && (it->second == "std::size_t" ||
                                                          it->second == "size_t")) {
                f_cpp_ << "readSizeT(" << name << ")";
            } else if (it != type->annotations_.end() && (it->second == "std::uint32_t" ||
                                                          it->second == "uint32_t" ||
                                                          it->second.find("unsigned") != std::string::npos)) {
                f_cpp_ << "readUI32(" << name << ")";
            } else {
                f_cpp_ << "readI32(" << name << ")";
            }
            break;
        }
        case t_base_type::TYPE_I64:
        {
            auto it = type->annotations_.find("yarp.type");
            if (it != type->annotations_.end() && (it->second == "std::uint64_t" ||
                                                   it->second == "uint64_t" ||
                                                   it->second.find("unsigned") != std::string::npos)) {
                f_cpp_ << "readUI64(" << name << ")";
            } else {
                f_cpp_ << "readI64(" << name << ")";
            }
            break;
        }
        case t_base_type::TYPE_DOUBLE:
        {
            auto it = type->annotations_.find("yarp.type");
            if (it != type->annotations_.end() && it->second == "yarp::conf::float32_t") {
                f_cpp_ << "readFloat32(" << name << ")";
            } else {
                f_cpp_ << "readFloat64(" << name << ")";
            }
            break;
        }
        default:
            throw "compiler error: no C++ reader for base type " + t_base_type::t_base_name(tbase) + name;
        }
        f_cpp_ << ") {\n";
        indent_up_cpp();
        {
            generate_deserialize_field_fallback(f_cpp_, tfield);
        }
        indent_down_cpp();
        f_cpp_ << indent_cpp() << "}\n";
    } else if (type->is_enum()) {
        std::string t = tmp("ecast");
        std::string t2 = tmp("cvrt");
        f_cpp_ << indent_cpp() << "int32_t " << t << ";\n";
        f_cpp_ << indent_cpp() << type_name(type) << "Vocab " << t2 << ";\n";
        f_cpp_ << indent_cpp() << "if (!reader.readEnum(" << t << ", " << t2 << ")) {\n";
        indent_up_cpp();
        {
            generate_deserialize_field_fallback(f_cpp_, tfield);
        }
        indent_down_cpp();
        f_cpp_ << indent_cpp() << "} else {\n";
        indent_up_cpp();
        {
            f_cpp_ << indent_cpp() << name << " = static_cast<" << type_name(type) << ">(" << t << ");\n";
        }
        indent_down_cpp();
        f_cpp_ << indent_cpp() << "}\n";
    } else {
        printf("DO NOT KNOW HOW TO DESERIALIZE FIELD '%s' TYPE '%s'\n",
               tfield->get_name().c_str(),
               type_name(type).c_str());
    }
}

void t_yarp_generator::generate_deserialize_struct(std::ostringstream& f_cpp_,
                                                   t_struct* tstruct,
                                                   const std::string& prefix,
                                                   bool force_nested)
{
    THRIFT_DEBUG_COMMENT(f_cpp_);

    (void /* unused */)tstruct;
    f_cpp_ << "read" << (force_nested ? "Nested" : "") << "(" << prefix << ")";
}

void t_yarp_generator::generate_deserialize_container(std::ostringstream& f_cpp_,
                                                      t_type* ttype,
                                                      const std::string& prefix)
{
    THRIFT_DEBUG_COMMENT(f_cpp_);

    std::string size = tmp("_size");
    std::string ktype = tmp("_ktype");
    std::string vtype = tmp("_vtype");
    std::string etype = tmp("_etype");

    t_container* tcontainer = (t_container*)ttype;
    bool use_push = tcontainer->has_cpp_name();

    f_cpp_ << indent_cpp() << prefix << ".clear();\n";
    f_cpp_ << indent_cpp() << "uint32_t " << size << ";\n";

    // Declare variables, read header
    if (ttype->is_map()) {
        // kttpe and vtype available
        f_cpp_ << indent_cpp() << "yarp::os::idl::WireState " << ktype << ";\n";
        f_cpp_ << indent_cpp() << "yarp::os::idl::WireState " << vtype << ";\n";
        f_cpp_ << indent_cpp() << "reader.readMapBegin(" << ktype << ", " << vtype << ", " << size << ");\n";
    } else if (ttype->is_set()) {
        f_cpp_ << indent_cpp() << "yarp::os::idl::WireState " << etype << ";\n";
        f_cpp_ << indent_cpp() << "reader.readSetBegin(" << etype << ", " << size << ");\n";
    } else if (ttype->is_list()) {
        f_cpp_ << indent_cpp() << "yarp::os::idl::WireState " << etype << ";\n";
        f_cpp_ << indent_cpp() << "reader.readListBegin(" << etype << ", " << size << ");\n";
        if (!use_push) {
            f_cpp_ << indent_cpp() << prefix << ".resize(" << size << ");\n";
        }
    }

    // For loop iterates over elements
    std::string i = tmp("_i");
    f_cpp_ << indent_cpp() << "for (size_t " << i << " = 0; " << i << " < " << size << "; ++" << i << ") {\n";
    indent_up_cpp();
    {
        if (ttype->is_map()) {
            generate_deserialize_map_element(f_cpp_, (t_map*)ttype, prefix);
        } else if (ttype->is_set()) {
            generate_deserialize_set_element(f_cpp_, (t_set*)ttype, prefix);
        } else if (ttype->is_list()) {
            generate_deserialize_list_element(f_cpp_, (t_list*)ttype, prefix, use_push, prefix + "[" + i + "]");
        }
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";

    // Read container end
    if (ttype->is_map()) {
        f_cpp_ << indent_cpp() << "reader.readMapEnd();\n";
    } else if (ttype->is_set()) {
        f_cpp_ << indent_cpp() << "reader.readSetEnd();\n";
    } else if (ttype->is_list()) {
        f_cpp_ << indent_cpp() << "reader.readListEnd();\n";
    }
}

void t_yarp_generator::generate_deserialize_map_element(std::ostringstream& f_cpp_,
                                                        t_map* tmap,
                                                        const std::string& prefix)
{
    THRIFT_DEBUG_COMMENT(f_cpp_);

    std::string lst = tmp("_lst");
    std::string size = tmp("_size");
    f_cpp_ << indent_cpp() << "uint32_t " << size << ";\n";
    f_cpp_ << indent_cpp() << "yarp::os::idl::WireState " << lst << ";\n";
    f_cpp_ << indent_cpp() << "reader.readListBegin(" << lst << ", " << size << ");\n";

    std::string key = tmp("_key");
    std::string val = tmp("_val");
    t_field fkey(tmap->get_key_type(), key);
    t_field fval(tmap->get_val_type(), val);

    f_cpp_ << indent_cpp() << declare_field(&fkey) << ";\n";

    generate_deserialize_field(f_cpp_, &fkey);
    f_cpp_ << indent_cpp() << declare_field(&fval, false, false, false, true) << " = " << prefix << "[" << key << "];\n";

    generate_deserialize_field(f_cpp_, &fval, "", "", true);

    f_cpp_ << indent_cpp() << "reader.readListEnd();\n";
}

void t_yarp_generator::generate_deserialize_set_element(std::ostringstream& f_cpp_,
                                                        t_set* tset,
                                                        const std::string& prefix)
{
    THRIFT_DEBUG_COMMENT(f_cpp_);

    std::string elem = tmp("_elem");
    t_field felem(tset->get_elem_type(), elem);

    f_cpp_ << indent_cpp() << declare_field(&felem) << ";\n";

    generate_deserialize_field(f_cpp_, &felem, "", "", true);

    f_cpp_ << indent_cpp() << prefix << ".insert(" << elem << ");\n";
}

void t_yarp_generator::generate_deserialize_list_element(std::ostringstream& f_cpp_,
                                                         t_list* tlist,
                                                         const std::string& prefix,
                                                         bool use_push,
                                                         const std::string& list_elem)
{
    THRIFT_DEBUG_COMMENT(f_cpp_);

    if (use_push) {
        std::string elem = tmp("_elem");
        t_field felem(tlist->get_elem_type(), elem);
        f_cpp_ << indent_cpp() << declare_field(&felem) << ";\n";
        generate_deserialize_field(f_cpp_, &felem, "", "", true);
        f_cpp_ << indent_cpp() << prefix << ".push_back(" << elem << ");\n";
    } else {
        t_field felem(tlist->get_elem_type(), list_elem);
        generate_deserialize_field(f_cpp_, &felem, "", "", true);
    }
}

std::string t_yarp_generator::declare_field(t_field* tfield,
                                            bool init,
                                            bool pointer,
                                            bool is_constant,
                                            bool reference,
                                            const std::string& force_type,
                                            const std::string& prefix)
{
    // TODO: do we ever need to initialize the field?
    std::string result = "";
    if (is_constant) {
        result += "const ";
    }
    if (force_type.empty()) {
        result += type_name(tfield->get_type());
    } else {
        result += force_type;
    }
    if (pointer) {
        result += "*";
    }
    if (reference) {
        result += "&";
    }
    if (result.length() > 0) {
        result += " ";
    }

    result += prefix;
    result += tfield->get_name();

    if (init) {
        t_type* type = get_true_type(tfield->get_type());

        if (type->is_base_type()) {
            t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
            switch (tbase) {
            case t_base_type::TYPE_VOID:
                break;
            case t_base_type::TYPE_STRING:
                result += " = \"\"";
                break;
            case t_base_type::TYPE_BOOL:
                result += " = false";
                break;
            case t_base_type::TYPE_I8:
            case t_base_type::TYPE_I16:
            case t_base_type::TYPE_I32:
            case t_base_type::TYPE_I64:
                result += " = 0";
                break;
            case t_base_type::TYPE_DOUBLE:
                result += " = 0.0";
                break;
            default:
                throw "compiler error: no C++ initializer for base type " + t_base_type::t_base_name(tbase);
            }
        } else if (type->is_enum()) {
            result += " = static_cast<" + type_name(type) + ">(0)";
        } else {
            result += " = {}";
        }
    }

    return result;
}

void t_yarp_generator::print_const_value(std::ostringstream& f_cpp_,
                                         const std::string& name,
                                         t_type* type,
                                         t_const_value* value)
{
    type = get_true_type(type);
    if (type->is_base_type()) {
        std::string v2 = render_const_value(f_cpp_, name, type, value);
        f_cpp_ << indent_cpp() << name << " = " << v2 << ";\n"
                    << '\n';
    } else if (type->is_enum()) {
        f_cpp_ << indent_cpp() << name << " = static_cast<" << type_name(type) << ">(" << value->get_integer() << ");\n"
                    << '\n';
    } else if (type->is_struct() || type->is_xception()) {
        const auto& fields = ((t_struct*)type)->get_members();
        const auto& value_map = value->get_map();
        for (const auto& value_item : value_map) {
            t_type* field_type = nullptr;
            for (const auto& field : fields) {
                if (field->get_name() == value_item.first->get_string()) {
                    field_type = field->get_type();
                }
            }
            if (field_type == nullptr) {
                throw "type error: " + type->get_name() + " has no field " + value_item.first->get_string();
            }
            std::string val = render_const_value(f_cpp_, name, field_type, value_item.second);
            f_cpp_ << indent_cpp() << name << "." << value_item.first->get_string() << " = " << val << ";\n";
            f_cpp_ << indent_cpp() << name << ".__isset." << value_item.first->get_string() << " = true;\n";
        }
        f_cpp_ << '\n';
    } else if (type->is_map()) {
        t_type* ktype = ((t_map*)type)->get_key_type();
        t_type* vtype = ((t_map*)type)->get_val_type();
        const auto& value_map = value->get_map();
        for (const auto& value_item : value_map) {
            std::string key = render_const_value(f_cpp_, name, ktype, value_item.first);
            std::string val = render_const_value(f_cpp_, name, vtype, value_item.second);
            f_cpp_ << indent_cpp() << name << ".insert(std::make_pair(" << key << ", " << val << "));\n";
        }
        f_cpp_ << '\n';
    } else if (type->is_list()) {
        t_type* etype = ((t_list*)type)->get_elem_type();
        const auto& values = value->get_list();
        for (const auto& value : values) {
            std::string val = render_const_value(f_cpp_, name, etype, value);
            f_cpp_ << indent_cpp() << name << ".push_back(" << val << ");\n";
        }
        f_cpp_ << '\n';
    } else if (type->is_set()) {
        t_type* etype = ((t_set*)type)->get_elem_type();
        const auto& values = value->get_list();
        for (const auto& value : values) {
            std::string val = render_const_value(f_cpp_, name, etype, value);
            f_cpp_ << indent_cpp() << name << ".insert(" << val << ");\n";
        }
        f_cpp_ << '\n';
    } else {
        throw "INVALID TYPE IN print_const_value: " + type->get_name();
    }
}

std::string t_yarp_generator::render_const_value(std::ostringstream& f_cpp_,
                                                 const std::string&  /*name*/,
                                                 t_type* type,
                                                 t_const_value* value)
{
    std::ostringstream render;

    if (type->is_base_type()) {
        t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
        switch (tbase) {
        case t_base_type::TYPE_STRING:
            render << '"' << get_escaped_string(value) << '"';
            break;
        case t_base_type::TYPE_BOOL:
            render << ((value->get_integer() > 0) ? "true" : "false");
            break;
        case t_base_type::TYPE_I8:
        case t_base_type::TYPE_I16:
        case t_base_type::TYPE_I32:
            render << value->get_integer();
            break;
        case t_base_type::TYPE_I64:
            render << value->get_integer() << "LL";
            break;
        case t_base_type::TYPE_DOUBLE:
            if (value->get_type() == t_const_value::CV_INTEGER) {
                render << value->get_integer();
            } else {
                render << value->get_double();
            }
            break;
        default:
            throw "compiler error: no const of base type " + t_base_type::t_base_name(tbase);
        }
    } else if (type->is_enum()) {
        render << "static_cast<" << type_name(type) << ">(" << value->get_integer() << ")";
    } else {
        std::string t = tmp("tmp");
        f_cpp_ << indent_cpp() << type_name(type) << " " << t << ";\n";
        print_const_value(f_cpp_, t, type, value);
        render << t;
    }

    return render.str();
}

int t_yarp_generator::flat_element_count(t_type* type)
{
    if (type->is_void()) {
        return 0;
    }
    if (!type->is_struct()) {
        return 1;
    }
    if (((t_struct*)type)->annotations_.find("yarp.name") != ((t_struct*)type)->annotations_.end()) {
        return 1;
    }
    return flat_element_count((t_struct*)type);
}

int t_yarp_generator::flat_element_count(t_struct* tstruct)
{
    int ct = 0;
    for (const auto& member : tstruct->get_members()) {
        // If field is annotated as "yarp.nested", increment by one (it will be
        // serialized as a list), otherwise increment by the number of members).
        auto it = member->annotations_.find("yarp.nested");
        if (it != member->annotations_.end() && it->second == "true") {
            ++ct;
        } else {
            ct += flat_element_count(member->get_type());
        }
    }
    return ct;
}

int t_yarp_generator::flat_element_count(t_function* fn)
{
    int ct = 0;
    for (const auto& member : fn->get_arglist()->get_members()) {
        // If field is annotated as "yarp.nested", increment by one (it will be
        // serialized as a list), otherwise increment by the number of members).
        auto it = member->annotations_.find("yarp.nested");
        if (it != member->annotations_.end() && it->second == "true") {
            ++ct;
        } else {
            ct += flat_element_count(member->get_type());
        }
    }
    return ct;
}

// END generate helpers
/******************************************************************************/

/******************************************************************************/
// BEGIN generate_typedef

/**
 * Generates a typedef.
 *
 * @param ttypedef The type definition
 */
void t_yarp_generator::generate_typedef(t_typedef* ttypedef)
{
    assert(indent_count_h() == 0);
    assert(indent_count_cpp() == 0);

    THRIFT_DEBUG_COMMENT(f_out_common_);

    std::string name = ttypedef->get_name();
    print_doc(f_out_common_, ttypedef);
    f_out_common_ << "typedef " << type_name(ttypedef->get_type()) << " " << name << ";\n";
}

// END generate_typedef
/******************************************************************************/

/******************************************************************************/
// BEGIN generate_enum

/**
 * Generates code for an enumerated type.
 *
 * @param tenum The enumeration
 */
void t_yarp_generator::generate_enum(t_enum* tenum)
{
    assert(indent_count_h() == 0);
    assert(indent_count_cpp() == 0);

    const auto& name = tenum->get_name();

    // Open header files
    std::string f_header_name = get_out_dir() + get_include_prefix(program_) + name + ".h";
    ofstream_with_content_based_conditional_update f_h_;
    f_h_.open(f_header_name);
    THRIFT_DEBUG_COMMENT(f_h_);

    // Open cpp files
    std::string f_cpp_name = get_out_dir() + get_include_prefix(program_) + name + ".cpp";
    ofstream_with_content_based_conditional_update f_cpp_;
    f_cpp_.open(f_cpp_name);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    // Add copyright
    f_h_ << copyright_comment();
    f_cpp_ << copyright_comment();

    // Add auto-generated warning
    f_h_ << autogen_comment();
    f_cpp_ << autogen_comment();

    // Add header guards to .h file
    f_h_ << "#ifndef YARP_THRIFT_GENERATOR_ENUM_" << upcase_string(name) << "_H\n";
    f_h_ << "#define YARP_THRIFT_GENERATOR_ENUM_" << upcase_string(name) << "_H\n";
    f_h_ << '\n';

    // Add includes to .h file
    f_h_ << "#include <yarp/os/Wire.h>\n";
    f_h_ << "#include <yarp/os/idl/WireTypes.h>\n";
    f_h_ << '\n';

    // Add includes to .cpp file
    f_cpp_ << "#include <yarp/os/Wire.h>\n";
    f_cpp_ << "#include <yarp/os/idl/WireTypes.h>\n";
    f_cpp_ << "#include <" << get_include_prefix(program_) << name << ".h>" << '\n';
    f_cpp_ << '\n';

    // Open namespace
    generate_namespace_open(f_h_, f_cpp_);

    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    // Add class documentation
    print_doc(f_h_, tenum);

    // Begin enum
    f_h_ << indent_h() << "enum " << name << '\n';
    f_h_ << indent_h() << "{\n";
    indent_up_h();
    {
        generate_enum_constant_list(tenum, f_h_, f_cpp_);
    }
    indent_down_h();
    f_h_ << indent_h() << "};\n";
    f_h_ << '\n';

    // Generate a character array of enum names for debugging purposes.
    f_h_ << "class " << name << "Vocab :\n";
    f_h_ << indent_initializer_h() << "public yarp::os::idl::WireVocab\n";
    f_h_ << indent_h() << "{\n";
    indent_up_h();
    {
        f_h_ << indent_access_specifier_h() << "public:\n";

        generate_enum_fromstring(tenum, f_h_, f_cpp_);
        generate_enum_tostring(tenum, f_h_, f_cpp_);
    }
    indent_down_h();

    // End class
    f_h_ << indent_h() << "};\n";

    // Close namespace
    generate_namespace_close(f_h_, f_cpp_);

    // Close header guard
    f_h_ << '\n';
    f_h_ << "#endif // YARP_THRIFT_GENERATOR_ENUM_" << upcase_string(name) << "_H\n";

    assert(indent_count_h() == 0);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_enum_constant_list(t_enum* tenum, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& constants = tenum->get_constants();
    bool first = true;
    for (const auto& constant : constants) {
        if (!first) {
            f_h_ << ",\n";
        }
        first = false;
        print_doc(f_h_, constant);
        f_h_ << indent_h() << constant->get_name() << " = " << constant->get_value();
    }
    f_h_ << '\n';
}

void t_yarp_generator::generate_enum_fromstring(t_enum* tenum, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tenum->get_name();
    const auto& constants = tenum->get_constants();

    f_h_ << indent_h() << "int fromString(const std::string& input) override;\n";

    f_cpp_ << indent_cpp() << "int " << name << "Vocab::fromString(const std::string& input)\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "// definitely needs optimizing :-)\n";
        for (const auto& constant : constants) {
            f_cpp_ << indent_cpp() << "if (input==\"" << constant->get_name() << "\")" << inline_return_cpp(std::string("static_cast<int>(") + constant->get_name() + ")");
        }
        f_cpp_ << indent_cpp() << "return -1;\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_enum_tostring(t_enum* tenum, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tenum->get_name();
    const auto& constants = tenum->get_constants();

    f_h_ << indent_h() << "std::string toString(int input) const override;\n";

    f_cpp_ << indent_cpp() << "std::string " << name << "Vocab::toString(int input) const\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "switch(static_cast<" << name << ">(input)) {\n";
        for (const auto& constant : constants) {
            f_cpp_ << indent_cpp() << "case " << constant->get_name() << ":\n";
            indent_up_cpp();
            {
                f_cpp_ << indent_cpp() << "return \"" << constant->get_name() << "\";\n";
            }
            indent_down_cpp();
        }
        f_cpp_ << indent_cpp() << "}\n";
        f_cpp_ << indent_cpp() << "return \"\";\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}


// END generate_enum
/******************************************************************************/

/******************************************************************************/
// BEGIN generate_const

/**
 * Generates a constant value
 */
void t_yarp_generator::generate_const(t_const* tconst)
{
    THRIFT_DEBUG_COMMENT(f_out_common_);

    const auto& name = tconst->get_name();
    const auto& type = tconst->get_type();
    const auto& value = tconst->get_value();
    print_doc(f_out_common_, tconst);
    f_out_common_ << "constexpr " << type_name(type) << " " << name << " = " << render_const_value(f_out_common_, name, type, value) << ";" << '\n';
}

// END generate_const
/******************************************************************************/

/******************************************************************************/
// BEGIN generate_struct

/**
 * Generates a struct definition for a thrift data type.
 *
 * @param tstruct The struct definition
 */
void t_yarp_generator::generate_struct(t_struct* tstruct)
{
    assert(indent_count_h() == 0);
    assert(indent_count_cpp() == 0);

    const auto& name = tstruct->get_name();
    const auto& members = tstruct->get_members();
    const auto& annotations = tstruct->annotations_;

    if (annotations.find("yarp.name") != annotations.end()) {
        structure_names_[name] = annotations.at("yarp.name");
        return;
    }
    if (annotations.find("cpp.name") != annotations.end()) {
        structure_names_[name] = annotations.at("cpp.name");
        return;
    }

    // Read annotations for defining the import/export macro (e.g. YARP_os_API)
    //  * yarp.api.include = the header file that contains the definition of
    //    the import/export keyword
    //  * yarp.api.keyword = the name of the keyword
    std::string yarp_api_include{};
    if (annotations.find("yarp.api.include") != annotations.end()) {
        yarp_api_include = annotations.at("yarp.api.include");
    }

    std::string yarp_api_keyword{};
    if (annotations.find("yarp.api.keyword") != annotations.end()) {
        yarp_api_keyword = annotations.at("yarp.api.keyword");
    }

    // Open header file
    std::string f_header_name = get_out_dir() + get_include_prefix(program_) + name + ".h";
    ofstream_with_content_based_conditional_update f_h_;
    f_h_.open(f_header_name);

    // Open cpp file
    std::string f_cpp_name = get_out_dir() + get_include_prefix(program_) + name + ".cpp";
    ofstream_with_content_based_conditional_update f_cpp_;
    f_cpp_.open(f_cpp_name);

    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    // Add copyright
    f_h_ << copyright_comment();
    f_cpp_ << copyright_comment();

    // Add auto-generated warning
    f_h_ << autogen_comment();
    f_cpp_ << autogen_comment();

    // Add header guards to .h file
    f_h_ << "#ifndef YARP_THRIFT_GENERATOR_STRUCT_" << upcase_string(name) << "_H\n";
    f_h_ << "#define YARP_THRIFT_GENERATOR_STRUCT_" << upcase_string(name) << "_H\n";
    f_h_ << '\n';

    // Add includes to .h file
    if (!yarp_api_include.empty()) {
        f_h_ << "#include <" << yarp_api_include << ">\n";
        f_h_ << '\n';
    }

    f_h_ << "#include <yarp/os/Wire.h>\n";
    f_h_ << "#include <yarp/os/idl/WireTypes.h>\n";
    if (need_common_) {
        f_h_ << '\n';
        f_h_ << "#include <" << get_include_prefix(program_) << program_->get_name() << "_common.h>" << '\n';
    }
    std::set<std::string> neededTypes;
    neededTypes.clear();
    for (const auto& member : members) {
        get_needed_type(member->get_type(), neededTypes);
    }
    for (const auto& neededType : neededTypes) {
        f_h_ << "#include <" << neededType << ">\n";
    }
    f_h_ << '\n';

    // Add includes to .cpp file
    f_cpp_ << "#include <" << get_include_prefix(program_) << name << ".h>\n";
    f_cpp_ << '\n';

    // Open namespace
    generate_namespace_open(f_h_, f_cpp_);

    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    // Add struct documentation
    print_doc(f_h_, tstruct);

    // Begin class
    f_h_ << indent_h() << "class " << yarp_api_keyword << (yarp_api_keyword.empty() ? "" : " ") << name << " :\n";
    f_h_ << indent_initializer_h() << "public yarp::os::idl::WirePortable\n";
    f_h_ << indent_h() << "{\n";
    indent_up_h();
    f_h_ << indent_access_specifier_h() << "public:\n";

    generate_struct_fields(tstruct, f_h_, f_cpp_);
    generate_struct_default_constructor(tstruct, f_h_, f_cpp_);
    generate_struct_constructor(tstruct, f_h_, f_cpp_);
    generate_struct_read_wirereader(tstruct, f_h_, f_cpp_);
    generate_struct_read_connectionreader(tstruct, f_h_, f_cpp_);
    generate_struct_write_wirereader(tstruct, f_h_, f_cpp_);
    generate_struct_write_connectionreader(tstruct, f_h_, f_cpp_);
    generate_struct_tostring(tstruct, f_h_, f_cpp_);
    generate_struct_unwrapped_helper(tstruct, f_h_, f_cpp_);

    // Add editor class, if not disabled
    if (!no_editor_) {
        generate_struct_editor(tstruct, f_h_, f_cpp_);
    }

    // Private members (read, write single fields)
    bool first = true;
    for (const auto& member : members) {
        if (first) {
            f_h_ << indent_access_specifier_h() << "private:\n";
        } else {
            f_h_ << '\n';
            f_cpp_ << '\n';
        }
        first = false;
        f_h_ << indent_h() << "// read/write " << member->get_name() << " field\n";
        generate_struct_field_read(tstruct, member, f_h_, f_cpp_);
        generate_struct_field_write(tstruct, member, f_h_, f_cpp_);
        generate_struct_field_nested_read(tstruct, member, f_h_, f_cpp_);
        generate_struct_field_nested_write(tstruct, member, f_h_, f_cpp_);
    }

    indent_down_h();

    // End class
    f_h_ << indent_h() << "};\n";

    // Close namespace
    generate_namespace_close(f_h_, f_cpp_);

    // Close header guard
    f_h_ << '\n';
    f_h_ << "#endif // YARP_THRIFT_GENERATOR_STRUCT_" << upcase_string(name) << "_H\n";

    assert(indent_count_h() == 0);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_fields(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    f_h_ << indent_h() << "// Fields\n";
    for (const auto& member : tstruct->get_members()) {
        std::string mname = member->get_name();
        std::string mtype = type_name(member->get_type());

        print_doc(f_h_, member);
        f_h_ << indent_h() << mtype << " " << mname << ";\n";
    }

    f_h_ << '\n';

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_default_constructor(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();
    const auto& members = tstruct->get_members();

    f_h_ << indent_h() << "// Default constructor\n";
    f_h_ << indent_h() << name << "();\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Default constructor\n";
    f_cpp_ << indent_cpp() << name << "::" << name << "() :\n";
    f_cpp_ << indent_initializer_cpp() << "WirePortable()";

    for (const auto& member : members) {
        f_cpp_ << ",\n";
        t_type* t = get_true_type(member->get_type());
        if (t->is_base_type() || t->is_enum()) {
            std::string dval;
            if (t->is_enum()) {
                dval += "(" + type_name(t) + ")";
            }
            dval += t->is_string() ? "\"\"" : "0";
            t_const_value* cv = member->get_value();
            if (cv != nullptr) {
                dval = render_const_value(f_cpp_, member->get_name(), t, cv);
            }
            f_cpp_ << indent_initializer_cpp() << member->get_name() << "(" << dval << ")";
        } else {
            f_cpp_ << indent_initializer_cpp() << member->get_name() << "()";
        }
    }
    f_cpp_ << '\n';
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_constructor(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();
    const auto& members = tstruct->get_members();

    f_h_ << indent_h() << "// Constructor with field values\n";
    f_h_ << indent_h() << name << "(";
    bool init_ctor = false;
    for (const auto& member: members) {
        t_type* t = get_true_type(member->get_type());
        if (init_ctor) {
            f_h_ << ",\n";
            f_h_ << indent_whitespaces_h(name.size() + 1);
        }
        init_ctor = true;
        f_h_ << type_name(t, false, true) << " " << member->get_name();
    }
    f_h_ << ");\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Constructor with field values\n";
    f_cpp_ << indent_cpp() << name << "::" << name << "(";
    init_ctor = false;
    for (const auto& member: members) {
        t_type* t = get_true_type(member->get_type());
        if (init_ctor) {
            f_cpp_ << ",\n";
            f_cpp_ << indent_whitespaces_cpp(name.size() + name.size() +3);
        }
        init_ctor = true;
        f_cpp_ << type_name(t, false, true) << " " << member->get_name();
    }
    f_cpp_ << ") :\n";
    f_cpp_ << indent_initializer_cpp() << "WirePortable()";

    for (const auto& member: members) {
        f_cpp_ << ",\n";
        f_cpp_ << indent_initializer_cpp() << member->get_name() << "(" << member->get_name() << ")";
    }
    f_cpp_ << '\n';
    f_cpp_ << indent_cpp() << "{\n";
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_read_wirereader(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();
    const auto& members = tstruct->get_members();

    f_h_ << indent_h() << "// Read structure on a Wire\n";
    f_h_ << indent_h() << "bool read(yarp::os::idl::WireReader& reader) override;\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Read structure on a Wire\n";
    f_cpp_ << indent_cpp() << "bool " << name << "::read(yarp::os::idl::WireReader& reader)\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        for (const auto& member : members) {
            std::string mname = member->get_name();
            f_cpp_ << indent_cpp() << "if (!read_" << mname << "(reader))" << inline_return_cpp("false");
        }
        f_cpp_ << indent_cpp() << "return !reader.isError();\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_read_connectionreader(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();

    f_h_ << indent_h() << "// Read structure on a Connection\n";
    f_h_ << indent_h() << "bool read(yarp::os::ConnectionReader& connection) override;\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Read structure on a Connection\n";
    f_cpp_ << indent_cpp() << "bool " << name << "::read(yarp::os::ConnectionReader& connection)\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "yarp::os::idl::WireReader reader(connection);\n";
        f_cpp_ << indent_cpp() << "if (!reader.readListHeader(" << flat_element_count(tstruct) << "))" << inline_return_cpp("false");
        f_cpp_ << indent_cpp() << "return read(reader);" << '\n';
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_write_wirereader(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();
    const auto& members = tstruct->get_members();

    f_h_ << indent_h() << "// Write structure on a Wire\n";
    f_h_ << indent_h() << "bool write(const yarp::os::idl::WireWriter& writer) const override;\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Write structure on a Wire\n";
    f_cpp_ << indent_cpp() << "bool " << name << "::write(const yarp::os::idl::WireWriter& writer) const\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        for (const auto& member : members) {
            std::string mname = member->get_name();
            f_cpp_ << indent_cpp() << "if (!write_" << mname << "(writer))" << inline_return_cpp("false");
        }
        f_cpp_ << indent_cpp() << "return !writer.isError();\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_write_connectionreader(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();

    f_h_ << indent_h() << "// Write structure on a Connection\n";
    f_h_ << indent_h() << "bool write(yarp::os::ConnectionWriter& connection) const override;\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Write structure on a Connection\n";
    f_cpp_ << indent_cpp() << "bool " << name << "::write(yarp::os::ConnectionWriter& connection) const\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "yarp::os::idl::WireWriter writer(connection);\n";
        f_cpp_ << indent_cpp() << "if (!writer.writeListHeader(" << flat_element_count(tstruct) << "))" << inline_return_cpp("false");
        f_cpp_ << indent_cpp() << "return write(writer);\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_tostring(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();

    f_h_ << indent_h() << "// Convert to a printable string\n";
    f_h_ << indent_h() << "std::string toString() const;\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Convert to a printable string\n";
    f_cpp_ << indent_cpp() << "std::string " << name << "::toString() const\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "yarp::os::Bottle b;\n";
        f_cpp_ << indent_cpp() << "b.read(*this);\n";
        f_cpp_ << indent_cpp() << "return b.toString();\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_unwrapped_helper(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();

    // unwrapped helper
    f_h_ << indent_h() << "// If you want to serialize this class without nesting, use this helper\n";
    f_h_ << indent_h() << "typedef yarp::os::idl::Unwrapped<" << name << "> unwrapped;\n";
    f_h_ << '\n';

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}

/******************************************************************************/
// BEGIN generate_struct_editor

void t_yarp_generator::generate_struct_editor(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();
    const auto& members = tstruct->get_members();

    f_h_ << indent_h() << "class Editor :\n";
    f_h_ << indent_initializer_h() << "public yarp::os::Wire,\n";
    f_h_ << indent_initializer_h() << "public yarp::os::PortWriter\n";
    f_h_ << indent_h() << "{\n";
    indent_up_h();
    {
        // Public members
        f_h_ << indent_access_specifier_h() << "public:\n";

        generate_struct_editor_default_constructor(tstruct, f_h_, f_cpp_);
        generate_struct_editor_baseclass_constructor(tstruct, f_h_, f_cpp_);
        generate_struct_editor_destructor(tstruct, f_h_, f_cpp_);

        // Deleted constructors and operator=
        THRIFT_DEBUG_COMMENT(f_h_);
        f_h_ << indent_h() << "// Editor: Deleted constructors and operator=\n";
        f_h_ << indent_h() << "Editor(const Editor& rhs) = delete;\n";
        f_h_ << indent_h() << "Editor(Editor&& rhs) = delete;\n";
        f_h_ << indent_h() << "Editor& operator=(const Editor& rhs) = delete;\n";
        f_h_ << indent_h() << "Editor& operator=(Editor&& rhs) = delete;\n";
        f_h_ << '\n';

        generate_struct_editor_edit(tstruct, f_h_, f_cpp_);
        generate_struct_editor_isvalid(tstruct, f_h_, f_cpp_);
        generate_struct_editor_state(tstruct, f_h_, f_cpp_);
        generate_struct_editor_start_editing(tstruct, f_h_, f_cpp_);
        generate_struct_editor_stop_editing(tstruct, f_h_, f_cpp_);

        for (const auto& member : members) {
            f_h_ << indent_h() << "// Editor: " << member->get_name() << " field\n";
            generate_struct_editor_field_setter(tstruct, member, f_h_, f_cpp_);
            // other container types: maps, sets
            if (get_true_type(member->get_type())->is_list()) {
                generate_struct_editor_field_setter_list(tstruct, member, f_h_, f_cpp_);
            }
            generate_struct_editor_field_getter(tstruct, member, f_h_, f_cpp_);
            generate_struct_editor_field_will_set(tstruct, member, f_h_, f_cpp_);
            generate_struct_editor_field_did_set(tstruct, member, f_h_, f_cpp_);
        }

        generate_struct_editor_clean(tstruct, f_h_, f_cpp_);
        generate_struct_editor_read(tstruct, f_h_, f_cpp_);
        generate_struct_editor_write(tstruct, f_h_, f_cpp_);

        // Private members
        f_h_ << indent_access_specifier_h() << "private:\n";

        // State
        THRIFT_DEBUG_COMMENT(f_h_);
        f_h_ << indent_h() << "// Editor: state\n";
        f_h_ << indent_h() << name << "* obj;\n";
        f_h_ << indent_h() << "bool obj_owned;\n";
        f_h_ << indent_h() << "int group;\n";
        f_h_ << '\n';

        // Dirty variables
        THRIFT_DEBUG_COMMENT(f_h_);
        f_h_ << indent_h() << "// Editor: dirty variables\n";
        f_h_ << indent_h() << "bool is_dirty;\n";
        for (const auto& member : members) {
            f_h_ << indent_h() << "bool is_dirty_" << member->get_name() << ";\n";
        }
        f_h_ << indent_h() << "int dirty_count;\n";
        f_h_ << '\n';

        // Methods
        generate_struct_editor_communicate(tstruct, f_h_, f_cpp_);
        generate_struct_editor_mark_dirty(tstruct, f_h_, f_cpp_);

        f_h_ << indent_h() << "// Editor: mark dirty single fields\n";
        for (const auto& member : members) {
            generate_struct_editor_field_mark_dirty(tstruct, member, f_h_, f_cpp_);
        }
        f_h_ << '\n';

        generate_struct_editor_dirty_flags(tstruct, f_h_, f_cpp_);
    }
    indent_down_h();
    f_h_ << indent_h() << "};\n";
    f_h_ << '\n';

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_editor_default_constructor(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();

    f_h_ << indent_h() << "// Editor: default constructor\n";
    f_h_ << indent_h() << "Editor();\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Editor: default constructor\n";
    f_cpp_ << indent_cpp() << name << "::Editor::Editor()\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "group = 0;\n";
        f_cpp_ << indent_cpp() << "obj_owned = true;\n";
        f_cpp_ << indent_cpp() << "obj = new " << name << ";\n";
        f_cpp_ << indent_cpp() << "dirty_flags(false);\n";
        f_cpp_ << indent_cpp() << "yarp().setOwner(*this);\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_editor_baseclass_constructor(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();

    // Editor: constructor with base class
    f_h_ << indent_h() << "// Editor: constructor with base class\n";
    f_h_ << indent_h() << "Editor(" << name << "& obj);\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Editor: constructor with base class\n";
    f_cpp_ << indent_cpp() << name << "::Editor::Editor(" << name << "& obj)\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "group = 0;\n";
        f_cpp_ << indent_cpp() << "obj_owned = false;\n";
        f_cpp_ << indent_cpp() << "edit(obj, false);\n";
        f_cpp_ << indent_cpp() << "yarp().setOwner(*this);\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_editor_destructor(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();

    f_h_ << indent_h() << "// Editor: destructor\n";
    f_h_ << indent_h() << "~Editor() override;\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Editor: destructor\n";
    f_cpp_ << indent_cpp() << name << "::Editor::~Editor()\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "if (obj_owned) {\n";
        indent_up_cpp();
        {
            f_cpp_ << indent_cpp() << "delete obj;\n";
        }
        indent_down_cpp();
        f_cpp_ << indent_cpp() << "}\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_editor_edit(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();

    f_h_ << indent_h() << "// Editor: edit\n";
    f_h_ << indent_h() << "bool edit(" << name << "& obj, bool dirty = true);\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Editor: edit\n";
    f_cpp_ << indent_cpp() << "bool " << name << "::Editor::edit(" << name << "& obj, bool dirty)\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "if (obj_owned) {\n";
        indent_up_cpp();
        {
            f_cpp_ << indent_cpp() << "delete this->obj;\n";
        }
        indent_down_cpp();
        f_cpp_ << indent_cpp() << "}\n";
        f_cpp_ << indent_cpp() << "this->obj = &obj;\n";
        f_cpp_ << indent_cpp() << "obj_owned = false;\n";
        f_cpp_ << indent_cpp() << "dirty_flags(dirty);\n";
        f_cpp_ << indent_cpp() << "return true;\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_editor_isvalid(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();

    f_h_ << indent_h() << "// Editor: validity check\n";
    f_h_ << indent_h() << "bool isValid() const;\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Editor: validity check\n";
    f_cpp_ << indent_cpp() << "bool " << name << "::Editor::isValid() const\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "return obj != nullptr;\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);}

void t_yarp_generator::generate_struct_editor_state(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();

    f_h_ << indent_h() << "// Editor: state\n";
    f_h_ << indent_h() << name << "& state();\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Editor: state\n";
    f_cpp_ << indent_cpp() << name << "& " << name << "::Editor::state()\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "return *obj;\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_editor_start_editing(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();

    f_h_ << indent_h() << "// Editor: start editing\n";
    f_h_ << indent_h() << "void start_editing();\n";
    f_h_ << '\n';

#ifndef YARP_NO_DEPRECATED // Since YARP 3.2
    f_h_ << "#ifndef YARP_NO_DEPRECATED // Since YARP 3.2\n";
    f_h_ << indent_h() << "YARP_DEPRECATED_MSG(\"Use start_editing() instead\")\n";
    f_h_ << indent_h() << "void begin()\n";
    f_h_ << indent_h() << "{\n";
    indent_up_h();
    {
        f_h_ << indent_h()<< "start_editing();\n";
    }
    indent_down_h();

    f_h_ << indent_h() << "}\n";
    f_h_ << "#endif // YARP_NO_DEPRECATED\n";
    f_h_ << '\n';
#endif // YARP_NO_DEPRECATED

    f_cpp_ << indent_cpp() << "// Editor: grouping begin\n";
    f_cpp_ << indent_cpp() << "void " << name << "::Editor::start_editing()\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "group++;\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_editor_stop_editing(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();

    f_h_ << indent_h() << "// Editor: stop editing\n";
    f_h_ << indent_h() << "void stop_editing();\n";
    f_h_ << '\n';

#ifndef YARP_NO_DEPRECATED // Since YARP 3.2
    f_h_ << "#ifndef YARP_NO_DEPRECATED // Since YARP 3.2\n";
    f_h_ << indent_h() << "YARP_DEPRECATED_MSG(\"Use stop_editing() instead\")\n";
    f_h_ << indent_h() << "void end()\n";
    f_h_ << indent_h() << "{\n";
    indent_up_h();
    {
        f_h_ << indent_h()<< "stop_editing();\n";
    }
    indent_down_h();
    f_h_ << indent_h() << "}\n";
    f_h_ << "#endif // YARP_NO_DEPRECATED\n";
    f_h_ << '\n';
#endif // YARP_NO_DEPRECATED

    f_cpp_ << indent_cpp() << "// Editor: grouping end\n";
    f_cpp_ << indent_cpp() << "void " << name << "::Editor::stop_editing()\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "group--;\n";
        f_cpp_ << indent_cpp() << "if (group == 0 && is_dirty) {\n";
        indent_up_cpp();
        {
            f_cpp_ << indent_cpp() << "communicate();\n";
        }
        indent_down_cpp();
        f_cpp_ << indent_cpp() << "}\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_editor_field_setter(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();
    const auto& mname = member->get_name();
    const auto& mtype = type_name(get_true_type(member->get_type()), false, true);

    f_h_ << indent_h() << "void set_" << mname << "(" << mtype << " " << mname << ");\n";

    f_cpp_ << indent_cpp() << "// Editor: " << mname << " setter\n";
    f_cpp_ << indent_cpp() << "void " << name << "::Editor::set_" << mname << "(" << mtype << " " << mname << ")\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "will_set_" << mname << "();\n";
        f_cpp_ << indent_cpp() << "obj->" << mname << " = " << mname << ";\n";
        f_cpp_ << indent_cpp() << "mark_dirty_" << mname << "();\n";
        f_cpp_ << indent_cpp() << "communicate();\n";
        f_cpp_ << indent_cpp() << "did_set_" << mname << "();\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_editor_field_setter_list(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    assert(get_true_type(member->get_type())->is_list());

    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();
    const auto& mname = member->get_name();
    const auto& mtype = type_name(static_cast<t_list*>(get_true_type(member->get_type()))->get_elem_type(), false, true);

    f_h_ << indent_h() << "void set_" << mname << "(size_t index, " << mtype << " elem);\n";

    f_cpp_ << indent_cpp() << "// Editor: " << mname << " setter (list)\n";
    f_cpp_ << indent_cpp() << "void " << name << "::Editor::set_" << mname << "(size_t index, " << mtype << " elem)\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "will_set_" << mname << "();\n";
        f_cpp_ << indent_cpp() << "obj->" << mname << "[index] = elem;\n";
        f_cpp_ << indent_cpp() << "mark_dirty_" << mname << "();\n";
        f_cpp_ << indent_cpp() << "communicate();\n";
        f_cpp_ << indent_cpp() << "did_set_" << mname << "();\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_editor_field_getter(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();
    const auto& mname = member->get_name();
    const auto& mtype = type_name(get_true_type(member->get_type()), false, true, true);

    f_h_ << indent_h() << mtype << " get_" << mname << "() const;\n";

    f_cpp_ << indent_cpp() << "// Editor: " << mname << " getter\n";
    f_cpp_ << indent_cpp() << mtype << " " << name << "::Editor::get_" << mname << "() const\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "return obj->" << mname << ";\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_editor_field_will_set(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();
    const auto& mname = member->get_name();

    f_h_ << indent_h() << "virtual bool will_set_" << mname << "();\n";

    f_cpp_ << indent_cpp() << "// Editor: " << mname << " will_set\n";
    f_cpp_ << indent_cpp() << "bool " << name << "::Editor::will_set_" << mname << "()\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "return true;\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_editor_field_did_set(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();
    const auto& mname = member->get_name();

    f_h_ << indent_h() << "virtual bool did_set_" << mname << "();\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Editor: " << mname << " did_set\n";
    f_cpp_ << indent_cpp() << "bool " << name << "::Editor::did_set_" << mname << "()\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "return true;\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_editor_clean(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();

    f_h_ << indent_h() << "// Editor: clean\n";
    f_h_ << indent_h() << "void clean();\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Editor: clean\n";
    f_cpp_ << indent_cpp() << "void " << name << "::Editor::clean()\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "dirty_flags(false);\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_editor_read(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();
    const auto& members = tstruct->get_members();

    f_h_ << indent_h() << "// Editor: read\n";
    f_h_ << indent_h() << "bool read(yarp::os::ConnectionReader& connection) override;\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Editor: read\n";
    f_cpp_ << indent_cpp() << "bool " << name << "::Editor::read(yarp::os::ConnectionReader& connection)\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        assert(indent_count_cpp() == 1);
        f_cpp_ << indent_cpp() << "if (!isValid())" << inline_return_cpp("false");
        f_cpp_ << indent_cpp() << "yarp::os::idl::WireReader reader(connection);\n";
        f_cpp_ << indent_cpp() << "reader.expectAccept();\n";
        f_cpp_ << indent_cpp() << "if (!reader.readListHeader())" << inline_return_cpp("false");
        f_cpp_ << indent_cpp() << "int len = reader.getLength();\n";
        f_cpp_ << indent_cpp() << "if (len == 0) {\n";
        indent_up_cpp();
        {
            f_cpp_ << indent_cpp() << "yarp::os::idl::WireWriter writer(reader);\n";
            f_cpp_ << indent_cpp() << "if (writer.isNull())" << inline_return_cpp("true");
            f_cpp_ << indent_cpp() << "if (!writer.writeListHeader(1))" << inline_return_cpp("false");
            f_cpp_ << indent_cpp() << "writer.writeString(\"send: 'help' or 'patch (param1 val1) (param2 val2)'\");\n";
            f_cpp_ << indent_cpp() << "return true;\n";
        }
        indent_down_cpp();
        f_cpp_ << indent_cpp() << "}\n";
        f_cpp_ << indent_cpp() << "std::string tag;\n";
        f_cpp_ << indent_cpp() << "if (!reader.readString(tag))" << inline_return_cpp("false");
        f_cpp_ << indent_cpp() << "if (tag == \"help\") {\n";
        indent_up_cpp();
        {
            f_cpp_ << indent_cpp() << "yarp::os::idl::WireWriter writer(reader);\n";
            f_cpp_ << indent_cpp() << "if (writer.isNull())" << inline_return_cpp("true");
            f_cpp_ << indent_cpp() << "if (!writer.writeListHeader(2))" << inline_return_cpp("false");
            f_cpp_ << indent_cpp() << "if (!writer.writeTag(\"many\", 1, 0))" << inline_return_cpp("false");
            f_cpp_ << indent_cpp() << "if (reader.getLength() > 0) {\n";
            indent_up_cpp();
            {
                f_cpp_ << indent_cpp() << "std::string field;\n";
                f_cpp_ << indent_cpp() << "if (!reader.readString(field))" << inline_return_cpp("false");

                for (const auto& member : members) {
                    std::string mname = member->get_name();
                    f_cpp_ << indent_cpp() << "if (field == \"" << mname << "\") {\n";
                    indent_up_cpp();
                    {
                        std::vector<std::string> doc;
                        quote_doc(doc, member);
                        f_cpp_ << indent_cpp() << "if (!writer.writeListHeader(" << (1 + doc.size()) << "))" << inline_return_cpp("false");
                        std::string mtype = type_name(member->get_type());
                        f_cpp_ << indent_cpp() << "if (!writer.writeString(\"" << mtype << " " << mname << "\"))" << inline_return_cpp("false");
                        for (int i = 0; i < (int)doc.size(); i++) {
                            f_cpp_ << indent_cpp() << "if (!writer.writeString(\"" << doc[i] << "\"))" << inline_return_cpp("false");
                        }
                    }
                    indent_down_cpp();
                    f_cpp_ << indent_cpp() << "}\n";
                }
            }
            indent_down_cpp();
            f_cpp_ << indent_cpp() << "}\n";

            f_cpp_ << indent_cpp() << "if (!writer.writeListHeader(" << (members.size() + 1) << "))" << inline_return_cpp("false");
            f_cpp_ << indent_cpp() << "writer.writeString(\"*** Available fields:\");\n";
            for (const auto& member : members) {
                std::string mname = member->get_name();
                f_cpp_ << indent_cpp() << "writer.writeString(\"" << mname << "\");\n";
            }
            f_cpp_ << indent_cpp() << "return true;\n";
        }
        indent_down_cpp();
        f_cpp_ << indent_cpp() << "}\n";

        f_cpp_ << indent_cpp() << "bool nested = true;\n";
        f_cpp_ << indent_cpp() << "bool have_act = false;\n";
        f_cpp_ << indent_cpp() << "if (tag != \"patch\") {\n";
        indent_up_cpp();
        {
            f_cpp_ << indent_cpp() << "if (((len - 1) % 2) != 0)" << inline_return_cpp("false");
            f_cpp_ << indent_cpp() << "len = 1 + ((len - 1) / 2);\n";
            f_cpp_ << indent_cpp() << "nested = false;\n";
            f_cpp_ << indent_cpp() << "have_act = true;\n";
        }
        indent_down_cpp();
        f_cpp_ << indent_cpp() << "}\n";

        f_cpp_ << indent_cpp() << "for (int i = 1; i < len; ++i) {\n";
        indent_up_cpp();
        {
            f_cpp_ << indent_cpp() << "if (nested && !reader.readListHeader(3))" << inline_return_cpp("false");
            f_cpp_ << indent_cpp() << "std::string act;\n";
            f_cpp_ << indent_cpp() << "std::string key;\n";
            f_cpp_ << indent_cpp() << "if (have_act) {\n";
            indent_up_cpp();
            {
                f_cpp_ << indent_cpp() << "act = tag;\n";
            }
            indent_down_cpp();
            f_cpp_ << indent_cpp() << "} else if (!reader.readString(act))" << inline_return_cpp("false");

            f_cpp_ << indent_cpp() << "if (!reader.readString(key))" << inline_return_cpp("false");

            // TODO: inefficient code follows
            bool first = true;
            for (const auto& member : members) {
                std::string mname = member->get_name();
                if (first) {
                    f_cpp_ << indent_cpp();
                    first = false;
                } else {
                    indent_down_cpp();
                    f_cpp_ << indent_cpp() << "} else ";
                }
                f_cpp_ << "if (key == \"" << mname << "\") {\n";
                indent_up_cpp();
                {
                    f_cpp_ << indent_cpp() << "will_set_" << mname << "();\n";
                    f_cpp_ << indent_cpp() << "if (!obj->nested_read_" << mname << "(reader))" << inline_return_cpp("false");
                    f_cpp_ << indent_cpp() << "did_set_" << mname << "();\n";
                }
            }
            if (!members.empty()) {
                indent_down_cpp();
                f_cpp_ << indent_cpp() << "} else {\n";
                indent_up_cpp();
            }
            f_cpp_ << indent_cpp() << "// would be useful to have a fallback here\n";
            if (!members.empty()) {
                indent_down_cpp();
                f_cpp_ << indent_cpp() << "}\n";
            }
        }
        indent_down_cpp();
        f_cpp_ << indent_cpp() << "}\n";

        f_cpp_ << indent_cpp() << "reader.accept();\n";
        f_cpp_ << indent_cpp() << "yarp::os::idl::WireWriter writer(reader);\n";
        f_cpp_ << indent_cpp() << "if (writer.isNull())" << inline_return_cpp("true");
        f_cpp_ << indent_cpp() << "writer.writeListHeader(1);\n";
        f_cpp_ << indent_cpp() << "writer.writeVocab32('o', 'k');\n";
        f_cpp_ << indent_cpp() << "return true;\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_editor_write(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();
    const auto& members = tstruct->get_members();

    f_h_ << indent_h() << "// Editor: write\n";
    f_h_ << indent_h() << "bool write(yarp::os::ConnectionWriter& connection) const override;\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Editor: write\n";
    f_cpp_ << indent_cpp() << "bool " << name << "::Editor::write(yarp::os::ConnectionWriter& connection) const\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "if (!isValid())" << inline_return_cpp("false");
        f_cpp_ << indent_cpp() << "yarp::os::idl::WireWriter writer(connection);\n";
        f_cpp_ << indent_cpp() << "if (!writer.writeListHeader(dirty_count + 1))" << inline_return_cpp("false");
        f_cpp_ << indent_cpp() << "if (!writer.writeString(\"patch\"))" << inline_return_cpp("false");
        for (const auto& member : members) {
            std::string mname = member->get_name();
            f_cpp_ << indent_cpp() << "if (is_dirty_" << mname << ") {\n";
            indent_up_cpp();
            {
                f_cpp_ << indent_cpp() << "if (!writer.writeListHeader(3))" << inline_return_cpp("false");
                f_cpp_ << indent_cpp() << "if (!writer.writeString(\"set\"))" << inline_return_cpp("false");
                f_cpp_ << indent_cpp() << "if (!writer.writeString(\"" << mname << "\"))" << inline_return_cpp("false");
                f_cpp_ << indent_cpp() << "if (!obj->nested_write_" << mname << "(writer))" << inline_return_cpp("false");
            }
            indent_down_cpp();
            f_cpp_ << indent_cpp() << "}\n";
        }
        f_cpp_ << indent_cpp() << "return !writer.isError();\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_editor_communicate(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();

    f_h_ << indent_h() << "// Editor: send if possible\n";
    f_h_ << indent_h() << "void communicate();\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Editor: send if possible\n";
    f_cpp_ << indent_cpp() << "void " << name << "::Editor::communicate()\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();\
    {
        f_cpp_ << indent_cpp() << "if (group != 0)" << inline_return_cpp();
        f_cpp_ << indent_cpp() << "if (yarp().canWrite()) {\n";
        indent_up_cpp();
        {
            f_cpp_ << indent_cpp() << "yarp().write(*this);\n";
            f_cpp_ << indent_cpp() << "clean();\n";
        }
        indent_down_cpp();
        f_cpp_ << indent_cpp() << "}\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);
}


void t_yarp_generator::generate_struct_editor_mark_dirty(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();

    f_h_ << indent_h() << "// Editor: mark dirty overall\n";
    f_h_ << indent_h() << "void mark_dirty();\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Editor: mark dirty overall\n";
    f_cpp_ << indent_cpp() << "void " << name << "::Editor::mark_dirty()\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "is_dirty = true;\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);

}

void t_yarp_generator::generate_struct_editor_field_mark_dirty(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();
    const auto& mname = member->get_name();

    f_h_ << indent_h() << "void mark_dirty_" << mname << "();\n";

    f_cpp_ << indent_cpp() << "// Editor: " << mname << " mark_dirty\n";
    f_cpp_ << indent_cpp() << "void " << name << "::Editor::mark_dirty_" << mname << "()\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "if (is_dirty_" << mname << ")" << inline_return_cpp();
        f_cpp_ << indent_cpp() << "dirty_count++;\n";
        f_cpp_ << indent_cpp() << "is_dirty_" << mname << " = true;\n";
        f_cpp_ << indent_cpp() << "mark_dirty();\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_editor_dirty_flags(t_struct* tstruct, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();
    const auto& members = tstruct->get_members();

    f_h_ << indent_h() << "// Editor: dirty_flags\n";
    f_h_ << indent_h() << "void dirty_flags(bool flag);\n";

    f_cpp_ << indent_cpp() << "// Editor: dirty_flags\n";
    f_cpp_ << indent_cpp() << "void " << name << "::Editor::dirty_flags(bool flag)\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "is_dirty = flag;\n";
        for (const auto& member : members) {
            std::string mname = member->get_name();
            f_cpp_ << indent_cpp() << "is_dirty_" << mname << " = flag;\n";
        }
        f_cpp_ << indent_cpp() << "dirty_count = flag ? " << members.size() << " : 0;\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 2);
    assert(indent_count_cpp() == 0);
}

// END generate_struct_editor
/******************************************************************************/

void t_yarp_generator::generate_struct_field_read(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();
    const auto& mname = member->get_name();

    f_h_ << indent_h() << "bool read_" << mname << "(yarp::os::idl::WireReader& reader);\n";

    f_cpp_ << indent_cpp() << "// read " << mname << " field\n";
    f_cpp_ << indent_cpp() << "bool " << name << "::read_" << mname << "(yarp::os::idl::WireReader& reader)\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        generate_deserialize_field(f_cpp_, member, "");
        f_cpp_ << indent_cpp() << "return true;\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_field_write(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();
    const auto& mname = member->get_name();

    f_h_ << indent_h() << "bool write_" << mname << "(const yarp::os::idl::WireWriter& writer) const;\n";

    f_cpp_ << indent_cpp() << "// write " << mname << " field\n";
    f_cpp_ << indent_cpp() << "bool " << name << "::write_" << mname << "(const yarp::os::idl::WireWriter& writer) const\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        generate_serialize_field(f_cpp_, member);
        f_cpp_ << indent_cpp() << "return true;\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}


void t_yarp_generator::generate_struct_field_nested_read(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();
    const auto& mname = member->get_name();

    f_h_ << indent_h() << "bool nested_read_" << mname << "(yarp::os::idl::WireReader& reader);\n";

    f_cpp_ << indent_cpp() << "// read (nested) " << mname << " field\n";
    f_cpp_ << indent_cpp() << "bool " << name << "::nested_read_" << mname << "(yarp::os::idl::WireReader& reader)\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        generate_deserialize_field(f_cpp_, member, "", "", true);
        f_cpp_ << indent_cpp() << "return true;\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_struct_field_nested_write(t_struct* tstruct, t_field* member, std::ostringstream& f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& name = tstruct->get_name();
    const auto& mname = member->get_name();

    f_h_ << indent_h() << "bool nested_write_" << mname << "(const yarp::os::idl::WireWriter& writer) const;\n";

    f_cpp_ << indent_cpp() << "// write (nested) " << mname << " field\n";
    f_cpp_ << indent_cpp() << "bool " << name << "::nested_write_" << mname << "(const yarp::os::idl::WireWriter& writer) const\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        generate_serialize_field(f_cpp_, member, "", "", true);
        f_cpp_ << indent_cpp() << "return true;\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}

// END generate_struct
/******************************************************************************/

/******************************************************************************/
// BEGIN generate_xception

void t_yarp_generator::generate_xception(t_struct* txception)
{
    generate_struct(txception);
}

// END generate_xception
/******************************************************************************/


/******************************************************************************/
// BEGIN generate_service

/**
 * Generates a service definition for a thrift data type.
 *
 * @param tservice The service definition
 */
void t_yarp_generator::generate_service(t_service* tservice)
{
    assert(indent_count_h() == 0);
    assert(indent_count_cpp() == 0);

    const auto& service_name = tservice->get_name();
    const auto& extends_service = tservice->get_extends();

    // Open header file
    std::string f_header_name = get_out_dir() + get_include_prefix(program_) + service_name + ".h";
    ofstream_with_content_based_conditional_update f_h_;
    f_h_.open(f_header_name);

    // Open cpp files
    std::string f_cpp_name = get_out_dir() + get_include_prefix(program_) + service_name + ".cpp";
    ofstream_with_content_based_conditional_update f_cpp_;
    f_cpp_.open(f_cpp_name);

    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    // Add copyright
    f_h_ << copyright_comment();
    f_cpp_ << copyright_comment();

    // Add auto-generated warning
    f_h_ << autogen_comment();
    f_cpp_ << autogen_comment();

    // Add header guards to .h file
    f_h_ << "#ifndef YARP_THRIFT_GENERATOR_SERVICE_" << upcase_string(service_name) << "_H\n";
    f_h_ << "#define YARP_THRIFT_GENERATOR_SERVICE_" << upcase_string(service_name) << "_H\n";
    f_h_ << '\n';

    // Add includes to .h file
    f_h_ << "#include <yarp/os/Wire.h>\n";
    f_h_ << "#include <yarp/os/idl/WireTypes.h>\n";

    if (need_common_) {
        f_h_ << "#include <" << get_include_prefix(program_) << program_->get_name() << "_common.h>" << '\n';
    }

    if (extends_service != nullptr) {
        f_h_ << "#include <" << get_include_prefix(extends_service->get_program()) << extends_service->get_name() << ".h>\n";
    }

    std::set<std::string> neededTypes;
    for (const auto& function : tservice->get_functions()) {
        auto returntype = function->get_returntype();
        if (!returntype->is_void()) {
            get_needed_type(returntype, neededTypes);
        }
        for (const auto& arg : function->get_arglist()->get_members()) {
            get_needed_type(arg->get_type(), neededTypes);
        }
    }

    for (const auto& neededType: neededTypes) {
        f_h_ << "#include <" << neededType << ">\n";
    }

    f_h_ << '\n';

    // Add includes to .cpp file
    f_cpp_ << "#include <" << get_include_prefix(tservice->get_program()) + service_name + ".h>" << '\n';
    f_cpp_ << '\n';
    f_cpp_ << "#include <yarp/os/idl/WireTypes.h>\n";
    f_cpp_ << '\n';

    // Open namespace
    generate_namespace_open(f_h_, f_cpp_);

    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    // Add service documentation
    print_doc(f_h_, tservice);

    // Begin class
    f_h_ << "class " << service_name_ << " :\n";
    f_h_ << indent_initializer_h() << "public " << (extends_service ? type_name(extends_service) : "yarp::os::Wire") << '\n';
    f_h_ << indent_h() << "{\n";
    indent_up_h();
    f_h_ << indent_access_specifier_h() << "public:\n";

    generate_service_helper_classes(tservice, f_cpp_);
    generate_service_constructor(tservice, f_h_, f_cpp_);

    // Functions
    for (const auto& function : tservice->get_functions()) {
        generate_service_method(tservice, function, f_h_, f_cpp_);
    }

    generate_service_help(tservice, f_h_, f_cpp_);
    generate_service_read(tservice, f_h_, f_cpp_);

    indent_down_h();

    // End class
    f_h_ << indent_h() << "};\n";

    // Close namespace
    generate_namespace_close(f_h_, f_cpp_);

    // Close header guard
    f_h_ << '\n';
    f_h_ << "#endif // YARP_THRIFT_GENERATOR_SERVICE_" << upcase_string(service_name) << "_H\n";

    assert(indent_count_h() == 0);
    assert(indent_count_cpp() == 0);
}


void t_yarp_generator::generate_service_helper_classes(t_service* tservice, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_cpp_);

    for (const auto& function : tservice->get_functions()) {
        // Helper classes declarations
        generate_service_helper_classes_decl(function, f_cpp_);

        // Helper classes implementations
        generate_service_helper_classes_impl_ctor(function, f_cpp_);
        generate_service_helper_classes_impl_write(function, f_cpp_);
        generate_service_helper_classes_impl_read(function, f_cpp_);

        assert(indent_count_h() == 1);
        assert(indent_count_cpp() == 0);
    }
}

void t_yarp_generator::generate_service_helper_classes_decl(t_function* function, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& fname = function->get_name();
    const auto& args = function->get_arglist()->get_members();
    const auto& returntype = function->get_returntype();
    const auto helper_class = std::string{service_name_ + "_" + fname + "_helper"};
    auto returnfield = t_field{returntype, "s_return_helper"};

    f_cpp_ << "class " << helper_class << " :\n";
    f_cpp_ << indent_initializer_cpp() << "public yarp::os::Portable\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_access_specifier_cpp() << "public:\n";
        f_cpp_ << indent_cpp() << "explicit " << function_prototype(function, false, false, false, "", helper_class) << ";\n";
        f_cpp_ << indent_cpp() << "bool write(yarp::os::ConnectionWriter& connection) const override;\n";
        f_cpp_ << indent_cpp() << "bool read(yarp::os::ConnectionReader& connection) override;\n";

        bool first = true;
        for (const auto& arg : args) {
            if (first) {
                f_cpp_ << '\n';
                first = false;
            }
            f_cpp_ << indent_cpp() << declare_field(arg, false, false, false, false, {}, "m_") << ";\n";
        }

        if (!returntype->is_void()) {
            f_cpp_ << '\n';
            // This variable is declared thread_local because of #2021, see
            // https://github.com/robotology/yarp/issues/2021 for details
            f_cpp_ << indent_cpp() << "thread_local static " << declare_field(&returnfield) << ";\n";
        }
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "};\n";
    f_cpp_ << '\n';

    if (!returntype->is_void()) {
        f_cpp_ << indent_cpp() << "thread_local " << type_name(returntype) << " " << helper_class << "::" << returnfield.get_name() << " = {};\n";
        f_cpp_ << '\n';
    }
}

void t_yarp_generator::generate_service_helper_classes_impl_ctor(t_function* function, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& fname = function->get_name();
    const auto& returntype = function->get_returntype();
    const auto helper_class = std::string{service_name_ + "_" + fname + "_helper"};
    auto returnfield = t_field{returntype, "s_return_helper"};

    f_cpp_ << indent_cpp() << function_prototype(function, false, false, false, helper_class, helper_class);
    bool first = true;
    for (const auto& arg : function->get_arglist()->get_members()) {
        if (first) {
            f_cpp_ << " :\n";
            first = false;
        } else {
            f_cpp_ << ",\n";
        }
        f_cpp_ << indent_initializer_cpp() << "m_" << arg->get_name() << "{" << arg->get_name() << "}";
    }

    f_cpp_ << '\n';
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        if (!returntype->is_void()) {
            if (returntype->is_base_type() || returntype->is_enum()) {
                f_cpp_ << indent_cpp() << returnfield.get_name() << " = {};\n";
            }
        }
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';
}

void t_yarp_generator::generate_service_helper_classes_impl_write(t_function* function, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& fname = function->get_name();
    const auto& args = function->get_arglist()->get_members();
    const auto helper_class = std::string{service_name_ + "_" + fname + "_helper"};
    const size_t sz = std::count(fname.begin(), fname.end(), '_') + 1;

    f_cpp_ << indent_cpp() << "bool " << helper_class << "::write(yarp::os::ConnectionWriter& connection) const\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "yarp::os::idl::WireWriter writer(connection);\n";
        f_cpp_ << indent_cpp() << "if (!writer.writeListHeader(" << flat_element_count(function) + sz << "))" << inline_return_cpp("false");
        f_cpp_ << indent_cpp() << "if (!writer.writeTag(\"" << fname << "\", 1, " << sz << "))" << inline_return_cpp("false");
        for(const auto& arg : args) {
            generate_serialize_field(f_cpp_, arg, "m_");
        }
        f_cpp_ << indent_cpp() << "return true;\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';
}

void t_yarp_generator::generate_service_helper_classes_impl_read(t_function* function, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& fname = function->get_name();
    const auto& returntype = function->get_returntype();
    const auto helper_class = std::string{service_name_ + "_" + fname + "_helper"};
    auto returnfield = t_field{returntype, "s_return_helper"};

    f_cpp_ << indent_cpp() << "bool " << helper_class << "::read(yarp::os::ConnectionReader& connection)\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        if (!function->is_oneway()) {
            f_cpp_ << indent_cpp() << "yarp::os::idl::WireReader reader(connection);\n";
            if (returntype->annotations_.find("yarp.name") == (returntype->annotations_.end())) {
                // Types annotated with yarp.name therefore are expected
                // to be able to serialize by themselves, therefore there is
                // no need to read them as lists.
                f_cpp_ << indent_cpp() << "if (!reader.readListReturn())" << inline_return_cpp("false");
            }
            if (!returntype->is_void()) {
                generate_deserialize_field(f_cpp_, &returnfield, "");
            }
        } else {
            f_cpp_ << indent_cpp() << "YARP_UNUSED(connection);\n";
        }
        f_cpp_ << indent_cpp() << "return true;\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';
}

void t_yarp_generator::generate_service_constructor(t_service* tservice, std::ostringstream&  f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    // Constructor
    f_h_ << indent_h() << "// Constructor\n";
    f_h_ << indent_h() << service_name_ << "();\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// Constructor\n";
    f_cpp_ << indent_cpp() << service_name_ << "::" << service_name_ << "()\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "yarp().setOwner(*this);\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_service_method(t_service* tservice, t_function* function, std::ostringstream&  f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    const auto& fname = function->get_name();
    const auto& returntype = function->get_returntype();
    const auto helper_class = std::string{service_name_ + "_" + fname + "_helper"};
    auto returnfield = t_field{returntype, "s_return_helper"};

    print_doc(f_h_, function);
    f_h_ << indent_h() << "virtual " << function_prototype(function, true, true, true) << ";\n";
    f_h_ << '\n';

    f_cpp_ << indent_cpp() << function_prototype(function, false, true, true, service_name_) << '\n';
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << helper_class << " helper{";
        bool first = true;
        for (const auto& arg : function->get_arglist()->get_members()) {
            if (!first) {
                f_cpp_ << ", ";
            }
            first = false;
            f_cpp_ << arg->get_name();
        }
        f_cpp_ << "};\n";
        f_cpp_ << indent_cpp() << "if (!yarp().canWrite()) {\n";
        indent_up_cpp();
        {
            f_cpp_ << indent_cpp() << "yError(\"Missing server method '%s'?\", \"";
            f_cpp_ << function_prototype(function, false, true, true, service_name_);
            f_cpp_ << "\");\n";
        }
        indent_down_cpp();
        f_cpp_ << indent_cpp() << "}\n";

        f_cpp_ << indent_cpp()
                    << (!returntype->is_void() ? "bool ok = " : "")
                    << (!function->is_oneway() ? "yarp().write(helper, helper);" : "yarp().write(helper);")
                    << '\n';
        if (!returntype->is_void()) {
            f_cpp_ << indent_cpp() << "return ok ? " << helper_class << "::" << returnfield.get_name() << " : " << type_name(returntype) << "{};\n";
        }
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_service_help(t_service* tservice, std::ostringstream&  f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    f_h_ << indent_h() << "// help method\n";
    if (tservice->get_extends() != nullptr) {
        f_h_ << indent_h() << "std::vector<std::string> help(const std::string& functionName = \"--all\") override;\n";
    } else {
        f_h_ << indent_h() << "virtual std::vector<std::string> help(const std::string& functionName = \"--all\");\n";
    }

    f_h_ << '\n';

    f_cpp_ << indent_cpp() << "// help method\n";
    f_cpp_ << indent_cpp() << "std::vector<std::string> " << service_name_ << "::help(const std::string& functionName)\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "bool showAll = (functionName == \"--all\");\n";
        f_cpp_ << indent_cpp() << "std::vector<std::string> helpString;\n";
        f_cpp_ << indent_cpp() << "if (showAll) {\n";
        indent_up_cpp();
        {
            f_cpp_ << indent_cpp() << "helpString.emplace_back(\"*** Available commands:\");\n";
            for (const auto& function : tservice->get_functions()) {
                f_cpp_ << indent_cpp() << "helpString.emplace_back(\"" << function->get_name() << "\");\n";
            }
            f_cpp_ << indent_cpp() << "helpString.emplace_back(\"help\");\n";
        }
        indent_down_cpp();
        f_cpp_ << indent_cpp() << "} else {\n";
        indent_up_cpp();
        {
            for (const auto& function : tservice->get_functions()) {
                f_cpp_ << indent_cpp() << "if (functionName == \"" << function->get_name() << "\") {\n";
                indent_up_cpp();
                {
                    auto helpList = print_help(function);
                    for (const auto& helpStr : helpList) {
                        f_cpp_ << indent_cpp() << "helpString.emplace_back(\"" << helpStr << " \");" << '\n';
                    }
                }
                indent_down_cpp();
                f_cpp_ << indent_cpp() << "}\n";
            }

            f_cpp_ << indent_cpp() << "if (functionName == \"help\") {\n";
            indent_up_cpp();
            {
                f_cpp_ << indent_cpp() << "helpString.emplace_back(\"std::vector<std::string> help(const std::string& functionName = \\\"--all\\\")\");" << '\n';
                f_cpp_ << indent_cpp() << "helpString.emplace_back(\"Return list of available commands, or help message for a specific function\");" << '\n';
                f_cpp_ << indent_cpp() << "helpString.emplace_back(\"@param functionName name of command for which to get a detailed description. If none or '--all' is provided, print list of available commands\");" << '\n';
                f_cpp_ << indent_cpp() << "helpString.emplace_back(\"@return list of strings (one string per line)\");" << '\n';
            }
            indent_down_cpp();
            f_cpp_ << indent_cpp() << "}\n";
        }
        indent_down_cpp();
        f_cpp_ << indent_cpp() << "}\n";
        f_cpp_ << indent_cpp() << "if (helpString.empty()) {\n";
        indent_up_cpp();
        {
            f_cpp_ << indent_cpp() << "helpString.emplace_back(\"Command not found\");\n";
        }
        indent_down_cpp();
        f_cpp_ << indent_cpp() << "}\n";
        f_cpp_ << indent_cpp() << "return helpString;\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";
    f_cpp_ << '\n';

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}

void t_yarp_generator::generate_service_read(t_service* tservice, std::ostringstream&  f_h_, std::ostringstream& f_cpp_)
{
    THRIFT_DEBUG_COMMENT(f_h_);
    THRIFT_DEBUG_COMMENT(f_cpp_);

    f_h_ << indent_h() << "// read from ConnectionReader\n";
    f_h_ << indent_h() << "bool read(yarp::os::ConnectionReader& connection) override;\n";

    f_cpp_ << indent_cpp() << "// read from ConnectionReader\n";
    f_cpp_ << indent_cpp() << "bool " << service_name_ << "::read(yarp::os::ConnectionReader& connection)\n";
    f_cpp_ << indent_cpp() << "{\n";
    indent_up_cpp();
    {
        f_cpp_ << indent_cpp() << "yarp::os::idl::WireReader reader(connection);\n";
        f_cpp_ << indent_cpp() << "reader.expectAccept();\n";
        f_cpp_ << indent_cpp() << "if (!reader.readListHeader()) {\n";
        indent_up_cpp();
        {
            f_cpp_ << indent_cpp() << "reader.fail();\n";
            f_cpp_ << indent_cpp() << "return false;\n";
        }
        indent_down_cpp();
        f_cpp_ << indent_cpp() << "}\n";

        f_cpp_ << '\n';

        f_cpp_ << indent_cpp() << "std::string tag = reader.readTag();\n";
        f_cpp_ << indent_cpp() << "bool direct = (tag == \"__direct__\");\n";
        f_cpp_ << indent_cpp() << "if (direct) {\n";
        indent_up_cpp();
        {
            f_cpp_ << indent_cpp() << "tag = reader.readTag();\n";
        }
        indent_down_cpp();
        f_cpp_ << indent_cpp() << "}\n";
        f_cpp_ << indent_cpp() << "while (!reader.isError()) {\n";
        indent_up_cpp();
        {
            // TODO: use quick lookup, this is just a test
            for (const auto& function : tservice->get_functions()) {

                const auto& fname = function->get_name();
                const auto& args = function->get_arglist()->get_members();
                const auto& returntype = function->get_returntype();
                const auto helper_class = std::string{service_name_ + "_" + fname + "_helper"};
                auto returnfield = t_field{returntype, "s_return_helper"};

                f_cpp_ << indent_cpp() << "if (tag == \"" << fname << "\") {\n";
                indent_up_cpp();
                {
                    for (const auto& arg : args) {
                        f_cpp_ << indent_cpp() << declare_field(arg) << ";\n";
                    }
                    for (const auto& arg : args) {
                        generate_deserialize_field(f_cpp_, arg, "");
                    }


                    if (function->is_oneway()) {
                        f_cpp_ << indent_cpp() << "if (!direct) {\n";
                        indent_up_cpp();
                        {
                            f_cpp_ << indent_cpp() << helper_class << " helper{";
                            bool first = true;
                            for (const auto& arg : args) {
                                if (!first) {
                                    f_cpp_ << ", ";
                                }
                                first = false;
                                f_cpp_ << arg->get_name();
                            }
                            f_cpp_ << "};\n";
                            f_cpp_ << indent_cpp() << "yarp().callback(helper, *this, \"__direct__\");\n";
                        }
                        indent_down_cpp();
                        f_cpp_ << indent_cpp() << "} else {\n";
                        indent_up_cpp();
                    }

                    if (!returntype->is_void()) {
                        f_cpp_ << indent_cpp() << helper_class << "::" << returnfield.get_name() << " = ";
                    } else {
                        f_cpp_ << indent_cpp();
                    }
                    f_cpp_ << fname << "(";
                    bool first = true;
                    for (const auto& arg : args) {
                        if (!first) {
                            f_cpp_ << ", ";
                        }
                        first = false;
                        f_cpp_ << arg->get_name();
                    }
                    f_cpp_ << ");\n";

                    if (function->is_oneway()) {
                        indent_down_cpp();
                        f_cpp_ << indent_cpp() << "}\n";
                    }

                    f_cpp_ << indent_cpp() << "yarp::os::idl::WireWriter writer(reader);\n";
                    f_cpp_ << indent_cpp() << "if (!writer.isNull()) {\n";
                    indent_up_cpp();
                    {
                        if (!function->is_oneway()) {
                            if (returntype->annotations_.find("yarp.name") == (returntype->annotations_.end())) {
                                // Types annotated with yarp.name therefore are expected
                                // to be able to serialize by themselves, therefore there is
                                // no need to write them as lists.
                                // For all the other types, append the number of fields
                                f_cpp_ << indent_cpp() << "if (!writer.writeListHeader(" << flat_element_count(returntype) << "))" << inline_return_cpp("false");
                            }
                            if (!returntype->is_void()) {
                                generate_serialize_field(f_cpp_, &returnfield, helper_class + "::");
                            }
                        } else {
                            // we are a oneway function
                            // if someone is expecting a reply (e.g. yarp rpc), give one
                            // (regular thrift client won't be expecting a reply, and
                            // writer.isNull test will have succeeded and stopped us earlier)
                            f_cpp_ << indent_cpp() << "if (!writer.writeOnewayResponse())" << inline_return_cpp("false");
                        }
                    }
                    indent_down_cpp();
                    f_cpp_ << indent_cpp() << "}\n";

                    f_cpp_ << indent_cpp() << "reader.accept();\n";
                    f_cpp_ << indent_cpp() << "return true;\n";
                }
                indent_down_cpp();
                f_cpp_ << indent_cpp() << "}\n";
            }

            // read "help" function
            f_cpp_ << indent_cpp() << "if (tag == \"help\") {\n";
            indent_up_cpp();
            {
                f_cpp_ << indent_cpp() << "std::string functionName;\n";
                f_cpp_ << indent_cpp() << "if (!reader.readString(functionName)) {\n";
                indent_up_cpp();
                {
                    f_cpp_ << indent_cpp() << "functionName = \"--all\";" << '\n';
                }
                indent_down_cpp();
                f_cpp_ << indent_cpp() << "}\n";
                f_cpp_ << indent_cpp() << "auto help_strings = help(functionName);" << '\n';
                f_cpp_ << indent_cpp() << "yarp::os::idl::WireWriter writer(reader);\n";
                f_cpp_ << indent_cpp() << "if (!writer.isNull()) {\n";
                indent_up_cpp();
                {
                    f_cpp_ << indent_cpp() << "if (!writer.writeListHeader(2))" << inline_return_cpp("false");
                    f_cpp_ << indent_cpp() << "if (!writer.writeTag(\"many\", 1, 0))" << inline_return_cpp("false");
                    f_cpp_ << indent_cpp() << "if (!writer.writeListBegin(BOTTLE_TAG_INT32, static_cast<uint32_t>(help_strings.size())))" << inline_return_cpp("false");
                    f_cpp_ << indent_cpp() << "for (const auto& help_string : help_strings) {\n";
                    indent_up_cpp();
                    {
                        f_cpp_ << indent_cpp() << "if (!writer.writeString(help_string))" << inline_return_cpp("false");
                    }
                    indent_down_cpp();
                    f_cpp_ << indent_cpp() << "}\n";
                    f_cpp_ << indent_cpp() << "if (!writer.writeListEnd())" << inline_return_cpp("false");
                }
                indent_down_cpp();
                f_cpp_ << indent_cpp() << "}\n";
                f_cpp_ << indent_cpp() << "reader.accept();\n";
                f_cpp_ << indent_cpp() << "return true;\n";
            }
            indent_down_cpp();
            f_cpp_ << indent_cpp() << "}\n";

            f_cpp_ << indent_cpp() << "if (reader.noMore()) {\n";
            indent_up_cpp();
            {
                f_cpp_ << indent_cpp() << "reader.fail();\n";
                f_cpp_ << indent_cpp() << "return false;\n";
            }
            indent_down_cpp();
            f_cpp_ << indent_cpp() << "}\n";

            f_cpp_ << indent_cpp() << "std::string next_tag = reader.readTag();\n";
            f_cpp_ << indent_cpp() << "if (next_tag == \"\") {\n";
            indent_up_cpp();
            {
                f_cpp_ << indent_cpp() << "break;\n";
            }
            indent_down_cpp();
            f_cpp_ << indent_cpp() << "}\n";
            f_cpp_ << indent_cpp() << "tag.append(\"_\").append(next_tag);\n";
        }
        indent_down_cpp();
        f_cpp_ << indent_cpp() << "}\n";
        f_cpp_ << indent_cpp() << "return false;\n";
    }
    indent_down_cpp();
    f_cpp_ << indent_cpp() << "}\n";

    assert(indent_count_h() == 1);
    assert(indent_count_cpp() == 0);
}

// END generate_service
/******************************************************************************/


THRIFT_REGISTER_GENERATOR(
    yarp,
    "YARP",
    "    include_prefix:       The include prefix to use for the generated files\n"
    "    no_namespace_prefix:  Omit the namespace from the include prefix\n"
    "    no_copyright:         Omit the copyright header.\n"
    "    no_editor:            Omit the generation of the Editor class for structs.\n"
    "    no_doc:               Omit doxygen documentation.\n"
    "    debug_generator:      Add generator debug information in generated code.\n")
