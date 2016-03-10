/*******************************************************************\

Module: Implication graph implementation

Author: Rajdeep Mukherjee, Peter Schrammel

\*******************************************************************/

#ifndef CPROVER_ACDL_IMPLICATION_GRAPH_H
#define CPROVER_ACDL_IMPLICATION_GRAPH_H

#include <util/graph.h>
#include "acdl_domain.h"

class acdl_implication_graph_nodet : public graph_nodet<empty_edget>
{
public:
  bool is_decision;
  unsigned level;
  bool conflict;
  acdl_domaint::meet_irreduciblet expr;
};

class acdl_implication_grapht : public graph<acdl_implication_graph_nodet>
{
public:  
  explicit acdl_implication_grapht()
    :
    current_level(0)
  {}
  
  unsigned current_level;
  void first_uip(nodest &cut);
  void add_deductions(const acdl_domaint::deductionst &m_ir);
  void add_deduction(const acdl_domaint::deductiont &m_ir);
  void add_decision(const acdl_domaint::meet_irreduciblet & m_ir);
  void print_graph_output();
  void output_graph(std::ostream &out) const;
  void output_graph_node(std::ostream &out, node_indext n) const; 

  void to_value(acdl_domaint::valuet &value) const;
  void remove_in_edges(node_indext n);
  void remove_out_edges(node_indext n);
  int graph_size();  
  acdl_implication_graph_nodet::node_indext find_node(const exprt &expr);
  void delete_graph_nodes(); 
protected:


};

#endif