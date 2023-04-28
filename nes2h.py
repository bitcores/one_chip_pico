inputfile = "PRG.nes"

def readinput():
    L = []
    fp = open(inputfile, "rb")
    while True:
        l = fp.read(1)
        if not l:
            break
        L.append(l)
    fp.close()
            
    return L

def splitinput():
    L = [i.split("\n") for i in open(inputfile).read().split("\n\n")]

    return L

inp = readinput()

start = "const uint8_t prg_data[] = {"
f = open("prg_data.h", "w")
f.write(start)
first = True


for i in inp[16:]:
    if not first:
        f.write(", ")
    else:
        first = False
    f.write("0x"+i.hex())

# if the source is a 16k ROM, duplicate it to 32k
# this might not work correctly in all cases
if len(inp) < 32_000:
    for i in inp[16:]:
        f.write(", 0x"+i.hex())
    
f.write("};")
    
f.close()

