#PART A
variables = {"design_name" : "alu_core", "clock_period" : 1.5, "area" : 125000, "wns" : -0.035}

for x,y in variables.items(): #forgot .()items so error came
    print(f"{x}: {y}")

    
#PART B
print("frequency", 1/variables["clock_period"])

#preferred way: 
#frequency = 1 / variables["clock_period"]
#print(f"Frequency: {frequency}")


#PART B
timing_clean = (variables["wns"] >= 0)
print(timing_clean)

#PART C
if timing_clean : 
    print("Timing Passed")
else: 
    print("Timing Failed")
    
    
#PART D
print(type(variables["design_name"]))
print(type(variables["clock_period"]))
print(type(variables["area"]))
print(type(timing_clean))