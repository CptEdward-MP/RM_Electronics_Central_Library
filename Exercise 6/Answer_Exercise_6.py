tests = [
    ("add_test", 120, "PASS"),
    ("sub_test", 135, "PASS"),
    ("branch_test", 180, "FAIL"),
    ("load_test", 150, "PASS"),
    ("hazard_test", 210, "FAIL")
]

# --------- Generate Log ---------
with open("simulation.log", "w") as f:
    for test_name, cycles, result in tests:
        f.write(f"Running test: {test_name}\n")
        f.write(f"RESULT: {result}\n")
        f.write(f"Cycles: {cycles}\n")
        f.write("\n")


# --------- Parse Log ---------
with open("simulation.log", "r") as f:

    test_count = 0
    pass_count = 0
    fail_count = 0
    failed_tests = []
    test_names = []
    cycles_list = []

    current_test_name = None  # ← Needed to associate FAIL with correct test

    for line in f:
        line = line.strip()

        # ❌ You incremented test_count for every line
        # Correct: increment only when new test starts
        if line.startswith("Running test:"):
            parts = line.split(":")
            current_test_name = parts[1].strip()
            test_names.append(current_test_name)
            test_count += 1   # ← Correct location

        elif line.startswith("RESULT:"):
            parts = line.split(":")
            value = parts[1].strip()  # ❌ You used wrong variable earlier
            
            if value == "PASS":
                pass_count += 1
            elif value == "FAIL":
                fail_count += 1
                failed_tests.append(current_test_name)  # ❌ You appended "FAIL"

        elif line.startswith("Cycles:"):
            parts = line.split(":")
            
            # ❌ You stored cycles as string
            # Must convert to int
            value = int(parts[1].strip())
            cycles_list.append(value)

    # Defensive handling
    if cycles_list:
        total_cycles = sum(cycles_list)

        # ❌ You divided by test_count which was wrong earlier
        average_cycles = total_cycles / len(cycles_list)

        max_cycles = max(cycles_list)

        # Longest test using index
        index_longest_test = cycles_list.index(max_cycles)
        longest_test = test_names[index_longest_test]

    else:
        total_cycles = 0
        average_cycles = 0
        max_cycles = 0
        longest_test = None


    print(f"Total tests: {test_count}")
    print(f"PASS: {pass_count}")
    print(f"FAIL: {fail_count}")
    print(f"Failed tests: {', '.join(failed_tests)}")
    print(f"Total cycles: {total_cycles}")
    print(f"Average cycles: {average_cycles:.2f}")
    print(f"Longest test: {longest_test}")
    print(f"Max cycles: {max_cycles}")
