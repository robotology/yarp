/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <set>

#include <stdlib.h>
#include <sys/stat.h>
#include <sstream>
#include "t_generator.h"
#include "t_oop_generator.h"
#include "platform.h"
using namespace std;


class yfn {
public:
  std::string name;
  bool split;
  int len;

  yfn(const std::string& name) {
    apply(name);
  }

  void apply(const std::string& name) {
    this->name = name;
    int ct = 1;
    int ct1 = 0;
    int ct_max = 0;
    for (string::size_type i=0; i<name.size(); i++) {
      if (name[i]=='_') {
        ct++;
        ct1 = 0;
      } else {
        ct1++;
        if (ct1>ct_max) ct_max = ct1;
      }
    }
    //split = (ct_max<=4);
    //len = split?ct:1;
    len = ct;
  }
};


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
class t_yarp_generator : public t_oop_generator {
 public:
  t_yarp_generator(
      t_program* program,
      const std::map<std::string, std::string>& parsed_options,
      const std::string& option_string)
    : t_oop_generator(program)
  {
    (void) option_string;

    std::map<std::string, std::string>::const_iterator iter;

    iter = parsed_options.find("cmake_supplies_headers");
    cmake_supplies_headers_ = (iter != parsed_options.end());

    iter = parsed_options.find("include_prefix");
    use_include_prefix_ = (iter != parsed_options.end());

    out_dir_base_ = "gen-yarp";
    gen_pure_enums_ = true;

    need_common_=false;

  }

  void generate_program();
  void generate_program_toc();
  void generate_program_toc_row(t_program* tprog);
  void generate_program_toc_rows(t_program* tprog,
         std::vector<t_program*>& finished);
  void generate_index();

  /**
   * Program-level generation functions
   */

  bool use_include_prefix_;
  std::string get_include_prefix(const t_program& program) const;

void getNeededType(t_type* curType, std::set<string>& neededTypes)
{
    string mtype;
    if (curType->is_struct())
    {
        if (((t_struct*)curType)->annotations_.find("yarp.includefile") != ((t_struct*)curType)->annotations_.end())
            mtype = ((t_struct*)curType)->annotations_["yarp.includefile"];
        else
            mtype = get_include_prefix(*(curType->get_program())) + curType->get_name() + ".h";

        neededTypes.insert(mtype);
       // cout << mtype <<endl;
        return;
    }

    if (curType->is_enum())
    {
        mtype = get_include_prefix(*(curType->get_program())) + curType->get_name() + ".h";
        neededTypes.insert(mtype);
        return;
    }


    if (curType->is_list()){
        getNeededType(((t_list*) curType)->get_elem_type(), neededTypes);
        return;
    }

    if (curType->is_set()){
        getNeededType(((t_set*) curType)->get_elem_type(), neededTypes);
        return;
    }

    if (curType->is_map())
    {
        getNeededType(((t_map*) curType)->get_key_type(), neededTypes);
        getNeededType(((t_map*) curType)->get_val_type(), neededTypes);
        return;
    }

}

  void generate_typedef (t_typedef*  ttypedef);
  void generate_enum    (t_enum*     tenum);
  void generate_const   (t_const*    tconst);
  void generate_struct  (t_struct*   tstruct);
  void generate_service (t_service*  tservice);
  void generate_xception(t_struct*   txception);

  void print_doc        (std::ofstream& out, t_doc* tdoc);
  void quote_doc       (std::vector<std::string>& doxyPar, t_doc* tdoc);
  std::vector<std::string> print_help       (t_function* tdoc);
  std::string print_type       (t_type* ttype);
  std::string print_const_value(t_const_value* tvalue,
                                t_type* ttype = NULL);

  std::string function_prototype(t_function *tfn, bool include_defaults,
                                 const char *prefix=NULL,
                                 const char *override_name=NULL);

  std::string declare_field(t_field* tfield, bool init=false, bool pointer=false, bool is_constant=false, bool reference=false, const char *force_type=NULL);

  std::string type_name(t_type* ttype, bool in_typedef=false, bool arg=false, bool ret = false);
  std::string base_type_name(t_base_type::t_base tbase);
  std::string namespace_prefix(std::string ns);
  std::string namespace_decorate(std::string ns, std::string str);
  bool namespace_open(ostream& os, std::string ns, bool terse = true);
  void namespace_close(ostream& os, std::string ns, bool terse = true);
  bool is_complex_type(t_type* ttype) {
    ttype = get_true_type(ttype);
    return
      ttype->is_container() ||
      ttype->is_struct() ||
      ttype->is_xception() ||
      (ttype->is_base_type() && (((t_base_type*)ttype)->get_base() == t_base_type::TYPE_STRING));
  }

  void generate_count_field          (std::ofstream& out,
                                      t_field*    tfield,
                                      std::string prefix="",
                                      std::string suffix="");

  void generate_serialize_field          (std::ofstream& out,
                                          t_field*    tfield,
                                          std::string prefix="",
                                          std::string suffix="",
                                          bool force_nesting = false);

  void generate_serialize_struct         (std::ofstream& out,
                                          t_struct*   tstruct,
                                          std::string prefix="",
                                          bool force_nesting = false);

  void generate_serialize_container      (std::ofstream& out,
                                          t_type*     ttype,
                                          std::string prefix="");

  void generate_serialize_map_element    (std::ofstream& out,
                                          t_map*      tmap,
                                          std::string iter);

  void generate_serialize_set_element    (std::ofstream& out,
                                          t_set*      tmap,
                                          std::string iter);

  void generate_serialize_list_element   (std::ofstream& out,
                                          t_list*     tlist,
                                          std::string iter);

  void generate_function_call            (ostream& out,
                                          t_function* tfunction,
                                          string target,
                                          string iface,
                                          string arg_prefix);


  void generate_deserialize_field_fallback(ofstream& out,
                                           t_field* tfield);


  void generate_deserialize_field        (std::ofstream& out,
                                          t_field*    tfield,
                                          std::string prefix="",
                                          std::string suffix="",
                                          bool force_nested = false);

  void generate_deserialize_struct       (std::ofstream& out,
                                          t_struct*   tstruct,
                                          std::string prefix="",
                                          bool force_nested = true);

  void generate_deserialize_container    (std::ofstream& out,
                                          t_type*     ttype,
                                          std::string prefix="");

  void generate_deserialize_set_element  (std::ofstream& out,
                                          t_set*      tset,
                                          std::string prefix="");

  void generate_deserialize_map_element  (std::ofstream& out,
                                          t_map*      tmap,
                                          std::string prefix="");

  void generate_deserialize_list_element (std::ofstream& out,
                                          t_list*     tlist,
                                          std::string prefix,
                                          bool push_back,
                                          std::string index);

  std::string type_to_enum(t_type* ttype);

  std::ofstream f_out_;
  std::ofstream f_out_common_; //in addition to **_index.h - can they be the same file?
  bool need_common_; //are there consts and typedef that we need to keep in a common file?

  bool gen_pure_enums_;
  bool cmake_supplies_headers_;
  std::map<std::string, std::string> structure_names_;

  std::string get_struct_name(t_struct *tstruct) {
    string sttname = tstruct->get_name();
    if (structure_names_.find(sttname)==structure_names_.end()) {
      return sttname;
    }
    return structure_names_[sttname];
  }

  std::string get_namespace(t_program *program) {
    std::string result = program->get_namespace("yarp");
    if (result!="") return result;
    return program->get_namespace("cpp");
  }

 void print_const_value(std::ofstream& out, std::string name, t_type* type, t_const_value* value);
  std::string render_const_value(std::ofstream& out, std::string name, t_type* type, t_const_value* value);

  int flat_element_count( t_type* type);
  int flat_element_count( t_struct* type);
  int flat_element_count(t_function* fn);

  void auto_warn(ostream& f_srv_) {
    f_srv_ << "// This is an automatically-generated file." << endl;
    f_srv_ << "// It could get re-generated if the ALLOW_IDL_GENERATION flag is on." << endl;
    f_srv_ << endl;
  }

  void generate_enum_constant_list(std::ofstream& f,
                                   const vector<t_enum_value*>& constants,
                                   const char* prefix,
                                   const char* suffix,
                                   bool include_values);

};


