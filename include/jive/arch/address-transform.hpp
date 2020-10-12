/*
 * Copyright 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_ARCH_ADDRESS_TRANSFORM_HPP
#define JIVE_ARCH_ADDRESS_TRANSFORM_HPP

#include <jive/arch/address.hpp>
#include <jive/arch/call.hpp>
#include <jive/arch/load.hpp>
#include <jive/arch/store.hpp>
#include <jive/common.hpp>
#include <jive/rvsdg/graph.hpp>
#include <jive/rvsdg/unary.hpp>
#include <jive/types/function.hpp>

namespace jive {

/* addr2bit operation */

class addr2bit_op final : public jive::unary_op {
public:
	virtual
	~addr2bit_op() noexcept;

private:
	addr2bit_op(size_t nbits, const jive::type & type);

	addr2bit_op(addr2bit_op &&) = default;

	addr2bit_op(const addr2bit_op &) = default;

public:
	virtual bool
	operator==(const operation & other) const noexcept override;

	virtual std::string
	debug_string() const override;

	virtual jive_unop_reduction_path_t
	can_reduce_operand(
		const jive::output * arg) const noexcept override;

	virtual jive::output *
	reduce_operand(
		jive_unop_reduction_path_t path,
		jive::output * arg) const override;

	inline size_t
	nbits() const noexcept
	{
		return nbits_;
	}

	inline const jive::type &
	original_type() const noexcept
	{
		return argument(0).type();
	}

	virtual std::unique_ptr<jive::operation>
	copy() const override;

	static inline jive::output *
	create(jive::output * operand, size_t nbits, const jive::type & type)
	{
		addr2bit_op op(nbits, type);
		return simple_node::create_normalized(operand->region(), op, {operand})[0];
	}

private:
	size_t nbits_;
};

/* bit2addr operator */

class bit2addr_op final : public jive::unary_op {
public:
	virtual
	~bit2addr_op() noexcept;

private:
	bit2addr_op(
		size_t nbits,
		const jive::type & type);

	bit2addr_op(bit2addr_op &&) = default;

	bit2addr_op(const bit2addr_op &) = default;

public:
	virtual bool
	operator==(const operation & other) const noexcept override;

	virtual std::string
	debug_string() const override;

	virtual jive_unop_reduction_path_t
	can_reduce_operand(
		const jive::output * arg) const noexcept override;

	virtual jive::output *
	reduce_operand(
		jive_unop_reduction_path_t path,
		jive::output * arg) const override;

	inline size_t
	nbits() const noexcept
	{
		return nbits_;
	}

	inline const jive::type &
	original_type() const noexcept
	{
		return result(0).type();
	}

	virtual std::unique_ptr<jive::operation>
	copy() const override;

	static inline jive::output *
	create(jive::output * operand, size_t nbits, const jive::type & type)
	{
		bit2addr_op op(nbits, type);
		return simple_node::create_normalized(operand->region(), op, {operand})[0];
	}

private:
	size_t nbits_;
};

/* reductions */

class memlayout_mapper;

void
transform_address(jive::node * node, memlayout_mapper & mapper);

void
transform_address(jive::graph * graph, memlayout_mapper & mapper);

}

#endif
