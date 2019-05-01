// Code generated - DO NOT EDIT.
// This file is a generated binding and any manual changes will be lost.

package main

import (
	"math/big"
	"strings"

	ethereum "github.com/ethereum/go-ethereum"
	"github.com/ethereum/go-ethereum/accounts/abi"
	"github.com/ethereum/go-ethereum/accounts/abi/bind"
	"github.com/ethereum/go-ethereum/common"
	"github.com/ethereum/go-ethereum/core/types"
	"github.com/ethereum/go-ethereum/event"
)

// Reference imports to suppress errors if they are not otherwise used.
var (
	_ = big.NewInt
	_ = strings.NewReader
	_ = ethereum.NotFound
	_ = abi.U256
	_ = bind.Bind
	_ = common.Big1
	_ = types.BloomLookup
	_ = event.NewSubscription
)

// PairingABI is the input ABI used to generate the binding from.
const PairingABI = "[]"

// PairingBin is the compiled bytecode used for deploying new contracts.
const PairingBin = `0x604c602c600b82828239805160001a60731460008114601c57601e565bfe5b5030600052607381538281f30073000000000000000000000000000000000000000030146080604052600080fd00a165627a7a72305820a189f21f5d200dd7792b0089cc62adfa5ec9c59230e2f92ed8c277ff8497c52d0029`

// DeployPairing deploys a new Ethereum contract, binding an instance of Pairing to it.
func DeployPairing(auth *bind.TransactOpts, backend bind.ContractBackend) (common.Address, *types.Transaction, *Pairing, error) {
	parsed, err := abi.JSON(strings.NewReader(PairingABI))
	if err != nil {
		return common.Address{}, nil, nil, err
	}
	address, tx, contract, err := bind.DeployContract(auth, parsed, common.FromHex(PairingBin), backend)
	if err != nil {
		return common.Address{}, nil, nil, err
	}
	return address, tx, &Pairing{PairingCaller: PairingCaller{contract: contract}, PairingTransactor: PairingTransactor{contract: contract}, PairingFilterer: PairingFilterer{contract: contract}}, nil
}

// Pairing is an auto generated Go binding around an Ethereum contract.
type Pairing struct {
	PairingCaller     // Read-only binding to the contract
	PairingTransactor // Write-only binding to the contract
	PairingFilterer   // Log filterer for contract events
}

// PairingCaller is an auto generated read-only Go binding around an Ethereum contract.
type PairingCaller struct {
	contract *bind.BoundContract // Generic contract wrapper for the low level calls
}

// PairingTransactor is an auto generated write-only Go binding around an Ethereum contract.
type PairingTransactor struct {
	contract *bind.BoundContract // Generic contract wrapper for the low level calls
}

// PairingFilterer is an auto generated log filtering Go binding around an Ethereum contract events.
type PairingFilterer struct {
	contract *bind.BoundContract // Generic contract wrapper for the low level calls
}

// PairingSession is an auto generated Go binding around an Ethereum contract,
// with pre-set call and transact options.
type PairingSession struct {
	Contract     *Pairing          // Generic contract binding to set the session for
	CallOpts     bind.CallOpts     // Call options to use throughout this session
	TransactOpts bind.TransactOpts // Transaction auth options to use throughout this session
}

// PairingCallerSession is an auto generated read-only Go binding around an Ethereum contract,
// with pre-set call options.
type PairingCallerSession struct {
	Contract *PairingCaller // Generic contract caller binding to set the session for
	CallOpts bind.CallOpts  // Call options to use throughout this session
}

// PairingTransactorSession is an auto generated write-only Go binding around an Ethereum contract,
// with pre-set transact options.
type PairingTransactorSession struct {
	Contract     *PairingTransactor // Generic contract transactor binding to set the session for
	TransactOpts bind.TransactOpts  // Transaction auth options to use throughout this session
}

// PairingRaw is an auto generated low-level Go binding around an Ethereum contract.
type PairingRaw struct {
	Contract *Pairing // Generic contract binding to access the raw methods on
}

// PairingCallerRaw is an auto generated low-level read-only Go binding around an Ethereum contract.
type PairingCallerRaw struct {
	Contract *PairingCaller // Generic read-only contract binding to access the raw methods on
}

// PairingTransactorRaw is an auto generated low-level write-only Go binding around an Ethereum contract.
type PairingTransactorRaw struct {
	Contract *PairingTransactor // Generic write-only contract binding to access the raw methods on
}

// NewPairing creates a new instance of Pairing, bound to a specific deployed contract.
func NewPairing(address common.Address, backend bind.ContractBackend) (*Pairing, error) {
	contract, err := bindPairing(address, backend, backend, backend)
	if err != nil {
		return nil, err
	}
	return &Pairing{PairingCaller: PairingCaller{contract: contract}, PairingTransactor: PairingTransactor{contract: contract}, PairingFilterer: PairingFilterer{contract: contract}}, nil
}

// NewPairingCaller creates a new read-only instance of Pairing, bound to a specific deployed contract.
func NewPairingCaller(address common.Address, caller bind.ContractCaller) (*PairingCaller, error) {
	contract, err := bindPairing(address, caller, nil, nil)
	if err != nil {
		return nil, err
	}
	return &PairingCaller{contract: contract}, nil
}

// NewPairingTransactor creates a new write-only instance of Pairing, bound to a specific deployed contract.
func NewPairingTransactor(address common.Address, transactor bind.ContractTransactor) (*PairingTransactor, error) {
	contract, err := bindPairing(address, nil, transactor, nil)
	if err != nil {
		return nil, err
	}
	return &PairingTransactor{contract: contract}, nil
}