string t_yarp_generator::type_to_enum(t_type* type) {
  type = get_true_type(type);

  if (type->is_base_type()) {
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
    switch (tbase) {
    case t_base_type::TYPE_VOID:
      throw "NO T_VOID CONSTRUCT";
    case t_base_type::TYPE_STRING:
      return "BOTTLE_TAG_STRING";
    case t_base_type::TYPE_BOOL:
      return "BOTTLE_TAG_VOCAB";
    case t_base_type::TYPE_BYTE:
      return "::apache::thrift::protocol::T_BYTE";
    case t_base_type::TYPE_I16:
      return "::apache::thrift::protocol::T_I16";
    case t_base_type::TYPE_I32:
      return "BOTTLE_TAG_INT";
    case t_base_type::TYPE_I64:
      return "::apache::thrift::protocol::T_I64";
    case t_base_type::TYPE_DOUBLE:
      return "BOTTLE_TAG_DOUBLE";
    }
  } else if (type->is_enum()) {
    return "BOTTLE_TAG_INT";
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



/////////////////////////////////////////////////////////////////////
/// C++ generator code begins
/////////////////////////////////////////////////////////////////////

string t_yarp_generator::type_name(t_type* ttype, bool in_typedef, bool arg,
                                   bool ret) {
  if (ttype->is_base_type()) {
    string bname = base_type_name(((t_base_type*)ttype)->get_base());
    if (!arg) {
      return bname;
    }

    if (((t_base_type*)ttype)->get_base() == t_base_type::TYPE_STRING) {
      return "const " + bname + "&";
    } else {
      if (ret) return bname;
      return "const " + bname;
    }
  }

  // Check for a custom overloaded C++ name
  if (ttype->is_container()) {
    string cname;

    t_container* tcontainer = (t_container*) ttype;
    if (tcontainer->has_cpp_name()) {
      cname = tcontainer->get_cpp_name();
    } else if (ttype->is_map()) {
      t_map* tmap = (t_map*) ttype;
      cname = "std::map<" +
        type_name(tmap->get_key_type(), in_typedef) + ", " +
        type_name(tmap->get_val_type(), in_typedef) + "> ";
    } else if (ttype->is_set()) {
      t_set* tset = (t_set*) ttype;
      cname = "std::set<" + type_name(tset->get_elem_type(), in_typedef) + "> ";
    } else if (ttype->is_list()) {
      t_list* tlist = (t_list*) ttype;
      cname = "std::vector<" + type_name(tlist->get_elem_type(), in_typedef) + "> ";
    }

    if (arg) {
      return "const " + cname + "&";
    } else {
      return cname;
    }
  }

  string class_prefix;
  if (in_typedef && (ttype->is_struct() || ttype->is_xception())) {
    class_prefix = "class ";
  }

  // Check if it needs to be namespaced
  string pname;

  if (ttype->is_struct()) {
    if (structure_names_.find(ttype->get_name())!=structure_names_.end()) {
      pname = structure_names_[ttype->get_name()];
    }
  }

  if (pname=="") {
    t_program* program = ttype->get_program();
    if (program != NULL && program != program_) {
      pname =
        class_prefix +
        namespace_prefix(get_namespace(program)) +
        ttype->get_name();
    } else {
      pname = class_prefix + ttype->get_name();
    }

    if (ttype->is_enum() && !gen_pure_enums_) {
      pname += "::type";
    }
  }

  if (arg) {
    if (is_complex_type(ttype)) {
      return "const " + pname + "&";
    } else {
      return "const " + pname;
    }
  } else {
    return pname;
  }
}

string t_yarp_generator::base_type_name(t_base_type::t_base tbase) {
  switch (tbase) {
  case t_base_type::TYPE_VOID:
    return "void";
  case t_base_type::TYPE_STRING:
    return "std::string";
  case t_base_type::TYPE_BOOL:
    return "bool";
  case t_base_type::TYPE_BYTE:
    return "int8_t";
  case t_base_type::TYPE_I16:
    return "int16_t";
  case t_base_type::TYPE_I32:
    return "int32_t";
  case t_base_type::TYPE_I64:
    return "int64_t";
  case t_base_type::TYPE_DOUBLE:
    return "double";
  default:
    throw "compiler error: no C++ base type name for base type " + t_base_type::t_base_name(tbase);
  }
}


string t_yarp_generator::namespace_decorate(string ns, string str) {
  if (ns=="") return str;
  string prefix = namespace_prefix(ns);
  if (prefix.find(" ::")==0) {
    prefix = prefix.substr(3,prefix.length());
  }
  return prefix + str;
}

string t_yarp_generator::namespace_prefix(string ns) {
  // Always start with "::", to avoid possible name collisions with
  // other names in one of the current namespaces.
  //
  // We also need a leading space, in case the name is used inside of a
  // template parameter.  "MyTemplate<::foo::Bar>" is not valid C++,
  // since "<:" is an alternative token for "[".
  string result = " ::";

  if (ns.size() == 0) {
    return result;
  }
  string::size_type loc;
  while ((loc = ns.find(".")) != string::npos) {
    result += ns.substr(0, loc);
    result += "::";
    ns = ns.substr(loc+1);
  }
  if (ns.size() > 0) {
    result += ns + "::";
  }
  return result;
}

bool t_yarp_generator::namespace_open(ostream& os, string ns, bool terse) {
  if (ns.size() == 0) {
    return false;
  }
  ns += ".";
  string separator = "";
  string::size_type loc;
  while ((loc = ns.find(".")) != string::npos) {
    if (!terse) indent(os);
    os << separator << "namespace " << ns.substr(0, loc) << " {";
    if (!terse) {
      os << endl;
      indent_up();
    } else {
      separator = " ";
    }
    ns = ns.substr(loc+1);
  }
  if (terse) {
    os << endl;
  }
  return true;
}

void t_yarp_generator::namespace_close(ostream& os, string ns, bool terse) {
  if (ns.size() == 0) return;
  ns += ".";
  string::size_type loc;
  while ((loc = ns.find(".")) != string::npos) {
    if (!terse) {
      indent_down();
      indent(os) << "}";
      os << endl;
    } else {
      os << "}";
    }
    ns = ns.substr(loc+1);
  }
  if (terse) {
    os << " // namespace" << endl;
  }
}

/////////////////////////////////////////////////////////////////////
/// C++ generator code ends
/////////////////////////////////////////////////////////////////////


/**
 * Emits the Table of Contents links at the top of the module's page
 */
void t_yarp_generator::generate_program_toc() {
  f_out_ << "<table><tr><th>Module</th><th>Services</th>"
   << "<th>Data types</th><th>Constants</th></tr>" << endl;
  generate_program_toc_row(program_);
  f_out_ << "</table>" << endl;
}


void t_yarp_generator::generate_program_toc_rows(t_program* tprog,
         std::vector<t_program*>& finished) {
  for (vector<t_program*>::iterator iter = finished.begin();
       iter != finished.end(); iter++) {
    if (tprog->get_path() == (*iter)->get_path()) {
      return;
    }
  }
  finished.push_back(tprog);
  generate_program_toc_row(tprog);
  vector<t_program*> includes = tprog->get_includes();
  for (vector<t_program*>::iterator iter = includes.begin();
       iter != includes.end(); iter++) {
    generate_program_toc_rows(*iter, finished);
  }
}

void t_yarp_generator::generate_program_toc_row(t_program* tprog) {
  string fname = tprog->get_name();
  f_out_ << "// " << fname << endl;
  if (!tprog->get_services().empty()) {
    vector<t_service*> services = tprog->get_services();
    vector<t_service*>::iterator sv_iter;
    for (sv_iter = services.begin(); sv_iter != services.end(); ++sv_iter) {
      string name = get_service_name(*sv_iter);
      //printf("//  %s (service)\n", name.c_str());
      f_out_ << "//  service: " << name << endl;
      //map<string,string> fn_yarp;
      vector<t_function*> functions = (*sv_iter)->get_functions();
      vector<t_function*>::iterator fn_iter;
      for (fn_iter = functions.begin(); fn_iter != functions.end(); ++fn_iter) {
        string fn_name = (*fn_iter)->get_name();
        //printf("//    %s (fn)\n", fn_name.c_str());
        f_out_ << "//    fn: " << fn_name << endl;
        //fn_yarp.insert(pair<string,string>(fn_name, yarp));
      }
      //for (map<string,string>::iterator yarp_iter = fn_yarp.begin();
      //yarp_iter != fn_yarp.end(); yarp_iter++) {
      //f_out_ << yarp_iter->second << endl;
      //}
    }
  }
  //map<string,string> data_types;
  if (!tprog->get_enums().empty()) {
    vector<t_enum*> enums = tprog->get_enums();
    vector<t_enum*>::iterator en_iter;
    for (en_iter = enums.begin(); en_iter != enums.end(); ++en_iter) {
      string name = (*en_iter)->get_name();
      f_out_ << "//  enum: " << name << endl;
      //data_types.insert(pair<string,string>(name, yarp));
    }
  }
  if (!tprog->get_typedefs().empty()) {
    vector<t_typedef*> typedefs = tprog->get_typedefs();
    vector<t_typedef*>::iterator td_iter;
    for (td_iter = typedefs.begin(); td_iter != typedefs.end(); ++td_iter) {
      string name = (*td_iter)->get_symbolic();
      //printf("//  %s (typedef)\n", name.c_str());
      f_out_ << "//  typedef: " << name << endl;
      //data_types.insert(pair<string,string>(name, yarp));
    }
  }
  if (!tprog->get_objects().empty()) {
    vector<t_struct*> objects = tprog->get_objects();
    vector<t_struct*>::iterator o_iter;
    for (o_iter = objects.begin(); o_iter != objects.end(); ++o_iter) {
      string name = (*o_iter)->get_name();
      f_out_ << "//  object: " << name << endl;
      //data_types.insert(pair<string,string>(name, yarp));
    }
  }
  //for (map<string,string>::iterator dt_iter = data_types.begin();
  //dt_iter != data_types.end(); dt_iter++) {
  //printf("//  ... %s ...\n", dt_iter->second.c_str());
  //}
  if (!tprog->get_consts().empty()) {
    //map<string,string> const_yarp;
    vector<t_const*> consts = tprog->get_consts();
    vector<t_const*>::iterator con_iter;
    for (con_iter = consts.begin(); con_iter != consts.end(); ++con_iter) {
      string name = (*con_iter)->get_name();
      f_out_ << "//  constant: " << name << endl;
      //string yarp ="<a href=\"" + fname + "#Const_" + name +
      //"\">" + name + "</a>";
      //const_yarp.insert(pair<string,string>(name, yarp));
    }
    //for (map<string,string>::iterator con_iter = const_yarp.begin();
    //con_iter != const_yarp.end(); con_iter++) {
    //printf("//  ... %s ...\n", con_iter->second.c_str());
    //}
  }
}

/**
 * Prepares for file generation by opening up the necessary file output
 * stream.
 */
void t_yarp_generator::generate_program() {
  // Make output directory
  //MKDIR(get_out_dir().c_str()); // MSVC not happy with this - paul

  string fname = get_out_dir() + program_->get_name() + "_index.h";
  f_out_.open(fname.c_str());
  f_out_ << "// Thrift module: " << program_->get_name() << endl;

  string fname2 = get_out_dir() + program_->get_name() + "_index.txt";
  ofstream f_out2_(fname2.c_str());

  string fname3 = get_out_dir() + program_->get_name() + "_indexALL.txt";
  ofstream f_out3_(fname3.c_str());

  //print_doc(program_);

  //generate_program_toc();

  if (!program_->get_consts().empty()) {
      need_common_=true;
      string fcommon_name= get_out_dir() + program_->get_name() + "_common.h";
      f_out_common_.open(fcommon_name.c_str());
      auto_warn(f_out_common_);
      f_out_common_ << "#ifndef YARP_THRIFT_GENERATOR_COMMON_" << program_->get_name() << endl;
      f_out_common_ << "#define YARP_THRIFT_GENERATOR_COMMON_" << program_->get_name() << endl;
      f_out_common_ << endl;
      f_out_common_ << endl << "// Constants" << endl;
      vector<t_const*> consts = program_->get_consts();
      //check needed inclusions
      std::set<string> neededTypes;
      neededTypes.clear();
      for (vector<t_const*>::iterator const_iter=consts.begin(); const_iter!=consts.end(); ++const_iter)
        getNeededType((*const_iter)->get_type(), neededTypes);
      for (std::set<string>::iterator inclIter=neededTypes.begin(); inclIter!=neededTypes.end(); ++inclIter){
        f_out_common_ << "#include <" << *inclIter << ">" << endl;
      }
      //
      generate_consts(consts);
  }

  if (!program_->get_enums().empty()) {
    // Generate enums
    f_out_ << endl << "// Enums" << endl;
    vector<t_enum*> enums = program_->get_enums();
    vector<t_enum*>::iterator en_iter;
    for (en_iter = enums.begin(); en_iter != enums.end(); ++en_iter) {
      generate_enum(*en_iter);
      f_out_ << "#include \"" << (*en_iter)->get_name() << ".h\"" << endl;
      f_out2_ << (*en_iter)->get_name() << ".h" << endl;
      f_out3_ << (*en_iter)->get_name() << ".h" << endl;
      f_out3_ << (*en_iter)->get_name() << ".cpp" << endl;
    }
  }

  if (!program_->get_typedefs().empty()) {
      need_common_=true;
    // Generate typedefs
    if (!f_out_common_.is_open())
    {
      string fcommon_name= get_out_dir() + program_->get_name() + "_common.h";
      f_out_common_.open(fcommon_name.c_str());
      f_out_common_ << "#ifndef YARP_THRIFT_GENERATOR_COMMON_" << program_->get_name() << endl;
      f_out_common_ << "#define YARP_THRIFT_GENERATOR_COMMON_" << program_->get_name() << endl;
      f_out_common_ << endl;
    }
    f_out_common_ << endl << "// Typedefs" << endl;
    vector<t_typedef*> typedefs = program_->get_typedefs();
    vector<t_typedef*>::iterator td_iter;

    //check needed inclusions
    std::set<string> neededTypes;
    neededTypes.clear();
    for (td_iter=typedefs.begin(); td_iter!=typedefs.end(); ++td_iter)
      getNeededType((*td_iter)->get_type(), neededTypes);
    for (std::set<string>::iterator inclIter=neededTypes.begin(); inclIter!=neededTypes.end(); ++inclIter){
         f_out_common_ << "#include <" << *inclIter << ">" << endl;
    }
    //

    for (td_iter = typedefs.begin(); td_iter != typedefs.end(); ++td_iter) {
      generate_typedef(*td_iter);
    }
  }

  if (!program_->get_objects().empty()) {
    // Generate structs and exceptions in declared order
    f_out_ << endl << "// Structures" << endl;
    vector<t_struct*> objects = program_->get_objects();
    vector<t_struct*>::iterator o_iter;
    for (o_iter = objects.begin(); o_iter != objects.end(); ++o_iter) {
      if ((*o_iter)->is_xception()) {
        generate_xception(*o_iter);
      } else {
        generate_struct(*o_iter);
        f_out_ << "#include \"" << (*o_iter)->get_name() << ".h\"" << endl;
        f_out2_ << (*o_iter)->get_name() << ".h" << endl;
        if ((*o_iter)->annotations_.find("yarp.includefile") == (*o_iter)->annotations_.end())
        {
          f_out3_ << (*o_iter)->get_name() << ".h" << endl;
          f_out3_ << (*o_iter)->get_name() << ".cpp" << endl;
        }
      }
    }
  }

  if (!program_->get_services().empty()) {
    // Generate services
    f_out_ << endl << "// Services" << endl;
    vector<t_service*> services = program_->get_services();
    vector<t_service*>::iterator sv_iter;
    for (sv_iter = services.begin(); sv_iter != services.end(); ++sv_iter) {
      service_name_ = get_service_name(*sv_iter);
      generate_service(*sv_iter);
      f_out_ << "#include \"" << (*sv_iter)->get_name() << ".h\"" << endl;
      f_out2_ << (*sv_iter)->get_name() << ".h" << endl;
      f_out3_ << (*sv_iter)->get_name() << ".h" << endl;
      f_out3_ << (*sv_iter)->get_name() << ".cpp" << endl;
    }
  }

  if (need_common_)
  {
      f_out2_ <<  program_->get_name() + "_common.h" << endl;
      f_out3_ <<  program_->get_name() + "_common.h" << endl;
  }

  f_out_.close();
  f_out2_.close();
  f_out3_.close();
  if(f_out_common_.is_open()){
      f_out_common_ << "#endif" << endl;
      f_out_common_ << endl;
      f_out_common_.close();
  }

  generate_index();
}

/**
 * Emits the index.html file for the recursive set of Thrift programs
 */
void t_yarp_generator::generate_index() {
  /*
  string index_fname = get_out_dir() + "index.cpp";
  f_out_.open(index_fname.c_str());
  vector<t_program*> programs;
  generate_program_toc_rows(program_, programs);
  f_out_.close();
  */
}

/**
 * If the provided documentable object has documentation attached, this
 * will emit it to the output stream in YARP format.
 */
void t_yarp_generator::print_doc(std::ofstream& out, t_doc* tdoc) {
  if (tdoc->has_doc()) {
    indent(out) << "/**"  << endl;
    string doc = tdoc->get_doc();
    size_t index;
    while ((index = doc.find_first_of("\r\n")) != string::npos) {
      if (index == 0) {
      } else {
        indent(out) << " * " << doc.substr(0, index) << endl;
      }
      if (index + 1 < doc.size() && doc.at(index) != doc.at(index + 1) &&
          (doc.at(index + 1) == '\r' || doc.at(index + 1) == '\n')) {
        index++;
      }
      doc = doc.substr(index + 1);
    }
    indent(out) << " */" << endl;
  }
}

static std::string replaceInString(const std::string& originalString, std::string toFind, std::string replacement)
{
  string docString(originalString);
  size_t foundToken=docString.find(toFind);
  while(foundToken!=string::npos)
  {
    docString.replace(foundToken, toFind.size(), replacement);
    foundToken=docString.find(toFind, foundToken+replacement.size());
  }
  return docString;
};

void t_yarp_generator::quote_doc(std::vector<std::string>& doxyPar, t_doc* tdoc) {
  string quotes="\"";
  string replacement="\\\"";
  std::string result;
  if (tdoc->has_doc()) {
    string doc = tdoc->get_doc();
    size_t index;
    while ((index = doc.find_first_of("\r\n")) != string::npos) {
      if (index!= 0) {
        //escape all quotes (TODO: may need to escape other characters?)
        doxyPar.push_back(replaceInString(doc.substr(0, index), quotes, replacement));
      }
      if (index + 1 < doc.size() && doc.at(index) != doc.at(index + 1) &&
    (doc.at(index + 1) == '\r' || doc.at(index + 1) == '\n')) {
        index++;
      }
      doc = doc.substr(index + 1);
    }
  }
}

std::vector<std::string> t_yarp_generator::print_help(t_function* tdoc) {
  std::vector<std::string> doxyPar;
  string quotes="\"";
  string replacement="\\\"";
  doxyPar.push_back(replaceInString(function_prototype(tdoc, true, NULL), quotes, replacement));
  quote_doc(doxyPar,tdoc);
  return doxyPar;
}

/**
 * Prints out the provided type in YARP
 */
string t_yarp_generator::print_type(t_type* ttype) {
  return type_name(ttype);
}

/**
 * Prints out an YARP representation of the provided constant value
 */
string t_yarp_generator::print_const_value(t_const_value* tvalue,
                                           t_type* ttype) {
  string result;
  bool first = true;
  char buf[10000];
  switch (tvalue->get_type()) {
  case t_const_value::CV_INTEGER:
    sprintf(buf,"%d",(int)tvalue->get_integer());
    result += buf;
    break;
  case t_const_value::CV_DOUBLE:
    sprintf(buf,"%g",tvalue->get_double());
    result += buf;
    break;
  case t_const_value::CV_STRING:
    result += string("\"") + get_escaped_string(tvalue) + "\"";
    break;
  case t_const_value::CV_MAP:
    {
      result += "{ ";
      map<t_const_value*, t_const_value*> map_elems = tvalue->get_map();
      map<t_const_value*, t_const_value*>::iterator map_iter;
      for (map_iter = map_elems.begin(); map_iter != map_elems.end(); map_iter++) {
        if (!first) {
          result += ", ";
        }
        first = false;
        result += print_const_value(map_iter->first);
        result += " = ";
        result += print_const_value(map_iter->second);
      }
      result += " }";
    }
    break;
  case t_const_value::CV_LIST:
    {
      result += "{ ";
      vector<t_const_value*> list_elems = tvalue->get_list();;
      vector<t_const_value*>::iterator list_iter;
      for (list_iter = list_elems.begin(); list_iter != list_elems.end(); list_iter++) {
        if (!first) {
          result += ", ";
        }
        first = false;
        result += print_const_value(*list_iter);
      }
      result += " }";
    }
    break;
  default:
    {
      bool done = false;
      if (ttype!=NULL) {
        if (ttype->is_enum()) {
          result += tvalue->get_identifier_name();
          done = true;
        }
      }
      if (!done) {
        result += "UNKNOWN";
      }
    }
    break;
  }
  return result;
}

/**
 * Generates a typedef.
 *
 * @param ttypedef The type definition
 */
void t_yarp_generator::generate_typedef(t_typedef* ttypedef) {
  string name = ttypedef->get_name();
  print_doc(f_out_common_,ttypedef);
  f_out_common_<< "typedef " << print_type(ttypedef->get_type()) << " " << name <<";" << endl;
}

/**
 * Generates code for an enumerated type.
 *
 * @param tenum The enumeration
 */
void t_yarp_generator::generate_enum(t_enum* tenum) {
  std::string enum_name = tenum->get_name();
  string f_header_name = get_out_dir()+enum_name+".h";
  string f_cpp_name = get_out_dir()+enum_name+".cpp";
  ofstream f_types_;
  f_types_.open(f_header_name.c_str());
  ofstream f_types_impl_;
  f_types_impl_.open(f_cpp_name.c_str());
  auto_warn(f_types_);
  auto_warn(f_types_impl_);

  f_types_ << "#ifndef YARP_THRIFT_GENERATOR_ENUM_" << enum_name << endl;
  f_types_ << "#define YARP_THRIFT_GENERATOR_ENUM_" << enum_name << endl;
  f_types_ << endl;
  f_types_ << "#include <yarp/os/Wire.h>" << endl;
  f_types_ << "#include <yarp/os/idl/WireTypes.h>" << endl;
  if (cmake_supplies_headers_) {
    f_types_ << "@HEADERS@" << endl;
  }
  f_types_ << endl;

  f_types_impl_ << "#include <yarp/os/Wire.h>" << endl;
  f_types_impl_ << "#include <yarp/os/idl/WireTypes.h>" << endl;
  if (cmake_supplies_headers_) {
    f_types_impl_ << "@HEADERS@" << endl;
  }
  else
  {
    f_types_impl_<<  "#include <" << get_include_prefix(*program_) + enum_name + ".h>" <<endl;
  }
  f_types_impl_ << endl;

  string ns = get_namespace(program_);

  namespace_open(f_types_,ns,false);
  //indent(f_types_) << "enum " << enum_name << ";" << endl << endl;
  //f_types_ << endl << endl;

  namespace_open(f_types_impl_,ns);
  f_types_impl_ << endl << endl;

  vector<t_enum_value*> constants = tenum->get_constants();

  print_doc(f_types_,tenum);
  if (!gen_pure_enums_) {
    enum_name = "type";
    f_types_ <<
      indent() << "struct " << tenum->get_name() << " {" << endl;
    indent_up();
  }
  f_types_ <<
    indent() << "enum " << enum_name;

  generate_enum_constant_list(f_types_, constants, "", "", true);

  if (!gen_pure_enums_) {
    indent_down();
    f_types_ << "};" << endl;
  }

  f_types_ << endl;

  /**
     Generate a character array of enum names for debugging purposes.
  */

  std::string prefix = "";
  if (!gen_pure_enums_) {
    prefix = tenum->get_name() + "::";
  }

  /*
  f_types_impl_ <<
    indent() << "int _k" << tenum->get_name() << "Values[] =";
  generate_enum_constant_list(f_types_impl_, constants, prefix.c_str(), "", false);

  f_types_impl_ <<
    indent() << "const char* _k" << tenum->get_name() << "Names[] =";
  generate_enum_constant_list(f_types_impl_, constants, "\"", "\"", false);


  f_types_ <<
    indent() << "extern const std::map<int, const char*> _" <<
    tenum->get_name() << "_VALUES_TO_NAMES;" << endl << endl;

  f_types_impl_ <<
    indent() << "const std::map<int, const char*> _" << tenum->get_name() <<
    "_VALUES_TO_NAMES(::apache::thrift::TEnumIterator(" << constants.size() <<
    ", _k" << tenum->get_name() << "Values" <<
    ", _k" << tenum->get_name() << "Names), " <<
    "::apache::thrift::TEnumIterator(-1, NULL, NULL));" << endl << endl;
  */

  //generate_local_reflection(f_types_, tenum, false);
  //generate_local_reflection(f_types_impl_, tenum, true);

  indent(f_types_) << "class " << enum_name << "Vocab;" << endl;
  namespace_close(f_types_,ns,false);
  f_types_ << endl;


  f_types_ << "class " << namespace_decorate(ns,enum_name) << "Vocab : public yarp::os::idl::WireVocab {" << endl;
  f_types_ << "public:" << endl;
  indent_up();
  indent(f_types_) << "virtual int fromString(const std::string& input);" << endl;
  indent(f_types_) << "virtual std::string toString(int input);" << endl;
  indent_down();
  f_types_ << "};" << endl;
  f_types_ << endl;

  ostream& os = f_types_impl_;

  os << "int " << enum_name << "Vocab::fromString(const std::string& input) {" << endl;
  indent_up();
  indent(os) << "// definitely needs optimizing :-)" << endl;
  for (vector<t_enum_value*>::const_iterator c_iter = constants.begin();
       c_iter != constants.end(); ++c_iter) {
    indent(os) << "if (input==\"" << (*c_iter)->get_name()
               << "\") return (int)"
               << (*c_iter)->get_name() << ";"
               << endl;
  }
  indent(os) << "return -1;" << endl;
  indent_down();
  os << "}" << endl;


  os << "std::string " << enum_name << "Vocab::toString(int input) {" << endl;
  indent_up();
  indent(os) << "switch((" << enum_name << ")input) {" << endl;
  for (vector<t_enum_value*>::const_iterator c_iter = constants.begin();
       c_iter != constants.end(); ++c_iter) {
    indent(os) << "case " << (*c_iter)->get_name() << ":" << endl;
    indent_up();
    indent(os) << "return \"" << (*c_iter)->get_name() << "\";" << endl;
    indent_down();
  }
  indent(os) << "}" << endl;
  indent(os) << "return \"\";" << endl;
  indent_down();
  os << "}" << endl;

  f_types_ << endl;

  namespace_close(f_types_impl_,ns);
  f_types_impl_ << endl << endl;

  f_types_ << "#endif" << endl;
}

void t_yarp_generator::generate_enum_constant_list(std::ofstream& f,
                                                   const vector<t_enum_value*>& constants,
                                                   const char* prefix,
                                                   const char* suffix,
                                                   bool include_values) {
  f << " {" << endl;
  indent_up();

  vector<t_enum_value*>::const_iterator c_iter;
  bool first = true;
  for (c_iter = constants.begin(); c_iter != constants.end(); ++c_iter) {
    if (first) {
      first = false;
    } else {
      f << "," << endl;
    }
    print_doc(f,(*c_iter));
    indent(f)
      << prefix << (*c_iter)->get_name() << suffix;
    if (include_values && (*c_iter)->has_value()) {
      f << " = " << (*c_iter)->get_value();
    }
  }

  f << endl;
  indent_down();
  indent(f) << "};" << endl;
}

/**
 * Generates a constant value
 */
void t_yarp_generator::generate_const(t_const* tconst) {
  string name = tconst->get_name();
  print_doc(f_out_common_,tconst);
  f_out_common_ << "const "<< print_type(tconst->get_type())<<" " << name << " = " << print_const_value(tconst->get_value()) <<";"<<endl;


}

/**
 * Returns the include prefix to use for a file generated by program, or the
 * empty string if no include prefix should be used.
 */
string t_yarp_generator::get_include_prefix(const t_program& program) const {
  string include_prefix = program.get_include_prefix();
  if (!use_include_prefix_ ||
      (include_prefix.size() > 0 && include_prefix[0] == '/')) {
    // if flag is turned off or this is absolute path, return empty prefix
    return "";
  }

  string::size_type last_slash = string::npos;
  if ((last_slash = include_prefix.rfind("/")) != string::npos) {
   // return include_prefix.substr(0, last_slash) + "/" + out_dir_base_ + "/";
   return include_prefix.substr(0, last_slash) + "/";
  }

  return "";
}

/**
 * Generates a struct definition for a thrift data type.
 *
 * @param tstruct The struct definition
 */
void t_yarp_generator::generate_struct(t_struct* tstruct) {
  string sttname = tstruct->get_name();
  if (tstruct->annotations_.find("yarp.name") != tstruct->annotations_.end()) {
    structure_names_[sttname] = tstruct->annotations_["yarp.name"];
    return;
  }
  if (tstruct->annotations_.find("cpp.name") != tstruct->annotations_.end()) {
    structure_names_[sttname] = tstruct->annotations_["cpp.name"];
    return;
  }

  string f_header_name = get_out_dir()+sttname+".h";
  string f_cpp_name = get_out_dir()+sttname+".cpp";
  ofstream f_stt_;
  f_stt_.open(f_header_name.c_str());
  ofstream f_cpp_;
  f_cpp_.open(f_cpp_name.c_str());

  auto_warn(f_cpp_);

  string name = tstruct->get_name();
  vector<t_field*> members = tstruct->get_members();
  vector<t_field*>::iterator mem_iter;

  auto_warn(f_stt_);
  f_stt_ << "#ifndef YARP_THRIFT_GENERATOR_STRUCT_" << name << endl;
  f_stt_ << "#define YARP_THRIFT_GENERATOR_STRUCT_" << name << endl;
  f_stt_ << endl;
  f_stt_ << "#include <yarp/os/Wire.h>" << endl;
  f_stt_ << "#include <yarp/os/idl/WireTypes.h>" << endl;
  if (cmake_supplies_headers_) {
    f_stt_ << "@HEADERS@" << endl;
  }
  else
  {
//    cout<< "Searching headers for " << name <<endl;
    if (need_common_)
      f_stt_ << "#include <"<< get_include_prefix(*program_) << program_->get_name() << "_common.h>" <<endl;
    std::set<string> neededTypes;
    neededTypes.clear();
    for (mem_iter = members.begin() ; mem_iter != members.end(); mem_iter++) {
      getNeededType((*mem_iter)->get_type(), neededTypes);
    }

    for (std::set<string>::iterator inclIter=neededTypes.begin(); inclIter!=neededTypes.end(); ++inclIter){
      f_stt_ << "#include <" << *inclIter << ">" << endl;
    }


  }
  f_stt_ << endl;



  string ns = get_namespace(program_);

  namespace_open(f_stt_,ns,false);
  indent(f_stt_) << "class " << name << ";" << endl;
  namespace_close(f_stt_,ns,false);
  f_stt_ << endl << endl;
  //add documentation (should add a generator option for it?)

  print_doc(f_stt_,tstruct);
  f_stt_ << "class " << namespace_decorate(ns,name) << " : public yarp::os::idl::WirePortable {" << endl;
  f_stt_ << "public:" << endl;
  indent_up();

  ofstream& out = f_stt_;

  indent(out) << "// Fields" << endl;
  for (mem_iter = members.begin() ; mem_iter != members.end(); mem_iter++) {
    string mname = (*mem_iter)->get_name();
    string mtype = print_type((*mem_iter)->get_type());

    print_doc(f_stt_,*mem_iter);
    indent(f_stt_) << mtype << " " << mname << ";" << endl;
  }

  // Default constructor
  out << endl;
  indent(out) << "// Default constructor" << endl;
  indent(out) <<
    tstruct->get_name() << "()";

  bool init_ctor = false;

  vector<t_field*>::const_iterator m_iter;
  for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
    t_type* t = get_true_type((*m_iter)->get_type());
    if (t->is_base_type() || t->is_enum()) {
      string dval;
      if (t->is_enum()) {
        dval += "(" + type_name(t) + ")";
      }
      dval += t->is_string() ? "\"\"" : "0";
      t_const_value* cv = (*m_iter)->get_value();
      if (cv != NULL) {
        dval = render_const_value(out, (*m_iter)->get_name(), t, cv);
      }
      if (!init_ctor) {
        init_ctor = true;
        out << " : ";
        out << (*m_iter)->get_name() << "(" << dval << ")";
      } else {
        out << ", " << (*m_iter)->get_name() << "(" << dval << ")";
      }
    }
  }
  out << " {" << endl;
  indent_up();
  for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
    t_type* t = get_true_type((*m_iter)->get_type());

    if (!t->is_base_type()) {
      t_const_value* cv = (*m_iter)->get_value();
      if (cv != NULL) {
        print_const_value(out, (*m_iter)->get_name(), t, cv);
      }
    }
  }
  scope_down(out);

  // Fill-out constructor
  out << endl;
  indent(out) << "// Constructor with field values" << endl;
  indent(out) << tstruct->get_name() << "(";
  init_ctor = false;
  for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
    t_type* t = get_true_type((*m_iter)->get_type());
    if (init_ctor) out << ",";
    init_ctor = true;
    out << type_name(t,false,true) << " ";
    out << (*m_iter)->get_name();
  }
  out << ")";

  init_ctor = false;
  for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
    if (!init_ctor) {
      init_ctor = true;
      out << " : ";
      out << (*m_iter)->get_name() << "(" << (*m_iter)->get_name() << ")";
    } else {
      out << ", " << (*m_iter)->get_name() << "(" << (*m_iter)->get_name() << ")";
    }
  }
  out << " {" << endl;
  indent_up();
  scope_down(out);

  // Copy constructor
  out << endl;
  indent(out) << "// Copy constructor" << endl;
  indent(out) << tstruct->get_name() << "(const " << tstruct->get_name()
              << "& __alt) : WirePortable(__alt) " << " {" << endl;
  indent_up();
  for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
    indent(out) << "this->" << (*m_iter)->get_name() << " = "
                << "__alt." << (*m_iter)->get_name() << ";" << endl;
  }
  scope_down(out);

  // Assignment operator
  out << endl;
  indent(out) << "// Assignment operator" << endl;
  indent(out) << "const " << tstruct->get_name() << "& operator = (const "
              << tstruct->get_name()
              << "& __alt) {" << endl;
  indent_up();
  for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
    indent(out) << "this->" << (*m_iter)->get_name() << " = "
                << "__alt." << (*m_iter)->get_name() << ";" << endl;
  }
  indent(out) << "return *this;" << endl;
  scope_down(out);

  out << endl;
  indent(out) << "// read and write structure on a connection" << endl;
  indent(out) << "bool read(yarp::os::idl::WireReader& reader);"
              << endl;

  indent(out) << "bool read(yarp::os::ConnectionReader& connection);"
              << endl;


  indent(out) << "bool write(yarp::os::idl::WireWriter& writer);"
              << endl;
  indent(out) << "bool write(yarp::os::ConnectionWriter& connection);"
              << endl;

  out << endl;

  indent_down();
  indent(out) << "private:" << endl;
  indent_up();

  for (mem_iter = members.begin() ; mem_iter != members.end(); mem_iter++) {
    string mname = (*mem_iter)->get_name();
    indent(out) << "bool write_" << mname << "(yarp::os::idl::WireWriter& writer);" << endl;
    indent(out) << "bool nested_write_" << mname << "(yarp::os::idl::WireWriter& writer);" << endl;
  }
  for (mem_iter = members.begin() ; mem_iter != members.end(); mem_iter++) {
    string mname = (*mem_iter)->get_name();
    indent(out) << "bool read_" << mname << "(yarp::os::idl::WireReader& reader);" << endl;
    indent(out) << "bool nested_read_" << mname << "(yarp::os::idl::WireReader& reader);" << endl;
  }

  indent_down();
  out << endl;
  indent(out) << "public:" << endl;
  indent_up();

  f_stt_ << endl;
  indent(f_stt_) << "yarp::os::ConstString toString();"
                 << endl;

  f_stt_ << endl;
  indent(f_stt_) << "// if you want to serialize this class without nesting, use this helper" << endl;
  indent(f_stt_) << "typedef yarp::os::idl::Unwrapped<" << namespace_decorate(ns,name) << " > unwrapped;" << endl;
  f_stt_ << endl;

  // Editor: setters, getters, individual serializers, and dirty flags?
  indent(out) << "class Editor : public yarp::os::Wire, public yarp::os::PortWriter {" << endl;
  indent(out) << "public:" << endl;
  indent_up();

  // Editor constructor
  out << endl;
  indent(out) << "Editor() {" << endl;
  indent_up();
  indent(out) << "group = 0;" << endl;
  indent(out) << "obj_owned = true;" << endl;
  indent(out) << "obj = new " << tstruct->get_name() <<  ";" << endl;
  indent(out) << "dirty_flags(false);" << endl;
  indent(out) << "yarp().setOwner(*this);" << endl;
  scope_down(out);
  out << endl;
  indent(out) << "Editor(" << tstruct->get_name()
              << "& obj) {" << endl;
  indent_up();
  indent(out) << "group = 0;" << endl;
  indent(out) << "obj_owned = false;" << endl;
  indent(out) << "edit(obj,false);" << endl;
  indent(out) << "yarp().setOwner(*this);" << endl;
  scope_down(out);

  out << endl;
  indent(out) << "bool edit(" << tstruct->get_name()
              << "& obj, bool dirty = true) {" << endl;
  indent_up();
  indent(out) << "if (obj_owned) delete this->obj;" << endl;
  indent(out) << "this->obj = &obj;" << endl;
  indent(out) << "obj_owned = false;" << endl;
  indent(out) << "dirty_flags(dirty);" << endl;
  indent(out) << "return true;" << endl;
  scope_down(out);

  // Editor destructor
  out << endl;
  indent(out) << "virtual ~Editor() {" << endl;
  indent(out) << "if (obj_owned) delete obj;" << endl;
  indent_up();
  scope_down(out);

  // Validity check
  out << endl;
  indent(out) << "bool isValid() const {" << endl;
  indent_up();
  indent(out) << "return obj!=0/*NULL*/;" << endl;
  scope_down(out);

  // State
  out << endl;
  indent(out) << tstruct->get_name() << "& state() { return *obj; }" << endl;
  out << endl;

  // Grouping
  indent(out) << "void begin() { group++; }" << endl;
  out << endl;
  indent(out) << "void end() {" << endl;
  indent_up();
  indent(out) << "group--;" << endl;
  indent(out) << "if (group==0&&is_dirty) communicate();" << endl;
  scope_down(out);

  // set
  for (mem_iter = members.begin() ; mem_iter != members.end(); mem_iter++) {
    string mname = (*mem_iter)->get_name();
    t_type* t = get_true_type((*mem_iter)->get_type());
    indent(out) << "void set_" << mname << "(" << type_name(t,false,true) << " " << mname << ") {" << endl;
    indent_up();
    indent(out) << "will_set_" << mname << "();" << endl;
    indent(out) << "obj->" << mname << " = " << mname << ";" << endl;
    indent(out) << "mark_dirty_" << mname << "();" << endl;
    indent(out) << "communicate();" << endl;
    indent(out) << "did_set_" << mname << "();" << endl;
    scope_down(out);

    // other container types: maps, sets
    if (t->is_list()) {
      indent(out) << "void set_" << mname << "(int index, " << type_name(((t_list*)t)->get_elem_type(),false,true) << " elem) {" << endl;
      indent_up();
      indent(out) << "will_set_" << mname << "();" << endl;
      indent(out) << "obj->" << mname << "[index] = elem;" << endl;
      indent(out) << "mark_dirty_" << mname << "();" << endl;
      indent(out) << "communicate();" << endl;
      indent(out) << "did_set_" << mname << "();" << endl;
      scope_down(out);
    }
  }

  // get
  for (mem_iter = members.begin() ; mem_iter != members.end(); mem_iter++) {
    string mname = (*mem_iter)->get_name();
    t_type* t = get_true_type((*mem_iter)->get_type());
    indent(out) <<  type_name(t,false,true,true) << " get_" << mname << "() {" << endl;
    indent_up();
    indent(out) << "return obj->" << mname << ";" << endl;
    scope_down(out);
  }

  // will_set
  for (mem_iter = members.begin() ; mem_iter != members.end(); mem_iter++) {
    string mname = (*mem_iter)->get_name();
    indent(out) <<  "virtual bool will_set_" << mname << "() { return true; }" << endl;
  }

  // did_set
  for (mem_iter = members.begin() ; mem_iter != members.end(); mem_iter++) {
    string mname = (*mem_iter)->get_name();
    indent(out) <<  "virtual bool did_set_" << mname << "() { return true; }" << endl;
  }

  // mark dirty overall
  indent(out) << "void clean() {" << endl;
  indent_up();
  indent(out) << "dirty_flags(false);" << endl;
  scope_down(out);


  // serialize
  indent(out) << "bool read(yarp::os::ConnectionReader& connection);" << endl;
  indent(out) << "bool write(yarp::os::ConnectionWriter& connection);" << endl;


  indent_down();
  indent(out) << "private:" << endl;
  indent_up();

  // State
  out << endl;
  indent(out) << tstruct->get_name() << " *obj;" << endl;
  out << endl;
  indent(out) << "bool obj_owned;" << endl;
  indent(out) << "int group;" << endl;
  out << endl;

  // Send if possible
  indent(out) << "void communicate() {" << endl;
  indent_up();
  indent(out) << "if (group!=0) return;" << endl;
  indent(out) << "if (yarp().canWrite()) {" << endl;
  indent_up();
  indent(out) << "yarp().write(*this);" << endl;
  indent(out) << "clean();" << endl;
  scope_down(out);
  scope_down(out);

  // mark dirty overall
  indent(out) << "void mark_dirty() {" << endl;
  indent_up();
  indent(out) << "is_dirty = true;" << endl;
  scope_down(out);

  // mark dirty
  for (mem_iter = members.begin() ; mem_iter != members.end(); mem_iter++) {
    string mname = (*mem_iter)->get_name();
    string mtype = print_type((*mem_iter)->get_type());
    indent(out) << "void mark_dirty_" << mname << "() {" << endl;
    indent_up();
    indent(out) << "if (is_dirty_" << mname << ") return;" << endl;
    indent(out) << "dirty_count++;" << endl;
    indent(out) << "is_dirty_" << mname << " = true;" << endl;
    indent(out) << "mark_dirty();" << endl;
    scope_down(out);
  }

  indent(out) << "void dirty_flags(bool flag) {" << endl;
  indent_up();
  indent(out) << "is_dirty = flag;" << endl;
  int ct = 0;
  for (mem_iter = members.begin() ; mem_iter != members.end(); mem_iter++) {
    string mname = (*mem_iter)->get_name();
    string mtype = print_type((*mem_iter)->get_type());
    indent(out) << "is_dirty_" << mname << " = flag;" << endl;
    ct++;
  }
  indent(out) << "dirty_count = flag ? " << ct << " : 0;" << endl;
  indent_down();
  indent(out) << "}" << endl;

  indent(out) << "bool is_dirty;" << endl;
  indent(out) << "int dirty_count;" << endl;
  int mem_ct = 0;
  for (mem_iter = members.begin() ; mem_iter != members.end(); mem_iter++) {
    string mname = (*mem_iter)->get_name();
    string mtype = print_type((*mem_iter)->get_type());
    indent(out) << "bool is_dirty_" << mname << ";" << endl;
    mem_ct++;
  }

  indent_down();
  indent(out) << "};" << endl;

  indent_down();
  indent(out) << "};" << endl;

  out << endl;

  out << "#endif" << endl;
  out << endl;


  // header done - now generate source

  if (cmake_supplies_headers_) {
    f_cpp_ << "@HEADERS@" << endl;
  }
  else
  {
    f_cpp_ <<  "#include <" << get_include_prefix(*program_) + name + ".h>" << endl;
  }
  f_cpp_ << endl;

  namespace_open(f_cpp_,ns);

  {
    ofstream& out = f_cpp_;

    for (mem_iter = members.begin() ; mem_iter != members.end(); mem_iter++) {
      string mname = (*mem_iter)->get_name();
      indent(out) << "bool " << name
                  << "::read_" << mname << "(yarp::os::idl::WireReader& reader) {"
                  << endl;
      indent_up();
      generate_deserialize_field(out, *mem_iter, "");
      indent(out) << "return true;" << endl;
      scope_down(out);
      indent(out) << "bool " << name
                  << "::nested_read_" << mname << "(yarp::os::idl::WireReader& reader) {"
                  << endl;
      indent_up();
      generate_deserialize_field(out, *mem_iter, "", "", true);
      indent(out) << "return true;" << endl;
      scope_down(out);
    }

    indent(out) << "bool " << name
                << "::read(yarp::os::idl::WireReader& reader) {"
                << endl;
    indent_up();
    for (mem_iter = members.begin() ; mem_iter != members.end(); mem_iter++) {
      string mname = (*mem_iter)->get_name();
      indent(out) << "if (!read_" << mname << "(reader)) return false;" << endl;
    }
    indent(out) << "return !reader.isError();"
                << endl;
    scope_down(out);
    out << endl;

    indent(out) << "bool " << name
                << "::read(yarp::os::ConnectionReader& connection) {"
                << endl;
    indent_up();
    indent(out) << "yarp::os::idl::WireReader reader(connection);"
                << endl;
    indent(out) << "if (!reader.readListHeader("
                << flat_element_count(tstruct)
                << ")) return false;"
                << endl;
    indent(out) << "return read(reader);"  << endl;
    scope_down(out);
    out << endl;


    for (mem_iter = members.begin() ; mem_iter != members.end(); mem_iter++) {
      string mname = (*mem_iter)->get_name();
      indent(out) << "bool " << name
                  << "::write_" << mname << "(yarp::os::idl::WireWriter& writer) {"
                  << endl;
      indent_up();
      generate_serialize_field(out, *mem_iter, "");
      indent(out) << "return true;" << endl;
      scope_down(out);
      indent(out) << "bool " << name
                  << "::nested_write_" << mname << "(yarp::os::idl::WireWriter& writer) {"
                  << endl;
      indent_up();
      generate_serialize_field(out, *mem_iter, "", "", true);
      indent(out) << "return true;" << endl;
      scope_down(out);
    }

    indent(out) << "bool " << name
                << "::write(yarp::os::idl::WireWriter& writer) {"
                << endl;
    indent_up();
    for (mem_iter=members.begin() ; mem_iter != members.end(); mem_iter++) {
      string mname = (*mem_iter)->get_name();
      indent(out) << "if (!write_" << mname << "(writer)) return false;" << endl;
    }
    indent(out) << "return !writer.isError();"
                << endl;
    scope_down(out);
    out << endl;

    indent(out) << "bool " << name
                << "::write(yarp::os::ConnectionWriter& connection) {"
                << endl;
    indent_up();
    indent(out) << "yarp::os::idl::WireWriter writer(connection);"
                << endl;
    indent(out) << "if (!writer.writeListHeader("
                << flat_element_count(tstruct)
                << ")) return false;"
                << endl;
    indent(out) << "return write(writer);" << endl;
    scope_down(out);
  }

  {
    ofstream& out = f_cpp_;

    indent(out) << "bool " << name
                << "::Editor::write(yarp::os::ConnectionWriter& connection) {"
                << endl;
    indent_up();
    indent(out) << "if (!isValid()) return false;" << endl;
    indent(out) << "yarp::os::idl::WireWriter writer(connection);"
                << endl;
    indent(out) << "if (!writer.writeListHeader(dirty_count+1)) return false;" << endl;
    indent(out) << "if (!writer.writeString(\"patch\")) return false;" << endl;
    for (mem_iter=members.begin() ; mem_iter != members.end(); mem_iter++) {
      string mname = (*mem_iter)->get_name();
      indent(out) << "if (is_dirty_" << mname << ") {" << endl;
      indent_up();
      indent(out) << "if (!writer.writeListHeader(3)) return false;" << endl;
      indent(out) << "if (!writer.writeString(\"set\")) return false;" << endl;
      indent(out) << "if (!writer.writeString(\"" << mname << "\")) return false;" << endl;
      indent(out) << "if (!obj->nested_write_" << mname << "(writer)) return false;" << endl;
      scope_down(out);
    }
    indent(out) << "return !writer.isError();"
                   << endl;
    scope_down(out);


    indent(out) << "bool " << name
                << "::Editor::read(yarp::os::ConnectionReader& connection) {"
                << endl;
    indent_up();
    indent(out) << "if (!isValid()) return false;" << endl;
    indent(out) << "yarp::os::idl::WireReader reader(connection);" << endl;
    indent(out) << "reader.expectAccept();" << endl;
    indent(out) << "if (!reader.readListHeader()) return false;" << endl;
    indent(out) << "int len = reader.getLength();" << endl;
    indent(out) << "if (len==0) {" << endl;
    indent_up();
    indent(out) << "yarp::os::idl::WireWriter writer(reader);" << endl;
    indent(out) << "if (writer.isNull()) return true;" << endl;
    indent(out) << "if (!writer.writeListHeader(1)) return false;" << endl;
    indent(out) << "writer.writeString(\"send: 'help' or 'patch (param1 val1) (param2 val2)'\");" << endl;
    indent(out) << "return true;" << endl;
    scope_down(out);
    indent(out) << "yarp::os::ConstString tag;" << endl;
    indent(out) << "if (!reader.readString(tag)) return false;" << endl;
    indent(out) << "if (tag==\"help\") {" << endl;
    indent_up();
    indent(out) << "yarp::os::idl::WireWriter writer(reader);" << endl;
    indent(out) << "if (writer.isNull()) return true;" << endl;
    indent(out) << "if (!writer.writeListHeader(2)) return false;" << endl;
    indent(out) << "if (!writer.writeTag(\"many\",1, 0)) return false;" << endl;
    indent(out) << "if (reader.getLength()>0) {" << endl;
    indent_up();
    indent(out) << "yarp::os::ConstString field;" << endl;
    indent(out) << "if (!reader.readString(field)) return false;" << endl;
    for (mem_iter = members.begin() ; mem_iter != members.end(); mem_iter++) {
      string mname = (*mem_iter)->get_name();
      indent(out) << "if (field==\"" << mname << "\") {" << endl;
      indent_up();
      std::vector<std::string> doc;
      quote_doc(doc,*mem_iter);
      indent(out) << "if (!writer.writeListHeader(" << (1+doc.size())
                  << ")) return false;" << endl;
      string mtype = print_type((*mem_iter)->get_type());
      indent(out) << "if (!writer.writeString(\"" << mtype << " " << mname << "\")) return false;" << endl;
      for (int i=0; i<(int)doc.size(); i++) {
        indent(out) << "if (!writer.writeString(\"" << doc[i] << "\")) return false;" << endl;
      }
      scope_down(out);
    }
    scope_down(out);
    indent(out) << "if (!writer.writeListHeader(" << (mem_ct+1)
                << ")) return false;" << endl;
    indent(out) << "writer.writeString(\"*** Available fields:\");" << endl;
    for (mem_iter = members.begin() ; mem_iter != members.end(); mem_iter++) {
      string mname = (*mem_iter)->get_name();
      indent(out) << "writer.writeString(\"" << mname << "\");" << endl;
    }
    indent(out) << "return true;" << endl;
    scope_down(out);
    indent(out) << "bool nested = true;" << endl;
    indent(out) << "bool have_act = false;" << endl;
    indent(out) << "if (tag!=\"patch\") {" << endl;
    indent_up();
    indent(out) << "if ((len-1)%2 != 0) return false;" << endl;
    indent(out) << "len = 1 + ((len-1)/2);" << endl;
    indent(out) << "nested = false;" << endl;
    indent(out) << "have_act = true;" << endl;
    scope_down(out);
    indent(out) << "for (int i=1; i<len; i++) {" << endl;
    indent_up();
    indent(out) << "if (nested && !reader.readListHeader(3)) return false;" << endl;
    indent(out) << "yarp::os::ConstString act;" << endl;
    indent(out) << "yarp::os::ConstString key;" << endl;
    indent(out) << "if (have_act) {" << endl;
    indent_up();
    indent(out) << "act = tag;" << endl;
    indent_down();
    indent(out) << "} else {" << endl;
    indent_up();
    indent(out) << "if (!reader.readString(act)) return false;" << endl;
    scope_down(out);
    indent(out) << "if (!reader.readString(key)) return false;" << endl;
    indent(out) << "// inefficient code follows, bug paulfitz to improve it" << endl;
    for (mem_iter=members.begin() ; mem_iter != members.end(); mem_iter++) {
      string mname = (*mem_iter)->get_name();
      if (mem_iter!=members.begin()) {
        indent_down();
        indent(out) << "} else ";
      } else {
        indent(out);
      }
      out <<  "if (key == \"" << mname << "\") {" << endl;
      indent_up();
      indent(out) << "will_set_" << mname << "();" << endl;
      indent(out) << "if (!obj->nested_read_" << mname << "(reader)) return false;" << endl;
      indent(out) << "did_set_" << mname << "();" << endl;
    }
    if (members.begin()!=members.end()) {
      indent_down();
      indent(out) << "} else {" << endl;
      indent_up();
    }
    indent(out) << "// would be useful to have a fallback here" << endl;
    if (members.begin()!=members.end()) {
      scope_down(out);
    }
    scope_down(out);
    indent(out) << "reader.accept();" << endl;
    indent(out) << "yarp::os::idl::WireWriter writer(reader);" << endl;
    indent(out) << "if (writer.isNull()) return true;" << endl;
    indent(out) << "writer.writeListHeader(1);" << endl;
    indent(out) << "writer.writeVocab(VOCAB2('o','k'));" << endl;
    indent(out) << "return true;" << endl;
    scope_down(out);

    out << endl;
  }

  indent(f_cpp_) << "yarp::os::ConstString " << name
                 << "::toString() {"
                 << endl;
  indent_up();
  indent(f_cpp_) << "yarp::os::Bottle b;" << endl;
  indent(f_cpp_) << "b.read(*this);" << endl;
  indent(f_cpp_) << "return b.toString();" << endl;
  scope_down(f_cpp_);

  namespace_close(f_cpp_,get_namespace(program_));
}

