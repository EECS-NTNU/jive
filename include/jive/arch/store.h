/*
 * Copyright 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 2013 2014 2015 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_ARCH_STORE_H
#define JIVE_ARCH_STORE_H

#include <algorithm>
#include <memory>

#include <jive/util/ptr-collection.h>
#include <jive/vsdg/node-normal-form.h>
#include <jive/vsdg/node.h>

namespace jive {

class store_op final : public simple_op {
public:
	virtual
	~store_op() noexcept;

	template<typename Types>
	inline
	store_op(
		const jive::value::type & address_type,
		const Types & state_types,
		const jive::value::type & data_type)
		: value_(data_type)
		, address_(address_type)
	{
		for (const auto & type : state_types)
			states_.push_back({std::move(type->copy())});
	}

	store_op(const store_op & other) = default;

	inline
	store_op(store_op && other) noexcept = default;

	virtual bool
	operator==(const operation & other) const noexcept override;

	virtual size_t
	narguments() const noexcept override;

	virtual const jive::port &
	argument(size_t index) const noexcept override;

	virtual size_t
	nresults() const noexcept override;

	virtual const jive::port &
	result(size_t index) const noexcept override;

	virtual std::string
	debug_string() const override;

	inline const jive::value::type &
	address_type() const noexcept
	{
		return *static_cast<const value::type*>(&address_.type());
	}

	inline const jive::value::type &
	data_type() const noexcept
	{
		return *static_cast<const value::type*>(&value_.type());
	}

	virtual std::unique_ptr<jive::operation>
	copy() const override;

private:
	jive::port value_;
	jive::port address_;
	std::vector<jive::port> states_;
};

}

std::vector<jive::output*>
jive_store_by_address_create(jive::output * address,
	const jive::value::type * datatype, jive::output * value,
	size_t nstates, jive::output * const states[]);

std::vector<jive::output*>
jive_store_by_bitstring_create(jive::output * address, size_t nbits,
	const jive::value::type * datatype, jive::output * value,
	size_t nstates, jive::output * const istates[]);

#endif