// NewPairingFilterer creates a new log filterer instance of Pairing, bound to a specific deployed contract.
func NewPairingFilterer(address common.Address, filterer bind.ContractFilterer) (*PairingFilterer, error) {
	contract, err := bindPairing(address, nil, nil, filterer)
	if err != nil {
		return nil, err
	}
	return &PairingFilterer{contract: contract}, nil
}

// bindPairing binds a generic wrapper to an already deployed contract.
func bindPairing(address common.Address, caller bind.ContractCaller, transactor bind.ContractTransactor, filterer bind.ContractFilterer) (*bind.BoundContract, error) {
	parsed, err := abi.JSON(strings.NewReader(PairingABI))
	if err != nil {
		return nil, err
	}
	return bind.NewBoundContract(address, parsed, caller, transactor, filterer), nil
}

// Call invokes the (constant) contract method with params as input values and
// sets the output to result. The result type might be a single field for simple
// returns, a slice of interfaces for anonymous returns and a struct for named
// returns.
func (_Pairing *PairingRaw) Call(opts *bind.CallOpts, result interface{}, method string, params ...interface{}) error {
	return _Pairing.Contract.PairingCaller.contract.Call(opts, result, method, params...)
}

// Transfer initiates a plain transaction to move funds to the contract, calling
// its default method if one is available.
func (_Pairing *PairingRaw) Transfer(opts *bind.TransactOpts) (*types.Transaction, error) {
	return _Pairing.Contract.PairingTransactor.contract.Transfer(opts)
}

// Transact invokes the (paid) contract method with params as input values.
func (_Pairing *PairingRaw) Transact(opts *bind.TransactOpts, method string, params ...interface{}) (*types.Transaction, error) {
	return _Pairing.Contract.PairingTransactor.contract.Transact(opts, method, params...)
}

// Call invokes the (constant) contract method with params as input values and
// sets the output to result. The result type might be a single field for simple
// returns, a slice of interfaces for anonymous returns and a struct for named
// returns.
func (_Pairing *PairingCallerRaw) Call(opts *bind.CallOpts, result interface{}, method string, params ...interface{}) error {
	return _Pairing.Contract.contract.Call(opts, result, method, params...)
}

// Transfer initiates a plain transaction to move funds to the contract, calling
// its default method if one is available.
func (_Pairing *PairingTransactorRaw) Transfer(opts *bind.TransactOpts) (*types.Transaction, error) {
	return _Pairing.Contract.contract.Transfer(opts)
}

// Transact invokes the (paid) contract method with params as input values.
func (_Pairing *PairingTransactorRaw) Transact(opts *bind.TransactOpts, method string, params ...interface{}) (*types.Transaction, error) {
	return _Pairing.Contract.contract.Transact(opts, method, params...)
}

// VerifierABI is the input ABI used to generate the binding from.
const VerifierABI = "[{\"constant\":false,\"inputs\":[{\"name\":\"a\",\"type\":\"uint256[2]\"},{\"name\":\"a_p\",\"type\":\"uint256[2]\"},{\"name\":\"b\",\"type\":\"uint256[2][2]\"},{\"name\":\"b_p\",\"type\":\"uint256[2]\"},{\"name\":\"c\",\"type\":\"uint256[2]\"},{\"name\":\"c_p\",\"type\":\"uint256[2]\"},{\"name\":\"h\",\"type\":\"uint256[2]\"},{\"name\":\"k\",\"type\":\"uint256[2]\"},{\"name\":\"input\",\"type\":\"uint256[]\"}],\"name\":\"verifySSIMTx\",\"outputs\":[{\"name\":\"r\",\"type\":\"bool\"}],\"payable\":false,\"stateMutability\":\"nonpayable\",\"type\":\"function\"},{\"constant\":false,\"inputs\":[{\"name\":\"a\",\"type\":\"uint256[2]\"},{\"name\":\"a_p\",\"type\":\"uint256[2]\"},{\"name\":\"b\",\"type\":\"uint256[2][2]\"},{\"name\":\"b_p\",\"type\":\"uint256[2]\"},{\"name\":\"c\",\"type\":\"uint256[2]\"},{\"name\":\"c_p\",\"type\":\"uint256[2]\"},{\"name\":\"h\",\"type\":\"uint256[2]\"},{\"name\":\"k\",\"type\":\"uint256[2]\"},{\"name\":\"input\",\"type\":\"uint256[]\"}],\"name\":\"verifyTx\",\"outputs\":[{\"name\":\"r\",\"type\":\"bool\"}],\"payable\":false,\"stateMutability\":\"nonpayable\",\"type\":\"function\"},{\"constant\":true,\"inputs\":[],\"name\":\"verifyingKeySet\",\"outputs\":[{\"name\":\"\",\"type\":\"bool\"}],\"payable\":false,\"stateMutability\":\"view\",\"type\":\"function\"},{\"constant\":false,\"inputs\":[{\"name\":\"A\",\"type\":\"uint256[2][2]\"},{\"name\":\"B\",\"type\":\"uint256[2]\"},{\"name\":\"C\",\"type\":\"uint256[2][2]\"},{\"name\":\"gamma\",\"type\":\"uint256[2][2]\"},{\"name\":\"gamma_beta_1\",\"type\":\"uint256[2]\"},{\"name\":\"gamma_beta_2\",\"type\":\"uint256[2][2]\"},{\"name\":\"Z\",\"type\":\"uint256[2][2]\"},{\"name\":\"IC\",\"type\":\"uint256[2][]\"}],\"name\":\"setVerifyingKey\",\"outputs\":[],\"payable\":false,\"stateMutability\":\"nonpayable\",\"type\":\"function\"},{\"anonymous\":false,\"inputs\":[{\"indexed\":false,\"name\":\"\",\"type\":\"string\"}],\"name\":\"Verified\",\"type\":\"event\"}]"

