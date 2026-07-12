# Proposal

## Why This Repository Exists

Every robotics team writes firmware.

Over time, every team also writes the same firmware again.

One person writes a driver.

Another person later writes same driver.

Even when the problem has already been solved, we often solve it again because the previous solution was never designed to be reused.

Eventually, the people who originally wrote the software graduate, projects change, and much of that experience disappears with them.

The next generation starts over.

Not because they have to.

Because there is nothing reliable to build upon.

This repository is an attempt to change that.

---

# The Problem

Every project teaches us something.

Sometimes we discover a bug.

Sometimes we discover a better design.

Sometimes we discover a limitation that only appears after months of use.

Unfortunately, those lessons usually stay inside a single project.

The next project repeats many of the same mistakes.

The project after that repeats them again.

Over time, we become better engineers, but our software often starts from the same place every year.

That means our experience grows much faster than our codebase.

---

# The Proposal

Instead of treating every project as a completely new beginning, we build a common collection of software that improves continuously.

Whenever we solve a problem well, we preserve that solution.

Whenever we discover a limitation, we document it.

Whenever someone improves a module, the improvement becomes available for every future project.

The goal is not to build software that is perfect.

The goal is to make every version slightly better than the last.

---

# Why Build Our Own?

There are many open-source libraries available today.

Many of them are excellent.

Many are also written for different hardware, different constraints, or different goals.

They often solve today's problem, but they do not always become tomorrow's foundation.

Building our own software gives us something equally important:

- We understand how it works.
- We understand why it was designed that way.
- We know its limitations.
- We can improve it whenever necessary.
- We are not dependent on someone else's decisions.

When appropriate, existing open-source work should absolutely be studied and used as inspiration.

But the software we depend on every season should also be software we understand and can confidently maintain.

---

# Continuous Improvement

No module in this repository should ever be considered "finished."

Every contribution should leave it in a better state than before.

Sometimes that improvement will be a new feature.

Sometimes it will be a bug fix.

Sometimes it will simply be better documentation.

Sometimes it will be removing unnecessary complexity.

Even small improvements matter.

A repository that improves by one percent every season becomes far more valuable than one that is rewritten from scratch every season.

---

# Documentation Matters

Every module should describe:

- What it does.
- How it should be used.
- Known limitations.
- Assumptions.
- Future improvements.

Understanding the limitations of a module is often as important as understanding its capabilities.

This allows future contributors to continue improving the software instead of rediscovering the same problems.

---

# Long-Term Vision

The purpose of this repository is not to support a single robot.

It is to create a growing collection of software that future projects can confidently build upon.

As the repository matures, new projects should spend less time rewriting existing software and more time solving new engineering problems.

Success is not measured by how much code this repository contains.

Success is measured by how little code future teams need to write again.