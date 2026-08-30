import math

clocks = [1.0, 1.2, 1.5, 2.0]

# ------------------------
# PART A – Generate Report
# ------------------------

with open("timing_report.txt", "w") as f:

    for clock in clocks:
        
        wns = 0.2 - (0.18 * clock)
        
        # ⚠️ Better to control float precision while writing
        f.write(f"Clock: {clock}\n")
        f.write(f"WNS: {wns:.3f}\n")  # ← FIX: formatted to 3 decimals
        f.write("\n")


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
            
            if value >= 0:
                pass_count += 1
            else:
                fail_count += 1
            
            wns_values.append(value)
    

    # ⚠️ Defensive block must define ALL derived values
    if wns_values:
        worst_wns = min(wns_values)
        sum_wns = math.fsum(wns_values)
        average_wns = sum_wns / clock_count
        failure_percentage = (fail_count / clock_count) * 100
    else:
        worst_wns = 0.0
        average_wns = 0.0
        failure_percentage = 0.0


    print(f"Total clocks analyzed: {clock_count}")
    print(f"Worst WNS: {worst_wns:.3f}")
    
    # ❌ You previously printed worst_wns again by mistake
    print(f"Average WNS: {average_wns:.3f}")  # ← FIX
    
    print(f"PASS count: {pass_count}")
    print(f"FAIL count: {fail_count}")
    print(f"Failure percentage: {failure_percentage:.2f}%")
