/*******************************************************************\

Module: Summary Checker using ACDL

Author: Peter Schrammel

\*******************************************************************/

#ifndef CPROVER_SUMMARY_CHECKER_ACDL_H
#define CPROVER_SUMMARY_CHECKER_ACDL_H

#include "summary_checker_base.h"

class summary_checker_acdlt:public summary_checker_baset
{
public:
  inline summary_checker_acdlt(optionst &_options):
    summary_checker_baset(_options)
  {
  }
  
  virtual resultt operator()(const goto_modelt &);

protected:

};

#endif
