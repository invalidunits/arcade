import struct
f = None

file_path = input("Highscore file:")

try:
    f = open(file_path, "xb")
except FileExistsError: 
    f = open(file_path, "wb")

while True:
    user = input("What is the username for the score? ").ljust(5)
    score = int(input("What is the score? "))        
    packed_score = struct.pack('>L', score)
    print("Packed score into: ", str(packed_score))
    
    
    f.write(user.encode("ascii"))
    f.write(packed_score)
    
    
    
    if (input("Exit (y/n)") == "y"):
        break
f.flush()
f.close()