// VerifierBin is the compiled bytecode used for deploying new contracts.
const VerifierBin = `0x60806040526000805460ff1916905534801561001a57600080fd5b50611a208061002a6000396000f3006080604052600436106100615763ffffffff7c010000000000000000000000000000000000000000000000000000000060003504166380d3cb708114610066578063c8e6ba4d1461021f578063f5282dea146103c4578063fe22023b146103d9575b600080fd5b34801561007257600080fd5b5060408051808201825261020b9136916004916044919083906002908390839080828437505060408051808201825294979695818101959450925060029150839083908082843750506040805180820190915293969594608081019493509150600290506000835b828210156101135760408051808201825290808402860190600290839083908082843750505091835250506001909101906020016100da565b50506040805180820182529396959481810194935091506002908390839080828437505060408051808201825294979695818101959450925060029150839083908082843750506040805180820182529497969581810195945092506002915083908390808284375050604080518082018252949796958181019594509250600291508390839080828437505060408051808201825294979695818101959450925060029150839083908082843750506040805186358801803560208181028481018201909552818452979a999888019792965090820194509250829190850190849080828437509497506106619650505050505050565b604080519115158252519081900360200190f35b34801561022b57600080fd5b5060408051808201825261020b9136916004916044919083906002908390839080828437505060408051808201825294979695818101959450925060029150839083908082843750506040805180820190915293969594608081019493509150600290506000835b828210156102cc576040805180820182529080840286019060029083908390808284375050509183525050600190910190602001610293565b50506040805180820182529396959481810194935091506002908390839080828437505060408051808201825294979695818101959450925060029150839083908082843750506040805180820182529497969581810195945092506002915083908390808284375050604080518082018252949796958181019594509250600291508390839080828437505060408051808201825294979695818101959450925060029150839083908082843750506040805186358801803560208181028481018201909552818452979a999888019792965090820194509250829190850190849080828437509497506109169650505050505050565b3480156103d057600080fd5b5061020b610afd565b3480156103e557600080fd5b506040805180820190915261065f9036906004906084908260026000835b8282101561043c576040805180820182529080840286019060029083908390808284375050509183525050600190910190602001610403565b5050604080518082018252939695948181019493509150600290839083908082843750506040805180820190915293969594608081019493509150600290506000835b828210156104b857604080518082018252908084028601906002908390839080828437505050918352505060019091019060200161047f565b5050604080518082019091529295949360808101939250905060026000835b828210156105105760408051808201825290808402860190600290839083908082843750505091835250506001909101906020016104d7565b5050604080518082018252939695948181019493509150600290839083908082843750506040805180820190915293969594608081019493509150600290506000835b8282101561058c576040805180820182529080840286019060029083908390808284375050509183525050600190910190602001610553565b5050604080518082019091529295949360808101939250905060026000835b828210156105e45760408051808201825290808402860190600290839083908082843750505091835250506001909101906020016105ab565b505060408051853587018035602081810284018101909452808352959897968301959194508201925090819060009085015b8282101561064f576040805180820182529080840287019060029083908390808284375050509183525050600190910190602001610616565b50939650610b0695505050505050565b005b600061066b611877565b600080546060919060ff16151561068157600080fd5b60408051808201909152808e6000602090810291909101518252018e600160200201519052835260408051808201909152808d6000602090810291909101518252018d600160209081029190910151909152840152604080516080810182529081908101808e600060200201516000602090810291909101518252018e6000602002015160016020020151815250815260200160408051908101604052808e600160028110151561072e57fe5b602090810291909101515182528f810151810151918101919091529152604085810192909252815180830183528c5181528c820151818301526060860152815180830183528b5181528b820151818301526080860152815180830183528a5181528a8201518183015260a08601528151808301835289518152898201518183015260e08601528151808301835288518152888201518183015260c08601528651825181815281830281019092019092529080156107f5578160200160208202803883390190505b509150600090505b845181101561083e57848181518110151561081457fe5b90602001906020020151828281518110151561082c57fe5b602090810290910101526001016107fd565b81600381518110151561084d57fe5b90602001906020020151600114801561086d575061086b8284610e2f565b155b15610901576040805160208082526022908201527f5472616e73616374696f6e207375636365737366756c6c792076657269666965818301527f642e000000000000000000000000000000000000000000000000000000000000606082015290517f3f3cfdb26fb5f9f1786ab4f1a1f9cd4c0b5e726cbdfc26e495261731aad44e399181900360800190a160019350610906565b600093505b5050509998505050505050505050565b6000610920611877565b600080546060919060ff16151561093657600080fd5b60408051808201909152808e6000602090810291909101518252018e600160200201519052835260408051808201909152808d6000602090810291909101518252018d600160209081029190910151909152840152604080516080810182529081908101808e600060200201516000602090810291909101518252018e6000602002015160016020020151815250815260200160408051908101604052808e60016002811015156109e357fe5b602090810291909101515182528f810151810151918101919091529152604085810192909252815180830183528c5181528c820151818301526060860152815180830183528b5181528b820151818301526080860152815180830183528a5181528a8201518183015260a08601528151808301835289518152898201518183015260e08601528151808301835288518152888201518183015260c0860152865182518181528183028101909201909252908015610aaa578160200160208202803883390190505b509150600090505b8451811015610af3578481815181101515610ac957fe5b906020019060200201518282815181101515610ae157fe5b60209081029091010152600101610ab2565b61086b8284610e2f565b60005460ff1681565b6000805460ff1615610b1757600080fd5b815160051015610b2657600080fd5b604080516080810182528a51518183019081528b5160209081015160608401529082528251808401909352808c01805151845251810151838201528101919091528051600190610b7990829060026118ec565b506020820151610b8f90600280840191906118ec565b50506040805180820182528a518082526020808d015192810183905260059190915560069190915581516080810183528a51518184019081528b518301516060830152815282518084019093528a820180515184525182015183830152908101919091528051909150600790610c0890829060026118ec565b506020820151610c1e90600280840191906118ec565b505060408051608081018252885151818301908152895160209081015160608401529082528251808401909352808a01805151845251810151838201528101919091528051909150600b90610c7690829060026118ec565b506020820151610c8c90600280840191906118ec565b505060408051808201825287518082526020808a0151928101839052600f919091556010919091558151608081018352875151818401908152885183015160608301528152825180840190935287820180515184525182015183830152908101919091528051909150601190610d0590829060026118ec565b506020820151610d1b90600280840191906118ec565b505060408051608081018252855151818301908152865160209081015160608401529082528251808401909352808701805151845251810151838201528101919091528051909150601590610d7390829060026118ec565b506020820151610d8990600280840191906118ec565b5050825160235550600090505b602354811015610e175760408051908101604052808383815181101515610db957fe5b6020908102919091018101515182528451910190849084908110610dd957fe5b6020908102919091018101510151905260198260058110610df657fe5b82516002919091029190910190815560209091015160019182015501610d96565b50506000805460ff1916600117905550505050505050565b6000610e3961192a565b60006001602201548551600101141515610e5257600080fd5b5050604080518082019091526000808252602082018190525b8451811015610eda57610ed082610ecb60196001850160058110610e8b57fe5b60020201604080519081016040529081600082015481526020016001820154815250508885815181101515610ebc57fe5b90602001906020020151611273565b6112c6565b9150600101610e6b565b604080518082019091526019548152601a546020820152610efc9083906112c6565b84516040805160808101808352939550610f8e9390916001918391820190839060029082845b815481526020019060010190808311610f2257505050918352505060408051808201918290526020909201919060028481019182845b815481526020019060010190808311610f5857505050505081525050610f818760200151611315565b610f896113a2565b611463565b1515610f9d576001925061126b565b60408051808201825260055481526006546020820152908501516060860151610fcb929190610f8190611315565b1515610fda576002925061126b565b6080848101516040805192830180825261105e93916007918391820190839060029082845b815481526020019060010190808311610fff57505050918352505060408051808201918290526020909201919060028481019182845b81548152602001906001019080831161103557505050505081525050610f818760a00151611315565b151561106d576003925061126b565b60c0840151604080516080810180835261119c9392600b918391820190839060029082845b81548152602001906001019080831161109257505050918352505060408051808201918290526020909201919060028481019182845b8154815260200190600101908083116110c8575050505050815250506111026110fd86610ecb8a600001518b608001516112c6565b611315565b604080516080810180835290916011918391820190839060029082845b81548152602001906001019080831161111f57505050918352505060408051808201918290526020909201919060028481019182845b8154815260200190600101908083116111555750505091909252505060408051808201909152600f54815260105460208201526111929150611315565b896040015161154e565b15156111ab576004925061126b565b6112576111bc8386600001516112c6565b85604001516111ce8760e00151611315565b604080516080810180835290916015918391820190839060029082845b8154815260200190600101908083116111eb57505050918352505060408051808201918290526020909201919060028481019182845b8154815260200190600101908083116112215750505050508152505061124a8960800151611315565b6112526113a2565b61154e565b1515611266576005925061126b565b600092505b505092915050565b61127b61192a565b611283611941565b83518152602080850151908201526040810183905260006060836080848460076107d05a03f190508080156112b7576112b9565bfe5b5080151561126b57600080fd5b6112ce61192a565b6112d6611960565b8351815260208085015181830152835160408301528301516060808301919091526000908360c0848460066107d05a03f190508080156112b7576112b9565b61131d61192a565b81517f30644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd479015801561135057506020830151155b15611370576040805180820190915260008082526020820152915061139c565b60408051908101604052808460000151815260200182856020015181151561139457fe5b068303905291505b50919050565b6113aa61197f565b50604080516080810182527f198e9393920d483a7260bfb731fb5d25f1aa493335a9e71297e485b7aef312c28183019081527f1800deef121f1e76426a00665e5c4479674322d4f75edadd46debd5cd992f6ed6060830152815281518083019092527f090689d0585ff075ec9e99ad690c3395bc4b313370b38ef355acdadcd122975b82527f12c85ea5db8c6deb4aab71808dcb408fe3d1e7690c43d37b4ce6cc0166fa7daa6020838101919091528101919091525b90565b6040805160028082526060828101909352600092918291816020015b61148761192a565b81526020019060019003908161147f57505060408051600280825260608201909252919350602082015b6114b961197f565b8152602001906001900390816114b1579050509050868260008151811015156114de57fe5b6020908102909101015281518590839060019081106114f957fe5b60209081029091010152805186908290600090811061151457fe5b60209081029091010152805184908290600190811061152f57fe5b602090810290910101526115438282611672565b979650505050505050565b604080516003808252608082019092526000916060918291816020015b61157361192a565b81526020019060019003908161156b57505060408051600380825260808201909252919350602082015b6115a561197f565b81526020019060019003908161159d579050509050888260008151811015156115ca57fe5b6020908102909101015281518790839060019081106115e557fe5b60209081029091010152815185908390600290811061160057fe5b60209081029091010152805188908290600090811061161b57fe5b60209081029091010152805186908290600190811061163657fe5b60209081029091010152805184908290600290811061165157fe5b602090810290910101526116658282611672565b9998505050505050505050565b6000806000606060006116836119a0565b865188516000911461169457600080fd5b88519550856006029450846040519080825280602002602001820160405280156116c8578160200160208202803883390190505b509350600092505b8583101561183d5788838151811015156116e657fe5b6020908102909101015151845185906006860290811061170257fe5b60209081029091010152885189908490811061171a57fe5b9060200190602002015160200151848460060260010181518110151561173c57fe5b60209081029091010152875188908490811061175457fe5b60209081029190910101515151845185906002600687020190811061177557fe5b60209081029091010152875188908490811061178d57fe5b60209081029190910181015151015184518590600360068702019081106117b057fe5b6020908102909101015287518890849081106117c857fe5b60209081029190910181015101515184518590600460068702019081106117eb57fe5b60209081029091010152875188908490811061180357fe5b6020908102919091018101518101510151845185906005600687020190811061182857fe5b602090810290910101526001909201916116d0565b6020826020870260208701600060086107d05a03f190508080156112b7575080151561186857600080fd5b50511515979650505050505050565b6102406040519081016040528061188c61192a565b815260200161189961192a565b81526020016118a661197f565b81526020016118b361192a565b81526020016118c061192a565b81526020016118cd61192a565b81526020016118da61192a565b81526020016118e761192a565b905290565b826002810192821561191a579160200282015b8281111561191a5782518255916020019190600101906118ff565b506119269291506119bf565b5090565b604080518082019091526000808252602082015290565b6060604051908101604052806003906020820280388339509192915050565b6080604051908101604052806004906020820280388339509192915050565b6080604051908101604052806119936119d9565b81526020016118e76119d9565b6020604051908101604052806001906020820280388339509192915050565b61146091905b8082111561192657600081556001016119c5565b604080518082018252906002908290803883395091929150505600a165627a7a72305820d8fd6eaa2d9da6dc912422ccd157f2a9f6c21fcfd5c1a5c6ede9d9563adcab110029`

