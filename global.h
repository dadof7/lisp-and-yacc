extern jmp_buf top; // main.c
extern char *error_string;

enum { e_ok = 0, // normal setjump return value
       e_eostr,
       e_bogus, 
       e_noint, 
       e_nocons, 
       e_symtabof,
       e_strtabof,
       e_undef, 
       e_apply, 
       e_consof, 
       e_div0, 
       e_arg,
       e_argtype,
       e_notsym,
       e_tokenp,
       e_eof,
       e_mularg,
       e_parseerr,
       e_dupargs,
       e_fopenerr,
       e_unix_exception,
       e_commaat_by_itself,
       e_commaat_evaled_to_non_list,
       e_notlist,
       e_macroex,
       e_evalerr,
};

extern cell_ptr top_level_env;  // the top level env, all binding info is here
extern cell_ptr the_last_expr;  // the last expression, can get by evaluating $			   
extern cell_ptr the_last_func;  // the last func or macro being evaluated, or just evaluated
extern cell_ptr the_last_args;  // the last arguments for the last func or macro call
extern cell_ptr the_last_fbody; // the last fbody for the last func or macro call

extern cell_ptr lambda_sym;
extern cell_ptr macro_sym;
extern cell_ptr true_sym;
extern cell_ptr progn_sym;
extern cell_ptr quote_sym;
extern cell_ptr bquote_sym;
extern cell_ptr comma_sym;
extern cell_ptr comma_at_sym;
extern cell_ptr andrest_sym;
extern cell_ptr debug_sym;

extern void init();
extern int gc_flag;
