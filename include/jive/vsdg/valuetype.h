/*
 * Copyright 2010 2011 2012 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 2014 2015 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_VSDG_VALUETYPE_H
#define JIVE_VSDG_VALUETYPE_H

#include <jive/vsdg/basetype.h>

namespace jive {
namespace value {

class type : public jive::base::type {
public:
	virtual ~type() noexcept;

	virtual jive::value::type * copy() const override = 0;

protected:
	inline constexpr type() noexcept : jive::base::type() {};
};

class gate : public jive::gate {
public:
	virtual ~gate() noexcept;

protected:
	gate(jive_graph * graph, const char name[], const jive::base::type & type);

private:
	gate(const gate & rhs) = delete;
	gate& operator=(const gate & rhs) = delete;
};

}
}

#endif
