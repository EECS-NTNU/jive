/*
 * Copyright 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2012 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/types/float/comparison/fltlesseq.h>

#include <jive/types/float/flttype.h>
#include <jive/vsdg/controltype.h>
#include <jive/vsdg/graph.h>
#include <jive/vsdg/node-private.h>

namespace jive {
namespace flt {

const char fltlesseq_name[] = "FLTLESSEQ";

}
}

jive::oport *
jive_fltlesseq(jive::oport * arg1, jive::oport * arg2)
{
	return jive::flt::le_op::normalized_create(arg1, arg2);
}
