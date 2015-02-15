/*
 * Copyright 2010 2011 2012 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2014 2015 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_VSDG_CONTROLTYPE_H
#define JIVE_VSDG_CONTROLTYPE_H

#include <jive/vsdg/statetype.h>

namespace jive {
namespace ctl {

class type final : public jive::state::type {
public:
	virtual ~type() noexcept;

	type(size_t nalternatives);

	virtual std::string debug_string() const override;

	virtual bool operator==(const jive::base::type & other) const noexcept override;

	virtual jive::ctl::type * copy() const override;

	virtual jive::gate * create_gate(jive_graph * graph, const char * name) const override;

	inline size_t
	nalternatives() const noexcept
	{
		return nalternatives_;
	}

private:
	size_t nalternatives_;
};

class gate final : public jive::state::gate {
public:
	virtual ~gate() noexcept;

	gate(size_t nalternatives, jive_graph * graph, const char name[]);

private:
	gate(const gate & rhs) = delete;
	gate& operator=(const gate & rhs) = delete;

	jive::ctl::type type_;
};

class value_repr {
public:
	value_repr(size_t alternative, size_t nalternatives);

	inline bool
	operator==(const jive::ctl::value_repr & other) const noexcept
	{
		return alternative_ == other.alternative_ && nalternatives_ == other.nalternatives_;
	}

	inline bool
	operator!=(const jive::ctl::value_repr & other) const noexcept
	{
		return !(*this == other);
	}

	inline size_t
	alternative() const noexcept
	{
		return alternative_;
	}

	inline size_t
	nalternatives() const noexcept
	{
		return nalternatives_;
	}

private:
	size_t alternative_;
	size_t nalternatives_;
};

const type boolean(2);

}
}

#endif
