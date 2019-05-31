import argparse
import os
import shutil
import json
import re


def generate(key):
    print('Generating constructor...')

    ic_len = len(re.findall('IC_', json.dumps(key)))

    ic_str = '_vk.IC_length = {};\n'.format(ic_len)
    for i in range(0, ic_len):
        k = 'IC_{}'.format(i)
        ic_str += '''
        _vk.IC[{}] = Pairing.G1Point(
            {},
            {}
        );'''.format(i, key[k][0], key[k][1])

    function_str = '''
        _vk.A = Pairing.G2Point(
            [{},
             {}],
            [{},
             {}]
        );
        _vk.B = Pairing.G1Point(
            {},
            {}
        );
        _vk.C = Pairing.G2Point(
            [{},
             {}],
            [{},
             {}]
        );
        _vk.gamma = Pairing.G2Point(
            [{},
             {}],
            [{},
             {}]
        );
        _vk.gammaBeta1 = Pairing.G1Point(
            {},
            {}
        );
        _vk.gammaBeta2 = Pairing.G2Point(
            [{},
             {}],
            [{},
             {}]
        );
        _vk.Z = Pairing.G2Point(
            [{},
             {}],
            [{},
             {}]);
        {}
    '''.format(key['A'][0][0], key['A'][0][1], key['A'][1][0], key['A'][1][1],
               key['B'][0], key['B'][1],
               key['C'][0][0], key['C'][0][1], key['C'][1][0], key['C'][1][1],
               key['gamma'][0][0], key['gamma'][0][1], key['gamma'][1][0], key['gamma'][1][1],
               key['gamma_beta_1'][0], key['gamma_beta_1'][1],
               key['gamma_beta_2'][0][0], key['gamma_beta_2'][0][1], key['gamma_beta_2'][1][0], key['gamma_beta_2'][1][1],
               key['Z'][0][0], key['Z'][0][1], key['Z'][1][0], key['Z'][1][1],
               ic_str)
    print(function_str)
    return function_str


def inject(code, file_name):
    with open(file_name) as contract_file:
        contract = contract_file.read()
        new_contract = contract.replace('/*init-key*/', code)
        print(new_contract)

    with open(file_name, 'w') as contract_file:
        contract_file.write(new_contract)


def main():
    parser = argparse.ArgumentParser(description='Generate zkSNARK based verifier smart contract')
    parser.add_argument('output', metavar='output', type=str, help='resulting smart contract')
    parser.add_argument('input', metavar='input', type=str, help='input smart contract template')
    parser.add_argument('key', metavar='key', type=str, help='verifier key')

    args = parser.parse_args()
    for f in [args.input, args.key]:
        if not os.path.isfile(f):
            print('File {} does not exist.'.format(f))
            parser.print_usage()
            exit(-1)

    os.remove(args.output)
    shutil.copyfile(args.input, args.output)

    with open(args.key) as key_file:
        print('Reading key...')
        key = json.load(key_file)
        print(json.dumps(key, indent=4))
        code = generate(key)
        inject(code, args.output)


if __name__ == '__main__':
    main()
