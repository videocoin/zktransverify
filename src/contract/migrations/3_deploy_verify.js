var Pairing = artifacts.require("./ZkTransVerify.sol");
var ZkTransVerify = artifacts.require("./ZkTransVerify.sol");
module.exports = function(deployer) {
  deployer.deploy(Pairing);
  deployer.link(Pairing, ZkTransVerify);
  deployer.deploy(ZkTransVerify);
};
