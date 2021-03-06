/*
 * TestMiner.h
 *
 *  Created on: Dec 14, 2017
 *      Author: ranjeet
 */

#ifndef ENERGIMINER_TESTMINER_H_
#define ENERGIMINER_TESTMINER_H_


#include "nrgcore/plant.h"
#include "nrgcore/miner.h"


namespace energi
{
  class TestMiner : public Miner
  {
  public:
    TestMiner(const Plant& plant, unsigned index);
    virtual ~TestMiner()
    {}

  protected:
    void trun() override;
    void kick_miner() override {}
  };

} /* namespace energi */

#endif /* ENERGIMINER_TESTMINER_H_ */
