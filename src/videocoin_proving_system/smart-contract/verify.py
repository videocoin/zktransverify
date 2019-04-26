import os
import sys, getopt
import re
import subprocess
import web3
from web3.contract import ConciseContract
from web3 import Web3, HTTPProvider
import json

web3 = Web3(HTTPProvider('http://localhost:7545'))

# parse arguments
srcfile = "-"
transfile = "-"

proof_file = "../temp/proof.json"
verifier_abi_n_addr_file = 'build/contracts/Verifier.json'  # to be moved to current folder after debug
# verifier_abi_n_addr_file = 'Verifier.json'
exec_genwitness = "../build/genwitness"
exec_genproof = "../build/genwitness"
winess_file = "mywitness"

genproof = False
argv = sys.argv[1:]

#
# Parse options
#
try:
    opts, args = getopt.getopt(argv, "gs:t:", ["srcfile=", "transfile="])
except getopt.GetoptError as err:
    # print str(err)  # will print something like "option -a not recognized"
    usage()
    sys.exit(2)

for opt, arg in opts:
    if opt in ("-s", "--srcfile"):
        srcfile = arg
    elif opt in ("-t", "--transfile"):
        transfile = arg
    elif opt in "-g":
        genproof = True

#
# Generate witness
#
if genproof:
    if srcfile == "-" or transfile == "-":
        print("usage: verify -s srcfile -t transfile")
        sys.exit(2)
    else:
        try:
            print("Starting " + exec_genwitness + " " + srcfile + " " + transfile)
            subprocess.run([exec_genwitness, srcfile, transfile])
        except IOError as e:
            print("Couldn't open or read file (%s)." % e)
            sys.exit(2)
    #
    # Format witness
    #       
    process = subprocess.Popen("zokrates compute-witness -a `cat mywitness`", shell=True)
    process.communicate()

    #
    # Generate proof
    #
    subprocess.run([exec_genproof, "generate-proof"])
#
# Initialize contract API
#

# Get the address and ABI of the contract
try:
    with open(verifier_abi_n_addr_file) as f:
        contractdef = json.load(f)
        abi = contractdef['abi']
except IOError as e:
    print("Couldn't open or read file (%s)." % e)
    sys.exit(2)
try:
    with open(proof_file) as p:
        proofandinput = json.load(p)
        proof = proofandinput['proof']
        inp = proofandinput['inputs']
except IOError as e:
    print("Couldn't open or read file (%s)." % e)
    sys.exit(2)

A_g = list(map(lambda x: web3.toInt(hexstr=x), proof['A_g']))
A_h = list(map(lambda x: web3.toInt(hexstr=x), proof['A_h']))
B_g0 = list(map(lambda x: web3.toInt(hexstr=x), proof['B_g'][0]))
B_g1 = list(map(lambda x: web3.toInt(hexstr=x), proof['B_g'][1]))
B_h = list(map(lambda x: web3.toInt(hexstr=x), proof['B_h']))
C_g = list(map(lambda x: web3.toInt(hexstr=x), proof['C_g']))
C_h = list(map(lambda x: web3.toInt(hexstr=x), proof['C_h']))
H = list(map(lambda x: web3.toInt(hexstr=x), proof['H']))
K = list(map(lambda x: web3.toInt(hexstr=x), proof['K']))
I = list(map(lambda x: web3.toInt(text=x), inp))

B_g = [B_g0, B_g1]

address = contractdef["networks"][web3.version.network]['address']
# Create a contract object
contract = web3.eth.contract(address, abi=abi)
# b
# Call contract
#

try:
    print("Calling contract verifyTx")
    txhash = contract.functions.verifyTx(A_g, A_h, B_g, B_h, C_g, C_h,
				H, K,
				I).transact()
    print(txhash)
    # Wait for transaction to be mined...
    web3.eth.waitForTransactionReceipt(txhash)
except IOError as e:
    print("Failed to call contract verifyTx (%s)." % e)
    sys.exit(2)

# proofninputs = artifacts.require("proof.json");
# Verifier.deployed().then(function(instance){return instance.verifyTx(proofninputs.proof.a, proofninputs.proof.b, proofninputs.proof.c, proofninputs.inputs);});