void t_yarp_generator::generate_xception(t_struct* txception) {
  generate_struct(txception);
}


std::string t_yarp_generator::function_prototype(t_function *tfn,
                                                 bool include_defaults,
                                                 const char *prefix,
                                                 const char *override_name) {
  string result = "";
  t_function **fn_iter = &tfn;
  string fn_name = (*fn_iter)->get_name();
  string return_type = print_type((*fn_iter)->get_returntype());
  if (override_name!=NULL) {
    fn_name = override_name;
    return_type = "void";
  }
  result += return_type;
  result += string(" ");
  if (prefix) {
    result += prefix;
    result += "::";
  }
  result += fn_name + "(";
  bool first = true;
  vector<t_field*> args = (*fn_iter)->get_arglist()->get_members();
  vector<t_field*>::iterator arg_iter = args.begin();
  if (arg_iter != args.end()) {
    for ( ; arg_iter != args.end(); arg_iter++) {
      if (!first) {
        result += ", ";
      }
      first = false;
      result += type_name((*arg_iter)->get_type(),false,true);
      result += string(" ") + (*arg_iter)->get_name();
      if (include_defaults) {
        if ((*arg_iter)->get_value() != NULL) {
          result += " = ";
          result += print_const_value((*arg_iter)->get_value(),
                                      (*arg_iter)->get_type());
        }
      }
    }
  }
  result += ")";
  return result;
}

