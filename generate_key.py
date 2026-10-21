import rsa

public_key, private_key = rsa.newkeys(256)

print("PUBLIC KEY")
print(f"n = {str(hex(public_key.n))[2:]}")
print(f"e = {str(hex(public_key.e))[2:]}\n")

print("PRIVATE KEY")
print(f"d = {str(hex(private_key.d))[2:]}")
print(f"e = {str(hex(private_key.e))[2:]}")