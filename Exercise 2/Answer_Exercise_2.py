# PART A

clock_periods = [1.0, 1.2, 1.5, 2.0]

print(clock_periods)  
# You originally did not print the entire list as required.
# The exercise explicitly asked to print the full list.

for clk in clock_periods:
    print(clk)
# Using 'clk' instead of 'i' improves readability.
# 'i' is usually used for index counters, not meaningful values.

print(clock_periods[0])
print(clock_periods[-1])
# Correct usage. Using -1 for last element is good Python practice.


# PART B

for clk in clock_periods:
    frequency = 1 / clk
    wns = 0.1 - (0.2 * clk)
    timing_clean = wns >= 0
    # In your version, logic was correct.
    # However, you did not print the clock value itself.
    # That makes output unclear when multiple configs run.

    print(f"Clock: {clk}")
    print(f"Frequency: {frequency:.3f}")
    # Using :.3f limits floating precision.
    # Without formatting, Python may print long decimal values.

    print(f"WNS: {wns:.3f}")
    print(f"Timing Clean: {timing_clean}")
    print("-" * 30)
    # Separator improves readability in multi-config output.


# PART C

results = []

for clk in clock_periods:

    wns = 0.1 - (0.2 * clk)
    # In your version, you did NOT recompute wns here.
    # You reused the variable from the previous loop.
    # That causes a logic bug because wns keeps its last value.

    result = {
        "clock": clk,
        "frequency": 1 / clk,
        "wns": wns,
        "timing_clean": wns >= 0
        # In your version:
        # "timing_clean": (wns >= 0)
        # was referencing old wns from Part B loop.
        # That would incorrectly assign the same timing result
        # to all configurations.
    }

    results.append(result)
    # Your append usage was correct.


for r in results:
    print(r)
# In your version you used 'for i in results'.
# That's fine, but meaningful names improve clarity.


# PART D

fail_count = 0

for r in results:

    if not r["timing_clean"]:
        fail_count += 1

    # Your version had multiple issues:
    #
    # 1. You wrote:
    #    if my_dict[timing_clean] == False:
    #
    # 2. You used 'my_dict' instead of loop variable.
    #    'my_dict' refers only to the last dictionary created.
    #
    # 3. You forgot quotes around "timing_clean".
    #    Dictionary keys must be accessed as strings.
    #
    # 4. You used timing_clean without defining it in this scope.
    #
    # Correct access pattern:
    # r["timing_clean"]

print(f"Total configs: {len(results)}")
print(f"Failures: {fail_count}")
