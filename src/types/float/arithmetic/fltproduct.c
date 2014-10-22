/*
 * Copyright 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2012 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/types/float/arithmetic/fltproduct.h>

#include <jive/types/float/flttype.h>
#include <jive/vsdg/graph.h>
#include <jive/vsdg/node-private.h>

namespace jive {
namespace flt {

const char fltproduct_name[] = "FLTPRODUCT";

}
}

jive::output *
jive_fltproduct(jive::output * arg1, jive::output * arg2)
{
	return jive::flt::mul_op::normalized_create(arg1, arg2);
}