void t_yarp_generator::generate_service(t_service* tservice) {
  string svcname = tservice->get_name();
  string f_header_name = get_out_dir()+svcname+".h";
  string f_cpp_name = get_out_dir()+svcname+".cpp";
  ofstream f_srv_;
  f_srv_.open(f_header_name.c_str());
  ofstream f_cpp_;
  f_cpp_.open(f_cpp_name.c_str());

  t_service* extends_service = tservice->get_extends();

  {
    auto_warn(f_srv_);
    f_srv_ << "#ifndef YARP_THRIFT_GENERATOR_" << svcname << endl;
    f_srv_ << "#define YARP_THRIFT_GENERATOR_" << svcname << endl;
    f_srv_ << endl;
    f_srv_ << "#include <yarp/os/Wire.h>" << endl;
    f_srv_ << "#include <yarp/os/idl/WireTypes.h>" << endl;

    auto_warn(f_cpp_);
    if (!cmake_supplies_headers_) {
      if (need_common_)
        f_srv_ << "#include <"<< get_include_prefix(*program_) << program_->get_name() << "_common.h>" <<endl;

      if (extends_service != NULL) {
        f_srv_ << "#include <" << get_include_prefix(*(extends_service->get_program())) << extends_service->get_name() << ".h>" << endl;
      }

      vector<t_function*> functions = tservice->get_functions();
      vector<t_function*>::iterator fn_iter;

      std::set<string> neededTypes;
      neededTypes.clear();
      for (fn_iter = functions.begin(); fn_iter != functions.end(); fn_iter++) {

          t_type* returntype = (*fn_iter)->get_returntype();
          if (!returntype->is_void()) {
              getNeededType(returntype, neededTypes);
          }
          vector<t_field*> args = (*fn_iter)->get_arglist()->get_members();
          vector<t_field*>::iterator arg_iter;
          for (arg_iter = args.begin(); arg_iter != args.end(); arg_iter++) {
              getNeededType((*arg_iter)->get_type(), neededTypes);
          }

       }

       for (std::set<string>::iterator inclIter=neededTypes.begin(); inclIter!=neededTypes.end(); ++inclIter){
            f_srv_ << "#include <" << *inclIter << ">" << endl;
       }

       f_srv_ << endl;

       f_cpp_ << "#include <" << get_include_prefix(*(tservice->get_program())) + svcname + ".h>" <<endl;
       //f_cpp_ << "#include <" << program_->get_name() << "_index.h>" << endl;
    } else {
      f_srv_ << "@HEADERS@" << endl << endl;
    }
    f_cpp_ << "#include <yarp/os/idl/WireTypes.h>" << endl;
    if (cmake_supplies_headers_) {
      f_cpp_ << "@HEADERS@" << endl;
    }
    f_cpp_ << endl;

    string ns = get_namespace(program_);

    namespace_open(f_srv_,ns,false);
    indent(f_srv_) << "class " << service_name_ << ";" << endl;
    namespace_close(f_srv_,ns,false);
    f_srv_ << endl << endl;

    namespace_open(f_cpp_,ns);
    f_cpp_ << endl << endl;

    vector<t_function*> functions = tservice->get_functions();
    vector<t_function*>::iterator fn_iter = functions.begin();
    {
      ofstream& f_curr_ = f_cpp_;
      for ( ; fn_iter != functions.end(); fn_iter++) {
        string fname = (*fn_iter)->get_name();
        f_curr_ << "class " << service_name_ << "_" << fname
                << " : public yarp::os::Portable {"
                << endl;
        indent(f_curr_) << "public:" << endl;
        indent_up();
        vector<t_field*> args = (*fn_iter)->get_arglist()->get_members();
        vector<t_field*>::iterator arg_iter = args.begin();
        for ( ; arg_iter != args.end(); arg_iter++) {
          indent(f_curr_) << declare_field(*arg_iter) << endl;
        }
        t_type* returntype = (*fn_iter)->get_returntype();
        t_field returnfield(returntype, "_return");
        if (!returntype->is_void()) {
          indent(f_curr_) << declare_field(&returnfield) << endl;
        }
        
        indent(f_curr_) << function_prototype(*fn_iter,false,NULL,"init") << ";" << endl;
        indent(f_curr_) << "virtual bool write(yarp::os::ConnectionWriter& connection);" << endl;
        indent(f_curr_) << "virtual bool read(yarp::os::ConnectionReader& connection);" << endl;

        indent_down();
        f_curr_ << "};" << endl;
        f_curr_ << endl;
      }
    }
    {
      ofstream& f_curr_ = f_cpp_;
      fn_iter = functions.begin();
      for ( ; fn_iter != functions.end(); fn_iter++) {
        string fname = (*fn_iter)->get_name();
        //f_curr_ << "class " << service_name_ << "_" << fname
        //        << " : public yarp::os::Portable {"
        //        << endl;
        //indent(f_curr_) << "public:" << endl;
        
        vector<t_field*> args = (*fn_iter)->get_arglist()->get_members();
        vector<t_field*>::iterator arg_iter;
        t_type* returntype = (*fn_iter)->get_returntype();
        t_field returnfield(returntype, "_return");
        indent(f_curr_) << "bool " << service_name_ << "_" << fname << "::write(yarp::os::ConnectionWriter& connection) {" << endl;
        indent_up();
        yfn y((*fn_iter)->get_name());
        indent(f_curr_) << "yarp::os::idl::WireWriter writer(connection);"
                        << endl;
        indent(f_curr_) << "if (!writer.writeListHeader("
                        << flat_element_count(*fn_iter)+y.len
                        << ")) return false;"
                        << endl;
        arg_iter = args.begin();
        indent(f_curr_) << "if (!writer.writeTag(\"" << y.name << "\",1," << y.len << ")) return false;" << endl;
        if (arg_iter != args.end()) {
          for ( ; arg_iter != args.end(); arg_iter++) {
            generate_serialize_field(f_curr_, *arg_iter, "");
          }
        }
        indent(f_curr_) << "return true;" << endl;
        indent_down();
        indent(f_curr_) << "}" << endl;
        f_curr_ << endl;
        
        indent(f_curr_) << "bool " << service_name_ << "_" << fname << "::read(yarp::os::ConnectionReader& connection) {" << endl;
        indent_up();
        if (!(*fn_iter)->is_oneway()) {
          indent(f_curr_) << "yarp::os::idl::WireReader reader(connection);"
                          << endl;
          indent(f_curr_) << "if (!reader.readListReturn("
                          << ")) return false;"
                          << endl;
          if (!returntype->is_void()) {
            generate_deserialize_field(f_curr_, &returnfield, "");
          }
        } else {
          indent(f_curr_) << "YARP_UNUSED(connection);" << endl;
        }
        indent(f_curr_) << "return true;" << endl;
        indent_down();
        indent(f_curr_) << "}" << endl;
        f_curr_ << endl;


        {
          indent(f_curr_) <<
            function_prototype(*fn_iter,false,(service_name_ + "_" + fname).c_str(),"init") << " {" << endl;
          indent_up();
          t_type* returntype = (*fn_iter)->get_returntype();
          t_field returnfield(returntype, "_return");
          if (!returntype->is_void()) {
            indent(f_curr_) << declare_field(&returnfield, true,false,false,false,"") << endl;
          }
          vector<t_field*> args = (*fn_iter)->get_arglist()->get_members();
          vector<t_field*>::iterator arg_iter = args.begin();
          if (arg_iter != args.end()) {
            for ( ; arg_iter != args.end(); arg_iter++) {
              indent(f_curr_) << "this->" << (*arg_iter)->get_name()
                              << " = "
                              << (*arg_iter)->get_name() << ";" << endl;
            }
          }
          indent_down();
          indent(f_curr_) << "}" << endl;
          f_curr_ << endl;
        }
      }
    }

    //add documentation (should add a generator option for it?)
   //if(tservice->has_doc())
    print_doc(f_srv_,tservice);
       // f_srv_ << "/** \class "<< svcname << f_header_name << f_header_name << endl<< print_doc(tservice)<< " */" <<endl;
    string extends = "";
    if (extends_service != NULL) {
      extends = " :  public " + print_type(extends_service);
    }
    else
    {
        extends = " : public yarp::os::Wire";
    }
    f_srv_ << "class " << namespace_decorate(ns,service_name_) << extends << " {" << endl;
    f_srv_ << "public:" << endl;
    indent_up();

    indent(f_srv_) << service_name_ << "();" << endl;

    indent_down();
    indent(f_cpp_) << service_name_ << "::" << service_name_ << "() {" << endl;
    indent_up();
    indent(f_cpp_) << "yarp().setOwner(*this);" << endl;
    scope_down(f_cpp_);
    indent_up();

    fn_iter = functions.begin();
    for ( ; fn_iter != functions.end(); fn_iter++) {
      //  if((*fn_iter)->has_doc())
      print_doc(f_srv_,(*fn_iter));
      indent(f_srv_) << "virtual " << function_prototype(*fn_iter,true)
                     << ";" << endl;

      indent_down();
      indent(f_cpp_) << function_prototype(*fn_iter,false,service_name_.c_str())
                     << " {" << endl;
      indent_up();

      t_type* returntype = (*fn_iter)->get_returntype();
      t_field returnfield(returntype, "_return");
      if (!returntype->is_void()) {
        indent(f_cpp_) << declare_field(&returnfield, true) << endl;
      }
      indent(f_cpp_) << service_name_ << "_" << (*fn_iter)->get_name()
                     << " helper;" << endl;
      vector<t_field*> args = (*fn_iter)->get_arglist()->get_members();
      vector<t_field*>::iterator arg_iter = args.begin();
      indent(f_cpp_) << "helper.init" << "(";
      if (arg_iter != args.end()) {
        bool first = true;
        for ( ; arg_iter != args.end(); arg_iter++) {
          if (!first) f_cpp_ << ",";
          first = false;
          f_cpp_ << (*arg_iter)->get_name();
        }
      }
      f_cpp_ << ");" << endl;
      indent(f_cpp_) << "if (!yarp().canWrite()) {" << endl;
      indent_up();
      indent(f_cpp_) << "fprintf(stderr,\"Missing server method '%s'?\\n\",\"";
      f_cpp_ << function_prototype(*fn_iter,false,service_name_.c_str());
      f_cpp_ << "\");" << endl;
      indent_down();
      indent(f_cpp_) << "}" << endl;
      indent(f_cpp_);
      if (!returntype->is_void()) {
        f_cpp_ << "bool ok = ";
      }
      if (!(*fn_iter)->is_oneway()) {
        f_cpp_ << "yarp().write(helper,helper);" << endl;
      } else {
        f_cpp_ << "yarp().write(helper);" << endl;
        //f_cpp_ << "yarp().callback(helper,*this,\"__direct__\");" << endl;
      }
      if (!returntype->is_void()) {
        indent(f_cpp_);
        f_cpp_ << "return ok?helper._return:_return;" << endl;
      }

      indent_down();
      indent(f_cpp_) << "}" << endl;
      indent_up();
    }


    indent(f_srv_) << "virtual bool read(yarp::os::ConnectionReader& connection);"
                   << endl;
    indent(f_srv_) << "virtual std::vector<std::string> help(const std::string& functionName=\"--all\");"
                   << endl;

    indent_down();
    indent(f_cpp_) << endl
                   << "bool " << service_name_
                   << "::read(yarp::os::ConnectionReader& connection) {"
                   << endl;

    indent_up();
    indent(f_cpp_) << "yarp::os::idl::WireReader reader(connection);" << endl;
    indent(f_cpp_) << "reader.expectAccept();" << endl;
    indent(f_cpp_) << "if (!reader.readListHeader()) { reader.fail(); return false; }"
                   << endl;
    indent(f_cpp_) << "yarp::os::ConstString tag = reader.readTag();" << endl;
    indent(f_cpp_) << "bool direct = (tag==\"__direct__\");" << endl;
    indent(f_cpp_) << "if (direct) tag = reader.readTag();" << endl;
    indent(f_cpp_) << "while (!reader.isError()) {" << endl;
    indent_up();
    indent(f_cpp_) << "// TODO: use quick lookup, this is just a test" << endl;
    //indent_up();
    fn_iter = functions.begin();
    for ( ; fn_iter != functions.end(); fn_iter++) {
      indent(f_cpp_) << "if (tag == \"" << (*fn_iter)->get_name() << "\") {" << endl;
      indent_up();
      vector<t_field*> args = (*fn_iter)->get_arglist()->get_members();
      vector<t_field*>::iterator arg_iter = args.begin();
      if (arg_iter != args.end()) {
        for ( ; arg_iter != args.end(); arg_iter++) {
          indent(f_cpp_) << declare_field(*arg_iter, false) << endl;
        }
        arg_iter = args.begin();
        for ( ; arg_iter != args.end(); arg_iter++) {
          generate_deserialize_field(f_cpp_, *arg_iter, "");
        }
      }

      if ((*fn_iter)->is_oneway()) {
        indent(f_cpp_) << "if (!direct) {" << endl;
        indent_up();
        indent(f_cpp_) << service_name_ << "_" << (*fn_iter)->get_name() << " helper;" << endl;
        indent(f_cpp_) << "helper.init(";
        arg_iter = args.begin();
        if (arg_iter != args.end()) {
          bool first = true;
          for ( ; arg_iter != args.end(); arg_iter++) {
            if (!first) f_cpp_ << ",";
            first = false;
            f_cpp_ << (*arg_iter)->get_name();
          }
        }
        f_cpp_ << ");" << endl;
        indent(f_cpp_) << "yarp().callback(helper,*this,\"__direct__\");" << endl;
        indent_down();
        indent(f_cpp_) << "} else {" << endl;
        indent_up();
      }

      t_type* returntype = (*fn_iter)->get_returntype();
      t_field returnfield(returntype, "_return");
      if (!returntype->is_void()) {
        indent(f_cpp_) << declare_field(&returnfield, false) << endl;
        indent(f_cpp_) << "_return = ";
      } else {
        indent(f_cpp_);
      }
      f_cpp_ << (*fn_iter)->get_name() << "(";
      arg_iter = args.begin();
      if (arg_iter != args.end()) {
        bool first = true;
        for ( ; arg_iter != args.end(); arg_iter++) {
          if (!first) f_cpp_ << ",";
          first = false;
          f_cpp_ << (*arg_iter)->get_name();
        }
      }
      f_cpp_ << ");" << endl;

      if ((*fn_iter)->is_oneway()) {
        indent_down();
        indent(f_cpp_) << "}" << endl;
      }

      indent(f_cpp_) << "yarp::os::idl::WireWriter writer(reader);" << endl;
      indent(f_cpp_) << "if (!writer.isNull()) {" << endl;
      indent_up();
      if (!(*fn_iter)->is_oneway()) {
        indent(f_cpp_) << "if (!writer.writeListHeader("
                       << flat_element_count(returntype)
                       << ")) return false;" << endl;
        if (!returntype->is_void()) {
          generate_serialize_field(f_cpp_, &returnfield, "");
        }
      } else {
        // we are a oneway function
        // if someone is expecting a reply (e.g. yarp rpc), give one
        // (regular thrift client won't be expecting a reply, and
        // writer.isNull test will have succeeded and stopped us earlier)
        indent(f_cpp_) << "if (!writer.writeOnewayResponse()) "
                       << "return false;" << endl;
      }
      indent_down();
      indent(f_cpp_) << "}" << endl;

      indent(f_cpp_) << "reader.accept();" << endl;
      indent(f_cpp_) << "return true;" << endl;
      indent_down();
      indent(f_cpp_) << "}" << endl;
    }
    // read "help" function
    indent(f_cpp_) << "if (tag == \"help\") {" <<endl;
    indent_up();
    indent(f_cpp_) << "std::string functionName;" <<endl;
    indent(f_cpp_) << "if (!reader.readString(functionName)) {" <<endl;
    indent_up();
    indent(f_cpp_) << "functionName = \"--all\";" <<endl;
    indent_down();
    indent(f_cpp_) << "}" <<endl;
    indent(f_cpp_) << "std::vector<std::string> _return=help(functionName);" <<endl;
    indent(f_cpp_) << "yarp::os::idl::WireWriter writer(reader);" << endl;
    indent_up();
    indent(f_cpp_) << "if (!writer.isNull()) {" << endl;
    indent_up();
    indent(f_cpp_) << "if (!writer.writeListHeader(2)) return false;" << endl;
    indent(f_cpp_) << "if (!writer.writeTag(\"many\",1, 0)) return false;" << endl;
    indent(f_cpp_) << "if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(_return.size()))) return false;" << endl;
    indent(f_cpp_) << "std::vector<std::string> ::iterator _iterHelp;" << endl;
    indent(f_cpp_) << "for (_iterHelp = _return.begin(); _iterHelp != _return.end(); ++_iterHelp)" << endl;
    indent(f_cpp_) << "{" << endl;
    indent_up();
    indent(f_cpp_) << "if (!writer.writeString(*_iterHelp)) return false;" << endl;
    indent_down();
    indent(f_cpp_) << " }" << endl;
    indent(f_cpp_) << "if (!writer.writeListEnd()) return false;" << endl;
    indent_down();
    indent(f_cpp_) << "}" << endl;
    indent_down();
    indent(f_cpp_) << "reader.accept();" << endl;
    indent(f_cpp_) << "return true;" << endl;
    indent_down();
    indent(f_cpp_) << "}" << endl;

    indent(f_cpp_) << "if (reader.noMore()) { reader.fail(); return false; }"
                   << endl;
    indent(f_cpp_) << "yarp::os::ConstString next_tag = reader.readTag();" << endl;
    indent(f_cpp_) << "if (next_tag==\"\") break;" << endl;
    indent(f_cpp_) << "tag = tag + \"_\" + next_tag;" << endl;
    indent_down();
    indent(f_cpp_) << "}" << endl;
    indent(f_cpp_) << "return false;" << endl;
    indent_down();
    indent(f_cpp_) << "}" << endl << endl;

    indent(f_cpp_) << "std::vector<std::string> " << service_name_ << "::help(const std::string& functionName) {" <<endl;
    indent_up();
    indent(f_cpp_) << "bool showAll=(functionName==\"--all\");" << endl;
    indent(f_cpp_) << "std::vector<std::string> helpString;" << endl;
    indent(f_cpp_) << "if(showAll) {" << endl;
    indent_up();
    indent(f_cpp_) << "helpString.push_back(\"*** Available commands:\");" << endl;
    for (fn_iter = functions.begin() ; fn_iter != functions.end(); fn_iter++) {
        indent(f_cpp_) << "helpString.push_back(\"" << (*fn_iter)->get_name() << "\");" << endl;
    }
    indent(f_cpp_) << "helpString.push_back(\"help\");" << endl;
    indent_down();
    indent(f_cpp_) << "}" << endl;
    indent(f_cpp_) << "else {"<<endl;
    indent_up();
    for ( fn_iter = functions.begin(); fn_iter != functions.end(); fn_iter++) {
      indent(f_cpp_) << "if (functionName==\"" << (*fn_iter)->get_name() << "\") {" << endl;
      indent_up();
      std::vector<std::string> helpList=print_help(*fn_iter);
      for (std::vector<std::string>::iterator helpIt=helpList.begin(); helpIt!=helpList.end(); ++helpIt)
        indent(f_cpp_) << "helpString.push_back(\""<< *helpIt<<" \");" <<endl;

      indent_down();
      indent(f_cpp_) << "}" <<endl;
    }

    indent(f_cpp_) << "if (functionName==\"help\") {" << endl;
    indent_up();
    indent(f_cpp_) << "helpString.push_back(\"std::vector<std::string> help(const std::string& functionName=\\\"--all\\\")\");" <<endl;
    indent(f_cpp_) << "helpString.push_back(\"Return list of available commands, or help message for a specific function\");" <<endl;
    indent(f_cpp_) << "helpString.push_back(\"@param functionName name of command for which to get a detailed description. If none or '--all' is provided, print list of available commands\");" <<endl;
    indent(f_cpp_) << "helpString.push_back(\"@return list of strings (one string per line)\");" <<endl;
    indent_down();
    indent(f_cpp_) << "}" <<endl;

    indent_down();
    indent(f_cpp_) << "}" <<endl;
    indent(f_cpp_) << "if ( helpString.empty()) helpString.push_back(\"Command not found\");"<<endl;
    indent(f_cpp_) << "return helpString;" << endl;
    indent_down();
    indent(f_cpp_) << "}" << endl;

    f_srv_ << "};" << endl
           << endl;

    namespace_close(f_cpp_,ns);
    f_cpp_ << endl << endl;

    f_srv_ << "#endif" << endl;
    f_srv_ << endl;
  }
}


