/*
 * Copyright 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2012 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/types/float/fltconstant.hpp>

#include <stdio.h>
#include <string.h>

#include <jive/rvsdg/graph.hpp>
#include <jive/types/float/fltoperation-classes.hpp>
#include <jive/types/float/flttype.hpp>

namespace jive {

// explicit instantiation
template class domain_const_op<flt::type, flt::value_repr, flt::format_value, flt::type_of_value>;

}

jive::output *
jive_fltconstant(jive::region * region, jive::flt::value_repr value)
{
	jive::flt::constant_op op(value);
	return jive::simple_node::create_normalized(region, op, {})[0];
}
