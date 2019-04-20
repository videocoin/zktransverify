var BN256G2 = artifacts.require("BN256G2");
var Pairing = artifacts.require("Pairing");
var Verifier = artifacts.require("Verifier");

module.exports = function(deployer) {
  deployer.deploy(BN256G2);
  deployer.link(BN256G2, Pairing);
  deployer.deploy(Pairing);
  deployer.link(Pairing, Verifier);
  deployer.deploy(Verifier);
};