void t_yarp_generator::generate_count_field(ofstream& out,
                                            t_field* tfield,
                                            string prefix,
                                            string suffix) {
  t_type* type = get_true_type(tfield->get_type());

  string name = prefix + tfield->get_name() + suffix;

  if (type->is_void()) {
    return;
  }

  if (type->is_struct() || type->is_xception()) {
    indent(out) << "ct += writer.count(" << name << ");" << endl;
  } else if (type->is_container()) {
    string iter = tmp("_iter");
    out <<
      indent() << type_name(type) << "::iterator " << iter << ";" << endl <<
      indent() << "for (" << iter << " = " << name  << ".begin(); " << name << " != " << name << ".end(); ++" << iter << ")" << endl;
    scope_up(out);
    out << "ct += " << iter << "->count(writer);" << endl;
    scope_down(out);
    indent(out) << "ct++;" << endl;
  } else if (type->is_base_type() || type->is_enum()) {
    indent(out) << "ct++;" << endl;
  } else {
    printf("DO NOT KNOW HOW TO COUNT FIELD '%s' TYPE '%s'\n",
           name.c_str(),
           type_name(type).c_str());
  }
}


void t_yarp_generator::generate_serialize_field(ofstream& out,
                                                t_field* tfield,
                                                string prefix,
                                                string suffix,
                                                bool force_nesting) {
  t_type* type = get_true_type(tfield->get_type());

  string name = prefix + tfield->get_name() + suffix;

  // Do nothing for void types
  if (type->is_void()) {
    throw "CANNOT GENERATE SERIALIZE CODE FOR void TYPE: " + name;
  }

  if (type->is_struct() || type->is_xception()) {
    indent(out) << "if (!writer.";
    generate_serialize_struct(out,
                              (t_struct*)type,
                              name,
                              force_nesting);
    out << ") return false;" << endl;
  } else if (type->is_container()) {
    generate_serialize_container(out, type, name);
  } else if (type->is_base_type() || type->is_enum()) {
    indent(out) << "if (!writer.";
    if (type->is_base_type()) {
      t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
      switch (tbase) {
      case t_base_type::TYPE_VOID:
        throw
          "compiler error: cannot serialize void field in a struct: " + name;
        break;
      case t_base_type::TYPE_STRING:
        if (((t_base_type*)type)->is_binary()) {
          out << "writeBinary(" << name << ")";
        }
        else {
          out << "writeString(" << name << ")";
        }
        break;
      case t_base_type::TYPE_BOOL:
        out << "writeBool(" << name << ")";
        break;
      case t_base_type::TYPE_BYTE:
        out << "writeByte(" << name << ")";
        break;
      case t_base_type::TYPE_I16:
        out << "writeI16(" << name << ")";
        break;
      case t_base_type::TYPE_I32:
        out << "writeI32(" << name << ")";
        break;
      case t_base_type::TYPE_I64:
        out << "writeI64(" << name << ")";
        break;
      case t_base_type::TYPE_DOUBLE:
        out << "writeDouble(" << name << ")";
        break;
      default:
        throw "compiler error: no C++ writer for base type " + t_base_type::t_base_name(tbase) + name;
      }
    } else if (type->is_enum()) {
      out << "writeI32((int32_t)" << name << ")";
    }
    out << ") return false;" << endl;
  } else {
    printf("DO NOT KNOW HOW TO SERIALIZE FIELD '%s' TYPE '%s'\n",
           name.c_str(),
           type_name(type).c_str());
  }
}


