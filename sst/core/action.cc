// Copyright 2009-2014 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
// 
// Copyright (c) 2009-2014, Sandia Corporation
// All rights reserved.
// 
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#include "sst_config.h"
#include "sst/core/serialization.h"
#include "sst/core/action.h"

#include "sst/core/simulation.h"

namespace SST {

void Action::endSimulation() {
    Simulation::getSimulation()->endSimulation();
}

} // namespace SST

BOOST_CLASS_EXPORT_IMPLEMENT(SST::Action);