// DeployVerifier deploys a new Ethereum contract, binding an instance of Verifier to it.
func DeployVerifier(auth *bind.TransactOpts, backend bind.ContractBackend) (common.Address, *types.Transaction, *Verifier, error) {
	parsed, err := abi.JSON(strings.NewReader(VerifierABI))
	if err != nil {
		return common.Address{}, nil, nil, err
	}
	address, tx, contract, err := bind.DeployContract(auth, parsed, common.FromHex(VerifierBin), backend)
	if err != nil {
		return common.Address{}, nil, nil, err
	}
	return address, tx, &Verifier{VerifierCaller: VerifierCaller{contract: contract}, VerifierTransactor: VerifierTransactor{contract: contract}, VerifierFilterer: VerifierFilterer{contract: contract}}, nil
}

// Verifier is an auto generated Go binding around an Ethereum contract.
type Verifier struct {
	VerifierCaller     // Read-only binding to the contract
	VerifierTransactor // Write-only binding to the contract
	VerifierFilterer   // Log filterer for contract events
}

// VerifierCaller is an auto generated read-only Go binding around an Ethereum contract.
type VerifierCaller struct {
	contract *bind.BoundContract // Generic contract wrapper for the low level calls
}

// VerifierTransactor is an auto generated write-only Go binding around an Ethereum contract.
type VerifierTransactor struct {
	contract *bind.BoundContract // Generic contract wrapper for the low level calls
}

