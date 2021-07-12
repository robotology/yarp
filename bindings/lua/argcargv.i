// SPDX-FileCopyrightText: 2012 stack exchange inc, Konrad Eisele
// SPDX-License-Identifier: CC-BY-SA-3.0

// Port of Ruby argcargv.i typemap file
// Code general idea was from: http://stackoverflow.com/questions/9860362/swig-lua-call-mint-argv-char-argc

/* ------------------------------------------------------------
 * --- Argc & Argv ---
 * ------------------------------------------------------------ */

/* ------------------------------------------------------------

   Use it as follow:

     %apply (int ARGC, char **ARGV) { (int argc, const char *argv[]) }

     int mainApp(int argc, const char **argv)
     {
       return argc;
     }

   then in the lua side:

     mainApp(arg);

   where arg is the global table containing all command line arguments,
   with the script name being store at index 0.

 * ------------------------------------------------------------ */

%{
SWIGINTERN int SWIG_argv_size(lua_State* L, int index) {
  int n=0;
  while(1){
    lua_rawgeti(L,index,n);
    if (lua_isnil(L,-1))
      break;
    ++n;
    lua_pop(L,1);
  }
  lua_pop(L,1);
  return n;
}
%}

%typemap(in) (int ARGC, char **ARGV) {
  if (lua_istable(L,$input)) {
    int i, size = SWIG_argv_size(L,$input);
    $1 = ($1_ltype) size;
    $2 = (char **) malloc((size+1)*sizeof(char *));
    for (i = 0; i < size; i++) {
      lua_rawgeti(L,$input,i);
      if (lua_isnil(L,-1))
   break;
      $2[i] = (char *)lua_tostring(L, -1);
      lua_pop(L,1);
    }
    $2[i]=NULL;
  } else {
    $1 = 0; $2 = 0;
    lua_pushstring(L,"Expecting argv array");
    lua_error(L);
  }
}

%typemap(typecheck, precedence=SWIG_TYPECHECK_STRING_ARRAY) (int ARGC, char **ARGV) {
  $1 = lua_istable(L,$input);
}

%typemap(freearg) (int ARGC, char **ARGV) {
  free((char *) $2);
}
