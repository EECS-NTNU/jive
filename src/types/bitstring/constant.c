/*
 * Copyright 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/types/bitstring/constant.h>

#include <string.h>

#include <jive/types/bitstring/type.h>
#include <jive/util/buffer.h>
#include <jive/vsdg/graph.h>
#include <jive/vsdg/operators.h>
#include <jive/vsdg/region.h>
#include <jive/vsdg/simple_node.h>

namespace jive {
namespace base {
// explicit instantiation
template class domain_const_op<
	bits::type, bits::value_repr, bits::format_value, bits::type_of_value
>;
}
}

jive::oport *
jive_bitconstant(jive::region * region, size_t nbits, const char bits[])
{
	jive::bits::constant_op op(jive::bits::value_repr(std::string(bits, nbits).c_str()));
	return jive::create_normalized(region, op, {})[0];
}

jive::oport *
jive_bitconstant_unsigned(jive::region * region, size_t nbits, uint64_t value)
{
	jive::bits::constant_op op(jive::bits::value_repr(nbits, value));
	return jive::create_normalized(region, op, {})[0];
}

jive::oport *
jive_bitconstant_signed(jive::region * region, size_t nbits, int64_t value)
{
	jive::bits::constant_op op(jive::bits::value_repr(nbits, value));
	return jive::create_normalized(region, op, {})[0];
}