// VerifierFilterer is an auto generated log filtering Go binding around an Ethereum contract events.
type VerifierFilterer struct {
	contract *bind.BoundContract // Generic contract wrapper for the low level calls
}

// VerifierSession is an auto generated Go binding around an Ethereum contract,
// with pre-set call and transact options.
type VerifierSession struct {
	Contract     *Verifier         // Generic contract binding to set the session for
	CallOpts     bind.CallOpts     // Call options to use throughout this session
	TransactOpts bind.TransactOpts // Transaction auth options to use throughout this session
}

// VerifierCallerSession is an auto generated read-only Go binding around an Ethereum contract,
// with pre-set call options.
type VerifierCallerSession struct {
	Contract *VerifierCaller // Generic contract caller binding to set the session for
	CallOpts bind.CallOpts   // Call options to use throughout this session
}

// VerifierTransactorSession is an auto generated write-only Go binding around an Ethereum contract,
// with pre-set transact options.
type VerifierTransactorSession struct {
	Contract     *VerifierTransactor // Generic contract transactor binding to set the session for
	TransactOpts bind.TransactOpts   // Transaction auth options to use throughout this session
}

// VerifierRaw is an auto generated low-level Go binding around an Ethereum contract.
type VerifierRaw struct {
	Contract *Verifier // Generic contract binding to access the raw methods on
}

// VerifierCallerRaw is an auto generated low-level read-only Go binding around an Ethereum contract.
type VerifierCallerRaw struct {
	Contract *VerifierCaller // Generic read-only contract binding to access the raw methods on
}

// VerifierTransactorRaw is an auto generated low-level write-only Go binding around an Ethereum contract.
type VerifierTransactorRaw struct {
	Contract *VerifierTransactor // Generic write-only contract binding to access the raw methods on
}

