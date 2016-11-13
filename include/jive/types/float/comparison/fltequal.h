/*
 * Copyright 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2012 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_TYPES_FLOAT_COMPARISON_FLTEQUAL_H
#define JIVE_TYPES_FLOAT_COMPARISON_FLTEQUAL_H

#include <jive/types/float/fltoperation-classes.h>

namespace jive {
namespace flt {

extern const char fltequal_name[];

typedef detail::make_cmpop<
	std::equal_to<value_repr>,
	fltequal_name,
	jive_binary_operation_commutative> eq_op;

}
}

jive::oport *
jive_fltequal(jive::oport * operand1, jive::oport * operand2);

#endif
