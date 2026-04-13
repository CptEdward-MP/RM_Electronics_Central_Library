# PART A — Write structured report

clocks = [1.0, 1.2, 1.5, 2.0]
# In your version, you declared clocks inside the with block.
# That is not wrong, but better practice is to define data outside
# and keep file blocks focused only on file operations.

with open("timing_report.txt", "w") as f:
# You correctly used "w" mode and 'with'. That part was good.

    for clk in clocks:

        wns = 0.1 - (0.2 * clk)
        status = "PASS" if wns >= 0 else "FAIL"
        # Your conditional expression was correct.

        f.write(f"Clock: {clk}\n")
        # You originally wrote:
        # file.write(f"{result}\n")
        #
        # That printed the entire dictionary representation:
        # {'clock': 1.0, 'frequency': 1.0, ...}
        #
        # That is NOT structured report formatting.
        # In real scripting, output format must be explicitly controlled.

        f.write(f"Frequency: {1/clk:.3f}\n")
        # You previously stored frequency in dictionary but did not format.
        # Also, you did not control decimal precision.
        # :.3f ensures fixed 3 decimal places.

        f.write(f"WNS: {wns:.3f}\n")
        # Same formatting issue applies here.

        f.write(f"Status: {status}\n")
        # Previously, you embedded status inside dictionary output.
        # Now it is clearly formatted.

        f.write("\n")
        # The exercise required blank line separation.
        # Your previous version did not include structured blank lines.


# PART B — Read entire file

with open("timing_report.txt", "r") as f:
# In your version:
# with open("timing_report.txt", "r") as read:
#     content = f.read()
#
# You created file handle named 'read'
# but then tried to use variable 'f'.
# That causes NameError.
#
# File handle variable name must match usage.

    content = f.read()
    print(content)


# PART C — Count PASS and FAIL

pass_count = content.count("PASS")
fail_count = content.count("FAIL")
# Your counting logic was correct.
# This part was implemented properly.

print(f"Total PASS: {pass_count}")
print(f"Total FAIL: {fail_count}")
