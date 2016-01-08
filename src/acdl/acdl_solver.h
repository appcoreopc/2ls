/*******************************************************************\

Module: ACDL Solver

Author: Rajdeep Mukherjee, Peter Schrammel

\*******************************************************************/

#ifndef CPROVER_ACDL_SOLVER_H
#define CPROVER_ACDL_SOLVER_H

#include <util/options.h>
#include <goto-programs/property_checker.h>

#include "../ssa/local_ssa.h"

#include "acdl_domain.h"
#include "acdl_decision_heuristics.h"


class acdl_solvert : public messaget
{
public:
  
  explicit acdl_solvert(const optionst &_options,
			acdl_domaint &_domain,
			acdl_decision_heuristicst &_decision)
    : 
    options(_options),
    domain(_domain),
    decision(_decision)
    {
    }  

  ~acdl_solvert() 
    {
    }

  property_checkert::resultt operator()(const local_SSAt &SSA);

protected:
  const optionst &options;
  acdl_domaint &domain;
  acdl_decision_heuristicst &decision;
    
  //typedef std::set<acdl_domaint::statementt> worklistt;
  typedef std::list<acdl_domaint::statementt> worklistt;
  typedef std::list<acdl_domaint::statementt> assert_listt;
  
  virtual void initialize_worklist(const local_SSAt &, worklistt &);
  virtual void select_vars(const exprt &statement, acdl_domaint::varst &vars);
  void update_worklist(const local_SSAt &SSA,
		       const acdl_domaint::varst &vars,
		       worklistt &worklist,
		       const acdl_domaint::statementt &statement=nil_exprt());
  void push_into_worklist (worklistt &worklist, const acdl_domaint::statementt &statement);
  void push_into_assertion_list (assert_listt &aexpr, const acdl_domaint::statementt &statement);
  
  const acdl_domaint::statementt pop_from_worklist (worklistt &worklist);

  bool check_statement (const exprt &expr, const acdl_domaint::varst &vars);

  typedef struct {
    typedef exprt nodet;
    typedef std::list<nodet> deduction_list;
    // root node is assumed nil_exprt()
    std::map<nodet, nodet> edges; //reverse edges,
                                  //i.e. e1 maps to e2 <=> directed edge (e2,e1)
    nodet current_node;
    int decision_level;
    std::map<nodet, acdl_domaint::valuet> backtrack_points;
    std::map<nodet, deduction_list> propagate_list;  // used to store list of deductions
  } decision_grapht; 
  
  property_checkert::resultt propagate(const local_SSAt &SSA,
				       acdl_domaint::valuet &v,
				       worklistt &worklist);

  void decide(const local_SSAt &SSA,
	      acdl_domaint::valuet &v,
	      decision_grapht &g,
	      worklistt &worklist,
        assert_listt &alist);
  
  property_checkert::resultt analyze_conflict(const local_SSAt &SSA,
			acdl_domaint::valuet &v,
      worklistt &worklist,
			decision_grapht &g);
};


#endif
 
