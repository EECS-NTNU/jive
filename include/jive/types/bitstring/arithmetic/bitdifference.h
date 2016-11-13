/*
 * Copyright 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_TYPES_BITSTRING_ARITHMETIC_BITDIFFERENCE_H
#define JIVE_TYPES_BITSTRING_ARITHMETIC_BITDIFFERENCE_H

#include <jive/types/bitstring/bitoperation-classes.h>

namespace jive {
namespace bits {

class sub_op final : public binary_op {
public:
	virtual ~sub_op() noexcept;

	inline sub_op(const jive::bits::type & type) noexcept
		: binary_op(type)
	{}

	virtual bool
	operator==(const operation & other) const noexcept override;
	virtual jive_binary_operation_flags
	flags() const noexcept override;

	virtual value_repr
	reduce_constants(
		const value_repr & arg1,
		const value_repr & arg2) const override;

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<jive::operation>
	copy() const override;
};

}
}

jive::oport *
jive_bitdifference(jive::oport * op1, jive::oport * op2);

#endif