void t_yarp_generator::generate_serialize_struct(ofstream& out,
                                                 t_struct* tstruct,
                                                 string prefix,
                                                 bool force_nesting) {
  if (force_nesting) {
    out << "writeNested(" << prefix << ")";
  } else {
    out << "write(" << prefix << ")";
  }
}

void t_yarp_generator::generate_serialize_container(ofstream& out,
                                                    t_type* ttype,
                                                    string prefix) {

  scope_up(out);

  if (ttype->is_map()) {
    indent(out) <<
      "if (!writer.writeMapBegin(" <<
      type_to_enum(((t_map*)ttype)->get_key_type()) << ", " <<
      type_to_enum(((t_map*)ttype)->get_val_type()) << ", " <<
      "static_cast<uint32_t>(" << prefix << ".size()))) return false;" << endl;
  } else if (ttype->is_set()) {
    indent(out) <<
      "if (!writer.writeSetBegin(" <<
      type_to_enum(((t_set*)ttype)->get_elem_type()) << ", " <<
      "static_cast<uint32_t>(" << prefix << ".size()))) return false;" << endl;
  } else if (ttype->is_list()) {
    indent(out) <<
      "if (!writer.writeListBegin(" <<
      type_to_enum(((t_list*)ttype)->get_elem_type()) << ", " <<
      "static_cast<uint32_t>(" << prefix << ".size()))) return false;" << endl;
  }

  string iter = tmp("_iter");
  out <<
    indent() << type_name(ttype) << "::iterator " << iter << ";" << endl <<
    indent() << "for (" << iter << " = " << prefix  << ".begin(); " << iter << " != " << prefix << ".end(); ++" << iter << ")" << endl;
  scope_up(out);
    if (ttype->is_map()) {
      generate_serialize_map_element(out, (t_map*)ttype, iter);
    } else if (ttype->is_set()) {
      generate_serialize_set_element(out, (t_set*)ttype, iter);
    } else if (ttype->is_list()) {
      generate_serialize_list_element(out, (t_list*)ttype, iter);
    }
  scope_down(out);

  if (ttype->is_map()) {
    indent(out) <<
      "if (!writer.writeMapEnd()) return false;" << endl;
  } else if (ttype->is_set()) {
    indent(out) <<
      "if (!writer.writeSetEnd()) return false;" << endl;
  } else if (ttype->is_list()) {
    indent(out) <<
      "if (!writer.writeListEnd()) return false;" << endl;
  }

  scope_down(out);
}

