/*
 * Copyright 2010 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2012 2013 2014 2015 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_TESTS_TESTNODES_H
#define JIVE_TESTS_TESTNODES_H

#include <memory>
#include <vector>

#include <jive/vsdg/node.h>

/* test node */

class test_operation final : public jive::operation {
public:
	virtual
	~test_operation() noexcept;

	test_operation(
		const std::vector<const jive::base::type*> & argument_types,
		const std::vector<const jive::base::type*> & result_types);

	test_operation(const test_operation & other);

	inline
	test_operation() noexcept {}

	inline
	test_operation(test_operation && other) noexcept = default;
	
	virtual bool
	operator==(const operation & other) const noexcept override;

	virtual size_t
	narguments() const noexcept override;

	virtual const jive::base::type &
	argument_type(size_t index) const noexcept override;

	virtual size_t
	nresults() const noexcept override;

	virtual const jive::base::type &
	result_type(size_t index) const noexcept override;
	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<jive::operation>
	copy() const override;

private:
	std::vector<std::unique_ptr<const jive::base::type>> argument_types_;
	std::vector<std::unique_ptr<const jive::base::type>> result_types_;
};

jive_node *
jive_test_node_create(
	jive_region * region,
	const std::vector<const jive::base::type*> & operand_types,
	const std::vector<jive::output*> & operands,
	const std::vector<const jive::base::type*> & result_types);

std::vector<jive::output*>
jive_test_node_create_normalized(
	jive_graph * graph,
	const std::vector<const jive::base::type*> & operand_types,
	const std::vector<jive::output*> & operands,
	const std::vector<const jive::base::type*> & result_types);

#endif
