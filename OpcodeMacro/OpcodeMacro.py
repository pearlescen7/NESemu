
filename = "Opcodes.txt"

instructions = {}

with open(filename) as f:
	linecounter = 0
	for line in f:
		curr_ins = line.rstrip('\n').split(", ")

		if(len(curr_ins) != 4):
			print("Error on line {0}".format(linecounter))
		linecounter += 1

		curr_ins[0] = int(curr_ins[0], 16)
		instructions[curr_ins[0]] = curr_ins

out = open("output.txt", "w")
out.write("{")
for i in range(0, 256):
	if(i % 16 == 0):
		out.write("\n\t")
	if i in instructions:
		out.write(f"{{\"{instructions[i][1]}\", &nes6502::{instructions[i][1]}, &nes6502::{instructions[i][2]}, {instructions[i][3]}}}")
	else:
		out.write(f"{{\"???\", &nes6502::XXX, &nes6502::IMP, 8}}")
	if(i < 255):
		out.write(", ")
out.write("\n}")

seen = {}
for item in instructions:
    if item not in seen:
        out.write(f"void {instructions[item][1]}();")
        seen[item] = True
