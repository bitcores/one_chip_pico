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

romdata = list()
for b in inp[16:]:
    b = int.from_bytes(b)
    romdata.append(b)

start = "uint8_t prg_data[] = {"
f = open("prg_data.h", "w")
f.write(start)
first = True


for i in romdata:
    if not first:
        f.write(", ")
    else:
        first = False
    f.write(hex(i))

# if the source is a 16k ROM, duplicate it to 32k
# this might not work correctly in all cases
if len(romdata) < 32_000:
    for i in romdata:
        f.write(", "+hex(i))
    
f.write("};")
    
f.close()


start = "uint8_t prg_cmp[] = {"
f = open("prg_cmp.h", "w")
f.write(start)
first = True
cb = None
cnt = 0
rem = None
tictok = 0

def writetof():
    global first, cb, rem, cnt
    if not first:
        f.write(", ")
    else:
        first = False
    
    if rem == None:
        rem = cnt
        f.write(hex(cb))

    else:
        rem = rem << 4
        rem += cb >> 4
        cb = cb & 15
        cb = cb << 4
        cb += cnt
        f.write(hex(rem)+", ")
        f.write(hex(cb))

        rem = None

def compressPRG():
    global cb, rem, cnt
    for i in romdata:
        if cb == None:
            cb = i
            continue

        if i == cb:
            cnt += 1

            if cnt < 15:
                continue
            else:
                # max repetitions, write out anyway
                writetof()
                cb = None
                cnt = 0
                           
        else:
            # write one or two bytes
            writetof()
            cb = i
            cnt = 0

compressPRG()

if len(romdata) < 32000:
    compressPRG()

if not cb == None:
    writetof()

if not rem == None:
    rem = rem << 4
    f.write(", "+hex(rem))
    
f.write("};")
    
f.close()