// NewVerifier creates a new instance of Verifier, bound to a specific deployed contract.
func NewVerifier(address common.Address, backend bind.ContractBackend) (*Verifier, error) {
	contract, err := bindVerifier(address, backend, backend, backend)
	if err != nil {
		return nil, err
	}
	return &Verifier{VerifierCaller: VerifierCaller{contract: contract}, VerifierTransactor: VerifierTransactor{contract: contract}, VerifierFilterer: VerifierFilterer{contract: contract}}, nil
}

// NewVerifierCaller creates a new read-only instance of Verifier, bound to a specific deployed contract.
func NewVerifierCaller(address common.Address, caller bind.ContractCaller) (*VerifierCaller, error) {
	contract, err := bindVerifier(address, caller, nil, nil)
	if err != nil {
		return nil, err
	}
	return &VerifierCaller{contract: contract}, nil
}

// NewVerifierTransactor creates a new write-only instance of Verifier, bound to a specific deployed contract.
func NewVerifierTransactor(address common.Address, transactor bind.ContractTransactor) (*VerifierTransactor, error) {
	contract, err := bindVerifier(address, nil, transactor, nil)
	if err != nil {
		return nil, err
	}
	return &VerifierTransactor{contract: contract}, nil
}

// NewVerifierFilterer creates a new log filterer instance of Verifier, bound to a specific deployed contract.
func NewVerifierFilterer(address common.Address, filterer bind.ContractFilterer) (*VerifierFilterer, error) {
	contract, err := bindVerifier(address, nil, nil, filterer)
	if err != nil {
		return nil, err
	}
	return &VerifierFilterer{contract: contract}, nil
}

// bindVerifier binds a generic wrapper to an already deployed contract.
func bindVerifier(address common.Address, caller bind.ContractCaller, transactor bind.ContractTransactor, filterer bind.ContractFilterer) (*bind.BoundContract, error) {
	parsed, err := abi.JSON(strings.NewReader(VerifierABI))
	if err != nil {
		return nil, err
	}
	return bind.NewBoundContract(address, parsed, caller, transactor, filterer), nil
}

// Call invokes the (constant) contract method with params as input values and
// sets the output to result. The result type might be a single field for simple
// returns, a slice of interfaces for anonymous returns and a struct for named
// returns.
func (_Verifier *VerifierRaw) Call(opts *bind.CallOpts, result interface{}, method string, params ...interface{}) error {
	return _Verifier.Contract.VerifierCaller.contract.Call(opts, result, method, params...)
}

// Transfer initiates a plain transaction to move funds to the contract, calling
// its default method if one is available.
func (_Verifier *VerifierRaw) Transfer(opts *bind.TransactOpts) (*types.Transaction, error) {
	return _Verifier.Contract.VerifierTransactor.contract.Transfer(opts)
}

// Transact invokes the (paid) contract method with params as input values.
func (_Verifier *VerifierRaw) Transact(opts *bind.TransactOpts, method string, params ...interface{}) (*types.Transaction, error) {
	return _Verifier.Contract.VerifierTransactor.contract.Transact(opts, method, params...)
}

// Call invokes the (constant) contract method with params as input values and
// sets the output to result. The result type might be a single field for simple
// returns, a slice of interfaces for anonymous returns and a struct for named
// returns.
func (_Verifier *VerifierCallerRaw) Call(opts *bind.CallOpts, result interface{}, method string, params ...interface{}) error {
	return _Verifier.Contract.contract.Call(opts, result, method, params...)
}

// Transfer initiates a plain transaction to move funds to the contract, calling
// its default method if one is available.
func (_Verifier *VerifierTransactorRaw) Transfer(opts *bind.TransactOpts) (*types.Transaction, error) {
	return _Verifier.Contract.contract.Transfer(opts)
}

// Transact invokes the (paid) contract method with params as input values.
func (_Verifier *VerifierTransactorRaw) Transact(opts *bind.TransactOpts, method string, params ...interface{}) (*types.Transaction, error) {
	return _Verifier.Contract.contract.Transact(opts, method, params...)
}

// VerifyingKeySet is a free data retrieval call binding the contract method 0xf5282dea.
//
// Solidity: function verifyingKeySet() constant returns(bool)
func (_Verifier *VerifierCaller) VerifyingKeySet(opts *bind.CallOpts) (bool, error) {
	var (
		ret0 = new(bool)
	)
	out := ret0
	err := _Verifier.contract.Call(opts, out, "verifyingKeySet")
	return *ret0, err
}

// VerifyingKeySet is a free data retrieval call binding the contract method 0xf5282dea.
//
// Solidity: function verifyingKeySet() constant returns(bool)
func (_Verifier *VerifierSession) VerifyingKeySet() (bool, error) {
	return _Verifier.Contract.VerifyingKeySet(&_Verifier.CallOpts)
}

// VerifyingKeySet is a free data retrieval call binding the contract method 0xf5282dea.
//
// Solidity: function verifyingKeySet() constant returns(bool)
func (_Verifier *VerifierCallerSession) VerifyingKeySet() (bool, error) {
	return _Verifier.Contract.VerifyingKeySet(&_Verifier.CallOpts)
}

// SetVerifyingKey is a paid mutator transaction binding the contract method 0xfe22023b.
//
// Solidity: function setVerifyingKey(uint256[2][2] A, uint256[2] B, uint256[2][2] C, uint256[2][2] gamma, uint256[2] gamma_beta_1, uint256[2][2] gamma_beta_2, uint256[2][2] Z, uint256[2][] IC) returns()
func (_Verifier *VerifierTransactor) SetVerifyingKey(opts *bind.TransactOpts, A [2][2]*big.Int, B [2]*big.Int, C [2][2]*big.Int, gamma [2][2]*big.Int, gamma_beta_1 [2]*big.Int, gamma_beta_2 [2][2]*big.Int, Z [2][2]*big.Int, IC [][2]*big.Int) (*types.Transaction, error) {
	return _Verifier.contract.Transact(opts, "setVerifyingKey", A, B, C, gamma, gamma_beta_1, gamma_beta_2, Z, IC)
}

