import web3
from web3.contract import ConciseContract
from web3 import Web3, HTTPProvider
import json

web3 = Web3(HTTPProvider('http://localhost:9545'))

# Get the address and ABI of the contract

f = open('build/contracts/Verifier.json')
contractdef = json.load(f)
abi=contractdef['abi']

p = open('proof.json')
proofandinput = json.load(p)
proof = proofandinput['proof']
inp = proofandinput['inputs']
A = list(map(lambda x: web3.toInt(hexstr=x), proof['a']))
B0 = list(map(lambda x: web3.toInt(hexstr=x), proof['b'][0]))
B1 = list(map(lambda x: web3.toInt(hexstr=x), proof['b'][1]))
C = list(map(lambda x: web3.toInt(hexstr=x), proof['c']))
I = list(map(lambda x: web3.toInt(hexstr=x), inp))
w,h = 2,2
B = [[0 for x in range(w)] for y in range(h)]
B[0] = B0
B[1] = B1

address = contractdef["networks"][web3.version.network]['address']
# Create a contract object
contract = web3.eth.contract(
    address,
    abi=abi,
    ContractFactoryClass=ConciseContract,
)

txhash = contract.verifyTx(A, B, C, I)

# proofninputs = artifacts.require("proof.json");
# Verifier.deployed().then(function(instance){return instance.verifyTx(proofninputs.proof.a, proofninputs.proof.b, proofninputs.proof.c, proofninputs.inputs);});
