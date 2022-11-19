#ifdef ENUM_INFO
#define PARAM_INFO( mode, str,funcname)  mode, 
#endif 
#ifdef MODESTR_INFO
#define PARAM_INFO( mode, str, funcname)  str, 
#endif 
#ifdef FUNCNAME_INFO
#define PARAM_INFO( mode, str, funcname)  funcname, 
#endif 
#ifdef FUNCDECLARE_INFO
#define PARAM_INFO( mode, str, funcname)  int funcname( int , char *[]); 
#endif 
#ifdef PARAM_INFO
PARAM_INFO( _MODEA, "A", g_chk_paramA) 
PARAM_INFO( _MODEB, "B", g_chk_paramB) 
PARAM_INFO( _MODEC, "C", g_chk_paramC) 
#undef PARAM_INFO
#endif 
#ifdef ENUM_INFO
#undef ENUM_INFO
#endif 
#ifdef MODESTR_INFO
#undef MODESTR_INFO
#endif 
#ifdef FUNCNAME_INFO
#undef FUNCNAME_INFO
#endif
#ifdef FUNCDECLARE_INFO
#undef FUNCDECLARE_INFO
#endif