// SetVerifyingKey is a paid mutator transaction binding the contract method 0xfe22023b.
//
// Solidity: function setVerifyingKey(uint256[2][2] A, uint256[2] B, uint256[2][2] C, uint256[2][2] gamma, uint256[2] gamma_beta_1, uint256[2][2] gamma_beta_2, uint256[2][2] Z, uint256[2][] IC) returns()
func (_Verifier *VerifierSession) SetVerifyingKey(A [2][2]*big.Int, B [2]*big.Int, C [2][2]*big.Int, gamma [2][2]*big.Int, gamma_beta_1 [2]*big.Int, gamma_beta_2 [2][2]*big.Int, Z [2][2]*big.Int, IC [][2]*big.Int) (*types.Transaction, error) {
	return _Verifier.Contract.SetVerifyingKey(&_Verifier.TransactOpts, A, B, C, gamma, gamma_beta_1, gamma_beta_2, Z, IC)
}

// SetVerifyingKey is a paid mutator transaction binding the contract method 0xfe22023b.
//
// Solidity: function setVerifyingKey(uint256[2][2] A, uint256[2] B, uint256[2][2] C, uint256[2][2] gamma, uint256[2] gamma_beta_1, uint256[2][2] gamma_beta_2, uint256[2][2] Z, uint256[2][] IC) returns()
func (_Verifier *VerifierTransactorSession) SetVerifyingKey(A [2][2]*big.Int, B [2]*big.Int, C [2][2]*big.Int, gamma [2][2]*big.Int, gamma_beta_1 [2]*big.Int, gamma_beta_2 [2][2]*big.Int, Z [2][2]*big.Int, IC [][2]*big.Int) (*types.Transaction, error) {
	return _Verifier.Contract.SetVerifyingKey(&_Verifier.TransactOpts, A, B, C, gamma, gamma_beta_1, gamma_beta_2, Z, IC)
}

// VerifySSIMTx is a paid mutator transaction binding the contract method 0x80d3cb70.
//
// Solidity: function verifySSIMTx(uint256[2] a, uint256[2] a_p, uint256[2][2] b, uint256[2] b_p, uint256[2] c, uint256[2] c_p, uint256[2] h, uint256[2] k, uint256[] input) returns(bool r)
func (_Verifier *VerifierTransactor) VerifySSIMTx(opts *bind.TransactOpts, a [2]*big.Int, a_p [2]*big.Int, b [2][2]*big.Int, b_p [2]*big.Int, c [2]*big.Int, c_p [2]*big.Int, h [2]*big.Int, k [2]*big.Int, input []*big.Int) (*types.Transaction, error) {
	return _Verifier.contract.Transact(opts, "verifySSIMTx", a, a_p, b, b_p, c, c_p, h, k, input)
}

// VerifySSIMTx is a paid mutator transaction binding the contract method 0x80d3cb70.
//
// Solidity: function verifySSIMTx(uint256[2] a, uint256[2] a_p, uint256[2][2] b, uint256[2] b_p, uint256[2] c, uint256[2] c_p, uint256[2] h, uint256[2] k, uint256[] input) returns(bool r)
func (_Verifier *VerifierSession) VerifySSIMTx(a [2]*big.Int, a_p [2]*big.Int, b [2][2]*big.Int, b_p [2]*big.Int, c [2]*big.Int, c_p [2]*big.Int, h [2]*big.Int, k [2]*big.Int, input []*big.Int) (*types.Transaction, error) {
	return _Verifier.Contract.VerifySSIMTx(&_Verifier.TransactOpts, a, a_p, b, b_p, c, c_p, h, k, input)
}

// VerifySSIMTx is a paid mutator transaction binding the contract method 0x80d3cb70.
//
// Solidity: function verifySSIMTx(uint256[2] a, uint256[2] a_p, uint256[2][2] b, uint256[2] b_p, uint256[2] c, uint256[2] c_p, uint256[2] h, uint256[2] k, uint256[] input) returns(bool r)
func (_Verifier *VerifierTransactorSession) VerifySSIMTx(a [2]*big.Int, a_p [2]*big.Int, b [2][2]*big.Int, b_p [2]*big.Int, c [2]*big.Int, c_p [2]*big.Int, h [2]*big.Int, k [2]*big.Int, input []*big.Int) (*types.Transaction, error) {
	return _Verifier.Contract.VerifySSIMTx(&_Verifier.TransactOpts, a, a_p, b, b_p, c, c_p, h, k, input)
}

// VerifyTx is a paid mutator transaction binding the contract method 0xc8e6ba4d.
//
// Solidity: function verifyTx(uint256[2] a, uint256[2] a_p, uint256[2][2] b, uint256[2] b_p, uint256[2] c, uint256[2] c_p, uint256[2] h, uint256[2] k, uint256[] input) returns(bool r)
func (_Verifier *VerifierTransactor) VerifyTx(opts *bind.TransactOpts, a [2]*big.Int, a_p [2]*big.Int, b [2][2]*big.Int, b_p [2]*big.Int, c [2]*big.Int, c_p [2]*big.Int, h [2]*big.Int, k [2]*big.Int, input []*big.Int) (*types.Transaction, error) {
	return _Verifier.contract.Transact(opts, "verifyTx", a, a_p, b, b_p, c, c_p, h, k, input)
}

