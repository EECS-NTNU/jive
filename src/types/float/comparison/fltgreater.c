/*
 * Copyright 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2012 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/types/float/comparison/fltgreater.h>

#include <jive/types/float/flttype.h>
#include <jive/vsdg/controltype.h>
#include <jive/vsdg/graph.h>
#include <jive/vsdg/node-private.h>

namespace jive {
namespace flt {

const char fltgreater_name[] = "FLTGREATER";

}
}

jive::oport *
jive_fltgreater(jive::oport * arg1, jive::oport * arg2)
{
	return jive::flt::gt_op::normalized_create(arg1, arg2);
}
