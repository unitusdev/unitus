*Unitus: A Merge-Mined, Multi-Algorithm cryptocurrency with a focus on network security, decentralisation and fair distribution*

---

Please Note: This is the master branch, only stable code releases are available here. Pre-built binaries for common platforms  are available in the Releases above. Active development is completed in the dev branch.

---

**Specifications**

* _Modern Wallet_ - Forked from Bitcoin Core 0.14.2
* _Multi-Algorithm_:
  * Lyra2re2 (replaced Blake256 in Feb 2016)
  * Skein
  * Argon2d (replaced Qubit in Jun 2017)
  * X11
  * Yescrypt
* All algorithms can be merge-mined (AuxPoW)
* Each algorithmhas a target 5 minute block time, resulting in a 1 minute block time for the whole network
* Difficulty Retargeting:
  * Each algorithm re-targets every block for that algorithm
  * Uses average block time for past 10 blocks to steer difficulty towards the target block time
  * Maximum change of 5% up or down during normal mining phase, but first 1999 blocks allowed 20% change up or down
* Fair Distribution:
  * No Premine / IPO / ICO / Developer Donation.
  * First 2,000 blocks increase from 1 UIS to 100 UIS (approx. 33 Hours) - total 31,799 UIS
  * Every 7 days / 10,080 blocks thereafter the reward decays by 1% (compound)
  * Decay continues until we reach 2 UIS (approx. June 2022, block 3,933,199, total 98,830,980 UIS) - and then this remains static for each block thereafter
  * Mined blocks mature after 180 confirmations
* No free transactions - minimum fee is 0.01 UIS
* Peer-to-Peer Port: 50603
* RPC Port: 50604