void t_yarp_generator::generate_serialize_map_element(ofstream& out,
                                                      t_map* tmap,
                                                      string iter) {
  indent(out) << "if (!writer.writeListBegin(0,2)) return false;" << endl;

  t_field kfield(tmap->get_key_type(), iter + "->first");
  generate_serialize_field(out, &kfield, "");

  t_field vfield(tmap->get_val_type(), iter + "->second");
  generate_serialize_field(out, &vfield, "");

  indent(out) << "if (!writer.writeListEnd()) return false;" << endl;
}

void t_yarp_generator::generate_serialize_set_element(ofstream& out,
                                                      t_set* tset,
                                                      string iter) {
  t_field efield(tset->get_elem_type(), "(*" + iter + ")");
  generate_serialize_field(out, &efield, "", "", true);
}

void t_yarp_generator::generate_serialize_list_element(ofstream& out,
                                                       t_list* tlist,
                                                       string iter) {
  t_field efield(tlist->get_elem_type(), "(*" + iter + ")");
  generate_serialize_field(out, &efield, "", "", true);
}



void t_yarp_generator::generate_deserialize_field_fallback(ofstream& out,
                                                           t_field* tfield) {
  out << "{" << endl;
  indent_up();
  if (tfield->get_value()!=NULL) {
    indent(out) << tfield->get_name() <<
      " = " <<
      print_const_value(tfield->get_value(),tfield->get_type()) <<
      ";" << endl;
  } else {
    indent(out) << "reader.fail();" << endl;
    indent(out) << "return false;" << endl;
  }
  indent_down();
  indent(out) << "}";
}