// VerifyTx is a paid mutator transaction binding the contract method 0xc8e6ba4d.
//
// Solidity: function verifyTx(uint256[2] a, uint256[2] a_p, uint256[2][2] b, uint256[2] b_p, uint256[2] c, uint256[2] c_p, uint256[2] h, uint256[2] k, uint256[] input) returns(bool r)
func (_Verifier *VerifierSession) VerifyTx(a [2]*big.Int, a_p [2]*big.Int, b [2][2]*big.Int, b_p [2]*big.Int, c [2]*big.Int, c_p [2]*big.Int, h [2]*big.Int, k [2]*big.Int, input []*big.Int) (*types.Transaction, error) {
	return _Verifier.Contract.VerifyTx(&_Verifier.TransactOpts, a, a_p, b, b_p, c, c_p, h, k, input)
}

// VerifyTx is a paid mutator transaction binding the contract method 0xc8e6ba4d.
//
// Solidity: function verifyTx(uint256[2] a, uint256[2] a_p, uint256[2][2] b, uint256[2] b_p, uint256[2] c, uint256[2] c_p, uint256[2] h, uint256[2] k, uint256[] input) returns(bool r)
func (_Verifier *VerifierTransactorSession) VerifyTx(a [2]*big.Int, a_p [2]*big.Int, b [2][2]*big.Int, b_p [2]*big.Int, c [2]*big.Int, c_p [2]*big.Int, h [2]*big.Int, k [2]*big.Int, input []*big.Int) (*types.Transaction, error) {
	return _Verifier.Contract.VerifyTx(&_Verifier.TransactOpts, a, a_p, b, b_p, c, c_p, h, k, input)
}

// VerifierVerifiedIterator is returned from FilterVerified and is used to iterate over the raw logs and unpacked data for Verified events raised by the Verifier contract.
type VerifierVerifiedIterator struct {
	Event *VerifierVerified // Event containing the contract specifics and raw log

	contract *bind.BoundContract // Generic contract to use for unpacking event data
	event    string              // Event name to use for unpacking event data

	logs chan types.Log        // Log channel receiving the found contract events
	sub  ethereum.Subscription // Subscription for errors, completion and termination
	done bool                  // Whether the subscription completed delivering logs
	fail error                 // Occurred error to stop iteration
}

// Next advances the iterator to the subsequent event, returning whether there
// are any more events found. In case of a retrieval or parsing error, false is
// returned and Error() can be queried for the exact failure.
func (it *VerifierVerifiedIterator) Next() bool {
	// If the iterator failed, stop iterating
	if it.fail != nil {
		return false
	}
	// If the iterator completed, deliver directly whatever's available
	if it.done {
		select {
		case log := <-it.logs:
			it.Event = new(VerifierVerified)
			if err := it.contract.UnpackLog(it.Event, it.event, log); err != nil {
				it.fail = err
				return false
			}
			it.Event.Raw = log
			return true

		default:
			return false
		}
	}
	// Iterator still in progress, wait for either a data or an error event
	select {
	case log := <-it.logs:
		it.Event = new(VerifierVerified)
		if err := it.contract.UnpackLog(it.Event, it.event, log); err != nil {
			it.fail = err
			return false
		}
		it.Event.Raw = log
		return true

	case err := <-it.sub.Err():
		it.done = true
		it.fail = err
		return it.Next()
	}
}

// Error returns any retrieval or parsing error occurred during filtering.
func (it *VerifierVerifiedIterator) Error() error {
	return it.fail
}

// Close terminates the iteration process, releasing any pending underlying
// resources.
func (it *VerifierVerifiedIterator) Close() error {
	it.sub.Unsubscribe()
	return nil
}

// VerifierVerified represents a Verified event raised by the Verifier contract.
type VerifierVerified struct {
	string
	Raw types.Log // Blockchain specific contextual infos
}

// FilterVerified is a free log retrieval operation binding the contract event 0x3f3cfdb26fb5f9f1786ab4f1a1f9cd4c0b5e726cbdfc26e495261731aad44e39.
//
// Solidity: event Verified(string )
func (_Verifier *VerifierFilterer) FilterVerified(opts *bind.FilterOpts) (*VerifierVerifiedIterator, error) {

	logs, sub, err := _Verifier.contract.FilterLogs(opts, "Verified")
	if err != nil {
		return nil, err
	}
	return &VerifierVerifiedIterator{contract: _Verifier.contract, event: "Verified", logs: logs, sub: sub}, nil
}

// WatchVerified is a free log subscription operation binding the contract event 0x3f3cfdb26fb5f9f1786ab4f1a1f9cd4c0b5e726cbdfc26e495261731aad44e39.
//
// Solidity: event Verified(string )
func (_Verifier *VerifierFilterer) WatchVerified(opts *bind.WatchOpts, sink chan<- *VerifierVerified) (event.Subscription, error) {

	logs, sub, err := _Verifier.contract.WatchLogs(opts, "Verified")
	if err != nil {
		return nil, err
	}
	return event.NewSubscription(func(quit <-chan struct{}) error {
		defer sub.Unsubscribe()
		for {
			select {
			case log := <-logs:
				// New log arrived, parse the event and forward to the user
				event := new(VerifierVerified)
				if err := _Verifier.contract.UnpackLog(event, "Verified", log); err != nil {
					return err
				}
				event.Raw = log

				select {
				case sink <- event:
				case err := <-sub.Err():
					return err
				case <-quit:
					return nil
				}
			case err := <-sub.Err():
				return err
			case <-quit:
				return nil
			}
		}
	}), nil
}
