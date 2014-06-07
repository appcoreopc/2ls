/*******************************************************************\

Module: Data Flow Analysis

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#define DEBUG

#include <solvers/sat/satcheck.h>
#include <solvers/flattening/bv_pointers.h>

#include "ssa_fixed_point.h"

#ifdef DEBUG
#include <iostream>
#endif

/*******************************************************************\

Function: ssa_fixed_pointt::tie_inputs_together

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void ssa_fixed_pointt::tie_inputs_together(std::list<exprt> &dest)
{
  // the following are inputs:
  // 1) the parameters of the functions
  // 2) any global objects mentioned
  // 3) the guard at the entry point

  if(SSA_old.goto_function.body.empty() ||
     SSA_new.goto_function.body.empty())
    return;
    
  // 1) function parameters
  
  const code_typet::parameterst &p_new=
    SSA_new.goto_function.type.parameters();
    
  const code_typet::parameterst &p_old=
    SSA_old.goto_function.type.parameters();

  for(unsigned p=0; p<p_new.size(); p++)
    if(p<p_old.size() &&
       p_old[p].type()==p_new[p].type())
    {
      symbol_exprt s_old(p_old[p].get_identifier(), p_old[p].type());
      symbol_exprt s_new(p_new[p].get_identifier(), p_new[p].type());
      s_old=SSA_old.name_input(ssa_objectt(s_old, ns));
      s_new=SSA_new.name_input(ssa_objectt(s_new, ns));

      dest.push_back(equal_exprt(s_old, s_new));
    }
    
  // 2) globals
  
  for(local_SSAt::objectst::const_iterator
      it=SSA_new.ssa_objects.objects.begin();
      it!=SSA_new.ssa_objects.objects.end();
      it++)
  {
    if(!SSA_new.has_static_lifetime(*it))
      continue;
      
    if(SSA_old.ssa_objects.objects.find(*it)==
       SSA_old.ssa_objects.objects.end())
      continue;
    
    symbol_exprt s_new=SSA_new.name_input(*it);
    symbol_exprt s_old=SSA_old.name_input(*it);

    dest.push_back(equal_exprt(s_old, s_new));
  }

  // 3) guard
  
  /*
  dest.set_to_true(
    equal_exprt(SSA_old.guard_symbol(l_old),
                SSA_new.guard_symbol(l_new)));
  */
}

/*******************************************************************\

Function: ssa_fixed_pointt::do_backwards_edge

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void ssa_fixed_pointt::do_backwards_edge(
  const local_SSAt &SSA,
  locationt from)
{
  assert(from->is_backwards_goto());
  
  //locationt to=from->get_target();

  // Record the objects modified by the loop to get
  // 'primed' (post-state) and 'unprimed' (pre-state) variables.
  for(local_SSAt::objectst::const_iterator
      o_it=SSA.ssa_objects.objects.begin();
      o_it!=SSA.ssa_objects.objects.end();
      o_it++)
  {
    symbol_exprt in=SSA.name(*o_it, local_SSAt::LOOP_BACK, from);
    symbol_exprt out=SSA.read_rhs(*o_it, from);
  
    fixed_point.pre_state_vars.push_back(in);
    fixed_point.post_state_vars.push_back(out);
  }

  ssa_objectt guard=SSA.guard_symbol();
  fixed_point.pre_state_vars.push_back(SSA.name(guard, local_SSAt::LOOP_BACK, from));
  fixed_point.post_state_vars.push_back(SSA.name(guard, local_SSAt::OUT, from));
}

/*******************************************************************\

Function: ssa_fixed_pointt::do_backwards_edges

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void ssa_fixed_pointt::do_backwards_edges()
{
  // old program, if applicable
  if(use_old)
  {  
    forall_goto_program_instructions(i_it, SSA_old.goto_function.body)
    {
      if(i_it->is_backwards_goto())
        do_backwards_edge(SSA_old, i_it);
    }
  }

  // new program
  forall_goto_program_instructions(i_it, SSA_new.goto_function.body)
  {
    if(i_it->is_backwards_goto())
      do_backwards_edge(SSA_new, i_it);
  }
}

/*******************************************************************\

Function: ssa_fixed_pointt::compute_fixed_point

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void ssa_fixed_pointt::compute_fixed_point()
{
  do_backwards_edges();

  // set up transition relation
  
  // new function
  fixed_point.transition_relation << SSA_new;

  if(use_old)
  {
    // old function, if applicable
    fixed_point.transition_relation << SSA_old;
    
    // tie inputs together, if applicable
    tie_inputs_together(fixed_point.transition_relation);
  }
  
  // compute the fixed-point
  fixed_point();
}

/*******************************************************************\

Function: ssa_fixed_pointt::countermodel_expr

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void ssa_fixed_pointt::countermodel_expr(
  const exprt &src,
  std::set<exprt> &dest)
{
  forall_operands(it, src)
    countermodel_expr(*it, dest);
  
  if(src.id()==ID_symbol)
    dest.insert(src);
}