void t_yarp_generator::generate_deserialize_field(ofstream& out,
                                                  t_field* tfield,
                                                  string prefix,
                                                  string suffix,
                                                  bool force_nested) {
  t_type* type = get_true_type(tfield->get_type());

  if (type->is_void()) {
    throw "CANNOT GENERATE DESERIALIZE CODE FOR void TYPE: " +
      prefix + tfield->get_name();
  }

  string name = prefix + tfield->get_name() + suffix;

  if (type->is_struct() || type->is_xception()) {
    indent(out) << "if (!reader.";
    generate_deserialize_struct(out, (t_struct*)type, name, force_nested);
    out << ") " ;
    generate_deserialize_field_fallback(out,tfield);
    out << endl;
  } else if (type->is_container()) {
    generate_deserialize_container(out, type, name);
  } else if (type->is_base_type()) {
    indent(out) << "if (!reader.";
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
    switch (tbase) {
    case t_base_type::TYPE_VOID:
      throw "compiler error: cannot serialize void field in a struct: " + name;
      break;
    case t_base_type::TYPE_STRING:
      if (((t_base_type*)type)->is_binary()) {
        out << "readBinary(" << name << ")";
      }
      else {
        out << "readString(" << name << ")";
      }
      break;
    case t_base_type::TYPE_BOOL:
      out << "readBool(" << name << ")";
      break;
    case t_base_type::TYPE_BYTE:
      out << "readByte(" << name << ")";
      break;
    case t_base_type::TYPE_I16:
      out << "readI16(" << name << ")";
      break;
    case t_base_type::TYPE_I32:
      out << "readI32(" << name << ")";
      break;
    case t_base_type::TYPE_I64:
      out << "readI64(" << name << ")";
      break;
    case t_base_type::TYPE_DOUBLE:
      out << "readDouble(" << name << ")";
      break;
    default:
      throw "compiler error: no C++ reader for base type " + t_base_type::t_base_name(tbase) + name;
    }
    out << ") ";
    generate_deserialize_field_fallback(out,tfield);
    out << endl;
  } else if (type->is_enum()) {
    string t = tmp("ecast");
    string t2 = tmp("cvrt");
    out <<
      indent() << "int32_t " << t << ";" << endl <<
      indent() << type_name(type) << "Vocab " << t2 << ";" << endl <<
      indent() << "if (!reader.readEnum(" << t << "," << t2 << ")) ";
    generate_deserialize_field_fallback(out,tfield);
    out << " else {" << endl;
    indent_up();
    out << indent() << name << " = (" << type_name(type) << ")" << t << ";" << endl;
    indent_down();
    indent(out) << "}" << endl;
  } else {
    printf("DO NOT KNOW HOW TO DESERIALIZE FIELD '%s' TYPE '%s'\n",
           tfield->get_name().c_str(), type_name(type).c_str());
  }
}

void t_yarp_generator::generate_deserialize_struct(ofstream& out,
                                                   t_struct* tstruct,
                                                   string prefix,
                                                   bool force_nested) {
  (void) tstruct;
  out <<
    "read" << (force_nested?"Nested":"") << "(" << prefix << ")";
}

void t_yarp_generator::generate_deserialize_container(ofstream& out,
                                                      t_type* ttype,
                                                      string prefix) {
  scope_up(out);

  string size = tmp("_size");
  string ktype = tmp("_ktype");
  string vtype = tmp("_vtype");
  string etype = tmp("_etype");

  t_container* tcontainer = (t_container*)ttype;
  bool use_push = tcontainer->has_cpp_name();

  indent(out) <<
    prefix << ".clear();" << endl <<
    indent() << "uint32_t " << size << ";" << endl;

  // Declare variables, read header
  if (ttype->is_map()) {
    // kttpe and vtype available
    out <<
      indent() << "yarp::os::idl::WireState " << ktype << ";" << endl <<
      indent() << "yarp::os::idl::WireState " << vtype << ";" << endl <<
      indent() << "reader.readMapBegin(" <<
      ktype << ", " << vtype << ", " << size << ");" << endl;
  } else if (ttype->is_set()) {
    out <<
      indent() << "yarp::os::idl::WireState " << etype << ";" << endl <<
      indent() << "reader.readSetBegin(" <<
      etype << ", " << size << ");" << endl;
  } else if (ttype->is_list()) {
    out <<
      indent() << "yarp::os::idl::WireState " << etype << ";" << endl <<
      indent() << "reader.readListBegin(" <<
      etype << ", " << size << ");" << endl;
    if (!use_push) {
      indent(out) << prefix << ".resize(" << size << ");" << endl;
    }
  }


  // For loop iterates over elements
  string i = tmp("_i");
  out <<
    indent() << "uint32_t " << i << ";" << endl <<
    indent() << "for (" << i << " = 0; " << i << " < " << size << "; ++" << i << ")" << endl;

    scope_up(out);

    if (ttype->is_map()) {
      generate_deserialize_map_element(out, (t_map*)ttype, prefix);
    } else if (ttype->is_set()) {
      generate_deserialize_set_element(out, (t_set*)ttype, prefix);
    } else if (ttype->is_list()) {
      generate_deserialize_list_element(out, (t_list*)ttype, prefix, use_push, i);
    }

    scope_down(out);

  // Read container end
  if (ttype->is_map()) {
    indent(out) << "reader.readMapEnd();" << endl;
  } else if (ttype->is_set()) {
    indent(out) << "reader.readSetEnd();" << endl;
  } else if (ttype->is_list()) {
    indent(out) << "reader.readListEnd();" << endl;
  }

  scope_down(out);
}


void t_yarp_generator::generate_deserialize_map_element(ofstream& out,
                                                        t_map* tmap,
                                                        string prefix) {
  string lst = tmp("_lst");
  string size = tmp("_size");
  indent(out) <<
    indent() << "uint32_t " << size << ";" << endl;
  out <<
    indent() << "yarp::os::idl::WireState " << lst << ";" << endl <<
    indent() << "reader.readListBegin(" <<
    lst << ", " << size << ");" << endl;

  string key = tmp("_key");
  string val = tmp("_val");
  t_field fkey(tmap->get_key_type(), key);
  t_field fval(tmap->get_val_type(), val);

  out <<
    indent() << declare_field(&fkey) << endl;

  generate_deserialize_field(out, &fkey);
  indent(out) <<
    declare_field(&fval, false, false, false, true) << " = " <<
    prefix << "[" << key << "];" << endl;

  generate_deserialize_field(out, &fval);

  out <<
    indent() << "reader.readListEnd();" << endl;
}

void t_yarp_generator::generate_deserialize_set_element(ofstream& out,
                                                        t_set* tset,
                                                        string prefix) {
  string elem = tmp("_elem");
  t_field felem(tset->get_elem_type(), elem);

  indent(out) <<
    declare_field(&felem) << endl;

  generate_deserialize_field(out, &felem,"","",true);

  indent(out) <<
    prefix << ".insert(" << elem << ");" << endl;
}

void t_yarp_generator::generate_deserialize_list_element(ofstream& out,
                                                         t_list* tlist,
                                                         string prefix,
                                                         bool use_push,
                                                         string index) {
  if (use_push) {
    string elem = tmp("_elem");
    t_field felem(tlist->get_elem_type(), elem);
    indent(out) << declare_field(&felem) << endl;
    generate_deserialize_field(out, &felem,"","",true);
    indent(out) << prefix << ".push_back(" << elem << ");" << endl;
  } else {
    t_field felem(tlist->get_elem_type(), prefix + "[" + index + "]");
    generate_deserialize_field(out, &felem,"","",true);
  }
}

string t_yarp_generator::declare_field(t_field* tfield, bool init, bool pointer, bool is_constant, bool reference, const char *force_type) {
  // TODO(mcslee): do we ever need to initialize the field?
  string result = "";
  if (is_constant) {
    result += "const ";
  }
  if (!force_type) {
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
  if (result.length()>0) {
    result += " ";
  }
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
      case t_base_type::TYPE_BYTE:
      case t_base_type::TYPE_I16:
      case t_base_type::TYPE_I32:
      case t_base_type::TYPE_I64:
        result += " = 0";
        break;
      case t_base_type::TYPE_DOUBLE:
        result += " = (double)0";
        break;
      default:
        throw "compiler error: no C++ initializer for base type " + t_base_type::t_base_name(tbase);
      }
    } else if (type->is_enum()) {
      result += " = (" + type_name(type) + ")0";
    }
  }
  if (!reference) {
    result += ";";
  }
  return result;
}



void t_yarp_generator::print_const_value(ofstream& out, string name, t_type* type, t_const_value* value) {
  type = get_true_type(type);
  if (type->is_base_type()) {
    string v2 = render_const_value(out, name, type, value);
    indent(out) << name << " = " << v2 << ";" << endl <<
      endl;
  } else if (type->is_enum()) {
    indent(out) << name << " = (" << type_name(type) << ")" << value->get_integer() << ";" << endl <<
      endl;
  } else if (type->is_struct() || type->is_xception()) {
    const vector<t_field*>& fields = ((t_struct*)type)->get_members();
    vector<t_field*>::const_iterator f_iter;
    const map<t_const_value*, t_const_value*>& val = value->get_map();
    map<t_const_value*, t_const_value*>::const_iterator v_iter;
    for (v_iter = val.begin(); v_iter != val.end(); ++v_iter) {
      t_type* field_type = NULL;
      for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
        if ((*f_iter)->get_name() == v_iter->first->get_string()) {
          field_type = (*f_iter)->get_type();
        }
      }
      if (field_type == NULL) {
        throw "type error: " + type->get_name() + " has no field " + v_iter->first->get_string();
      }
      string val = render_const_value(out, name, field_type, v_iter->second);
      indent(out) << name << "." << v_iter->first->get_string() << " = " << val << ";" << endl;
      indent(out) << name << ".__isset." << v_iter->first->get_string() << " = true;" << endl;
    }
    out << endl;
  } else if (type->is_map()) {
    t_type* ktype = ((t_map*)type)->get_key_type();
    t_type* vtype = ((t_map*)type)->get_val_type();
    const map<t_const_value*, t_const_value*>& val = value->get_map();
    map<t_const_value*, t_const_value*>::const_iterator v_iter;
    for (v_iter = val.begin(); v_iter != val.end(); ++v_iter) {
      string key = render_const_value(out, name, ktype, v_iter->first);
      string val = render_const_value(out, name, vtype, v_iter->second);
      indent(out) << name << ".insert(std::make_pair(" << key << ", " << val << "));" << endl;
    }
    out << endl;
  } else if (type->is_list()) {
    t_type* etype = ((t_list*)type)->get_elem_type();
    const vector<t_const_value*>& val = value->get_list();
    vector<t_const_value*>::const_iterator v_iter;
    for (v_iter = val.begin(); v_iter != val.end(); ++v_iter) {
      string val = render_const_value(out, name, etype, *v_iter);
      indent(out) << name << ".push_back(" << val << ");" << endl;
    }
    out << endl;
  } else if (type->is_set()) {
    t_type* etype = ((t_set*)type)->get_elem_type();
    const vector<t_const_value*>& val = value->get_list();
    vector<t_const_value*>::const_iterator v_iter;
    for (v_iter = val.begin(); v_iter != val.end(); ++v_iter) {
      string val = render_const_value(out, name, etype, *v_iter);
      indent(out) << name << ".insert(" << val << ");" << endl;
    }
    out << endl;
  } else {
    throw "INVALID TYPE IN print_const_value: " + type->get_name();
  }
}


string t_yarp_generator::render_const_value(ofstream& out, string name, t_type* type, t_const_value* value) {
  (void) name;
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
    case t_base_type::TYPE_BYTE:
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
    render << "(" << type_name(type) << ")" << value->get_integer();
  } else {
    string t = tmp("tmp");
    indent(out) << type_name(type) << " " << t << ";" << endl;
    print_const_value(out, t, type, value);
    render << t;
  }

  return render.str();
}


int t_yarp_generator::flat_element_count(t_type* type) {
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

int t_yarp_generator::flat_element_count(t_struct* tstruct) {
  vector<t_field*> members = tstruct->get_members();
  vector<t_field*>::iterator mem_iter;
  int ct = 0;
  for (mem_iter = members.begin() ; mem_iter != members.end(); mem_iter++) {
    ct += flat_element_count((*mem_iter)->get_type());
  }
  return ct;
}

int t_yarp_generator::flat_element_count(t_function* fn) {
  vector<t_field*> members = fn->get_arglist()->get_members();
  vector<t_field*>::iterator mem_iter;
  int ct = 0;
  for (mem_iter = members.begin() ; mem_iter != members.end(); mem_iter++) {
    ct += flat_element_count((*mem_iter)->get_type());
  }
  return ct;
}


THRIFT_REGISTER_GENERATOR(yarp, "YARP", "")

