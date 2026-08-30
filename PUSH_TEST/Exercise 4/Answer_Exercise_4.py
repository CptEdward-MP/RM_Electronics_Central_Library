clocks = [1.0, 1.2, 1.5, 2.0]

# ------------------------
# PART A – Generate Report
# ------------------------

with open("timing_report.txt", "w") as f:
    
    # ❌ You forgot to loop over clocks
    # Without this, only one entry (and crash due to undefined 'clock')
    for clock in clocks:
        
        wns = 0.2 - (0.18 * clock)
        
        # Optional but cleaner: format to 3 decimal places
        f.write(f"Clock: {clock}\n")
        f.write(f"WNS: {wns}\n")
        f.write("\n")  # Correct blank line


# ------------------------
# PART B – Parse Report
# ------------------------

with open("timing_report.txt", "r") as f:
    
    clock_count = 0
    pass_count = 0
    fail_count = 0
    wns_values = []
    
    for line in f:
        line = line.strip()
        
        if line.startswith("Clock:"):
            clock_count += 1
            
        if line.startswith("WNS:"):
            parts = line.split(":")
            value = float(parts[1].strip())
            
            # ❌ Your inline if syntax was invalid Python
            # Python does not allow: if (cond) statement else statement
            if value >= 0:
                pass_count += 1
            else:
                fail_count += 1
            
            wns_values.append(value)
    
    
    # ❌ If file had no WNS lines, worst_wns would be undefined
    if wns_values:
        worst_wns = min(wns_values)
    else:
        worst_wns = None   # Defensive programming
    
    
    print(f"Total clocks analyzed: {clock_count}")
    print(f"Worst WNS: {worst_wns}")
    print(f"PASS count: {pass_count}")
    print(f"FAIL count: {fail_count}")
