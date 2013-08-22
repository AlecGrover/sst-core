// Copyright 2009-2013 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
// 
// Copyright (c) 2009-2013, Sandia Corporation
// All rights reserved.
// 
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.


#ifndef SST_CORE_EXIT_H
#define SST_CORE_EXIT_H

#include <set>

#include "sst/core/sst_types.h"
#include "sst/core/action.h"
#include <sst/core/serialization.h>

namespace SST{

#define _EXIT_DBG( fmt, args...) __DBG( DBG_EXIT, Exit, fmt, ## args )

class Simulation;
class TimeConverter;

class Exit : public Action {
public:
    // Exit needs to register a handler during constructor time, which
    // requires a simulation object.  But the simulation class creates
    // an Exit object during it's construction, meaning that
    // Simulation::getSimulation() won't work yet.  So Exit is the one
    // exception to the "constructors shouldn't take simulation
    // pointers" rule.  However, it still needs to follow the "classes
    // shouldn't contain pointers back to Simulation" rule.
    Exit( Simulation* sim, TimeConverter* period, bool single_rank );
    ~Exit();
    
    bool refInc( ComponentId_t );
    bool refDec( ComponentId_t );

private:
    Exit() { } // for serialization only
    Exit(const Exit&);           // Don't implement
    void operator=(Exit const&); // Don't implement

//     bool handler( Event* );
    void execute(void);
    
//     EventHandler< Exit, bool, Event* >* m_functor;
    unsigned int    m_refCount;
    TimeConverter*  m_period;
    std::set<ComponentId_t> m_idSet;

    bool single_rank;
    
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};

} // namespace SST

BOOST_CLASS_EXPORT_KEY(SST::Exit)

#endif // SST_EXIT_H
