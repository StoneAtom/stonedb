/*
   Copyright (c) 2014, 2021, Oracle and/or its affiliates.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License, version 2.0,
   as published by the Free Software Foundation.

   This program is also distributed with certain software (including
   but not limited to OpenSSL) that is licensed under separate terms,
   as designated in a particular file or component or in included license
   documentation.  The authors of MySQL hereby grant you an additional
   permission to link the program and your derivative works with the
   separately licensed software that they have included with MySQL.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License, version 2.0, for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "client_priv.h"
#include "my_default.h"
#include <sstream>
#include "help_options.h"
#include "abstract_program.h"

#include <welcome_copyright_notice.h> /* ORACLE_WELCOME_COPYRIGHT_NOTICE */

using namespace Mysql::Tools::Base::Options;
using Mysql::Tools::Base::Abstract_program;
using std::string;

extern const char *load_default_groups[];

Help_options::Help_options(Abstract_program *program)
  : m_program(program)
{}

void Help_options::create_options()
{
  this->create_new_option("help", "Display this help message and exit.")
    ->set_short_character('?')
    ->add_callback(new Instance_callback<void, char*, Help_options>(
    this, &Help_options::help_callback));

  this->create_new_option("version", "Output version information and exit.")
    ->set_short_character('V')
    ->add_callback(new Instance_callback<void, char*, Help_options>(
    this, &Help_options::version_callback));
}

void Help_options::help_callback(char* argument MY_ATTRIBUTE((unused)))
{
  this->print_usage();
  exit(0);
}

void Help_options::version_callback(char* argument MY_ATTRIBUTE((unused)))
{
  this->print_version_line();
  exit(0);
}


/** A helper function. Prints the program version line. */
void Help_options::print_version_line()
{
  printf("%s  Ver %s Distrib %s, for %s (%s)\n",
         this->m_program->get_name().c_str(),
         this->m_program->get_version().c_str(),
         MYSQL_SERVER_VERSION, SYSTEM_TYPE, MACHINE_TYPE);
}


void Mysql::Tools::Base::Options::Help_options::print_usage()
{

  this->print_version_line();

  std::ostringstream s;
  s << m_program->get_first_release_year();
  string first_year_str(s.str());
  string copyright;

  if (first_year_str == COPYRIGHT_NOTICE_CURRENT_YEAR)
  {
    copyright= ORACLE_WELCOME_COPYRIGHT_NOTICE("2021");
  }
  else
  {
#define FIRST_YEAR_CONSTANT "$first_year$"
    string first_year_constant_str= FIRST_YEAR_CONSTANT;

    copyright= ORACLE_WELCOME_COPYRIGHT_NOTICE("2021");
    copyright= copyright.replace(copyright.find(first_year_constant_str),
                                 first_year_constant_str.length(), first_year_str);
  }

  printf("%s\n%s\n",
         copyright.c_str(),
         this->m_program->get_description().c_str());
  /*
    Turn default for zombies off so that the help on how to 
    turn them off text won't show up.
    This is safe to do since it's followed by a call to exit().
   */
  for (struct my_option *optp= this->m_program->get_options_array();
       optp->name; optp++)
  {
    if (!strcmp(optp->name, "secure-auth"))
    {
      optp->def_value= 0;
      break;
    }
  }
  this->m_program->short_usage();
  print_defaults("my", load_default_groups);
  my_print_help(this->m_program->get_options_array());
  my_print_variables(this->m_program->get_options_array());
}
