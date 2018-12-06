import bn256
    
msg="hello"

# Get public key and random private key
priv,pub = bn256.g2_random()

print "Message:\t",msg
print "\n======================"
print "Private key:\t",priv
print "Public key:\t",pub


# Get signature
pt = bn256.g1_hash_to_point(msg)

assert pt.is_on_curve()
sig=bn256.g1_compress(pt.scalar_mul(priv))

print "\n======================"
print "\nPoint for message (Pt):\t",pt
print "Signature (p x Pt): ",sig


# Verify signature

unsig = bn256.g1_uncompress(sig)

assert type(pub) == bn256.curve_twist
assert type(unsig) == bn256.curve_point

msg_pt = bn256.g1_hash_to_point(msg)

assert msg_pt.is_on_curve()

rtn1 = bn256.optimal_ate(pub, msg_pt)
rtn2 = bn256.optimal_ate(bn256.twist_G, unsig)

if (rtn1 == rtn2):
	print "\nSignature verified"
else:
	print "\nSignature not verified"
