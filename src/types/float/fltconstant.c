/*
 * Copyright 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2012 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/types/float/fltconstant.h>

#include <stdio.h>
#include <string.h>

#include <jive/types/float/fltoperation-classes.h>
#include <jive/types/float/flttype.h>
#include <jive/util/buffer.h>
#include <jive/vsdg/graph.h>
#include <jive/vsdg/operators/nullary.h>

namespace jive {
namespace base {
// explicit instantiation
template class domain_const_op<
	flt::type, flt::value_repr, flt::format_value, flt::type_of_value
>;
}
}

jive::oport *
jive_fltconstant(jive::region * region, jive::flt::value_repr value)
{
	jive::flt::constant_op op(value);
	return jive_node_create_normalized(region, op, {})[0];
}
